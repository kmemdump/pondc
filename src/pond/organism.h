#ifndef POND_ORGANISM_H
#define POND_ORGANISM_H

#include "pond/pangenome.h"
#include "raylib.h"
#include <stdbool.h>

typedef struct Organism Organism;

struct Organism {
  bool isDead;
  float startMass;
  float mass;
  float age;
  Organism **collidingWith;
  int collidingCount;
  int collidingCapacity;

  Color colour;
  Color randomColour;

  Genome *genome;
  Motor *motor;
  Appearance *appearance;
  Senses *senses;
};

Organism *organism_create(Genome *genome, Vector2 displacement);
void organism_update(Organism *o);
void organism_show(Organism *o);
void organism_chomp(Organism *o, Organism *other);
float organism_howFat(Organism *o);
Organism *organism_makeBaby(Organism *o);
void organism_photosynthesise(Organism *o);
float organism_maxMass(Organism *o);
void organism_sense(Organism *o, Organism *other);
Species organism_species(Organism *o);
bool organism_canIEat(Organism *o, Organism *other);
void organism_addColliding(Organism *o, Organism *other);
void organism_destroy(Organism *organism);

#endif
