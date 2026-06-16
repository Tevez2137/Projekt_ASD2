#include "dekametrowcy.h"

// Element neutralny dla funkcji MAX - duża ujemna wartość
const int MINUS_NIESKONCZONOSC = -1000000000; 
const WezelDrzewa ELEMENT_NEUTRALNY = {MINUS_NIESKONCZONOSC, -1};

DrzewoPrzedzialowe::DrzewoPrzedzialowe(const std::vector<Dekametrowiec>& dekametrowcy) {
    A = dekametrowcy;
    n = A.size();
    if(n > 0) {
        tree.assign(4 * n + 1, ELEMENT_NEUTRALNY);  // 4n żeby drzewo starczyło na najgorszy rozkład
        BUILD(1, 0, n - 1);
    }
}

void DrzewoPrzedzialowe::BUILD(int v, int l, int r) {
    // Jak przedział zwinie się do 1 elementu to mamy liścia
    if (l == r) {
        tree[v] = {A[l].glosnosc, A[l].ID};
    } else {
        int mid = (l + r) / 2;
        // Odpalamy dla lewego dziecka (2*v) i prawego (2*v+1)
        BUILD(2 * v, l, mid);         
        BUILD(2 * v + 1, mid + 1, r); 
        
        
        WezelDrzewa L = tree[2 * v];
        WezelDrzewa R = tree[2 * v + 1];

        //wybieramy wiekszą glosnosc, a przy remisie mniejsze id
        if (L.maxGlosnosc > R.maxGlosnosc) tree[v] = L;
        else if (L.maxGlosnosc < R.maxGlosnosc) tree[v] = R;
        else {
            if (L.straznikID == -1) tree[v] = R;
            else if (R.straznikID == -1) tree[v] = L;
            else tree[v] = (L.straznikID <= R.straznikID) ? L : R;
        }
    }
}

WezelDrzewa DrzewoPrzedzialowe::QUERY(int v, int l, int r, int ql, int qr) {
    // jesli jest poza przedzialem to zwracamy element neutralny
    if (r < ql || qr < l) {
        return ELEMENT_NEUTRALNY;
    }
    
    // zapytanie idealnie pokrywa lub jest większe niż obecny przedział
    if (ql <= l && r <= qr) {
        return tree[v];
    }
    
    // zahacza o nasz przedział, rozbijamy zapytanie na poddrzewa i z nich bierzemy max
    int mid = (l + r) / 2;
    WezelDrzewa x = QUERY(2 * v, l, mid, ql, qr);
    WezelDrzewa y = QUERY(2 * v + 1, mid + 1, r, ql, qr);

    if (x.maxGlosnosc > y.maxGlosnosc) return x;
    if (x.maxGlosnosc < y.maxGlosnosc) return y;
    if (x.straznikID == -1) return y;
    if (y.straznikID == -1) return x;
    return (x.straznikID <= y.straznikID) ? x : y;
}

int DrzewoPrzedzialowe::zapytajONajglosniejszego(int ql, int qr) {
    if (n == 0 || ql > qr) return -1;
    WezelDrzewa wynik = QUERY(1, 0, n - 1, ql, qr);
    return wynik.straznikID;
}