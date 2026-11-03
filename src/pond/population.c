#include "pond/population.h"
#include "pond/log.h"
#include "pond/organism.h"
#include "pond/types.h"
#include <stdio.h>
#include <stdlib.h>

void population_init(Population *p, Painter *painter, int size) {
  p->organismCapacity = 100000;
  p->organisms = (Organism **)malloc(sizeof(Organism *) * p->organismCapacity);
  p->organismCount = 0;
  p->mass = 0;

  for (int i = 0; i < size; i++) {
    Genome *genome = pangenome_getGenome(
        &p->pangenome, pangenome_randomSpecies(&p->pangenome));
    if (genome == NULL) {
      return;
    }
    if (genome->initCount > genome->initCountMax) {
      return;
    }
    genome->initCount++;
    Vector2 displacement = pond_vcopy(
        (Vector2){pond_random(0, g_width), pond_random(0, g_height)});
    population_add(p, painter, organism_create(painter, genome, displacement));
  }
}

Organism *population_add(Population *p, Painter *painter, Organism *organism) {
  if (p->organismCount >= p->organismCapacity) {
    p->organismCapacity *= 2;
    p->organisms = (Organism **)realloc(p->organisms, sizeof(Organism *) *
                                                          p->organismCapacity);
  }
  p->organisms[p->organismCount] = organism;
  p->organismCount++;
  return organism;
}

Vector2 population_randomDisplacement(Population *p) {
  return (Vector2){pond_random(0, g_width), pond_random(0, g_height)};
}

void population_update(Population *p, Painter *painter) {
  // CLEAN UP THE DEAD: START
  int theDeadCount = 0;
  for (int i = 0; i < p->organismCount; i++) {
    if (p->organisms[i]->isDead) {
      theDeadCount++;
    }
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
  int theBabyCount = 0;
  for (int i = 0, j = 0; i < theAliveCount; i++) {
    Organism *organism = theAlive[i];
    float howFat = organism_howFat(organism);
    if (howFat > 0 && pond_random(0, 1) < howFat) {
      theBabyCount++;
    }
  }

  Organism **theBabies = (Organism **)malloc(sizeof(Organism *) * theBabyCount);
  for (int i = 0, j = 0; i < theAliveCount; i++) {
    Organism *organism = theAlive[i];
    float howFat = organism_howFat(organism);
    if (howFat > 0 && pond_random(0, 1) < howFat) {
      theBabies[j++] = organism_makeBaby(organism);
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
  float mass = 0;

  for (int i = 0; i < p->organismCount; i++) {
    Organism *organism = p->organisms[i];

    organism_show(organism);
    organism_update(organism);
    population_interactWithOthers(p, organism, i);

    if (!g_biomassLock) {
      organism_photosynthesise(organism);
    }

    mass += organism->mass;
    speciesCounts[organism->genome.species]++;
  }

  g_biomassLock = (mass > g_biomassLimit);

  char buf[128];
  snprintf(buf, sizeof(buf), "Total biomass: %f", mass);
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
      pond_vsub(organism_displacement(o2), organism_displacement(o1));
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
