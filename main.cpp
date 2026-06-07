#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "src/mapa.h"
#include "src/kopalnia.h"
#include "src/krasnoludek.h"
#include "src/ksiegi.h"
#include "src/otoczka.h"
#include "src/dekametrowcy.h"

using namespace std;

string czytajPlik(const string& sciezka) {
    ifstream plik(sciezka);
    if (!plik.is_open()) return "";
    return string((istreambuf_iterator<char>(plik)), istreambuf_iterator<char>());
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        Graph g(1);
        g.init();
        return 0;
    }

    string komenda = argv[1];

    // GŁÓWNY ZAKRES DZIAŁANIA GUI - WSZYSTKO W PAMIĘCI
    if (komenda == "GUI_DATA_DUMP") {
        ElektroniczneKsiegi ekKop;
        string skompKop = ekKop.wczytajArchiwumZDysku("data/kopalnie.bin");
        string csvKop = ekKop.dekompresuj(skompKop);

        ElektroniczneKsiegi ekKras;
        string skompKras = ekKras.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
        string csvKras = ekKras.dekompresuj(skompKras);

        cout << "---KOPALNIE---\n" << csvKop << "\n";
        cout << "---KRASNOLUDKI---\n" << csvKras << "\n";

        // WYŁUSKIWANIE OTOCZKI (Pancerne parsowanie bez znaków specjalnych \r)
        vector<int> aktywneID;
        stringstream ssKras(csvKras);
        string linia; 
        while(getline(ssKras, linia)) {
            if(linia.empty() || linia.find("ID") != string::npos) continue; 
            stringstream ss(linia); string id_str, id_kop_str;
            getline(ss, id_str, ','); getline(ss, id_kop_str, ',');
            try {
                int id_k = stoi(id_kop_str);
                if (id_k > 0) aktywneID.push_back(id_k);
            } catch(...) {}
        }

        vector<Wspolrzedne> punktyDoOtoczki;
        stringstream ssKop(csvKop); 
        while(getline(ssKop, linia)) {
            if(linia.empty() || linia.find("ID") != string::npos) continue;
            stringstream ss(linia); string id_str, x_str, y_str;
            getline(ss, id_str, ','); getline(ss, x_str, ','); getline(ss, y_str, ',');
            try {
                int id_k = stoi(id_str);
                if(find(aktywneID.begin(), aktywneID.end(), id_k) != aktywneID.end()) {
                    punktyDoOtoczki.push_back({stoi(x_str), stoi(y_str)});
                }
            } catch(...) {}
        }

        cout << "---OTOCZKA---\n";
        vector<Wspolrzedne> otoczka;
        if(punktyDoOtoczki.size() >= 2) {
            otoczka = zbudujOtoczke(punktyDoOtoczki);
            for(auto& p : otoczka) cout << p.x << "," << p.y << "\n";
        }

        // SALWA DEKAMETROWCÓW W LOCIE (Gdy GUI przekaże argumenty)
        if (argc >= 5 && string(argv[2]) == "SALWA") {
            try {
                int lewy_indeks = stoi(argv[3]);
                int prawy_indeks = stoi(argv[4]);

                if (otoczka.size() >= 2) {
                    vector<Dekametrowiec> oddzial;
                    int idx = 0;
                    for (const auto& p : otoczka) {
                        int glosnosc = ((p.x * 3 + p.y * 7) % 71) + 30; 
                        oddzial.push_back({1000 + idx, glosnosc});
                        idx++;
                    }

                    int n = oddzial.size();
                    if (lewy_indeks >= n) lewy_indeks = 0;
                    if (prawy_indeks >= n) prawy_indeks = n - 1;

                    DrzewoPrzedzialowe drzewo(oddzial);
                    int dowodcaID = -1;

                    if (lewy_indeks <= prawy_indeks) {
                        dowodcaID = drzewo.zapytajONajglosniejszego(lewy_indeks, prawy_indeks);
                    } else {
                        int id1 = drzewo.zapytajONajglosniejszego(lewy_indeks, n - 1);
                        int id2 = drzewo.zapytajONajglosniejszego(0, prawy_indeks);
                        int g1 = -1, g2 = -1;
                        for (const auto& d : oddzial) {
                            if (d.ID == id1) g1 = d.glosnosc;
                            if (d.ID == id2) g2 = d.glosnosc;
                        }
                        dowodcaID = (g1 > g2) ? id1 : id2;
                    }
                    cout << "---SALWA---\n";
                    cout << lewy_indeks << " " << prawy_indeks << " " << dowodcaID << "\n";
                }
            } catch(...) {}
        }
        return 0;
    }
    // OBSŁUGA BAZY DANYCH
    else if (komenda == "ADD_DWARF" && argc >= 7) {
        ElektroniczneKsiegi ek;
        string skomp = ek.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
        string csv = ek.dekompresuj(skomp);
        if (!csv.empty() && csv.back() != '\n') csv += '\n';
        csv += string(argv[2]) + "," + string(argv[3]) + "," + string(argv[4]) + "," + string(argv[5]) + "," + string(argv[6]) + "\n";
        ek.budujDrzewoHuffmana(csv);
        ek.zapiszArchiwumNaDysk("data/dane_krasnoludkow.bin", ek.kompresuj(csv));
        Graph g(1); g.init();
    }
    else if (komenda == "IMPORT" && argc >= 4) {
        ElektroniczneKsiegi ek;
        string kop_csv = czytajPlik(argv[2]);
        string kras_csv = czytajPlik(argv[3]);
        ek.budujDrzewoHuffmana(kop_csv);
        ek.zapiszArchiwumNaDysk("data/kopalnie.bin", ek.kompresuj(kop_csv));
        ek.budujDrzewoHuffmana(kras_csv);
        ek.zapiszArchiwumNaDysk("data/dane_krasnoludkow.bin", ek.kompresuj(kras_csv));
        Graph g(1); g.init();
    }
    else if (komenda == "EKSPORT" && argc >= 3) {
        ElektroniczneKsiegi ek;
        string skomp = ek.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
        string csv = ek.dekompresuj(skomp);
        ofstream out(argv[2]); out << csv; out.close();
    }
    else {
        Graph g(1); g.init();
    }
    return 0;
}