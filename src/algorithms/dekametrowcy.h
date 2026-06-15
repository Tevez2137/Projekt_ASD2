#ifndef DEKAMETROWCY_H
#define DEKAMETROWCY_H

#include <vector>
#include <algorithm>

// Pojedynczy strażnik / dekametrowiec na trasie patrolowej
struct Dekametrowiec {
    int ID;
    int glosnosc;
};

// Struktura węzła
struct WezelDrzewa {
    int maxGlosnosc;
    int straznikID;
};

class DrzewoPrzedzialowe {
private:
    std::vector<WezelDrzewa> tree;
    std::vector<Dekametrowiec> A;
    int n;

    void BUILD(int v, int l, int r);
    WezelDrzewa QUERY(int v, int l, int r, int ql, int qr);
    
    // Opcjonalnie: update gdyby jakiś krasnoludek ochrypł (zmiana głośności)
    void UPDATE(int v, int l, int r, int i, int nowaGlosnosc);

public:
    DrzewoPrzedzialowe(const std::vector<Dekametrowiec>& dekametrowcy);
    int zapytajONajglosniejszego(int ql, int qr);
};

#endif