#ifndef DATA_SAVER_H
#define DATA_SAVER_H

#include <vector>
#include <string>
#include "../models/krasnoludek.h"

class DataSaver {
public:
    static void zapiszWyniki(const std::string& sciezka, const std::vector<Krasnoludek>& krasnoludki);
};

#endif