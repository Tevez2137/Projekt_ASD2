#ifndef MAPA_H
#define MAPA_H

#include <vector>
#include <string>
#include "kopalnia.h"
#include "krasnoludek.h"
#include "otoczka.h"

struct Edge {
    int to;
    int capacity;
    int flow;
    int cost;
    int rev_idx;
};

class Graph {
public:
    int vertices;
    int zrodlo;
    int ujscie;
    std::vector<std::vector<Edge>> adj;
    std::vector<Krasnoludek> krasnoludki;
    std::vector<Kopalnia> kopalnie;
    
    // Zmienna przechowująca otoczkę w pamięci RAM!
    std::vector<Wspolrzedne> aktualnaOtoczka;

    Graph(int v);
    void init();
    void buildGraph();
    void addEdge(int u, int v, int cap, int cost);
    int findMaxFlow(int s, int t);
    void minCostMaxFlow(int start, int end);
    void obliczTraseKsiecia();
    void obliczKsiegi();
    void saveResults(const std::string& filename);
};

#endif