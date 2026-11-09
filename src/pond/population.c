#include "pond/population.h"
#include "pond/log.h"
#include "pond/motor.h"
#include "pond/organism.h"
#include "pond/painter.h"
#include "pond/pangenome.h"
#include "pond/types.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define POPULATION_SIZE 100
#define BIOMASS_LIMIT 2000.0f
#define POPULATION_CAPACITY 100000

void population_init(Population *p) {
  p->organismCapacity = POPULATION_CAPACITY;
  p->organisms = (Organism **)malloc(sizeof(Organism *) * p->organismCapacity);
  p->organismCount = 0;
  p->mass = 0;

  for (int i = SPECIES_ALGAE; i < SPECIES_SNAKE + 1; i++) {
    for (int j = 0; j <= p->pangenome.speciesInitCountMax[i]; j++) {
      Vector2 displacement = {pond_random(0, g_width),
                              pond_random(0, g_height)};
      p->organisms[p->organismCount++] = organism_create(i, displacement);
    }
  }
}

void population_update(Population *p) {
  double t0 = GetTime();
  // CLEAN UP THE DEAD: START
  int theDeadCount = 0;
  for (int i = 0; i < p->organismCount; i++) {
    theDeadCount += p->organisms[i]->isDead;
  }

  int theAliveCount = p->organismCount - theDeadCount;
  char buf1[128];
  snprintf(buf1, sizeof(buf1), "org, alive, dead count: %d, %d, %d",
           p->organismCount, theAliveCount, theDeadCount);
  log_debug(buf1);
  Organism **theAlive = (Organism **)malloc(sizeof(Organism *) * theAliveCount);
  for (int i = 0, j = 0; i < p->organismCount; i++) {
    if (p->organisms[i]->isDead) {
      organism_free(p->organisms[i]);
    } else {
      theAlive[j++] = p->organisms[i];
    }
  }
  // CLEAN UP THE DEAD: END

  double t1 = GetTime();

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

  double t2 = GetTime();

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
  free(theAlive);
  free(theBabies);
  p->organisms = newOrganisms;
  // SET NEW POPULATION: END

  double t3 = GetTime();

  // UPDATE ORGANISMS: START
  int speciesCounts[4] = {0, 0, 0, 0};
  p->mass = 0;

  const int loopTimerCount = 4;
  float *loopTimes = calloc(sizeof(int), loopTimerCount);

  for (int i = 0; i < p->organismCount; i++) {
    Organism *organism = p->organisms[i];
    double tA = GetTime();
    organism_show(organism);
    double tB = GetTime();
    loopTimes[0] += tB - tA;
    population_interactWithOthers(p, organism, i);
    double tC = GetTime();
    loopTimes[1] += tC - tB;
    organism_update(organism);
    double tD = GetTime();
    loopTimes[2] += tD - tC;
    if (!g_biomassLock) {
      organism_photosynthesise(organism);
    }
    double tE = GetTime();
    loopTimes[3] += tE - tD;

    p->mass += organism->mass;
    speciesCounts[organism->genome->species]++;
  }

  char b[256];
  snprintf(b, sizeof(b),
           "loop: ms show=%.2f interact=%.2f update=%.2f photo=%.2f",
           loopTimes[0] * 1000, loopTimes[1] * 1000, loopTimes[2] * 1000,
           loopTimes[3] * 1000);
  log_debug(b);
  // UPDATE ORGANISMS: END

  double t4 = GetTime();

  snprintf(b, sizeof(b),
           "ms dead=%.2f baby=%.2f rebuild=%.2f org loop=%.2f (pop=%d)",
           (t1 - t0) * 1000, (t2 - t1) * 1000, (t3 - t2) * 1000,
           (t4 - t3) * 1000, p->organismCount);
  log_debug(b);

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
  for (int i = index + 1; i < p->organismCount; i++) {
    if (organism->isDead) {
      return;
    }
    Organism *other = p->organisms[i];
    if (other->isDead || other == organism) {
      continue;
    }

    organism_sense(organism, other);
    organism_sense(other, organism);

    if (!population_areMeeting(p, organism, other)) {
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

bool population_areMeeting(Population *p, Organism *o1, Organism *o2) {
  Vector2 selfToOther =
      pond_vsub(o2->motor->displacement, o1->motor->displacement);
  float distanceBetweenCenters = pond_vmag(selfToOther);
  assert(isfinite(distanceBetweenCenters));
  if (distanceBetweenCenters > (o2->mass + o1->mass) / 2.0f) {
    return false;
  }

  return true;
}

void population_free(Population *p) {
  for (int i = 0; i < p->organismCount; i++) {
    organism_free(p->organisms[i]);
  }
  free(p->organisms);
  pangenome_free(&p->pangenome);
}
