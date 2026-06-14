#include "dekametrowcy.h"

// Element neutralny dla funkcji MAX - duża ujemna wartość
const int MINUS_NIESKONCZONOSC = -1000000000; 
const WezelDrzewa ELEMENT_NEUTRALNY = {MINUS_NIESKONCZONOSC, -1};

DrzewoPrzedzialowe::DrzewoPrzedzialowe(const std::vector<Dekametrowiec>& dekametrowcy) {
    A = dekametrowcy;
    n = A.size();
    if(n > 0) {
        tree.assign(4 * n + 1, ELEMENT_NEUTRALNY);
        BUILD(1, 0, n - 1);
    }
}

void DrzewoPrzedzialowe::BUILD(int v, int l, int r) {
    if (l == r) {
        tree[v] = {A[l].glosnosc, A[l].ID};
    } else {
        int mid = (l + r) / 2;
        BUILD(2 * v, l, mid);          // BUILD(v.left, 1, mid, f)
        BUILD(2 * v + 1, mid + 1, r);  // BUILD(v.right, mid+1, r, f)
        
        // Funkcja f to u nas max(lewy, prawy) - wybieramy większą głośność.
        // Przy remisie wybieramy mniejsze ID, aby zachować deterministyczne zachowanie.
        WezelDrzewa L = tree[2 * v];
        WezelDrzewa R = tree[2 * v + 1];
        if (L.maxGlosnosc > R.maxGlosnosc) tree[v] = L;
        else if (L.maxGlosnosc < R.maxGlosnosc) tree[v] = R;
        else {
            if (L.krasnoludekID == -1) tree[v] = R;
            else if (R.krasnoludekID == -1) tree[v] = L;
            else tree[v] = (L.krasnoludekID <= R.krasnoludekID) ? L : R;
        }
    }
}

WezelDrzewa DrzewoPrzedzialowe::QUERY(int v, int l, int r, int ql, int qr) {
    // 1) Jeśli przedział całkowicie poza zapytaniem (r < ql lub qr < l)
    if (r < ql || qr < l) {
        return ELEMENT_NEUTRALNY;
    }
    
    // 2) Jeśli przedział całkowicie wewnątrz zapytania (ql <= l oraz r <= qr)
    if (ql <= l && r <= qr) {
        return tree[v];
    }
    
    // Część "else" - wykonaj a), b), c), d)
    int mid = (l + r) / 2;
    WezelDrzewa x = QUERY(2 * v, l, mid, ql, qr);
    WezelDrzewa y = QUERY(2 * v + 1, mid + 1, r, ql, qr);

    // Zwróć f(x, y) - funkcja zwraca maksimum; przy remisie deterministycznie mniejsze ID
    if (x.maxGlosnosc > y.maxGlosnosc) return x;
    if (x.maxGlosnosc < y.maxGlosnosc) return y;
    if (x.krasnoludekID == -1) return y;
    if (y.krasnoludekID == -1) return x;
    return (x.krasnoludekID <= y.krasnoludekID) ? x : y;
}

int DrzewoPrzedzialowe::zapytajONajglosniejszego(int ql, int qr) {
    if (n == 0 || ql > qr) return -1;
    // Wywołanie: QUERY(root, 1, n, ql, qr, f)
    WezelDrzewa wynik = QUERY(1, 0, n - 1, ql, qr);
    return wynik.krasnoludekID;
}