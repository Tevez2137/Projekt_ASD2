#include "mapa.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include <fstream>

using namespace std;

const int INF = 1e9;

Graph::Graph(int v) : vertices(v) {
    adj.resize(vertices);
    ifstream dane("../dane/mapa.csv");
    if (dane.is_open()) {
        // Odczyt danych z pliku CSV
        // Przykład formatu: u,v,cap,cost
        int u, v, cap, cost;
        while (dane >> u >> v >> cap >> cost) {
            addEdge(u, v, cap, cost);
        }
        dane.close();
    } else {
        cerr << "Nie można otworzyć pliku mapa.csv" << endl;
    }
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

            // Jeśli przepychamy flow z Krasnoludka do Kopalni
            if (p != start && curr != end && pushFlow > 0) {
                // zapis do pliku z celu wyświetlenia w GUI
            }

            curr = p;
        }

        totalFlow += pushFlow;
        totalCost += pushFlow * dist[end];
    }

    // zapis do pliku z celu wyświetlenia w GUI {totalFlow, totalCost};
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

        for (int i = 0; i < adj[u].size(); i++) {
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