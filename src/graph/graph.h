#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include "../models/kopalnia.h"
#include "../models/krasnoludek.h"

struct Edge {
    int to;
    int capacity;
    int flow;
    int cost; //odległość z domku do kopalni
    int rev_idx; // indeks krawędzi powrotnej
};

class Graph {
public:
    int vertices;
    int zrodlo;
    int ujscie;
    std::vector<std::vector<Edge>> adj;
    std::vector<Krasnoludek> krasnoludki;
    std::vector<Kopalnia> kopalnie;

    Graph(int v);
    void buildGraph();
    void addEdge(int u, int v, int cap, int cost);

    // Edmonds-Karp - przypisuje max krasnali, ignorując dystanse
    int findMaxFlow(int s, int t);

    // Szuka ujemnych cykli żeby zoptymalizować im drogę do pracy
    void minCostMaxFlow(int start, int end);
    void przypiszWyniki();
};

#endif