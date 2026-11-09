#include "pond/organism.h"
#include "pond/appearance.h"
#include "pond/log.h"
#include "pond/motor.h"
#include "pond/painter.h"
#include "pond/pangenome.h"
#include "pond/types.h"
#include <assert.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

const float ENTROPY = 0.99f;
const float SENSE_DISTANCE = 400.0f;
const float ATTRACTION_RATE = 10.0f;
const float REPULSION_RATE = 12.0f;

Organism *organism_create(Species species, Vector2 displacement) {
  Organism *o = (Organism *)calloc(1, sizeof(Organism));

  o->genome = genome_create(species);
  o->startMass =
      pond_random(o->genome->dna.lowerMassLimit, o->genome->dna.upperMassLimit);
  o->mass = o->startMass;
  o->colour = o->genome->dna.colour;
  o->randomColour = painter_randomColour();
  o->motor = genome_createMotor(o->genome, o, displacement, pond_vrandom2D());
  o->appearance = genome_createAppearance(o->genome, o, o->genome->dna.colour);

  return o;
}

void organism_sensesAttract(Organism *o) {
  Vector2 selfToAttraction = pond_vsub(o->attraction, o->motor->displacement);
  Vector2 direction = pond_vnormalize(selfToAttraction);
  direction = pond_vmult(direction, ATTRACTION_RATE);
  Vector2 d = o->motor->displacement;
  d = pond_vadd(d, direction);
  o->motor->displacement = d;
  o->hasAttraction = false;
}

void organism_sensesRepulse(Organism *o) {
  Vector2 selfToRepulsion = pond_vsub(o->repulsion, o->motor->displacement);
  Vector2 direction =
      pond_vmult(pond_vnormalize(selfToRepulsion), REPULSION_RATE);
  Vector2 d = o->motor->displacement;
  d = pond_vsub(d, direction);
  o->motor->displacement = d;
  o->hasRepulsion = false;
}

void organism_update(Organism *o) {
  o->age += CLOCK_INCREMENT;
  o->mass -= CLOCK_INCREMENT * o->genome->dna.metabolicRate / ENTROPY;

  if (o->mass < o->startMass / o->genome->dna.emaciationQuotient) {
    o->isDead = true;
    return;
  }

  if (o->age > o->genome->dna.maxAge) {
    o->isDead = true;
  }

  motor_update(o->motor);

  if (o->hasAttraction) {
    organism_sensesAttract(o);
  } else if (o->hasRepulsion) {
    organism_sensesRepulse(o);
  }
}

void organism_show(Organism *o) {
  if (o == NULL) {
    log_error("organism_show: Organism is NULL");
    return;
  }
  appearance_show(o->appearance);
  painter_showOrganism(o);
}

void organism_chomp(Organism *o, Organism *other) {
  o->mass = (o->mass + other->mass) * ENTROPY;
  other->isDead = true;

  painter_debugOrganism(o, RED);
  painter_debugOrganism(other, GREEN);

  char buf[128];
  snprintf(buf, sizeof(buf),
           "organism_chomp: Predator species: %d, Prey species: %d",
           o->genome->species, other->genome->species);
  log_debug(buf);
}

float organism_howFat(Organism *o) {
  return (o->mass - o->startMass) / o->startMass * o->genome->dna.fertility;
}

Organism *organism_makeBaby(Organism *o) {
  painter_debugOrganism(o, (Color){0, 255, 255, 255});

  Vector2 displacement = pond_vmult(pond_vrandom2D(), o->mass);
  o->motor->displacement = pond_vadd(displacement, o->motor->displacement);

  Organism *baby = organism_create(o->genome->species, displacement);
  baby->mass = o->mass * o->genome->dna.fissionFactor * ENTROPY;
  baby->motor->velocity = o->motor->velocity;
  o->mass = (o->mass - baby->mass / ENTROPY) * ENTROPY;

  painter_showOrganism(baby);
  painter_debugOrganism(o, BLUE);
  painter_debugOrganism(baby, GREEN);

  return baby;
}

void organism_photosynthesise(Organism *o) {
  if (o->genome->dna.photosynthesisIncrement <= 0) {
    return;
  }
  o->mass += o->genome->dna.photosynthesisIncrement;
}

float organism_maxMass(Organism *o) {
  return o->startMass * o->genome->dna.maxFoodCapacityCoefficient;
}

void organism_sense(Organism *o, Organism *other) {
  Vector2 selfToOther =
      pond_vsub(other->motor->displacement, o->motor->displacement);
  float distanceBetweenCenters = pond_vmag(selfToOther);
  float distanceBetween =
      distanceBetweenCenters - (other->mass + o->mass) / 2.0f;

  if (distanceBetween > SENSE_DISTANCE) {
    return;
  }

  if (organism_canIEat(o, other) &&
      (!o->hasAttraction || o->distanceToAttraction > distanceBetween)) {
    o->attraction = other->motor->displacement;
    o->distanceToAttraction = distanceBetween;
  } else if (organism_canIEat(other, o) &&
             (!o->hasRepulsion || o->distanceToRepulsion > distanceBetween)) {
    o->repulsion = other->motor->displacement;
    o->distanceToRepulsion = distanceBetween;
  }
}

bool organism_canIEat(Organism *o, Organism *other) {
  return (o->mass * o->genome->dna.predatorFactor) > other->mass &&
         !other->isDead &&
         o->mass < o->startMass * o->genome->dna.maxFoodCapacityCoefficient &&
         genome_diet_contains(o->genome, other->genome->species);
}

void organism_addColliding(Organism *o, Organism *other) {
  if (o->collidingCount >= o->collidingCapacity) {
    o->collidingCapacity = o->collidingCapacity ? o->collidingCapacity + 1 : 1;
    o->collidingWith = (Organism **)realloc(
        o->collidingWith, sizeof(Organism *) * o->collidingCapacity);
  }
  o->collidingWith[o->collidingCount++] = other;

  if (other->collidingCount >= other->collidingCapacity) {
    other->collidingCapacity =
        other->collidingCapacity ? other->collidingCapacity + 1 : 1;
    other->collidingWith = (Organism **)realloc(
        other->collidingWith, sizeof(Organism *) * other->collidingCapacity);
  }
  other->collidingWith[other->collidingCount++] = o;
}

void organism_free(Organism *organism) {
  free(organism->motor);
  appearance_free(organism->appearance);
  genome_free(organism->genome);
  free(organism->collidingWith);
  free(organism);
}
