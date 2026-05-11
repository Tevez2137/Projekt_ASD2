#include "otoczka.h"
#include <algorithm>
#include <cmath>

using namespace std;

long long skret(Wspolrzedne a, Wspolrzedne b, Wspolrzedne c) {
    long long dx1 = b.x - a.x;
    long long dy1 = b.y - a.y;
    long long dx2 = c.x - a.x;
    long long dy2 = c.y - a.y;
    return (dx1 * dy2) - (dy1 * dx2);
}

long long dystansDoKwadratu(Wspolrzedne p1, Wspolrzedne p2) {
    long long dx = p1.x - p2.x;
    long long dy = p1.y - p2.y;
    return (dx * dx) + (dy * dy);
}

vector<Wspolrzedne> zbudujOtoczke(vector<Wspolrzedne> punkty) {
    if (punkty.size() < 3) return punkty; // Jeśli mamy 1-2 punkty, otoczka to po prostu one

    // Szukamy punktu startowego (najniższy Y, a przy remisach najniższy X)
    int start = 0;
    for (int i = 1; i < (int)punkty.size(); i++) {
        if (punkty[i].y < punkty[start].y || (punkty[i].y == punkty[start].y && punkty[i].x < punkty[start].x))
            start = i;
    }
    swap(punkty[0], punkty[start]);
    Wspolrzedne p0 = punkty[0];

    // Sortujemy resztę kątowo
    sort(punkty.begin() + 1, punkty.end(), [&](Wspolrzedne a, Wspolrzedne b) {
        long long s = skret(p0, a, b);
        if (s == 0) return dystansDoKwadratu(p0, a) < dystansDoKwadratu(p0, b);
        return s > 0;
    });

    // Budujemy otoczkę
    vector<Wspolrzedne> otoczka;
    for (int i = 0; i < (int)punkty.size(); i++) {
        while (otoczka.size() >= 2 && skret(otoczka[otoczka.size()-2], otoczka.back(), punkty[i]) <= 0) {
            otoczka.pop_back();
        }
        otoczka.push_back(punkty[i]);
    }

    return otoczka;
}

double obliczObwod(const vector<Wspolrzedne>& otoczka) {
    if (otoczka.size() < 2) return 0;
    
    double dystans = 0;
    for (int i = 0; i < (int)otoczka.size(); i++) {
        Wspolrzedne a = otoczka[i];
        Wspolrzedne b = otoczka[(i + 1) % otoczka.size()];
        dystans += hypot(a.x - b.x, a.y - b.y);
    }
    
    if (otoczka.size() == 2) dystans *= 2; // Droga tam i z powrotem
    
    return dystans;
}