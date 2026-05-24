#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include <sstream>

#include "src/mapa.h"
#include "src/kopalnia.h"
#include "src/krasnoludek.h"
#include "src/ksiegi.h"

int main(int argc, char* argv[])
{
    if (argc > 1) {
        std::string akcja = argv[1];
        ElektroniczneKsiegi ek;

        if (akcja == "IMPORT" && argc >= 4) {
            std::string kop_path = argv[2];
            std::string kras_path = argv[3];
            
            // Wczytanie z CSV
            std::ifstream fk(kop_path);
            std::string kop_csv((std::istreambuf_iterator<char>(fk)), std::istreambuf_iterator<char>());
            fk.close();

            std::ifstream fd(kras_path);
            std::string kras_csv((std::istreambuf_iterator<char>(fd)), std::istreambuf_iterator<char>());
            fd.close();

            // Kompresja i zapis do .bin
            ek.budujDrzewoHuffmana(kop_csv);
            ek.zapiszArchiwumNaDysk("data/kopalnie.bin", ek.kompresuj(kop_csv));

            ek.budujDrzewoHuffmana(kras_csv);
            ek.zapiszArchiwumNaDysk("data/dane_krasnoludkow.bin", ek.kompresuj(kras_csv));
            return 0;
        } 
        else if (akcja == "EKSPORT" && argc >= 3) {
            std::string docelowa = argv[2];
            
            std::string skomp = ek.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
            if (!skomp.empty()) {
                std::string csv = ek.dekompresuj(skomp);
                std::ofstream out(docelowa);
                out << csv;
                out.close();
            }
            return 0;
        }
        else if (akcja == "GUI_DATA_DUMP") {
            // Python GUI needs data to draw. Decompress both to stdout using separate dictionary contexts.
            ElektroniczneKsiegi ekKop;
            std::string kop_skomp = ekKop.wczytajArchiwumZDysku("data/kopalnie.bin");
            std::string kop_csv = ekKop.dekompresuj(kop_skomp);
            
            ElektroniczneKsiegi ekKras;
            std::string kras_skomp = ekKras.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
            std::string kras_csv = ekKras.dekompresuj(kras_skomp);
            
            std::cout << "---KOPALNIE---\n";
            std::cout << kop_csv << "\n";
            std::cout << "---KRASNOLUDKI---\n";
            std::cout << kras_csv << "\n";
            return 0;
        }
        else if (akcja == "ADD_DWARF" && argc >= 7) {
            std::string skomp = ek.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
            if (!skomp.empty()) {
                std::string csv = ek.dekompresuj(skomp);
                csv += std::string(argv[2]) + "," + argv[3] + "," + argv[4] + "," + argv[5] + "," + argv[6] + "\n";
                ek.budujDrzewoHuffmana(csv);
                ek.zapiszArchiwumNaDysk("data/dane_krasnoludkow.bin", ek.kompresuj(csv));
            }
            return 0;
        }
    }

    // Normalne uruchomienie (MCMF)
    Graph g(1);
    g.init();
    
    return 0;
}