#ifndef MAPA_H
#define MAPA_H

#include <vector>
#include <utility>
#include <string>

#include "kopalnia.h"
#include "krasnoludek.h"
#include "ksiegi.h"

// Struktura krawędzi
struct Edge {
    int to;
    int capacity;
    int flow;
    int cost;
    int rev_idx;
};

class Graph {
private:
    int vertices;
    std::vector<std::vector<Edge>> adj;

public:
    // Wektory przechowujące wczytane dane
    std::vector<Kopalnia> kopalnie;
    std::vector<Krasnoludek> krasnoludki;

    int zrodlo;
    int ujscie;

    // Podstawowe funkcje grafowe
    Graph(int v);
    void addEdge(int u, int v, int cap, int cost);
    void minCostMaxFlow(int start, int end);
    int findMaxFlow(int s, int t);
    // Funkcje do zarządzania rurociągiem danych
    void init();                                     // Główny silnik (odpala resztę)
    void buildGraph();                               // Buduje krawędzie i filtruje preferencje
    void saveResults(const std::string& filename);   // Zapisuje wyniki do pliku txt
    
    void obliczTraseKsiecia();
    void obliczSalwe();
    void obliczKsiegi();
};

#endif