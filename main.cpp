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

// Funkcja pomocnicza do odczytu całego pliku tekstowego
string czytajPlik(const string &sciezka)
{
    ifstream plik(sciezka);
    if (!plik.is_open())
        return "";
    return string((istreambuf_iterator<char>(plik)), istreambuf_iterator<char>());
}

int main(int argc, char *argv[])
{
    // 1. Zwykłe uruchomienie (bez argumentów) np. z przycisku "Uruchom MCMF"
    if (argc == 1)
    {
        Graph g(1);
        g.init();
        return 0;
    }

    // Odczytujemy komendę wysłaną z Pythona
    string komenda = argv[1];
    ElektroniczneKsiegi ek;

    // 2. Obsługa IMPORTU z GUI (odczytuje CSV, psuje je na 0 i 1, i ładuje do pamięci BIN)
    if (komenda == "IMPORT" && argc >= 4)
    {
        string kop_csv = czytajPlik(argv[2]);
        string kras_csv = czytajPlik(argv[3]);

        ek.budujDrzewoHuffmana(kop_csv);
        ek.zapiszArchiwumNaDysk("data/kopalnie.bin", ek.kompresuj(kop_csv));

        ek.budujDrzewoHuffmana(kras_csv);
        ek.zapiszArchiwumNaDysk("data/dane_krasnoludkow.bin", ek.kompresuj(kras_csv));

        // Po pomyślnym załadowaniu do pamięci RAM/BIN, uruchamiamy silnik
        Graph g(1);
        g.init();
    }
    // 3. Obsługa EKSPORTU (odzyskuje CSV z zer i jedynek z pliku BIN)
    else if (komenda == "EKSPORT" && argc >= 3)
    {
        string docelowa = argv[2];
        string skomp = ek.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
        string csv = ek.dekompresuj(skomp);

        ofstream out(docelowa);
        out << csv;
        out.close();
    }
    // 4. Obsługa DODAWANIA DOMKU z mapy w GUI
    else if (komenda == "ADD_DWARF" && argc >= 7)
    {
        // Dekompresujemy bazę
        string skomp = ek.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");
        string csv = ek.dekompresuj(skomp);

        // Doklejamy w pamięci nowego krasnala z GUI
        csv += string(argv[2]) + "," + string(argv[3]) + "," + string(argv[4]) + "," + string(argv[5]) + "," + string(argv[6]) + "\n";

        // Kompresujemy bazę z powrotem i zapisujemy
        ek.budujDrzewoHuffmana(csv);
        ek.zapiszArchiwumNaDysk("data/dane_krasnoludkow.bin", ek.kompresuj(csv));

        // Przeliczamy grafy
        Graph g(1);
        g.init();
    }
    // 5. Obsługa czytania bazy bez odpalania ciężkich algorytmów MCMF (na potrzebe startu GUI)
    else if (komenda == "GUI_DATA_DUMP")
    {
        string skompKop = ek.wczytajArchiwumZDysku("data/kopalnie.bin");
        string skompKras = ek.wczytajArchiwumZDysku("data/dane_krasnoludkow.bin");

        // Wyrzucamy czysty tekst do konsoli (żeby Python przechwycił go funkcją capture_output=True)
        cout << "---KOPALNIE---\n";
        cout << ek.dekompresuj(skompKop) << "\n";
        cout << "---KRASNOLUDKI---\n";
        cout << ek.dekompresuj(skompKras) << "\n";
    }

    return 0;
}