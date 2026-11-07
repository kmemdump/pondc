#include "pond/organism.h"
#include "pond/appearance.h"
#include "pond/log.h"
#include "pond/motor.h"
#include "pond/painter.h"
#include "pond/senses.h"
#include "pond/types.h"
#include <stdio.h>
#include <stdlib.h>

Organism *organism_create(Genome *genome, Vector2 displacement) {
  Organism *o = (Organism *)calloc(1, sizeof(Organism));
  o->genome = *genome;
  genome_createDna(genome);

  o->startMass =
      pond_random(o->genome.dna.lowerMassLimit, o->genome.dna.upperMassLimit);
  o->mass = o->startMass;
  o->colour = o->genome.dna.colour;
  o->randomColour = painter_randomColour();

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
  if (o == NULL) {
    log_error("organism_show: Organism is NULL");
    return;
  }
  appearance_show(o->appearance);
  painter_showOrganism(o);
}

void organism_chomp(Organism *o, Organism *other) {
  char buf[128];
  snprintf(buf, sizeof(buf), "organism_chomp: Prey species: %d",
           organism_species(other));
  log_debug(buf);

  snprintf(buf, sizeof(buf), "organism_chomp: Predator species: %d",
           organism_species(o));
  log_debug(buf);

  o->mass = (o->mass + other->mass) * ENTROPY;
  other->isDead = true;

  painter_debugOrganism(o, GREEN);
  o->mass = o->mass - other->mass;
  painter_debugOrganism(o, RED);
  o->mass = o->mass + other->mass;
  painter_debugOrganism(other, MAROON);

  log_debug("organism chomp: complete");
}

float organism_howFat(Organism *o) {
  return (o->mass - o->startMass) / o->startMass * o->genome.dna.fertility;
}

Organism *organism_makeBaby(Organism *o) {
  painter_debugOrganism(o, (Color){0, 255, 255, 255});

  Vector2 displacement = pond_vmult(pond_vrandom2D(), o->mass);
  o->motor->displacement = pond_vadd(displacement, o->motor->displacement);

  Organism *baby = organism_create(&o->genome, displacement);
  baby->mass = o->mass * o->genome.dna.fissionFactor * ENTROPY;
  baby->motor->velocity = o->motor->velocity;
  o->mass = (o->mass - baby->mass / ENTROPY) * ENTROPY;

  painter_showOrganism(baby);
  painter_debugOrganism(o, BLUE);
  painter_debugOrganism(baby, GREEN);

  return baby;
}

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
