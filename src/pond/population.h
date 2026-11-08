#ifndef POND_POPULATION_H
#define POND_POPULATION_H

#include "pond/pangenome.h"

typedef struct Population Population;

struct Population {
  int organismCount;
  int organismCapacity;
  float mass;
  Organism **organisms;
  Pangenome pangenome;
};

void population_init(Population *p);
void population_update(Population *p);
void population_interactWithOthers(Population *p, Organism *organism,
                                   int index);
bool population_areMeeting(Population *p, Organism *o1, Organism *o2);
void population_free(Population *p);

#endif
