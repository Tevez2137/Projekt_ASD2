#include "mapa.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include "krasnoludek.h"
#include "kopalnia.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cmath> // Wymagane dla std::hypot i std::round

using namespace std;

const int INF = 1e9;

// ==========================================
// 1. GŁÓWNY SILNIK (WCZYTYWANIE + ODPALANIE)
// ==========================================
void Graph::init() {
    // A) Odczyt kopalni
    ifstream plikKopalnie("data/kopalnie.csv");
    if (plikKopalnie.is_open()) {
        string linia;
        getline(plikKopalnie, linia); // Pomijamy nagłówek
        
        while (getline(plikKopalnie, linia)) {
            if (linia.empty()) continue;
            stringstream ss(linia);
            string temp;

            int  iloscMiejsc;
            Wspolrzedne wspolrzedne;
            string surowiec,ID;

            getline(ss, temp, ','); ID = temp;
            getline(ss, temp, ','); wspolrzedne.x = stoi(temp); 
            getline(ss, temp, ','); wspolrzedne.y = stoi(temp);
            getline(ss, surowiec, ',');
            getline(ss, temp, ','); iloscMiejsc = stoi(temp);

            this->kopalnie.push_back(Kopalnia(ID, wspolrzedne, surowiec, iloscMiejsc));
        }
        plikKopalnie.close();
    } else {
        cout << "Nie mozna otworzyc pliku data/kopalnie.csv!" << endl;
    }

    // B) Odczyt krasnoludków
    ifstream plikKrasnoludki("data/dane_krasnoludkow.csv");
    if (plikKrasnoludki.is_open()) {
        string linia;
        getline(plikKrasnoludki, linia); // Pomijamy nagłówek
        
        while (getline(plikKrasnoludki, linia)) {
            if (linia.empty()) continue;
            
            string ID,ID_kopalni;
            vector<string> mineraly;
            Domek domek;
            stringstream ss(linia);
            string temp;

            getline(ss, temp, ','); ID = temp;
            getline(ss, temp, ','); ID_kopalni = temp;

            // Czytamy Minerały i rozbijamy po średniku
            string wszystkieMineraly;
            getline(ss, wszystkieMineraly, ',');
            stringstream ssMineraly(wszystkieMineraly);
            string pojedynczyMineral;
            while (getline(ssMineraly, pojedynczyMineral, ';')) {
                mineraly.push_back(pojedynczyMineral);
            }

            // Czytamy współrzędne domku
            getline(ss, temp, ','); domek.x = stoi(temp);
            getline(ss, temp, ','); domek.y = stoi(temp);

            this->krasnoludki.push_back(Krasnoludek(ID, ID_kopalni, mineraly, domek));
        }
        plikKrasnoludki.close();
    } else {
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
    
    cout << "Obliczanie optymalnego przydzialu (MCMF)..." << endl;
    this->minCostMaxFlow(this->zrodlo, this->ujscie); 
    
    cout << "Zapisywanie wynikow dla Pythona..." << endl;
    this->saveResults("data/przydzialy.txt"); 
}

// ==========================================
// 2. BUDOWANIE INTELIGENTNEGO GRAFU
// ==========================================
void Graph::buildGraph() {
    int N = this->krasnoludki.size();
    int M = this->kopalnie.size();

    // A) Źródło -> Krasnoludki (pojemność 1, koszt 0)
    for (int i = 0; i < N; ++i) {
        addEdge(this->zrodlo, i + 1, 1, 0); 
    }

    // B) Krasnoludki -> Kopalnie (FILTROWANIE SUROWCÓW + KOSZT JAKO DYSTANS)
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            std::string surowiecKopalni = this->kopalnie[j].surowiec;
            
            // Sprawdzamy czy krasnoludek lubi ten surowiec
            auto it = std::find(
                this->krasnoludki[i].mineraly.begin(), 
                this->krasnoludki[i].mineraly.end(), 
                surowiecKopalni
            );

            if (it != this->krasnoludki[i].mineraly.end()) {
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
    for (int j = 0; j < M; ++j) {
        addEdge(N + j + 1, this->ujscie, this->kopalnie[j].iloscMiejsc, 0);
    }
}

// ==========================================
// 3. GENEROWANIE WYNIKÓW DLA GUI
// ==========================================
void Graph::saveResults(const std::string& filename) {
    std::ofstream plik(filename);
    if (!plik.is_open()) {
        cout << "Blad przy tworzeniu pliku: " << filename << endl;
        return;
    }

    int N = this->krasnoludki.size();
    
    // Szukamy przepływu po krasnoludkach (wierzchołki od 1 do N)
    for (int u = 1; u <= N; u++) {
        for (const auto& e : adj[u]) {
            // Jeśli poszedł flow i krawędź nie wraca do źródła
            if (e.flow > 0 && e.to != this->zrodlo) {
                
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
Graph::Graph(int v) : vertices(v) {
    adj.resize(vertices);
}

void Graph::addEdge(int u, int v, int cap, int cost) {
    adj[u].push_back({v, cap, 0, cost, (int)adj[v].size()});
    adj[v].push_back({u, 0, 0, -cost, (int)adj[u].size() - 1});
}

void Graph::minCostMaxFlow(int start, int end) {
    int totalFlow = 0;
    int totalCost = 0;
    vector<int> dist(vertices);
    vector<int> parent(vertices);
    vector<int> parentEdge(vertices);
    vector<bool> inQueue(vertices);

    while (spfa(start, end, dist, parent, parentEdge, inQueue)) {
        int pushFlow = INF;
        int curr = end;
        while (curr != start) {
            int p = parent[curr];
            int e_idx = parentEdge[curr];
            pushFlow = min(pushFlow, adj[p][e_idx].capacity - adj[p][e_idx].flow);
            curr = p;
        }
        curr = end;
        while (curr != start) {
            int p = parent[curr];
            int e_idx = parentEdge[curr];
            int rev_idx = adj[p][e_idx].rev_idx;
            adj[p][e_idx].flow += pushFlow;
            adj[curr][rev_idx].flow -= pushFlow;
            curr = p;
        }
        totalFlow += pushFlow;
        totalCost += pushFlow * dist[end];
    }
    
    cout << "-> Przeslano " << totalFlow << " krasnoludkow." << endl;
    cout << "-> Laczny koszt podrozy (kilometry): " << totalCost << endl;
}

bool Graph::spfa(int start, int end, vector<int>& dist, vector<int>& parent, vector<int>& parentEdge, vector<bool>& inQueue) {
    fill(dist.begin(), dist.end(), INF);
    fill(parent.begin(), parent.end(), -1);
    fill(inQueue.begin(), inQueue.end(), false);

    queue<int> q;
    dist[start] = 0;
    q.push(start);
    inQueue[start] = true;

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        inQueue[u] = false;

        for (int i = 0; i < (int)adj[u].size(); i++) {
            Edge& e = adj[u][i];
            if (e.capacity - e.flow > 0 && dist[u] + e.cost < dist[e.to]) {
                dist[e.to] = dist[u] + e.cost;
                parent[e.to] = u;
                parentEdge[e.to] = i;
                if (!inQueue[e.to]) {
                    q.push(e.to);
                    inQueue[e.to] = true;
                }
            }
        }
    }
    return dist[end] != INF;
}