#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
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
// Pomocnicza funkcja zczytujaca do stringa zawartosc pliku (glownie uzywana podczas zrzutow testowych)
string czytajPlik(const string& sciezka) {
    ifstream plik(sciezka);
    if (!plik.is_open()) return "";
    return string((istreambuf_iterator<char>(plik)), istreambuf_iterator<char>());
}

// punkt wejscia aplikacji sluzacy glownie do komunikacji (via argumenty CLI i Standard Output) z interfejsem graficznym w Pythonie
int main(int argc, char* argv[]) {
    // brak argumentow to po prostu glowna egzekucja programu wczytujacego z bazy bin
    if (argc == 1) {
        Graph g(1);
        g.init();
        return 0;
    }

    string komenda = argv[1];

    // zrzucenie calej bazy do formatu CSV na standardowe wyjscie, aby interfejs graficzny (Python) mogl narysowac biezaca mape
    if (komenda == "GUI_DATA_DUMP") {
        ElektroniczneKsiegi ekKop;
        string skompKop = ekKop.wczytajArchiwumZDysku("data/kopalnie.bin");
        string csvKop = ekKop.dekompresuj(skompKop);

        ElektroniczneKsiegi ekKras;
        string skompKras = ekKras.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
        string csvKras = ekKras.dekompresuj(skompKras);
        // specjalne naglowki ulatwiajace pythonowi parsowanie zrzutu w locie
        cout << "---KOPALNIE---\n" << csvKop << "\n";
        cout << "---KRASNOLUDKI---\n" << csvKras << "\n";

        // otoczka patrolowa wysylana do GUI celem poprawnego narysowania fioletowego muru
        struct SmallDwarf { int id; int id_kopalni; int x; int y; };
        vector<int> aktywneID;
        vector<SmallDwarf> krasnoludki;
        unordered_set<int> seenDwarfIDs;
        stringstream ssKras(csvKras);
        string linia;
        while(getline(ssKras, linia)) {
            if(linia.empty() || linia.find("ID") != string::npos) continue;
            stringstream ss(linia);
            string id_str, id_kop_str, mineraly_str, x_str, y_str;
            getline(ss, id_str, ','); getline(ss, id_kop_str, ','); getline(ss, mineraly_str, ',');
            getline(ss, x_str, ','); getline(ss, y_str, ',');
            try {
                int id_k = stoi(id_kop_str);
                int id = stoi(id_str);
                int x = stoi(x_str);
                int y = stoi(y_str);
                if (seenDwarfIDs.insert(id).second) {
                    krasnoludki.push_back({id, id_k, x, y});
                    if (id_k > 0) aktywneID.push_back(id_k);
                }
            } catch(...) {}
        }

        vector<pair<Wspolrzedne,int>> punktyDoOtoczkiWithID;
        stringstream ssKop(csvKop);
        while(getline(ssKop, linia)) {
            if(linia.empty() || linia.find("ID") != string::npos) continue;
            stringstream ss(linia); string id_str, x_str, y_str;
            getline(ss, id_str, ','); getline(ss, x_str, ','); getline(ss, y_str, ',');
            try {
                int id_k = stoi(id_str);
                if(find(aktywneID.begin(), aktywneID.end(), id_k) != aktywneID.end()) {
                    punktyDoOtoczkiWithID.push_back({{stoi(x_str), stoi(y_str)}, id_k});
                }
            } catch(...) {}
        }

        cout << "---OTOCZKA---\n";
        vector<Wspolrzedne> otoczka;
        if(punktyDoOtoczkiWithID.size() >= 2) {
            vector<Wspolrzedne> coords; coords.reserve(punktyDoOtoczkiWithID.size());
            for (auto &pp : punktyDoOtoczkiWithID) coords.push_back(pp.first);
            otoczka = zbudujOtoczke(coords);
            for(auto& p : otoczka) cout << p.x << "," << p.y << "\n";
        }

        if (argc >= 5 && string(argv[2]) == "SALWA") {
            try {
                int lewy_indeks = stoi(argv[3]);
                int prawy_indeks = stoi(argv[4]);

                if (otoczka.size() >= 2) {
                    vector<Dekametrowiec> oddzial;
                    vector<int> chosenPerIdx;
                    int idx = 0;
                    for (const auto& p : otoczka) {
                        int origKopalniaID = -1;
                        for (const auto &pp : punktyDoOtoczkiWithID) {
                            if (pp.first.x == p.x && pp.first.y == p.y) { origKopalniaID = pp.second; break; }
                        }
                        int useID = 1000 + idx;
                        int glosnosc = ((p.x * 3 + p.y * 7) % 71) + 30; 
                        std::vector<int> kandydaci;
                        int bestID = -1;
                        int bestG = -1000000000;
                        if (origKopalniaID != -1) {
                            for (const auto &kd : krasnoludki) {
                                if (kd.id_kopalni > 0 && kd.id_kopalni == origKopalniaID) {
                                    kandydaci.push_back(kd.id);
                                    int g = ((kd.x * 3 + kd.y * 7) % 71) + 30;
                                    if (g > bestG || (g == bestG && (bestID == -1 || kd.id < bestID))) {
                                        bestG = g; bestID = kd.id;
                                    }
                                }
                            }
                            if (bestID != -1) { useID = bestID; glosnosc = bestG; }
                        }
                        chosenPerIdx.push_back(bestID);
                        oddzial.push_back({useID, glosnosc});
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

                    int highlightID = -1, hx = -1, hy = -1;
                    int realCandidate = -1;
                    if (dowodcaID >= 1000) {
                        int idx_do = dowodcaID - 1000;
                        if (idx_do >= 0 && idx_do < (int)chosenPerIdx.size()) realCandidate = chosenPerIdx[idx_do];
                    } else { realCandidate = dowodcaID; }
                    
                    if (realCandidate != -1) {
                        for (const auto &kd : krasnoludki) {
                            if (kd.id == realCandidate) { highlightID = kd.id; hx = kd.x; hy = kd.y; break; }
                        }
                    }
                    if (highlightID != -1) {
                        cout << "---HIGHLIGHT---\n";
                        cout << highlightID << "," << hx << "," << hy << ",pink\n";
                    }
                }
            } catch(...) {}
        }
        return 0;
    }
    // NOWY KOD: Moduł dedykowany księgom, odpalany błyskawicznie bez MCMF
    else if (komenda == "KSIEGI") {
        Graph g(1); 
        g.obliczKsiegi();
        return 0;
    }
    // obsluga bazy danych 
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
    else if (komenda == "EKSPORT" && argc >= 4) {
        ElektroniczneKsiegi ek;
        
        // Eksport Krasnoludków do pierwszego pliku
        string skompKras = ek.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
        string csvKras = ek.dekompresuj(skompKras);
        ofstream outKras(argv[2]); outKras << csvKras; outKras.close();

        // Eksport Kopalni do drugiego pliku
        string skompKop = ek.wczytajArchiwumZDysku("data/kopalnie.bin");
        string csvKop = ek.dekompresuj(skompKop);
        ofstream outKop(argv[3]); outKop << csvKop; outKop.close();
    }
    else {
        Graph g(1); g.init();
    }
    return 0;
}