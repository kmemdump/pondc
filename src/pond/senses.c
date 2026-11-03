#include "pond/senses.h"
#include "pond/log.h"
#include "pond/organism.h"
#include "pond/types.h"
#include <stdlib.h>

void senses_update(Senses *s, Organism *other) {
  Vector2 selfToOther =
      pond_vsub(organism_displacement(other), organism_displacement(s->self));
  float distanceBetweenCenters = pond_vmag(selfToOther);
  float distanceBetween =
      distanceBetweenCenters - (other->mass + s->self->mass) / 2.0f;

  if (distanceBetween > SENSE_DISTANCE) {
    return;
  }

  if (senses_isAttraction(s, other) &&
      (!s->hasAttraction || s->distanceToAttraction > distanceBetween)) {
    s->attraction = pond_vcopy(organism_displacement(other));
    s->distanceToAttraction = distanceBetween;
  } else if (senses_isRepulsion(s, other) &&
             (!s->hasRepulsion || s->distanceToRepulsion > distanceBetween)) {
    s->repulsion = pond_vcopy(organism_displacement(other));
    s->distanceToRepulsion = distanceBetween;
  }
}

void senses_execute(Senses *s) {
  if (s->hasAttraction) {
    senses_attract(s);
  }
  if (s->hasRepulsion) {
    senses_repulse(s);
  }
}

void senses_attract(Senses *s) {
  Vector2 selfToAttraction =
      pond_vsub(s->attraction, organism_displacement(s->self));
  Vector2 direction = pond_vnormalize(selfToAttraction);
  direction = pond_vmult(direction, ATTRACTION_RATE);
  Vector2 d = organism_displacement(s->self);
  d = pond_vadd(d, direction);
  organism_setDisplacement(s->self, d);
  s->hasAttraction = false;
}

void senses_repulse(Senses *s) {
  Vector2 selfToRepulsion =
      pond_vsub(s->repulsion, organism_displacement(s->self));
  Vector2 direction =
      pond_vmult(pond_vnormalize(selfToRepulsion), REPULSION_RATE);
  Vector2 d = organism_displacement(s->self);
  d = pond_vsub(d, direction);
  organism_setDisplacement(s->self, d);
  s->hasRepulsion = false;
}

bool senses_isAttraction(Senses *s, Organism *other) {
  for (int i = 0; i < s->preyCount; i++) {
    if (s->prey[i] == other->genome.species) {
      return true;
    }
  }
  return false;
}

bool senses_isRepulsion(Senses *s, Organism *other) {
  for (int i = 0; i < s->predatorCount; i++) {
    if (s->predators[i] == other->genome.species) {
      return true;
    }
  }
  return false;
}

Senses *senses_create(Organism *organism, Species species) {
  Senses *s = (Senses *)calloc(1, sizeof(Senses));
  s->self = organism;

  if (species == SPECIES_SNAKE) {
    s->preyCount = 1;
    s->prey = (Species *)malloc(sizeof(Species) * s->preyCount);
    s->prey[0] = SPECIES_FISH;
  } else if (species == SPECIES_FISH) {
    s->preyCount = 1;
    s->prey = (Species *)malloc(sizeof(Species) * s->preyCount);
    s->prey[0] = SPECIES_ALGAE;
    s->predatorCount = 1;
    s->predators = (Species *)malloc(sizeof(Species) * s->predatorCount);
    s->predators[0] = SPECIES_SNAKE;
  }

  return s;
}
