#include "otoczka.h"
#include <algorithm>
#include <cmath>

using namespace std;


// Iloczyn wektorowy. Logika wygląda tak:
// Wynik > 0: skręt w lewo
// Wynik < 0: skręt w prawo
// Wynik == 0: punkty współliniowe

long long skret(Wspolrzedne a, Wspolrzedne b, Wspolrzedne c) {
    long long dx1 = b.x - a.x;
    long long dy1 = b.y - a.y;
    long long dx2 = c.x - a.x;
    long long dy2 = c.y - a.y;
    return (dx1 * dy2) - (dy1 * dx2);
}

//liczy dystans , odwrocony pitagoras z geometrii analitycznej

long long dystansDoKwadratu(Wspolrzedne p1, Wspolrzedne p2) {
    long long dx = p1.x - p2.x;
    long long dy = p1.y - p2.y;
    return (dx * dx) + (dy * dy);
}

//glowny algorytm grahama do budowy otoczki wypukłej

vector<Wspolrzedne> zbudujOtoczke(vector<Wspolrzedne> punkty) {
    // Zabezpieczenie: jeśli mamy 1 lub 2 kopalnie, to nie ma z czego budować wielokąta.
    // Zwracamy je po prostu w niezmienionej formie.
    if (punkty.size() < 3) return punkty; 

    // szukamy punktu startowego (najniższe y, a przy remisach bierzemy skrajny lewy po x)

    int start = 0;
    for (int i = 1; i < (int)punkty.size(); i++) {
        if (punkty[i].y < punkty[start].y || (punkty[i].y == punkty[start].y && punkty[i].x < punkty[start].x))
            start = i;
    }

    swap(punkty[0], punkty[start]);
    Wspolrzedne p0 = punkty[0];

    // sortujemy wszystko poza p0 kątowo względem p0

    sort(punkty.begin() + 1, punkty.end(), [&](Wspolrzedne a, Wspolrzedne b) {
        long long s = skret(p0, a, b);
        if (s == 0) return dystansDoKwadratu(p0, a) < dystansDoKwadratu(p0, b);
        return s > 0;
    });

    // budujemy otoczkę uzywajac stosu 
    vector<Wspolrzedne> otoczka;
    for (int i = 0; i < (int)punkty.size(); i++) {
        // wyrzucamy z naszej otoczki ostatni punkt jeżeli dołożenie obecnego punkty[i] 
        // zmusza nas do skrętu w prawo lub cofnięcia się
        while (otoczka.size() >= 2 && skret(otoczka[otoczka.size()-2], otoczka.back(), punkty[i]) <= 0) {
            otoczka.pop_back();
        }
        otoczka.push_back(punkty[i]);
    }
    return otoczka;
}
// Oblicza całkowitą długość trasy patrolowej do wyświetlenia w interfejsie.
double obliczObwod(const vector<Wspolrzedne>& otoczka) {
    if (otoczka.size() < 2) return 0;
    
    double dystans = 0;
    for (int i = 0; i < (int)otoczka.size(); i++) {
        Wspolrzedne a = otoczka[i];
        //operator modulo zapewnia nam ze po ostatnim punkcie wracamy do pierwszego, zamykając obwód.
        Wspolrzedne b = otoczka[(i + 1) % otoczka.size()];
        // funkcja hypot wewnętrznie liczy sqrt(x^2 + y^2), zapobiegając przepełnieniu przy dużych liczbach.
        dystans += hypot(a.x - b.x, a.y - b.y);
    }
    // Jeśli zredukowało nas do dwóch punktów, to robimy ścieżkę do drugiego punktu i wracamy (x2)
    if (otoczka.size() == 2) dystans *= 2; // Droga tam i z powrotem
    
    return dystans;
}