//
// Created by Gzalo on 28/4/2023.
//

#ifndef LD53_CABLE_H
#define LD53_CABLE_H


#include "Source.h"
#include "Sink.h"

class Cable {
public:
    Source *left;
    Sink *right;
    int size; // 1, 2, 3
    int offset; // 0 (top), 1 (middle), 2 (bottom)
    int health;
};


#endif //LD53_CABLE_H
