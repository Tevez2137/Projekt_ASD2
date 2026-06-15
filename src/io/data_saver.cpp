#include "data_saver.h"
#include "../algorithms/ksiegi.h"
#include <sstream>
#include <iostream>

using namespace std;

void DataSaver::zapiszWyniki(const string& sciezka, const vector<Krasnoludek>& krasnoludki) {
    stringstream ssOut;
    ssOut << "ID,ID_kopalni,Mineraly,X,Y\n";
    for (const auto& k : krasnoludki) {
        ssOut << k.ID << "," << k.ID_kopalni << ",";
        for (size_t j = 0; j < k.mineraly.size(); ++j) {
            ssOut << k.mineraly[j];
            if (j < k.mineraly.size() - 1) ssOut << ";";
        }
        ssOut << "," << k.domek.x << "," << k.domek.y << "\n";
    }
    
    ElektroniczneKsiegi ek;
    string csvWynik = ssOut.str();
    ek.budujDrzewoHuffmana(csvWynik);
    ek.zapiszArchiwumNaDysk(sciezka, ek.kompresuj(csvWynik));
    cout << "Gotowe! Zaktualizowano binarne archiwum krasnoludkow." << endl;
}