#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
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
            // Parsujemy krasnoludkow: zbieramy ID, ID_kopalni oraz współrzędne domu
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
                    // Keep only first occurrence of a given dwarf ID (ignore duplicates)
                    if (seenDwarfIDs.insert(id).second) {
                        krasnoludki.push_back({id, id_k, x, y});
                        if (id_k > 0) aktywneID.push_back(id_k);
                    }
                } catch(...) {}
            }

        // Zbieramy punkty kopalni razem z ich oryginalnym ID (tylko te, które mają przydzielonych pracowników)
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

        // SALWA DEKAMETROWCÓW W LOCIE (jesli GUI przekaze argumenty zakresu zapytania z widoku Pythonowego)
        if (argc >= 5 && string(argv[2]) == "SALWA") {
            try {
                int lewy_indeks = stoi(argv[3]);
                int prawy_indeks = stoi(argv[4]);
                int spacing = 100;
                if (argc >= 6) spacing = max(1, stoi(argv[5]));

                vector<Wspolrzedne> straznicyPunkty;
                vector<double> straznicyDist;
                vector<double> vertexDist;
                if (otoczka.size() >= 2) {
                    vector<double> edge_len(otoczka.size());
                    vertexDist.assign(otoczka.size() + 1, 0.0);
                    double perimeter = 0.0;
                    for (size_t i = 0; i < otoczka.size(); ++i) {
                        const auto& a = otoczka[i];
                        const auto& b = otoczka[(i + 1) % otoczka.size()];
                        double dx = double(b.x - a.x);
                        double dy = double(b.y - a.y);
                        edge_len[i] = sqrt(dx * dx + dy * dy);
                        perimeter += edge_len[i];
                        vertexDist[i + 1] = perimeter;
                    }
                    double current = 0.0;
                    while (current < perimeter) {
                        double rem = current;
                        size_t edge = 0;
                        while (edge < edge_len.size() && rem > edge_len[edge]) {
                            rem -= edge_len[edge];
                            edge++;
                        }
                        if (edge >= edge_len.size()) {
                            edge = edge_len.size() - 1;
                            rem = edge_len[edge];
                        }
                        const auto& p1 = otoczka[edge];
                        const auto& p2 = otoczka[(edge + 1) % otoczka.size()];
                        double t = (edge_len[edge] <= 0.0) ? 0.0 : rem / edge_len[edge];
                        int x = int(round(p1.x + t * (p2.x - p1.x)));
                        int y = int(round(p1.y + t * (p2.y - p1.y)));
                        straznicyPunkty.push_back({x, y});
                        straznicyDist.push_back(current);
                        current += spacing;
                    }
                    if (straznicyPunkty.empty() && !otoczka.empty()) {
                        straznicyPunkty.push_back(otoczka[0]);
                        straznicyDist.push_back(0.0);
                    }
                }

                cout << "---STRAZNICY---\n";
                for (int idx = 0; idx < (int)straznicyPunkty.size(); ++idx) {
                    const auto& p = straznicyPunkty[idx];
                    int id = 1000 + idx;
                    int glosnosc = ((p.x * 3 + p.y * 7) % 71) + 30;
                    cout << id << "," << p.x << "," << p.y << "," << glosnosc << "\n";
                }

                if (straznicyPunkty.size() >= 1) {
                    vector<int> wybraneIndeksy;
                    if (lewy_indeks < 0) lewy_indeks = 0;
                    if (prawy_indeks < 0) prawy_indeks = 0;
                    if (lewy_indeks >= (int)otoczka.size()) lewy_indeks = 0;
                    if (prawy_indeks >= (int)otoczka.size()) prawy_indeks = otoczka.size() - 1;
                    double startDist = vertexDist[lewy_indeks];
                    double endDist = vertexDist[prawy_indeks];
                    const double eps = 1e-9;
                    if (lewy_indeks <= prawy_indeks) {
                        for (int i = 0; i < (int)straznicyDist.size(); ++i) {
                            if (straznicyDist[i] + eps >= startDist && straznicyDist[i] <= endDist + eps) {
                                wybraneIndeksy.push_back(i);
                            }
                        }
                    } else {
                        for (int i = 0; i < (int)straznicyDist.size(); ++i) {
                            if (straznicyDist[i] + eps >= startDist || straznicyDist[i] <= endDist + eps) {
                                wybraneIndeksy.push_back(i);
                            }
                        }
                    }

                    int dowodcaID = -1;
                    int maxGlosnosc = -1;
                    for (int idx : wybraneIndeksy) {
                        const auto& p = straznicyPunkty[idx];
                        int id = 1000 + idx;
                        int glosnosc = ((p.x * 3 + p.y * 7) % 71) + 30;
                        if (glosnosc > maxGlosnosc || (glosnosc == maxGlosnosc && (dowodcaID == -1 || id < dowodcaID))) {
                            maxGlosnosc = glosnosc;
                            dowodcaID = id;
                        }
                    }

                    if (dowodcaID != -1) {
                        cout << "---SALWA---\n";
                        cout << lewy_indeks << " " << prawy_indeks << " " << dowodcaID << "\n";

                        int highlightID = -1, hx = -1, hy = -1;
                        int idx_do = dowodcaID - 1000;
                        if (idx_do >= 0 && idx_do < (int)straznicyPunkty.size()) {
                            highlightID = dowodcaID;
                            hx = straznicyPunkty[idx_do].x;
                            hy = straznicyPunkty[idx_do].y;
                        }
                        if (highlightID != -1) {
                            cout << "---HIGHLIGHT---\n";
                            cout << highlightID << "," << hx << "," << hy << ",pink\n";
                        }
                    }
                }
            } catch(...) {}
        }
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