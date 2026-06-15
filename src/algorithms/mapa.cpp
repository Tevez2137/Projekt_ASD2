#include "mapa.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include "../io/data_loader.h"
#include "../io/data_saver.h"
#include "ksiegi.h"

using namespace std;

void Mapa::init() {
    kopalnie = DataLoader::wczytajKopalnie("data/kopalnie.bin");
    krasnoludki = DataLoader::wczytajKrasnoludki("data/dane_krasnoludkow.bin");
    if (kopalnie.empty() || krasnoludki.empty()) return;

    int N = krasnoludki.size();
    int M = kopalnie.size();
    
    // Graf robi tylko to do czego zostal stworzony!
    Graph g(N + M + 2);
    g.krasnoludki = krasnoludki;
    g.kopalnie = kopalnie;
    g.zrodlo = 0;
    g.ujscie = N + M + 1;

    cout << "Budowanie sieci polaczen z bazy binarnej..." << endl;
    g.buildGraph();
    g.minCostMaxFlow(g.zrodlo, g.ujscie);
    g.przypiszWyniki();

    // Pobranie zaktualizowanych danych z grafu
    this->krasnoludki = g.krasnoludki; 
    
    obliczTraseKsiecia(g);
    obliczKsiegi();
    
    DataSaver::zapiszWyniki("data/dane_krasnoludkow.bin", this->krasnoludki);
}

void Mapa::obliczTraseKsiecia(Graph& g) {
    vector<Wspolrzedne> punkty;
    int N = krasnoludki.size();
    for (size_t j = 0; j < kopalnie.size(); j++) {
        int u = N + j + 1;
        bool uzywana = false;
        for (auto &e : g.adj[u]) {
            if (e.to == g.ujscie && e.flow > 0) { uzywana = true; break; }
        }
        if (uzywana) punkty.push_back(kopalnie[j].wspolrzedne);
    }
    if (punkty.size() < 2) return;

    aktualnaOtoczka = zbudujOtoczke(punkty);
    double dystans = obliczObwod(aktualnaOtoczka);
    cout << "PROBLEM 2: Trasa patrolowa Ksiecia: " << round(dystans) << " km\n";
}

void Mapa::obliczKsiegi() {
    std::ifstream plikAkcji("data/akcja_ksiegi.txt");
    std::string akcja = "";
    if (plikAkcji.is_open()) { plikAkcji >> akcja; plikAkcji.close(); }
    if (akcja.empty()) return;

    std::ifstream plikKsiegi("data/ksiega.txt");
    std::string tekst((std::istreambuf_iterator<char>(plikKsiegi)), std::istreambuf_iterator<char>());
    plikKsiegi.close();

    ElektroniczneKsiegi ek;
    std::ofstream plikWynikow("data/wyniki_ksiegi.txt");

    if (akcja == "KOMPRESJA") {
        ek.budujDrzewoHuffmana(tekst);
        std::string skompresowany = ek.kompresuj(tekst);
        int oryg = tekst.length() * 8;
        double oszczednosc = oryg > 0 ? (1.0 - (double)skompresowany.length() / oryg) * 100.0 : 0.0;
        if (plikWynikow.is_open()) plikWynikow << "KOMPRESJA\n" << oryg << " " << skompresowany.length() << " " << oszczednosc << "\n";
    } 
    else if (akcja == "SZUKAJ") {
        std::string wzorzec = "";
        std::ifstream plikWzorca("data/wzorzec.txt");
        if (plikWzorca.is_open()) { std::getline(plikWzorca, wzorzec); plikWzorca.close(); }
        std::vector<int> poz = ek.szukajRabinKarp(tekst, wzorzec);
        if (plikWynikow.is_open()) {
            plikWynikow << "SZUKAJ\n" << poz.size() << "\n";
            for (int p : poz) plikWynikow << p << " ";
            plikWynikow << "\n";
        }
    }
}