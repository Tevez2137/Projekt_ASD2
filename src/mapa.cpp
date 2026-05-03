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


void init(){

    vector<Kopalnia> kopalnie;
    vector<Krasnoludek> krasnoludki;

    ifstream plikKopalnie("kopalnie.csv");
    if(plikKopalnie.is_open()){
        string linia;
        while(getline(plikKopalnie,linia)){
          if (linia.empty()) continue;

            stringstream ss(linia);
            string temp;
            
            int ID, ID_kopalni;
            vector<string> mineraly;
            Domek domek;

            getline(ss, temp, ','); ID = stoi(temp);
            getline(ss, temp, ','); ID_kopalni = stoi(temp);
            
            string wszystkieMineraly;
            getline(ss, wszystkieMineraly, ',');
            
            stringstream ssMineraly(wszystkieMineraly);
            string m;
            while (getline(ssMineraly, m, ';')) {
                mineraly.push_back(m);
            }

            getline(ss, temp, ','); domek.x = stoi(temp);
            getline(ss, temp, ','); domek.y = stoi(temp);

            krasnoludki.push_back(Krasnoludek(ID, ID_kopalni, mineraly, domek));

            //kopalnie.push_back(Kopalnia(ID, wspolrzedne, surowiec, iloscMiejsc));
        }
        plikKopalnie.close();
    }else
    {
        cout<<"Nie można otworzyć pliku kopalnie.txt"<<endl;
    }   
    ifstream plikKrasnoludki("krasnoludki.csv");
    if(plikKrasnoludki.is_open()){  
        string linia;
        while(getline(plikKrasnoludki,linia)){
        int ID;
          int ID_kopalni;
          vector<string> mineraly;
          Domek domek;
          stringstream ss(linia);
          string temp;  
            getline(ss,temp,',');
            ID = stoi(temp);
            getline(ss,temp,',');
            ID_kopalni = stoi(temp);
            getline(ss,temp,',');
            // 3. Czytamy Minerały (jako jeden string, np. "Rubiny;Zloto")
            string wszystkieMineraly;
            getline(ss, wszystkieMineraly, ',');
    
            // Rozbijamy minerały po średniku
            stringstream ssMineraly(wszystkieMineraly);
            string pojedynczyMineral;
            while (getline(ssMineraly, pojedynczyMineral, ';')) {
                    mineraly.push_back(pojedynczyMineral);
                }

            getline(ss, temp, ',');
            domek.x = std::stoi(temp);

            getline(ss, temp, ',');
            domek.y = stoi(temp);    

          krasnoludki.push_back(Krasnoludek(ID,ID_kopalni,mineraly,domek));

        }
        plikKrasnoludki.close();
    }else
    {
        cout<<"Nie można otworzyć pliku krasnoludki.txt"<<endl;
    }
}



Graph::Graph(int v) : vertices(v) {
    adj.resize(vertices);
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