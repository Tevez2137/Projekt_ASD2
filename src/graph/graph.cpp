#include "graph.h"
#include <queue>
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;
const int INF = 1e9;

Graph::Graph(int v) : vertices(v) { adj.resize(vertices); }

void Graph::addEdge(int u, int v, int cap, int cost)
{
    adj[u].push_back({v, cap, 0, cost, (int)adj[v].size()});
    adj[v].push_back({u, 0, 0, -cost, (int)adj[u].size() - 1});
}

void Graph::buildGraph()
{
    int N = krasnoludki.size();
    int M = kopalnie.size();
    for (int i = 0; i < N; ++i)
        addEdge(zrodlo, i + 1, 1, 0);
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            string surowiec = kopalnie[j].surowiec;
            auto it = find(krasnoludki[i].mineraly.begin(), krasnoludki[i].mineraly.end(), surowiec);
            if (it != krasnoludki[i].mineraly.end())
            {
                double dx = krasnoludki[i].domek.x - kopalnie[j].wspolrzedne.x;
                double dy = krasnoludki[i].domek.y - kopalnie[j].wspolrzedne.y;
                int dystans = round(hypot(dx, dy));
                addEdge(i + 1, N + j + 1, 1, dystans);
            }
        }
    }
    for (int j = 0; j < M; ++j)
        addEdge(N + j + 1, ujscie, kopalnie[j].iloscMiejsc, 0);
}

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
            for (size_t i = 0; i < adj[u].size(); i++)
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

void Graph::minCostMaxFlow(int start, int end)
{
    int totalFlow = findMaxFlow(start, end);
    cout << "-> Faza 1: Znaleziono maksymalny przeplyw: " << totalFlow << " krasnoludkow." << endl;
    vector<int> dist(vertices), parent(vertices), edge_to_parent(vertices);
    while (true)
    {
        fill(dist.begin(), dist.end(), 0);
        fill(parent.begin(), parent.end(), -1);
        fill(edge_to_parent.begin(), edge_to_parent.end(), -1);
        int node_in_cycle = -1;
        for (int i = 0; i < vertices; i++)
        {
            node_in_cycle = -1;
            for (int u = 0; u < vertices; u++)
            {
                for (size_t j = 0; j < adj[u].size(); j++)
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
            push = min(push, adj[p.first][p.second].capacity - adj[p.first][p.second].flow);
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
}

void Graph::przypiszWyniki()
{
    int N = krasnoludki.size();
    for (int i = 0; i < N; ++i)
        krasnoludki[i].ID_kopalni = "0";
    for (int u = 1; u <= N; u++)
    {
        for (const auto &e : adj[u])
        {
            if (e.flow > 0 && e.to != zrodlo)
            {
                int kopalnia_idx = e.to - N - 1;
                krasnoludki[u - 1].ID_kopalni = kopalnie[kopalnia_idx].ID;
            }
        }
    }
}