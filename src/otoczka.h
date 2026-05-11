#ifndef OTOCZKA_H
#define OTOCZKA_H

#include <vector>
#include "kopalnia.h" // Musimy dołączyć, żeby znać strukturę "Wspolrzedne"

// Funkcje pomocnicze do geometrii
long long skret(Wspolrzedne a, Wspolrzedne b, Wspolrzedne c);
long long dystansDoKwadratu(Wspolrzedne p1, Wspolrzedne p2);

// Główny algorytm Grahama
std::vector<Wspolrzedne> zbudujOtoczke(std::vector<Wspolrzedne> punkty);

// Funkcja licząca obwód wygenerowanej otoczki
double obliczObwod(const std::vector<Wspolrzedne>& otoczka);

#endif