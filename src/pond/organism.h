#ifndef POND_ORGANISM_H
#define POND_ORGANISM_H

#include "pond/pangenome.h"
#include "raylib.h"
#include <stdbool.h>

typedef struct Organism Organism;

struct Organism {
  bool isDead;
  bool hasAttraction;
  bool hasRepulsion;
  int collidingCount;
  int collidingCapacity;
  float startMass;
  float mass;
  float age;
  float distanceToAttraction;
  float distanceToRepulsion;
  Color colour;
  Color randomColour;

  Vector2 attraction;
  Vector2 repulsion;

  Organism **collidingWith;
  Genome *genome;
  Motor *motor;
  Appearance *appearance;
};

Organism *organism_create(Species species, Vector2 displacement);
void organism_update(Organism *o);
void organism_show(Organism *o);
void organism_chomp(Organism *o, Organism *other);
float organism_howFat(Organism *o);
Organism *organism_makeBaby(Organism *o);
void organism_photosynthesise(Organism *o);
float organism_maxMass(Organism *o);
void organism_sense(Organism *o, Organism *other);
bool organism_canIEat(Organism *o, Organism *other);
void organism_addColliding(Organism *o, Organism *other);
void organism_free(Organism *organism);
void organism_sensesAttract(Organism *o);
void organism_sensesRepulse(Organism *o);

#endif
