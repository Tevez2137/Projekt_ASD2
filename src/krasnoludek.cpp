#include "krasnoludek.h"
#include <iostream>

using namespace std;

Krasnoludek::Krasnoludek(string ID, string ID_kopalni, vector<string> mineraly, Domek domek)
{
        this->ID = ID;
        this->mineraly = mineraly;
        this->domek = domek;
        this->ID_kopalni = ID_kopalni;
}
