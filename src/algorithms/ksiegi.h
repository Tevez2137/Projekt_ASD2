#ifndef KSIEGI_H
#define KSIEGI_H

#include <string>
#include <vector>
#include <map>

// Węzeł drzewa Huffmana
struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode *left, *right;
    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    ~HuffmanNode() {
        delete left;
        delete right;
    }
};

// Komparator do kolejki priorytetowej (od najmniejszej częstotliwości)
struct CompareNodes {
    bool operator()(HuffmanNode* l, HuffmanNode* r) {
        return l->freq > r->freq;
    }
};

class ElektroniczneKsiegi {
private:
    HuffmanNode* root;
    std::map<char, std::string> huffmanCodes;
    void generateCodes(HuffmanNode* node, std::string str);

public:
    ElektroniczneKsiegi() : root(nullptr) {}
    ~ElektroniczneKsiegi() { delete root; }

    // Moduł Kompresji (Huffman)
    void budujDrzewoHuffmana(const std::string& tekst);
    std::string kompresuj(const std::string& tekst);
    std::string dekompresuj(const std::string& skompresowany);

    void zapiszArchiwumNaDysk(const std::string& sciezka, const std::string& skompresowany);
    std::string wczytajArchiwumZDysku(const std::string& sciezka);
    
    // Moduł Wyszukiwania (Rabin-Karp)
    std::vector<int> szukajRabinKarp(const std::string& tekst, const std::string& wzorzec);
};

#endif