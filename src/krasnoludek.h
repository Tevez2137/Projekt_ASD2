#ifndef KRASNOLUDEK_H
#define KRASNOLUDEK_H


#include <vector>
#include <string>

using namespace std;

//klasa z krasnoludkami / dane , podstawowe funkcje do wypisywania danych poszczegolnego ludziczka 
//kazdy krasnoludek ma swoj domek ;P 


    struct Domek{
        int x;
        int y;
    };


class Krasnoludek{
    private:
    
    public:
    string ID;
    string ID_kopalni;
    vector<string> mineraly;
    Domek domek;



    Krasnoludek(string ID,string ID_kopalni,vector<string> mineraly,Domek domek);
    
};

#endif