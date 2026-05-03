//twoerzenie danych mapy dla gui  
//w tym miejscu klasa przyjmuje dane kopalni/domkow, ubiera w calosc i pozwala wyswietlic w gui 
#ifndef MAPA_H
#define MAPA_H

#include <vector>
#include <utility> // dla std::pair

// Ulepszona struktura krawędzi
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

    bool spfa(int start, int end, std::vector<int>& dist, std::vector<int>& parent, std::vector<int>& parentEdge, std::vector<bool>& inQueue);

public:
    void init();
    Graph(int v);
    void addEdge(int u, int v, int cap, int cost);
    void minCostMaxFlow(int start, int end);
    
};

#endif