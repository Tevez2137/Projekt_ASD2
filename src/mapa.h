#ifndef MAPA_H
#define MAPA_H

#include <vector>
#include <utility> // dla std::pair
#include <string>

// Koniecznie dołączamy nagłówki naszych obiektów!
#include "kopalnia.h"
#include "krasnoludek.h"

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

    // Prywatna funkcja pomocnicza do szukania najtańszej ścieżki
    bool spfa(int start, int end, std::vector<int>& dist, std::vector<int>& parent, std::vector<int>& parentEdge, std::vector<bool>& inQueue);

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

    // Funkcje do zarządzania rurociągiem danych
    void init();                                     // Główny silnik (odpala resztę)
    void buildGraph();                               // Buduje krawędzie i filtruje preferencje
    void saveResults(const std::string& filename);   // Zapisuje wyniki do pliku txt
};

#endif