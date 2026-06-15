#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <vector>
#include <string>
#include "../models/kopalnia.h"
#include "../models/krasnoludek.h"

class DataLoader {
public:
    static std::vector<Kopalnia> wczytajKopalnie(const std::string& sciezka);
    static std::vector<Krasnoludek> wczytajKrasnoludki(const std::string& sciezka);
};

#endif