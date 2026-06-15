#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include "../models/kopalnia.h"
#include "../models/krasnoludek.h"

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

    Graph(int v);
    void buildGraph();
    void addEdge(int u, int v, int cap, int cost);
    int findMaxFlow(int s, int t);
    void minCostMaxFlow(int start, int end);
    void przypiszWyniki(); // Tylko przetwarza przepływy na ID kopalni
};

#endif