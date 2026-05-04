//co potrzebuje kopalnia: gdzie jest , co przyjmuje , jaki surowiec , ilu pracownikow 
//otoczka wypukla trasy patrolu

#ifndef KOPALNIA_H
#define KOPALNIA_H

#include<string>
#include<vector>

using namespace std;

struct Wspolrzedne{
    int x;
    int y;
};


class Kopalnia
{
public:
    int ID;
    Wspolrzedne wspolrzedne;
    string surowiec;
    int iloscMiejsc;

    Kopalnia(int ID, Wspolrzedne wspolrzedne, string surowiec, int iloscMiejsc);

    int odleglosc(Wspolrzedne p1 , Wspolrzedne p2);
    bool porownaj(Wspolrzedne p1, Wspolrzedne p2);
    vector<Wspolrzedne> algorytmGrahama(vector<Wspolrzedne> punkty);
    int iloczynKwadratowy(Wspolrzedne p1, Wspolrzedne p2, Wspolrzedne p3);
    

};

#endif