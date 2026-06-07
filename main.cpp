#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "src/mapa.h"
#include "src/kopalnia.h"
#include "src/krasnoludek.h"
#include "src/ksiegi.h"

using namespace std;

string czytajPlik(const string& sciezka) {
    ifstream plik(sciezka);
    if (!plik.is_open()) return "";
    return string((istreambuf_iterator<char>(plik)), istreambuf_iterator<char>());
}

int main(int argc, char* argv[]) {
    // Odpalenie bez argumentów (np. z make run)
    if (argc == 1) {
        Graph g(1);
        g.init();
        return 0;
    }

    string komenda = argv[1];
    ElektroniczneKsiegi ek;

    // 1. Zrzut danych dla GUI (Z BIN do stringa w pamięci RAM, bez plików tymczasowych)
    // 1. Zrzut danych dla GUI (Z BIN do stringa w pamięci RAM, bez plików tymczasowych)
    if (komenda == "GUI_DATA_DUMP") {
        // Obiekt tylko dla kopalni
        ElektroniczneKsiegi ekKop;
        string skompKop = ekKop.wczytajArchiwumZDysku("data/kopalnie.bin");
        cout << "---KOPALNIE---\n";
        cout << ekKop.dekompresuj(skompKop) << "\n";

        // Obiekt tylko dla krasnoludków (żeby nie nadpisać drzewa Huffmana!)
        ElektroniczneKsiegi ekKras;
        string skompKras = ekKras.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
        cout << "---KRASNOLUDKI---\n";
        cout << ekKras.dekompresuj(skompKras) << "\n";
    }
    // 2. Dodawanie nowego pracownika bezpośrednio do bazy BIN
    else if (komenda == "ADD_DWARF" && argc >= 7) {
        string skomp = ek.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
        string csv = ek.dekompresuj(skomp);
        
        // Zabezpieczenie przed brakiem nowej linii
        if (!csv.empty() && csv.back() != '\n') csv += '\n';
        csv += string(argv[2]) + "," + string(argv[3]) + "," + string(argv[4]) + "," + string(argv[5]) + "," + string(argv[6]) + "\n";
        
        ek.budujDrzewoHuffmana(csv);
        ek.zapiszArchiwumNaDysk("data/dane_krasnoludkow.bin", ek.kompresuj(csv));
        
        // Przeliczamy graf na nowej bazie
        Graph g(1);
        g.init();
    }
    // 3. IMPORT: Zamienia wejściowe pliki CSV od użytkownika na naszą bazę BIN
    else if (komenda == "IMPORT" && argc >= 4) {
        string kop_csv = czytajPlik(argv[2]);
        string kras_csv = czytajPlik(argv[3]);

        ek.budujDrzewoHuffmana(kop_csv);
        ek.zapiszArchiwumNaDysk("data/kopalnie.bin", ek.kompresuj(kop_csv));

        ek.budujDrzewoHuffmana(kras_csv);
        ek.zapiszArchiwumNaDysk("data/dane_krasnoludkow.bin", ek.kompresuj(kras_csv));

        Graph g(1);
        g.init();
    }
    // 4. EKSPORT: Wypakowuje naszą bazę BIN z powrotem do czytelnego CSV
    else if (komenda == "EKSPORT" && argc >= 3) {
        string skomp = ek.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
        string csv = ek.dekompresuj(skomp);
        ofstream out(argv[2]);
        out << csv;
        out.close();
    }
    else {
        Graph g(1);
        g.init();
    }

    return 0;
}