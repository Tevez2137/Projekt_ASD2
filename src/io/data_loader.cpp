#include "data_loader.h"
#include "../algorithms/ksiegi.h"
#include <sstream>

using namespace std;

vector<Kopalnia> DataLoader::wczytajKopalnie(const string& sciezka) {
    vector<Kopalnia> kopalnie;
    ElektroniczneKsiegi ek;
    string skompKop = ek.wczytajArchiwumZDysku(sciezka);
    string csvKop = ek.dekompresuj(skompKop);
    if (csvKop.empty()) return kopalnie;

    stringstream ssKop(csvKop);
    string linia; getline(ssKop, linia);
    while (getline(ssKop, linia)) {
        if (linia.empty()) continue;
        stringstream ss(linia); string t, sur, id; int m; Wspolrzedne w;
        getline(ss, t, ','); id = t;
        getline(ss, t, ','); w.x = stoi(t);
        getline(ss, t, ','); w.y = stoi(t);
        getline(ss, sur, ',');
        getline(ss, t, ','); m = stoi(t);
        kopalnie.push_back(Kopalnia(id, w, sur, m));
    }
    return kopalnie;
}

vector<Krasnoludek> DataLoader::wczytajKrasnoludki(const string& sciezka) {
    vector<Krasnoludek> krasnoludki;
    ElektroniczneKsiegi ek;
    string skompKras = ek.wczytajArchiwumZDysku(sciezka);
    string csvKras = ek.dekompresuj(skompKras);
    if (csvKras.empty()) return krasnoludki;

    stringstream ssKras(csvKras);
    string linia; getline(ssKras, linia);
    while (getline(ssKras, linia)) {
        if (linia.empty()) continue;
        string id, idKop, t, mAll, m1; Domek d; vector<string> min;
        stringstream ss(linia);
        getline(ss, t, ','); id = t;
        getline(ss, t, ','); idKop = t;
        getline(ss, mAll, ','); stringstream ssMin(mAll);
        while (getline(ssMin, m1, ';')) min.push_back(m1);
        getline(ss, t, ','); d.x = stoi(t);
        getline(ss, t, ','); d.y = stoi(t);
        krasnoludki.push_back(Krasnoludek(id, idKop, min, d));
    }
    return krasnoludki;
}