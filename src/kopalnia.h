// co potrzebuje kopalnia: gdzie jest , co przyjmuje , jaki surowiec , ilu pracownikow
#ifndef KOPALNIA_H
#define KOPALNIA_H

#include <string>

using namespace std;

struct Wspolrzedne
{
    int x;
    int y;
};

class Kopalnia
{
public:
    string ID;
    Wspolrzedne wspolrzedne;
    string surowiec;
    int iloscMiejsc;

    Kopalnia(string ID, Wspolrzedne wspolrzedne, string surowiec, int iloscMiejsc);
};

#endif