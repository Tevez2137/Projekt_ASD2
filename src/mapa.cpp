#include "mapa.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include "krasnoludek.h"
#include "kopalnia.h"
#include "otoczka.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cmath> // Wymagane dla std::hypot i std::round
#include <vector>

using namespace std;

const int INF = 1e9;

// ==========================================
// 1. GŁÓWNY SILNIK (WCZYTYWANIE + ODPALANIE)
// ==========================================
void Graph::init()
{
    // A) Odczyt kopalni
    ifstream plikKopalnie("data/kopalnie.csv");
    if (plikKopalnie.is_open())
    {
        string linia;
        getline(plikKopalnie, linia); // Pomijamy nagłówek

        while (getline(plikKopalnie, linia))
        {
            if (linia.empty())
                continue;
            stringstream ss(linia);
            string temp;

            int iloscMiejsc;
            Wspolrzedne wspolrzedne;
            string surowiec, ID;

            getline(ss, temp, ',');
            ID = temp;
            getline(ss, temp, ',');
            wspolrzedne.x = stoi(temp);
            getline(ss, temp, ',');
            wspolrzedne.y = stoi(temp);
            getline(ss, surowiec, ',');
            getline(ss, temp, ',');
            iloscMiejsc = stoi(temp);

            this->kopalnie.push_back(Kopalnia(ID, wspolrzedne, surowiec, iloscMiejsc));
        }
        plikKopalnie.close();
    }
    else
    {
        cout << "Nie mozna otworzyc pliku data/kopalnie.csv!" << endl;
    }

    // B) Odczyt krasnoludków
    ifstream plikKrasnoludki("data/dane_krasnoludkow.csv");
    if (plikKrasnoludki.is_open())
    {
        string linia;
        getline(plikKrasnoludki, linia); // Pomijamy nagłówek

        while (getline(plikKrasnoludki, linia))
        {
            if (linia.empty())
                continue;

            string ID, ID_kopalni;
            vector<string> mineraly;
            Domek domek;
            stringstream ss(linia);
            string temp;

            getline(ss, temp, ',');
            ID = temp;
            getline(ss, temp, ',');
            ID_kopalni = temp;

            // Czytamy Minerały i rozbijamy po średniku
            string wszystkieMineraly;
            getline(ss, wszystkieMineraly, ',');
            stringstream ssMineraly(wszystkieMineraly);
            string pojedynczyMineral;
            while (getline(ssMineraly, pojedynczyMineral, ';'))
            {
                mineraly.push_back(pojedynczyMineral);
            }

            // Czytamy współrzędne domku
            getline(ss, temp, ',');
            domek.x = stoi(temp);
            getline(ss, temp, ',');
            domek.y = stoi(temp);

            this->krasnoludki.push_back(Krasnoludek(ID, ID_kopalni, mineraly, domek));
        }
        plikKrasnoludki.close();
    }
    else
    {
        cout << "Nie mozna otworzyc pliku data/dane_krasnoludkow.csv!" << endl;
    }

    // C) Ustawiamy rozmiar grafu po wczytaniu danych
    int N = this->krasnoludki.size();
    int M = this->kopalnie.size();

    this->vertices = N + M + 2; // +2 bo Zrodlo(0) i Ujscie(N+M+1)
    this->adj.resize(this->vertices);

    this->zrodlo = 0;
    this->ujscie = N + M + 1;

    // ==================================================
    // MAGIA AUTOUZUPELNIANIA (odpalanie reszty procesu)
    // ==================================================
    cout << "Budowanie sieci polaczen (z filtrowaniem surowcow)..." << endl;
    this->buildGraph();

    cout << "Obliczanie optymalnego przydzialu (Cycle Canceling)..." << endl;
    this->minCostMaxFlow(this->zrodlo, this->ujscie);

    this->obliczTraseKsiecia();

    cout << "Zapisywanie wynikow dla Pythona..." << endl;
    this->saveResults("data/przydzialy.txt");
}

// ==========================================
// 2. BUDOWANIE INTELIGENTNEGO GRAFU
// ==========================================
void Graph::buildGraph()
{
    int N = this->krasnoludki.size();
    int M = this->kopalnie.size();

    // A) Źródło -> Krasnoludki (pojemność 1, koszt 0)
    for (int i = 0; i < N; ++i)
    {
        addEdge(this->zrodlo, i + 1, 1, 0);
    }

    // B) Krasnoludki -> Kopalnie (FILTROWANIE SUROWCÓW + KOSZT JAKO DYSTANS)
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            std::string surowiecKopalni = this->kopalnie[j].surowiec;

            // Sprawdzamy czy krasnoludek lubi ten surowiec
            auto it = std::find(
                this->krasnoludki[i].mineraly.begin(),
                this->krasnoludki[i].mineraly.end(),
                surowiecKopalni);

            if (it != this->krasnoludki[i].mineraly.end())
            {
                // Liczymy dystans
                double dx = this->krasnoludki[i].domek.x - this->kopalnie[j].wspolrzedne.x;
                double dy = this->krasnoludki[i].domek.y - this->kopalnie[j].wspolrzedne.y;
                int dystans = std::round(std::hypot(dx, dy));

                // Dodajemy krawędź
                addEdge(i + 1, N + j + 1, 1, dystans);
            }
        }
    }

    // C) Kopalnie -> Ujście (pojemność = ilość miejsc, koszt 0)
    for (int j = 0; j < M; ++j)
    {
        addEdge(N + j + 1, this->ujscie, this->kopalnie[j].iloscMiejsc, 0);
    }
}

// ==========================================
// 3. GENEROWANIE WYNIKÓW DLA GUI
// ==========================================
void Graph::saveResults(const std::string &filename)
{
    std::ofstream plik(filename);
    if (!plik.is_open())
    {
        cout << "Blad przy tworzeniu pliku: " << filename << endl;
        return;
    }

    int N = this->krasnoludki.size();

    // Szukamy przepływu po krasnoludkach (wierzchołki od 1 do N)
    for (int u = 1; u <= N; u++)
    {
        for (const auto &e : adj[u])
        {
            // Jeśli poszedł flow i krawędź nie wraca do źródła
            if (e.flow > 0 && e.to != this->zrodlo)
            {

                // Odkodowujemy indeks kopalni
                int kopalnia_idx = e.to - N - 1;

                // Zapisujemy: ID_Krasnoludka ID_Kopalni
                plik << this->krasnoludki[u - 1].ID << " " << this->kopalnie[kopalnia_idx].ID << "\n";
            }
        }
    }
    plik.close();
    cout << "Gotowe! Zapisano wyniki do: " << filename << endl;
}

// ==========================================
// 4. FUNKCJE GRAFOWE (ORYGINALNE)
// ==========================================
Graph::Graph(int v) : vertices(v)
{
    adj.resize(vertices);
}

void Graph::addEdge(int u, int v, int cap, int cost)
{
    adj[u].push_back({v, cap, 0, cost, (int)adj[v].size()});
    adj[v].push_back({u, 0, 0, -cost, (int)adj[u].size() - 1});
}

// ==========================================================
// 5. FAZA 1: MAKSYMALNY PRZEPŁYW (Edmonds-Karp)
// ==========================================================
int Graph::findMaxFlow(int s, int t)
{
    int flow = 0;
    vector<int> parent(vertices);
    vector<int> edge_from(vertices);

    while (true)
    {
        fill(parent.begin(), parent.end(), -1);
        queue<int> q;
        q.push(s);
        parent[s] = s;

        while (!q.empty())
        {
            int u = q.front();
            q.pop();
            for (int i = 0; i < (int)adj[u].size(); i++)
            {
                Edge &e = adj[u][i];
                if (parent[e.to] == -1 && e.capacity > e.flow)
                {
                    parent[e.to] = u;
                    edge_from[e.to] = i;
                    q.push(e.to);
                }
            }
        }

        if (parent[t] == -1)
            break;

        int push = INF;
        for (int v = t; v != s; v = parent[v])
        {
            int u = parent[v];
            int idx = edge_from[v];
            push = min(push, adj[u][idx].capacity - adj[u][idx].flow);
        }

        for (int v = t; v != s; v = parent[v])
        {
            int u = parent[v];
            int idx = edge_from[v];
            int rev_idx = adj[u][idx].rev_idx;
            adj[u][idx].flow += push;
            adj[v][rev_idx].flow -= push;
        }
        flow += push;
    }
    return flow;
}

// ==========================================================
// 6. FAZA 2: USUWANIE CYKLI O UJEMNYM KOSZCIE (Zoptymalizowane)
// ==========================================================
void Graph::minCostMaxFlow(int start, int end)
{
    int totalFlow = findMaxFlow(start, end);
    cout << "-> Faza 1: Znaleziono maksymalny przeplyw: " << totalFlow << " krasnoludkow." << endl;

    // OPTYMALIZACJA PAMIĘCI: Deklarujemy wektory TYLKO RAZ poza pętlą
    vector<int> dist(vertices);
    vector<int> parent(vertices);
    vector<int> edge_to_parent(vertices);

    while (true)
    {
        // Zamiast tworzyć na nowo, po prostu je "zerujemy"
        fill(dist.begin(), dist.end(), 0);
        fill(parent.begin(), parent.end(), -1);
        fill(edge_to_parent.begin(), edge_to_parent.end(), -1);

        int node_in_cycle = -1;

        for (int i = 0; i < vertices; i++)
        {
            node_in_cycle = -1;
            for (int u = 0; u < vertices; u++)
            {
                for (int j = 0; j < (int)adj[u].size(); j++)
                {
                    Edge &e = adj[u][j];
                    if (e.capacity > e.flow && dist[e.to] > dist[u] + e.cost)
                    {
                        dist[e.to] = dist[u] + e.cost;
                        parent[e.to] = u;
                        edge_to_parent[e.to] = j;
                        node_in_cycle = e.to;
                    }
                }
            }
        }

        if (node_in_cycle == -1)
            break;

        for (int i = 0; i < vertices; i++)
            node_in_cycle = parent[node_in_cycle];

        vector<pair<int, int>> cycle;
        int curr = node_in_cycle;
        do
        {
            int prev = parent[curr];
            cycle.push_back({prev, edge_to_parent[curr]});
            curr = prev;
        } while (curr != node_in_cycle);

        int push = INF;
        for (auto &p : cycle)
        {
            push = min(push, adj[p.first][p.second].capacity - adj[p.first][p.second].flow);
        }

        for (auto &p : cycle)
        {
            int u = p.first;
            int idx = p.second;
            int v = adj[u][idx].to;
            int rev_idx = adj[u][idx].rev_idx;
            adj[u][idx].flow += push;
            adj[v][rev_idx].flow -= push;
        }
    }

    long long totalCost = 0;
    for (int u = 0; u < vertices; u++)
    {
        for (auto &e : adj[u])
        {
            if (e.flow > 0 && e.cost > 0)
                totalCost += (long long)e.flow * e.cost;
        }
    }
    cout << "-> Faza 2: Optymalizacja zakonczona." << endl;
    cout << "-> Laczny najmniejszy dystans krasnoludkow: " << totalCost << " km" << endl;
}

// ==========================================================
// 7. Budowanie otoczki wypukłej (dla wizualizacji trasy Księcia)
// ==========================================================

void Graph::obliczTraseKsiecia()
{
    vector<Wspolrzedne> punkty;
    int N = krasnoludki.size();

    // 1. Wybieramy tylko kopalnie, ktore faktycznie pracuja (maja flow do ujscia)
    for (int j = 0; j < kopalnie.size(); j++)
    {
        int u = N + j + 1;
        bool uzywana = false;
        for (auto &e : adj[u])
        {
            if (e.to == ujscie && e.flow > 0)
            {
                uzywana = true;
                break;
            }
        }
        if (uzywana)
            punkty.push_back(kopalnie[j].wspolrzedne);
    }

    if (punkty.size() < 2)
    {
        cout << "Problem 2: Trasa patrolowa: 0 km (za malo kopalni)\n";
        return;
    }

    // 2. MAGIA: Wywołujemy nasz nowy moduł!
    vector<Wspolrzedne> otoczka = zbudujOtoczke(punkty);
    double dystans = obliczObwod(otoczka);

    cout << "---------------------------------------------------\n";
    cout << "PROBLEM 2: Trasa patrolowa Ksiecia: " << round(dystans) << " km\n";
    cout << "---------------------------------------------------\n";
}