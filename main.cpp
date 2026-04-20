#include<fstream>
#include<iostream>
#include<string>
#include<vector>

#include "src/kopalnia.h"
#include "src/krasnoludek.h"

int main()
{
    vector<Kopalnia> kopalnie;
    vector<Krasnoludek> krasnoludki;

    ifstream plikKopalnie("kopalnie.csv");
    if(plikKopalnie.is_open()){
        string linia;
        while(getline(plikKopalnie,linia)){
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
            //krasnoludki.push_back(Krasnoludek(ID, ID_kopalni, mineraly, domek));
        }
        plikKrasnoludki.close();
    }else
    {
        cout<<"Nie można otworzyć pliku krasnoludki.txt"<<endl;
    }
}