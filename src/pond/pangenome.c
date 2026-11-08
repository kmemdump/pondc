#include "pond/pangenome.h"
#include "pond/appearance.h"
#include "pond/log.h"
#include "pond/motor.h"
#include "pond/organism.h"
#include "pond/senses.h"
#include "pond/types.h"
#include <assert.h>
#include <raylib.h>
#include <stdlib.h>

#define SNAKE_COUNT_INIT 2
#define FISH_COUNT_INIT 6
#define ALGAE_COUNT_INIT 1000

const float GROWTH_RATE_SNAKE = 0.3;
const float GROWTH_RATE_FISH = 0.4;

void pangenome_init(Pangenome *p) {
  p->speciesInitCountMax = malloc(sizeof(Species) * 4);
  p->speciesInitCountMax[SPECIES_SNAKE] = SNAKE_COUNT_INIT;
  p->speciesInitCountMax[SPECIES_FISH] = FISH_COUNT_INIT;
  p->speciesInitCountMax[SPECIES_ALGAE] = ALGAE_COUNT_INIT;
}

bool genome_diet_contains(Genome *g, Species species) {
  for (int i = 0; i < g->dietCount; i++) {
    if (g->diet[i] == species) {
      return true;
    }
  }
  return false;
}

Genome *genome_create(Species species) {
  assert(species);

  Genome *genome = malloc(sizeof(Genome));
  if (species == SPECIES_SNAKE) {
    genome->species = SPECIES_SNAKE;
    genome->dietCount = 1;
    genome->diet = malloc(sizeof(Species) * genome->dietCount);
    genome->diet[0] = SPECIES_FISH;
  }

  if (species == SPECIES_FISH) {
    genome->species = SPECIES_FISH;
    genome->dietCount = 2;
    genome->diet = malloc(sizeof(Species) * genome->dietCount);
    genome->diet[0] = SPECIES_SNAKE;
    genome->diet[1] = SPECIES_ALGAE;
  }

  if (species == SPECIES_ALGAE) {
    genome->species = SPECIES_ALGAE;
    genome->dietCount = 0;
    genome->diet = NULL;
  }

  genome_initDna(genome);

  return genome;
}

void genome_initDna(Genome *g) {
  assert(g->species != SPECIES_NULL);

  if (g->species == SPECIES_SNAKE) {
    g->dna.colour = MAROON;
    g->dna.predatorFactor = 1.5f;
    g->dna.fertility = 0.0002f;
    g->dna.maxFoodCapacityCoefficient = 3.0f;
    g->dna.lowerMassLimit = 20;
    g->dna.upperMassLimit = 30;
    g->dna.fissionFactor = 0.5f;
    g->dna.photosynthesisIncrement = 0;
    g->dna.maxAge = 1000;
    g->dna.metabolicRate = 0.01f;
    g->dna.emaciationQuotient = 1.1f;
  } else if (g->species == SPECIES_FISH) {
    g->dna.colour = BLUE;
    g->dna.predatorFactor = 0.7f;
    g->dna.fertility = 0.002f;
    g->dna.maxFoodCapacityCoefficient = 1.8f;
    g->dna.lowerMassLimit = 10;
    g->dna.upperMassLimit = 15;
    g->dna.fissionFactor = 0.3f;
    g->dna.photosynthesisIncrement = 0;
    g->dna.maxAge = 300;
    g->dna.metabolicRate = 0.02f;
    g->dna.emaciationQuotient = 5;
  } else if (g->species == SPECIES_ALGAE) {
    g->dna.colour = GREEN;
    g->dna.predatorFactor = 0.0f;
    g->dna.fertility = 1;
    g->dna.maxFoodCapacityCoefficient = 100;
    g->dna.lowerMassLimit = 5;
    g->dna.upperMassLimit = 5;
    g->dna.fissionFactor = 0.5f;
    g->dna.photosynthesisIncrement = 0.2f;
    g->dna.maxAge = 50000;
    g->dna.metabolicRate = 0;
    g->dna.emaciationQuotient = 5;
  }
}

Motor *genome_createMotor(Genome *g, Organism *organism,
                          Vector2 startDisplacement, Vector2 startVelocity) {
  MotorKind kind = MOTOR_KIND_BASE;
  if (g->species == SPECIES_SNAKE) {
    kind = MOTOR_KIND_SNAKE;
  } else if (g->species == SPECIES_FISH) {
    kind = MOTOR_KIND_FISH;
  } else if (g->species == SPECIES_ALGAE) {
    kind = MOTOR_KIND_ALGAE;
  }
  return motor_create(kind, organism, startDisplacement, startVelocity);
}

Appearance *genome_createAppearance(Genome *g, Organism *organism,
                                    Color colour) {
  Appearance *appearance = (Appearance *)calloc(1, sizeof(Appearance));
  appearance->organism = organism;
  appearance->colour = colour;
  if (g->species == SPECIES_SNAKE) {
    appearance->kind = APPEARANCE_KIND_SNAKE;
  } else if (g->species == SPECIES_FISH) {
    appearance->kind = APPEARANCE_KIND_FISH;
  } else if (g->species == SPECIES_ALGAE) {
    appearance->kind = APPEARANCE_KIND_ALGAE;
  }

  if (appearance->kind == APPEARANCE_KIND_FISH ||
      appearance->kind == APPEARANCE_KIND_SNAKE) {
    appearance->growthRate = (appearance->kind == APPEARANCE_KIND_FISH)
                                 ? GROWTH_RATE_FISH
                                 : GROWTH_RATE_SNAKE;
    appearance->pointCapacity =
        (int)(organism->mass / appearance->growthRate) + 2;
    appearance->points =
        (Vector2 *)malloc(sizeof(Vector2) * appearance->pointCapacity);
    float i = 0;
    while (i <= organism->mass) {
      appearance->points[appearance->pointCount].x =
          organism->motor->displacement.x;
      appearance->points[appearance->pointCount].y =
          organism->motor->displacement.y;
      appearance->pointCount++;
      i += appearance->growthRate;
    }
  }
  return appearance;
}

Senses *genome_createSenses(Genome *g, Organism *organism) {
  Senses *senses = (Senses *)calloc(1, sizeof(Senses));
  senses->self = organism;
  if (g->species == SPECIES_SNAKE) {
    senses->preyCount = 1;
    senses->prey = (Species *)malloc(sizeof(Species) * senses->preyCount);
    senses->prey[0] = SPECIES_FISH;
  } else if (g->species == SPECIES_FISH) {
    senses->preyCount = 1;
    senses->prey = (Species *)malloc(sizeof(Species) * senses->preyCount);
    senses->prey[0] = SPECIES_ALGAE;
    senses->predatorCount = 1;
    senses->predators =
        (Species *)malloc(sizeof(Species) * senses->predatorCount);
    senses->predators[0] = SPECIES_SNAKE;
  }
  return senses;
}

void genome_free(Genome *g) {
  if (g->diet) {
    free(g->diet);
  }
  free(g);
}
