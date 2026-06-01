#include "dekametrowcy.h"

// Element neutralny dla funkcji MAX
const int MINUS_NIESKONCZONOSC = -1; 
const WezelDrzewa ELEMENT_NEUTRALNY = {MINUS_NIESKONCZONOSC, -1};

DrzewoPrzedzialowe::DrzewoPrzedzialowe(const std::vector<Dekametrowiec>& dekametrowcy) {
    A = dekametrowcy;
    n = A.size();
    if(n > 0) {
        tree.resize(4 * n + 1);
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
        
        // Funkcja f to u nas max(lewy, prawy) - wybieramy większą głośność)
        if (tree[2 * v].maxGlosnosc >= tree[2 * v + 1].maxGlosnosc) {
            tree[v] = tree[2 * v];
        } else {
            tree[v] = tree[2 * v + 1];
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
    
    // Zwróć f(x, y) - funkcja zwraca maksimum
    if (x.maxGlosnosc >= y.maxGlosnosc) return x;
    return y;
}

int DrzewoPrzedzialowe::zapytajONajglosniejszego(int ql, int qr) {
    if (n == 0 || ql > qr) return -1;
    // Wywołanie: QUERY(root, 1, n, ql, qr, f)
    WezelDrzewa wynik = QUERY(1, 0, n - 1, ql, qr);
    return wynik.krasnoludekID;
}