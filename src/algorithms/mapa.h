#ifndef MAPA_H
#define MAPA_H

#include <vector>
#include "../models/kopalnia.h"
#include "../models/krasnoludek.h"
#include "../graph/graph.h"
#include "otoczka.h"

class Mapa {
public:
    std::vector<Krasnoludek> krasnoludki;
    std::vector<Kopalnia> kopalnie;
    std::vector<Wspolrzedne> aktualnaOtoczka;

    void init();
    void obliczTraseKsiecia(Graph& g);
    void obliczKsiegi();
};

#endif