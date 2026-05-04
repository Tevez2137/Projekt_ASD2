#include "mapa.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include "krasnoludek.h"
#include "kopalnia.h"
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

const int INF = 1e9;

void Graph::init()
{

    vector<Kopalnia> kopalnie;
    vector<Krasnoludek> krasnoludki;

    ifstream plikKopalnie("./data/kopalnie.csv");
    if (plikKopalnie.is_open())
    {
        string linia;
        getline(plikKopalnie, linia); // pomijamy nagłówek
        while (getline(plikKopalnie, linia))
        {
            if (linia.empty())
                continue;

            stringstream ss(linia);
            string temp, mineral;
            Wspolrzedne wspolrzedne;

            string ID, ID_kopalni;

            getline(ss, ID, ',');

            getline(ss, temp, ',');
            wspolrzedne.x = stoi(temp);
            getline(ss, temp, ',');
            wspolrzedne.y = stoi(temp);

            getline(ss, mineral, ',');

            getline(ss, temp, ',');
            int iloscMiejsc = stoi(temp);

            kopalnie.push_back(Kopalnia(ID, wspolrzedne, mineral, iloscMiejsc));
        }
        plikKopalnie.close();
    }
    else
    {
        cout << "Nie można otworzyć pliku kopalnie.csv" << endl;
    }
    ifstream plikKrasnoludki("./data/dane_krasnoludkow.csv");
    if (plikKrasnoludki.is_open())
    {
        string linia;
        getline(plikKrasnoludki, linia); // pomijamy nagłówek
        while (getline(plikKrasnoludki, linia))
        {
            string ID;
            string ID_kopalni;
            vector<string> mineraly;
            Domek domek;
            stringstream ss(linia);
            string temp;
            getline(ss, temp, ',');
            ID = temp;
            getline(ss, temp, ',');
            ID_kopalni = temp;
            getline(ss, temp, ',');
            // 3. Czytamy Minerały (jako jeden string, np. "Rubiny;Zloto")
            string wszystkieMineraly;
            getline(ss, wszystkieMineraly, ',');

            // Rozbijamy minerały po średniku
            stringstream ssMineraly(wszystkieMineraly);
            string pojedynczyMineral;
            while (getline(ssMineraly, pojedynczyMineral, ';'))
            {
                mineraly.push_back(pojedynczyMineral);
            }

            getline(ss, temp, ',');
            domek.x = stoi(temp);

            getline(ss, temp, ',');
            domek.y = stoi(temp);

            krasnoludki.push_back(Krasnoludek(ID, ID_kopalni, mineraly, domek));
        }
        plikKrasnoludki.close();
    }
    else
    {
        cout << "Nie można otworzyć pliku krasnoludki.txt" << endl;
    }

    Graph g(kopalnie.size() + krasnoludki.size() + 2); // +2 dla źródła i ujścia
    int source = 0;
    int sink = g.vertices - 1;
    for (size_t i = 0; i < krasnoludki.size(); i++)
    {
        g.addEdge(source, 1 + i, 1, 0); // krawędź ze źródła do każdego krasnoludka
    }
    for (size_t i = 0; i < kopalnie.size(); i++)
    {
        g.addEdge(1 + krasnoludki.size() + i, sink, kopalnie[i].iloscMiejsc, 0); // krawędź z każdej kopalni do ujścia
    }
    

}

Graph::Graph(int v) : vertices(v)
{
    adj.resize(vertices);
}

void Graph::addEdge(int u, int v, int cap, int cost)
{
    adj[u].push_back({v, cap, 0, cost, (int)adj[v].size()});
    adj[v].push_back({u, 0, 0, -cost, (int)adj[u].size() - 1});
}

void Graph::minCostMaxFlow(int start, int end)
{
    int totalFlow = 0;
    int totalCost = 0;

    vector<int> dist(vertices);
    vector<int> parent(vertices);
    vector<int> parentEdge(vertices);
    vector<bool> inQueue(vertices);

    while (spfa(start, end, dist, parent, parentEdge, inQueue))
    {
        int pushFlow = INF;

        int curr = end;
        while (curr != start)
        {
            int p = parent[curr];
            int e_idx = parentEdge[curr];
            pushFlow = min(pushFlow, adj[p][e_idx].capacity - adj[p][e_idx].flow);
            curr = p;
        }

        curr = end;
        while (curr != start)
        {
            int p = parent[curr];
            int e_idx = parentEdge[curr];
            int rev_idx = adj[p][e_idx].rev_idx;

            adj[p][e_idx].flow += pushFlow;
            adj[curr][rev_idx].flow -= pushFlow;

            // Jeśli przepychamy flow z Krasnoludka do Kopalni
            if (p != start && curr != end && pushFlow > 0)
            {
                // zapis do pliku z celu wyświetlenia w GUI
            }

            curr = p;
        }

        totalFlow += pushFlow;
        totalCost += pushFlow * dist[end];
    }

    // zapis do pliku z celu wyświetlenia w GUI {totalFlow, totalCost};
}

bool Graph::spfa(int start, int end, vector<int> &dist, vector<int> &parent, vector<int> &parentEdge, vector<bool> &inQueue)
{
    fill(dist.begin(), dist.end(), INF);
    fill(parent.begin(), parent.end(), -1);
    fill(inQueue.begin(), inQueue.end(), false);

    queue<int> q;
    dist[start] = 0;
    q.push(start);
    inQueue[start] = true;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        inQueue[u] = false;

        for (int i = 0; i < (int)adj[u].size(); i++)
        {
            Edge &e = adj[u][i];
            if (e.capacity - e.flow > 0 && dist[u] + e.cost < dist[e.to])
            {
                dist[e.to] = dist[u] + e.cost;
                parent[e.to] = u;
                parentEdge[e.to] = i;

                if (!inQueue[e.to])
                {
                    q.push(e.to);
                    inQueue[e.to] = true;
                }
            }
        }
    }
    return dist[end] != INF;
}