#include "pond/organism.h"
#include "pond/appearance.h"
#include "pond/log.h"
#include "pond/motor.h"
#include "pond/painter.h"
#include "pond/senses.h"
#include "pond/types.h"
#include <stdlib.h>

Organism *organism_create(Painter *painter, Genome *genome,
                          Vector2 displacement) {
  Organism *o = (Organism *)calloc(1, sizeof(Organism));
  o->painter = painter;
  o->genome = *genome;
  genome_createDna(genome);

  o->startMass =
      pond_random(o->genome.dna.lowerMassLimit, o->genome.dna.upperMassLimit);
  o->mass = o->startMass;
  o->colour = o->genome.dna.colour;
  o->randomColour = painter_randomColour(painter);

  o->motor = genome_createMotor(&o->genome, o, displacement, pond_vrandom2D());
  o->appearance = genome_createAppearance(&o->genome, o, o->genome.dna.colour);
  o->senses = genome_createSenses(&o->genome, o);

  return o;
}

void organism_update(Organism *o) {
  o->age += CLOCK_INCREMENT;
  o->mass -= CLOCK_INCREMENT * o->genome.dna.metabolicRate / ENTROPY;

  if (o->mass < o->startMass / o->genome.dna.emaciationQuotient) {
    o->isDead = true;
    return;
  }

  if (o->age > o->genome.dna.maxAge) {
    o->isDead = true;
  }

  motor_update(o->motor);
  senses_execute(o->senses);
}

void organism_show(Organism *o) {
  appearance_show(o->appearance);
  painter_show_organism(o->painter, o);
}

void organism_chomp(Organism *o, Organism *other) {
  o->mass = (o->mass + other->mass) * ENTROPY;
  other->isDead = true;

  painter_show_flag(o->painter, o, GREEN,
                    o->painter->drawPredation ||
                        o->painter->drawPostChompGrowth);

  o->mass = o->mass - other->mass;

  painter_show_flag(o->painter, o, RED,
                    o->painter->drawPredation || o->painter->drawPredator);

  o->mass = o->mass + other->mass;

  painter_show_flag(o->painter, other, MAROON,
                    o->painter->drawPredation || o->painter->drawDead);
}

Vector2 organism_displacement(Organism *o) { return o->motor->displacement; }

void organism_setDisplacement(Organism *o, Vector2 v) {
  o->motor->displacement = v;
}

Vector2 organism_velocity(Organism *o) { return o->motor->velocity; }

void organism_setVelocity(Organism *o, Vector2 v) { o->motor->velocity = v; }

Vector2 organism_acceleration(Organism *o) { return o->motor->acceleration; }

void organism_setAcceleration(Organism *o, Vector2 v) {
  o->motor->acceleration = v;
}

float organism_howFat(Organism *o) {
  return (o->mass - o->startMass) / o->startMass * o->genome.dna.fertility;
}

Organism *organism_makeBaby(Organism *o) {
  painter_show_flag(o->painter, o, (Color){0, 255, 255, 255},
                    o->painter->drawBirth);

  Vector2 displacement = pond_vmult(pond_vrandom2D(), o->mass);
  displacement = pond_vadd(displacement, organism_displacement(o));

  Organism *baby = organism_create(o->painter, &o->genome, displacement);
  baby->mass = o->mass * o->genome.dna.fissionFactor * ENTROPY;
  organism_setVelocity(baby, baby->motor->velocity);
  o->mass = (o->mass - baby->mass / ENTROPY) * ENTROPY;

  painter_show_flag(o->painter, o, BLUE,
                    o->painter->drawBirth || o->painter->drawDaddy);
  painter_show_flag(o->painter, baby, GREEN,
                    o->painter->drawBirth || o->painter->drawBaby);

  return baby;
}

float organism_timeOffset(Organism *o) { return o->motor->timeOffset; }

void organism_photosynthesise(Organism *o) {
  if (o->genome.dna.photosynthesisIncrement <= 0) {
    return;
  }
  o->mass += o->genome.dna.photosynthesisIncrement;
}

float organism_maxMass(Organism *o) {
  return o->startMass * o->genome.dna.maxFoodCapacityCoefficient;
}

void organism_sense(Organism *o, Organism *other) {
  senses_update(o->senses, other);
}

Species organism_species(Organism *o) { return o->genome.species; }

bool organism_canIEat(Organism *o, Organism *other) {
  return (o->mass * o->genome.dna.predatorFactor) > other->mass &&
         !other->isDead &&
         o->mass < o->startMass * o->genome.dna.maxFoodCapacityCoefficient &&
         genome_diet_contains(&o->genome, other->genome.species);
}

void organism_addColliding(Organism *o, Organism *other) {
  if (o->collidingCount >= o->collidingCapacity) {
    o->collidingCapacity = o->collidingCapacity ? o->collidingCapacity * 2 : 4;
    o->collidingWith = (Organism **)realloc(
        o->collidingWith, sizeof(Organism *) * o->collidingCapacity);
  }
  o->collidingWith[o->collidingCount++] = other;

  if (other->collidingCount >= other->collidingCapacity) {
    other->collidingCapacity =
        other->collidingCapacity ? other->collidingCapacity * 2 : 4;
    other->collidingWith = (Organism **)realloc(
        other->collidingWith, sizeof(Organism *) * other->collidingCapacity);
  }
  other->collidingWith[other->collidingCount++] = o;
}
