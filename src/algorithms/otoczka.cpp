#include "otoczka.h"
#include <algorithm>
#include <cmath>

using namespace std;


//funkcja obliczajaca iloczyn wektorowy
//pozwala ustalic kierunke skretu dzieki 3 punktom
// > 0 jeśli skręcamy w lewo , to nas interesuje aby stworzyc otoczke
// < 0 jeśli skręcamy w prawo (wklęsłość, punkt 'b' będzie odrzucony)
// == 0 jeśli punkty leżą idealnie na jednej prostej (współliniowe)

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

    // Szukamy punktu startowego (najnizszy y , jesli beda takie same to uzywamy X najnizszego)

    int start = 0;
    for (int i = 1; i < (int)punkty.size(); i++) {
        if (punkty[i].y < punkty[start].y || (punkty[i].y == punkty[start].y && punkty[i].x < punkty[start].x))
            start = i;
    }
    // Przenosimy nasz punkt startowy na sam początek tablicy (indeks 0), żeby ułatwić sortowanie.
    swap(punkty[0], punkty[start]);
    Wspolrzedne p0 = punkty[0];

    // Sortujemy resztę kątowo wzgledem punktu startowego (p0) i w przypadku współliniowości, sortujemy według odległości od p0.

    sort(punkty.begin() + 1, punkty.end(), [&](Wspolrzedne a, Wspolrzedne b) {
        long long s = skret(p0, a, b);
        // Jeśli dwie kopalnie leżą w tej samej linii od punktu startowego, 
        // najpierw na stos trafi ta, która jest bliżej.
        if (s == 0) return dystansDoKwadratu(p0, a) < dystansDoKwadratu(p0, b);
        return s > 0;
    });

    // Budujemy otoczkę uzywajac stosu 
    vector<Wspolrzedne> otoczka;
    for (int i = 0; i < (int)punkty.size(); i++) {
        // najwazniejsza petla ktora radzi sobie z wklęsłościami i współliniowością
        // Patrzymy na dwa ostatnie punkty na stosie i aktualnie badany punkt.
        // Jeśli NIE tworzą one skrętu w lewo (czyli s <= 0, np. linia wchodzi do wewnątrz figury), 
        // to usuwamy ostatni punkt ze stosu  
        // Robimy to w pętli tak długo, aż znów zaczniemy budować zewnętrzny obwód.
        // punkty ktore zostana na stosie to te ktore tworza otoczke wypukla
        while (otoczka.size() >= 2 && skret(otoczka[otoczka.size()-2], otoczka.back(), punkty[i]) <= 0) {
            otoczka.pop_back();
        }
        // Gdy upewnimy się, że kierunek jest poprawny (w lewo), odkładamy punkt na stos.
        otoczka.push_back(punkty[i]);
    }
    // Zwracamy listę zredukowaną tylko do kopalni leżących na zewnętrznych krawędziach.
    return otoczka;
}
// Oblicza całkowitą długość trasy patrolowej (obwód) do wyświetlenia w interfejsie.
double obliczObwod(const vector<Wspolrzedne>& otoczka) {
    if (otoczka.size() < 2) return 0;
    
    double dystans = 0;
    // Iterujemy po wszystkich kopalniach z wyznaczonej wcześniej otoczki.
    for (int i = 0; i < (int)otoczka.size(); i++) {
        Wspolrzedne a = otoczka[i];
        //operator modulo zapewnia nam ze po ostatnim punkcie wracamy do pierwszego, zamykając obwód.
        Wspolrzedne b = otoczka[(i + 1) % otoczka.size()];
        // Funkcja hypot wewnętrznie liczy sqrt(x^2 + y^2), zapobiegając przepełnieniu (overflow) przy dużych liczbach.
        dystans += hypot(a.x - b.x, a.y - b.y);
    }
    // Zabezpieczenie na wypadek tzw. zdegenerowanego wielokąta (zostały tylko 2 kopalnie).
    // Dwa punkty tworzą odcinek, a nie wielokąt zamknięty. Patrol musi pójść w jedną stronę i wrócić,
    // więc ręcznie mnożymy dystans x2.
    if (otoczka.size() == 2) dystans *= 2; // Droga tam i z powrotem
    
    return dystans;
}