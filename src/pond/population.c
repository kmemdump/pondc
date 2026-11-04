#include "pond/population.h"
#include "pond/log.h"
#include "pond/motor.h"
#include "pond/organism.h"
#include "pond/types.h"
#include <stdio.h>
#include <stdlib.h>

#define POPULATION_SIZE 100
#define BIOMASS_LIMIT 4000.0f
#define POPULATION_CAPACITY 100000

void population_init(Population *p, Painter *painter) {
  p->organismCapacity = POPULATION_CAPACITY;
  p->organisms = (Organism **)malloc(sizeof(Organism *) * p->organismCapacity);
  p->organismCount = 0;
  p->mass = 0;

  for (int i = SPECIES_ALGAE; i < SPECIES_SNAKE + 1; i++) {
    Genome *genome = pangenome_getGenome(&p->pangenome, i);
    for (int j = 0; j <= genome->initCountMax; j++) {
      Vector2 displacement = {pond_random(0, g_width),
                              pond_random(0, g_height)};
      p->organisms[p->organismCount++] =
          organism_create(painter, genome, displacement);
    }
  }
}

Vector2 population_randomDisplacement(Population *p) {
  return (Vector2){pond_random(0, g_width), pond_random(0, g_height)};
}

void population_update(Population *p, Painter *painter) {
  // CLEAN UP THE DEAD: START
  int theDeadCount = 0;
  for (int i = 0; i < p->organismCount; i++) {
    theDeadCount += p->organisms[i]->isDead;
  }

  int theAliveCount = p->organismCount - theDeadCount;
  Organism **theAlive = (Organism **)malloc(sizeof(Organism *) * theAliveCount);
  for (int i = 0, j = 0; i < p->organismCount; i++) {
    if (p->organisms[i]->isDead) {
      free(p->organisms[i]);
    } else {
      theAlive[j++] = p->organisms[i];
    }
  }
  // CLEAN UP THE DEAD: END

  // MAKE SOME BABIES: START
  Organism **theBabies =
      (Organism **)malloc(sizeof(Organism *) * theAliveCount);
  int theBabyCount = 0;
  for (int i = 0; i < theAliveCount; i++) {
    Organism *organism = theAlive[i];
    float howFat = organism_howFat(organism);
    if (howFat > 0 && pond_random(0, 1) < howFat) {
      theBabies[theBabyCount++] = organism_makeBaby(organism);
    }
  }
  // MAKE SOME BABIES: END

  // SET NEW POPULATION: START
  p->organismCount = theBabyCount + theAliveCount;
  Organism **newOrganisms =
      (Organism **)malloc(sizeof(Organism *) * p->organismCount);
  for (int i = 0; i < theAliveCount; i++) {
    newOrganisms[i] = theAlive[i];
  }

  int j = 0;
  for (int i = theAliveCount; i < p->organismCount; i++) {
    newOrganisms[i] = theBabies[j++];
  }
  free(p->organisms);
  p->organisms = newOrganisms;
  // SET NEW POPULATION: END

  int speciesCounts[4] = {0, 0, 0, 0};
  p->mass = 0;

  for (int i = 0; i < p->organismCount; i++) {
    Organism *organism = p->organisms[i];
    organism_show(organism);
    organism_update(organism);
    population_interactWithOthers(p, organism, i);
    if (!g_biomassLock) {
      organism_photosynthesise(organism);
    }

    p->mass += organism->mass;
    speciesCounts[organism->genome.species]++;
  }

  g_biomassLock = p->mass > BIOMASS_LIMIT;

  char buf[128];
  snprintf(buf, sizeof(buf), "Total biomass: %f", p->mass);
  log_info(buf);
  snprintf(buf, sizeof(buf), "Population size : %d", p->organismCount);
  log_info(buf);
  for (int i = 0; i < SPECIES_SNAKE + 1; i++) {
    char sbuf[128];
    snprintf(sbuf, sizeof(sbuf), "Species: %d, Count: %d", i, speciesCounts[i]);
    log_info(sbuf);
  }
}

void population_interactWithOthers(Population *p, Organism *organism,
                                   int index) {
  if (organism->isDead) {
    return;
  }

  for (int i = index + 1; i < p->organismCount; i++) {
    Organism *other = p->organisms[i];
    if (other->isDead || other == organism) {
      continue;
    }

    organism_sense(organism, other);
    organism_sense(other, organism);

    if (!population_areMeeting(p, organism, other)) {
      continue;
    }

    if (organism_species(organism) == organism_species(other)) {
      organism_addColliding(organism, other);
      continue;
    }

    if (organism_canIEat(organism, other)) {
      organism_chomp(organism, other);
    } else if (organism_canIEat(other, organism)) {
      organism_chomp(other, organism);
    } else {
      organism_addColliding(organism, other);
    }
  }
}

void population_remove(Population *p, Organism *organism) {}

bool population_areMeeting(Population *p, Organism *o1, Organism *o2) {
  Vector2 selfToOther =
      pond_vsub(o2->motor->displacement, o1->motor->displacement);
  float distanceBetweenCenters = pond_vmag(selfToOther);
  if (distanceBetweenCenters > (o2->mass + o1->mass) / 2.0f) {
    return false;
  }

  painter_show_flag(o1->painter, o1, BLACK,
                    o1->painter->drawOverlap || o1->painter->drawCollision);
  painter_show_flag(o1->painter, o2, BLACK,
                    o1->painter->drawOverlap || o1->painter->drawCollision);
  return true;
}
