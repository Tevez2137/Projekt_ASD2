#include "ksiegi.h"
#include <queue>
#include <map>

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

    int d = 256; // Wielkość alfabetu ASCII
    int q = 101; // Liczba pierwsza do modulo w funkcji haszującej
    int h = 1;
    int p = 0; // Wartość hasza wzorca
    int t = 0; // Wartość hasza bieżącego okna tekstu

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