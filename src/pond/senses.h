#ifndef POND_SENSES_H
#define POND_SENSES_H

#include "pond/types.h"

typedef struct Senses Senses;

struct Senses {
    Organism *self;
    Vector2 attraction;
    bool hasAttraction;
    float distanceToAttraction;
    Vector2 repulsion;
    bool hasRepulsion;
    float distanceToRepulsion;
    Species *prey;
    int preyCount;
    Species *predators;
    int predatorCount;
};

void senses_update(Senses *s, Organism *other);
void senses_execute(Senses *s);
void senses_attract(Senses *s);
void senses_repulse(Senses *s);
bool senses_isAttraction(Senses *s, Organism *other);
bool senses_isRepulsion(Senses *s, Organism *other);
Senses *senses_create(Organism *organism, Species species);

#endif
