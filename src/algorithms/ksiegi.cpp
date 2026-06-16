#include "ksiegi.h"
#include <queue>
#include <map>
#include <fstream>
#include<iostream>

void ElektroniczneKsiegi::generateCodes(HuffmanNode* node, std::string str) {
    if (!node) return;
    if (!node->left && !node->right) {
        huffmanCodes[node->ch] = str;
    }
    generateCodes(node->left, str + "0");
    generateCodes(node->right, str + "1");
}

void ElektroniczneKsiegi::budujDrzewoHuffmana(const std::string& tekst) {
    if (tekst.empty()) return;
    delete root;
    root = nullptr;
    huffmanCodes.clear();

    std::map<char, int> freq;
    for (char ch : tekst) {
        freq[ch]++;
    }

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> pq;
    for (auto pair : freq) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }
    // gdyby tekst składał się z tylko jednego powtarzającego się znaku
    if (pq.size() == 1) {
        HuffmanNode* single = pq.top();
        pq.pop();
        root = new HuffmanNode('\0', single->freq);
        root->left = single;
        generateCodes(root, "");
        return;
    }

    while (pq.size() != 1) {
        HuffmanNode *left = pq.top(); pq.pop();
        HuffmanNode *right = pq.top(); pq.pop();
        HuffmanNode *top = new HuffmanNode('\0', left->freq + right->freq);
        top->left = left;
        top->right = right;
        pq.push(top);
    }
    root = pq.top();
    generateCodes(root, "");
}

// prosta zamiana; lecimy po znakach i zastępujemy je gotowymi kodami bitowymi
std::string ElektroniczneKsiegi::kompresuj(const std::string& tekst) {
    std::string skompresowany = "";
    for (char ch : tekst) {
        skompresowany += huffmanCodes[ch];
    }
    return skompresowany;
}

std::vector<int> ElektroniczneKsiegi::szukajRabinKarp(const std::string& tekst, const std::string& wzorzec) {
    std::vector<int> pozycje;
    int n = tekst.length();
    int m = wzorzec.length();
    if (m == 0 || n < m) return pozycje;

    int d = 256; // wielkosc alfabetu ASCII
    int q = 101; // liczba pierwsza uzywana do modulo
    int h = 1;
    int p = 0; // wartosc hasza wzorca
    int t = 0; // wartosc hasza obecnego okna

    for (int i = 0; i < m - 1; i++) {
        h = (h * d) % q;
    }

    for (int i = 0; i < m; i++) {
        p = (d * p + wzorzec[i]) % q;
        t = (d * t + tekst[i]) % q;
    }

    for (int i = 0; i <= n - m; i++) {
        if (p == t) {
            bool dopasowanie = true;
            for (int j = 0; j < m; j++) {
                if (tekst[i + j] != wzorzec[j]) {
                    dopasowanie = false;
                    break;
                }
            }
            if (dopasowanie) {
                pozycje.push_back(i);
            }
        }
        if (i < n - m) {
            t = (d * (t - tekst[i] * h) + tekst[i + m]) % q;
            if (t < 0) t = (t + q);
        }
    }
    return pozycje;
}
// zapisuje wygenerowany słownik i skompresowany tekst do pliku .bin
void ElektroniczneKsiegi::zapiszArchiwumNaDysk(const std::string& sciezka, const std::string& skompresowany) {
    std::ofstream out(sciezka);
    if (!out.is_open()) return;
    out << huffmanCodes.size() << "\n";
    for (auto const& pair : huffmanCodes) {
        out << (int)pair.first << " " << pair.second << "\n";
    }
    out << skompresowany << "\n";
    out.close();
}

std::string ElektroniczneKsiegi::wczytajArchiwumZDysku(const std::string& sciezka) {
    std::ifstream in(sciezka);
    if (!in.is_open()) return "";
    int rozmiar_slownika;
    if (!(in >> rozmiar_slownika)) return ""; 
    huffmanCodes.clear();
    for (int i = 0; i < rozmiar_slownika; i++) {
        int kod_ascii;
        std::string kod_huffmana;
        in >> kod_ascii >> kod_huffmana;
        huffmanCodes[(char)kod_ascii] = kod_huffmana;
    }
    std::string skompresowany;
    in >> skompresowany;
    in.close();
    return skompresowany;
}

std::string ElektroniczneKsiegi::dekompresuj(const std::string& skompresowany) {
    std::map<std::string, char> odwroconaMapa;
    for (auto const& pair : huffmanCodes) {
        odwroconaMapa[pair.second] = pair.first;
    }
    std::string zdekodowany = "";
    std::string bufor = "";
    for (char bit : skompresowany) {
        bufor += bit;
        if (odwroconaMapa.count(bufor)) {
            zdekodowany += odwroconaMapa[bufor];
            bufor = ""; 
        }
    }
    return zdekodowany;
}