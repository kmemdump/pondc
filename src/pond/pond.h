#ifndef POND_POND_H
#define POND_POND_H

#include "pond/population.h"
#include "pond/painter.h"

typedef struct Pond {
    Color background;
    float clock;
    float clockMax;
    float clockIncrement;
    float entropy;
    Population population;
    Painter painter;
} Pond;

void pond_init(Pond *p, int width, int height);
void pond_update(Pond *p);
void pond_drawMouse(Pond *p);

#endif
