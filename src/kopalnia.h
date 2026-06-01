#ifndef KOPALNIA_H
#define KOPALNIA_H

#include <string>
#include <vector>

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