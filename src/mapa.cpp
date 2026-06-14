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
#include <cmath> 
#include <vector>
#include "dekametrowcy.h"
#include "ksiegi.h"

using namespace std;
const int INF = 1e9;

void Graph::init() {
    ElektroniczneKsiegi ek;

    string skompKop = ek.wczytajArchiwumZDysku("data/kopalnie.bin");
    string csvKop = ek.dekompresuj(skompKop);
    if (csvKop.empty()) return;

    stringstream ssKop(csvKop);
    string linia; getline(ssKop, linia);
    while (getline(ssKop, linia)) {
        if (linia.empty()) continue;
        stringstream ss(linia); string t, sur, id; int m; Wspolrzedne w;
        getline(ss, t, ','); id = t;
        getline(ss, t, ','); w.x = stoi(t);
        getline(ss, t, ','); w.y = stoi(t);
        getline(ss, sur, ',');
        getline(ss, t, ','); m = stoi(t);
        this->kopalnie.push_back(Kopalnia(id, w, sur, m));
    }

    string skompKras = ek.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
    string csvKras = ek.dekompresuj(skompKras);
    if (csvKras.empty()) return;

    stringstream ssKras(csvKras);
    getline(ssKras, linia);
    while (getline(ssKras, linia)) {
        if (linia.empty()) continue;
        string id, idKop, t, mAll, m1; Domek d; vector<string> min;
        stringstream ss(linia);
        getline(ss, t, ','); id = t;
        getline(ss, t, ','); idKop = t;
        getline(ss, mAll, ','); stringstream ssMin(mAll);
        while (getline(ssMin, m1, ';')) min.push_back(m1);
        getline(ss, t, ','); d.x = stoi(t);
        getline(ss, t, ','); d.y = stoi(t);
        this->krasnoludki.push_back(Krasnoludek(id, idKop, min, d));
    }

    int N = this->krasnoludki.size();
    int M = this->kopalnie.size();
    this->vertices = N + M + 2; 
    this->adj.resize(this->vertices);
    this->zrodlo = 0;
    this->ujscie = N + M + 1;

    cout << "Budowanie sieci polaczen z bazy binarnej..." << endl;
    this->buildGraph();
    this->minCostMaxFlow(this->zrodlo, this->ujscie);
    this->obliczTraseKsiecia();
    this->obliczKsiegi();
    this->saveResults(""); 
}

void Graph::saveResults(const std::string& /*filename*/) {
    int N = this->krasnoludki.size();
    for (int i = 0; i < N; ++i) this->krasnoludki[i].ID_kopalni = "0"; 

    for (int u = 1; u <= N; u++) {
        for (const auto& e : adj[u]) {
            if (e.flow > 0 && e.to != this->zrodlo) {
                int kopalnia_idx = e.to - N - 1;
                this->krasnoludki[u - 1].ID_kopalni = this->kopalnie[kopalnia_idx].ID;
            }
        }
    }

    std::stringstream ssOut;
    ssOut << "ID,ID_kopalni,Mineraly,X,Y\n";
    for (int i = 0; i < N; ++i) {
        ssOut << this->krasnoludki[i].ID << "," << this->krasnoludki[i].ID_kopalni << ",";
        for (size_t j = 0; j < this->krasnoludki[i].mineraly.size(); ++j) {
            ssOut << this->krasnoludki[i].mineraly[j];
            if (j < this->krasnoludki[i].mineraly.size() - 1) ssOut << ";";
        }
        ssOut << "," << this->krasnoludki[i].domek.x << "," << this->krasnoludki[i].domek.y << "\n";
    }
    
    ElektroniczneKsiegi ek;
    string csvWynik = ssOut.str();
    ek.budujDrzewoHuffmana(csvWynik);
    ek.zapiszArchiwumNaDysk("data/dane_krasnoludkow.bin", ek.kompresuj(csvWynik));
    cout << "Gotowe! Zaktualizowano binarne archiwum krasnoludkow." << endl;
}

Graph::Graph(int v) : vertices(v) { adj.resize(vertices); }

void Graph::addEdge(int u, int v, int cap, int cost) {
    adj[u].push_back({v, cap, 0, cost, (int)adj[v].size()});
    adj[v].push_back({u, 0, 0, -cost, (int)adj[u].size() - 1});
}

void Graph::buildGraph() {
    int N = this->krasnoludki.size();
    int M = this->kopalnie.size();
    for (int i = 0; i < N; ++i) addEdge(this->zrodlo, i + 1, 1, 0);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            std::string surowiec = this->kopalnie[j].surowiec;
            auto it = std::find(this->krasnoludki[i].mineraly.begin(), this->krasnoludki[i].mineraly.end(), surowiec);
            if (it != this->krasnoludki[i].mineraly.end()) {
                double dx = this->krasnoludki[i].domek.x - this->kopalnie[j].wspolrzedne.x;
                double dy = this->krasnoludki[i].domek.y - this->kopalnie[j].wspolrzedne.y;
                int dystans = std::round(std::hypot(dx, dy));
                addEdge(i + 1, N + j + 1, 1, dystans);
            }
        }
    }
    for (int j = 0; j < M; ++j) addEdge(N + j + 1, this->ujscie, this->kopalnie[j].iloscMiejsc, 0);
}

int Graph::findMaxFlow(int s, int t) {
    int flow = 0;
    vector<int> parent(vertices);
    vector<int> edge_from(vertices);

    while (true) {
        fill(parent.begin(), parent.end(), -1);
        queue<int> q; q.push(s); parent[s] = s;

        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (size_t i = 0; i < adj[u].size(); i++) {
                Edge &e = adj[u][i];
                if (parent[e.to] == -1 && e.capacity > e.flow) {
                    parent[e.to] = u; edge_from[e.to] = i; q.push(e.to);
                }
            }
        }
        if (parent[t] == -1) break;

        int push = INF;
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v]; int idx = edge_from[v];
            push = min(push, adj[u][idx].capacity - adj[u][idx].flow);
        }

        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v]; int idx = edge_from[v]; int rev_idx = adj[u][idx].rev_idx;
            adj[u][idx].flow += push; adj[v][rev_idx].flow -= push;
        }
        flow += push;
    }
    return flow;
}



//algorytm min cost max flow 
//rozwiazuje on problem przydzialu krasnoludkow do kopalni w taki sposob, aby maksymalizowac liczbe przydzielonych krasnoludkow,
// a jednoczesnie minimalizowac laczny koszt (dystans) przydzialu.
//Algorytm ten dziala w dwoch fazach: najpierw znajduje 
//maksymalny przeplyw w sieci, a nastepnie szuka cykli ujemnych kosztow, aby zoptymalizowac koszt przydzialu.

void Graph::minCostMaxFlow(int start, int end) {
    int totalFlow = findMaxFlow(start, end);
    cout << "-> Faza 1: Znaleziono maksymalny przeplyw: " << totalFlow << " krasnoludkow." << endl;
    vector<int> dist(vertices), parent(vertices), edge_to_parent(vertices);

    while (true) {
        fill(dist.begin(), dist.end(), 0);
        fill(parent.begin(), parent.end(), -1);
        fill(edge_to_parent.begin(), edge_to_parent.end(), -1);
        int node_in_cycle = -1;

        for (int i = 0; i < vertices; i++) {
            node_in_cycle = -1;
            for (int u = 0; u < vertices; u++) {
                for (size_t j = 0; j < adj[u].size(); j++) {
                    Edge &e = adj[u][j];
                    if (e.capacity > e.flow && dist[e.to] > dist[u] + e.cost) {
                        dist[e.to] = dist[u] + e.cost; parent[e.to] = u;
                        edge_to_parent[e.to] = j; node_in_cycle = e.to;
                    }
                }
            }
        }
        if (node_in_cycle == -1) break;
        for (int i = 0; i < vertices; i++) node_in_cycle = parent[node_in_cycle];

        vector<pair<int, int>> cycle;
        int curr = node_in_cycle;
        do {
            int prev = parent[curr];
            cycle.push_back({prev, edge_to_parent[curr]}); curr = prev;
        } while (curr != node_in_cycle);

        int push = INF;
        for (auto &p : cycle) push = min(push, adj[p.first][p.second].capacity - adj[p.first][p.second].flow);
        for (auto &p : cycle) {
            int u = p.first; int idx = p.second;
            int v = adj[u][idx].to; int rev_idx = adj[u][idx].rev_idx;
            adj[u][idx].flow += push; adj[v][rev_idx].flow -= push;
        }
    }
}

void Graph::obliczTraseKsiecia() {
    vector<Wspolrzedne> punkty;
    int N = krasnoludki.size();

    for (size_t j = 0; j < kopalnie.size(); j++) {
        int u = N + j + 1;
        bool uzywana = false;
        for (auto &e : adj[u]) {
            if (e.to == ujscie && e.flow > 0) { uzywana = true; break; }
        }
        if (uzywana) punkty.push_back(kopalnie[j].wspolrzedne);
    }

    if (punkty.size() < 2) return;

    // ZAPIS W PAMIĘCI RAM
    vector<Wspolrzedne> otoczka = zbudujOtoczke(punkty);
    this->aktualnaOtoczka = otoczka; 
    
    double dystans = obliczObwod(otoczka);
    cout << "PROBLEM 2: Trasa patrolowa Ksiecia: " << round(dystans) << " km\n";
}

void Graph::obliczKsiegi() {
    std::ifstream plikAkcji("data/akcja_ksiegi.txt");
    std::string akcja = "";
    if (plikAkcji.is_open()) { plikAkcji >> akcja; plikAkcji.close(); }
    if (akcja.empty()) return;

    std::ifstream plikKsiegi("data/ksiega.txt");
    std::string tekst((std::istreambuf_iterator<char>(plikKsiegi)), std::istreambuf_iterator<char>());
    plikKsiegi.close();

    ElektroniczneKsiegi ek;
    std::ofstream plikWynikow("data/wyniki_ksiegi.txt");

    if (akcja == "KOMPRESJA") {
        ek.budujDrzewoHuffmana(tekst);
        std::string skompresowany = ek.kompresuj(tekst);
        int oryg = tekst.length() * 8;
        double oszczednosc = oryg > 0 ? (1.0 - (double)skompresowany.length() / oryg) * 100.0 : 0.0;
        if (plikWynikow.is_open()) plikWynikow << "KOMPRESJA\n" << oryg << " " << skompresowany.length() << " " << oszczednosc << "\n";
    } 
    else if (akcja == "SZUKAJ") {
        std::string wzorzec = "";
        std::ifstream plikWzorca("data/wzorzec.txt");
        if (plikWzorca.is_open()) { std::getline(plikWzorca, wzorzec); plikWzorca.close(); }
        std::vector<int> poz = ek.szukajRabinKarp(tekst, wzorzec);
        if (plikWynikow.is_open()) {
            plikWynikow << "SZUKAJ\n" << poz.size() << "\n";
            for (int p : poz) plikWynikow << p << " ";
            plikWynikow << "\n";
        }
    }
}