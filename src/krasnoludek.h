#ifndef KRASNOLUDEK_H
#define KRASNOLUDEK_H


#include <vector>
#include <string>

using namespace std;
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