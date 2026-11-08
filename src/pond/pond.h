#ifndef POND_POND_H
#define POND_POND_H

#include "pond/population.h"

typedef struct Pond {
  float clock;
  float clockMax;
  float clockIncrement;
  float entropy;
  Population population;
} Pond;

void pond_init(Pond *p, int width, int height);
void pond_update(Pond *p);
void pond_drawMouse();
void pond_free(Pond *p);

#endif
