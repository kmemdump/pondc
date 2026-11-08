#ifndef POND_SENSES_H
#define POND_SENSES_H

#include "pond/types.h"

typedef struct Senses Senses;

struct Senses {
  bool hasAttraction;
  bool hasRepulsion;
  int preyCount;
  int predatorCount;
  Species *prey;
  Species *predators;
  Organism *self;
  Vector2 attraction;
  Vector2 repulsion;
  float distanceToAttraction;
  float distanceToRepulsion;
};

void senses_update(Senses *s, Organism *other);
void senses_execute(Senses *s);
void senses_attract(Senses *s);
void senses_repulse(Senses *s);
bool senses_isAttraction(Senses *s, Organism *other);
bool senses_isRepulsion(Senses *s, Organism *other);
Senses *senses_create(Organism *organism, Species species);
void senses_free(Senses *s);

#endif
