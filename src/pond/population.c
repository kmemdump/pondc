#include "pond/population.h"
#include "pond/log.h"
#include "pond/organism.h"
#include "pond/types.h"
#include <stdio.h>
#include <stdlib.h>

void population_init(Population *p, Painter *painter, int size) {
  p->organismCapacity = 16;
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
  int speciesCounts[3] = {0, 0, 0};
  float mass = 0;

  Organism **theBabies = NULL;
  int theBabyCount = 0;
  int theBabyCapacity = 0;
  Organism **theDead = NULL;
  int theDeadCount = 0;
  int theDeadCapacity = 0;

  for (int i = 0; i < p->organismCount; i++) {
    Organism *organism = p->organisms[i];
    if (organism->isDead) {
      if (theDeadCount >= theDeadCapacity) {
        theDeadCapacity = theDeadCapacity ? theDeadCapacity * 2 : 4;
        theDead =
            (Organism **)realloc(theDead, sizeof(Organism *) * theDeadCapacity);
      }
      theDead[theDeadCount++] = organism;
      continue;
    }

    organism_show(organism);
    organism_update(organism);
    population_interactWithOthers(p, organism, i);

    float howFat = organism_howFat(organism);
    if (howFat > 0 && pond_random(0, 1) < howFat) {
      Organism *baby = organism_makeBaby(organism);
      if (theBabyCount >= theBabyCapacity) {
        theBabyCapacity = theBabyCapacity ? theBabyCapacity * 2 : 4;
        theBabies = (Organism **)realloc(theBabies,
                                         sizeof(Organism *) * theBabyCapacity);
      }
      theBabies[theBabyCount++] = baby;
    }

    if (!g_biomassLock) {
      organism_photosynthesise(organism);
    }

    mass += organism->mass;
    speciesCounts[organism->genome.species]++;
  }

  int writeIndex = 0;
  for (int i = 0; i < p->organismCount; i++) {
    if (p->organisms[i]->isDead) {
      continue;
    }
    if (writeIndex != i) {
      p->organisms[writeIndex] = p->organisms[i];
    }
    writeIndex++;
  }
  p->organismCount = writeIndex;

  for (int i = 0; i < theBabyCount; i++) {
    if (p->organismCount >= p->organismCapacity) {
      p->organismCapacity *= 2;
      p->organisms = (Organism **)realloc(
          p->organisms, sizeof(Organism *) * p->organismCapacity);
    }
    p->organisms[p->organismCount++] = theBabies[i];
  }

  g_biomassLock = (mass > g_biomassLimit);

  char buf[128];
  snprintf(buf, sizeof(buf), "Total biomass: %f", mass);
  pond_log_info(buf);
  snprintf(buf, sizeof(buf), "Population size : %d", p->organismCount);
  pond_log_info(buf);
  for (int i = 0; i < 3; i++) {
    char sbuf[128];
    snprintf(sbuf, sizeof(sbuf), "Species: %d, Count: %d", i, speciesCounts[i]);
    pond_log_info(sbuf);
  }
}

void population_interactWithOthers(Population *p, Organism *organism,
                                   int index) {
  if (organism->isDead) {
    return;
  }

  for (int i = index + 1; i < p->organismCount; i++) {
    Organism *other = p->organisms[i];

    organism_sense(organism, other);
    organism_sense(other, organism);

    if (other == organism || !population_areMeeting(p, organism, other) ||
        other->isDead) {
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
