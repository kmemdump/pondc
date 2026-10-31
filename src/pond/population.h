#ifndef POND_POPULATION_H
#define POND_POPULATION_H

#include "pond/pangenome.h"
#include "pond/painter.h"

typedef struct Population Population;

struct Population {
    Pangenome pangenome;
    Organism **organisms;
    int organismCount;
    int organismCapacity;
    float mass;
};

void population_init(Population *p, Painter *painter, int size);
Organism *population_add(Population *p, Painter *painter, Organism *organism);
Vector2 population_randomDisplacement(Population *p);
void population_update(Population *p, Painter *painter);
void population_interactWithOthers(Population *p, Organism *organism, int index);
void population_remove(Population *p, Organism *organism);
bool population_areMeeting(Population *p, Organism *o1, Organism *o2);

#endif
