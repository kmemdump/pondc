#include "pond/pangenome.h"
#include "pond/appearance.h"
#include "pond/log.h"
#include "pond/motor.h"
#include "pond/senses.h"
#include "pond/types.h"
#include <raylib.h>
#include <stdlib.h>

#define SNAKE_COUNT_INIT 2
#define FISH_COUNT_INIT 6
#define ALGAE_COUNT_INIT 1000

void pangenome_init(Pangenome *p) {
  p->snake.species = SPECIES_SNAKE;
  p->snake.initCountMax = SNAKE_COUNT_INIT;
  p->snake.dietCount = 1;
  p->snake.diet[0] = SPECIES_FISH;

  p->fish.species = SPECIES_FISH;
  p->fish.initCountMax = FISH_COUNT_INIT;
  p->fish.dietCount = 2;
  p->fish.diet[0] = SPECIES_SNAKE;
  p->fish.diet[1] = SPECIES_ALGAE;

  p->algae.species = SPECIES_ALGAE;
  p->algae.initCountMax = ALGAE_COUNT_INIT;
  p->algae.dietCount = 0;

  p->speciesCount = 3;
  p->speciesList[0] = SPECIES_ALGAE;
  p->speciesList[1] = SPECIES_FISH;
  p->speciesList[2] = SPECIES_SNAKE;
}

Species pangenome_randomSpecies(Pangenome *p) {
  return p
      ->speciesList[(int)(rand() / (double)RAND_MAX * (double)p->speciesCount)];
}

Genome *pangenome_getGenome(Pangenome *p, Species species) {
  if (species == SPECIES_SNAKE) {
    return &p->snake;
  }
  if (species == SPECIES_FISH) {
    return &p->fish;
  }
  if (species == SPECIES_ALGAE) {
    return &p->algae;
  }
  log_error("Null genome for species.");
  return NULL;
}

bool genome_diet_contains(Genome *g, Species species) {
  for (int i = 0; i < g->dietCount; i++) {
    if (g->diet[i] == species) {
      return true;
    }
  }
  return false;
}

void *genome_createDna(Genome *g) {
  Dna *dna = (Dna *)calloc(1, sizeof(Dna));

  if (g->species == SPECIES_SNAKE) {
    dna->colour = MAROON;
    dna->predatorFactor = 1.5f;
    dna->fertility = 0.0002f;
    dna->maxFoodCapacityCoefficient = 3.0f;
    dna->lowerMassLimit = 20;
    dna->upperMassLimit = 30;
    dna->fissionFactor = 0.5f;
    dna->photosynthesisIncrement = 0;
    dna->maxAge = 1000;
    dna->metabolicRate = 0.01f;
    dna->emaciationQuotient = 1.1f;
  } else if (g->species == SPECIES_FISH) {
    dna->colour = BLUE;
    dna->predatorFactor = 0.7f;
    dna->fertility = 0.002f;
    dna->maxFoodCapacityCoefficient = 1.8f;
    dna->lowerMassLimit = 10;
    dna->upperMassLimit = 15;
    dna->fissionFactor = 0.3f;
    dna->photosynthesisIncrement = 0;
    dna->maxAge = 300;
    dna->metabolicRate = 0.02f;
    dna->emaciationQuotient = 5;
  } else if (g->species == SPECIES_ALGAE) {
    dna->colour = GREEN;
    dna->predatorFactor = 0.0f;
    dna->fertility = 1;
    dna->maxFoodCapacityCoefficient = 100;
    dna->lowerMassLimit = 5;
    dna->upperMassLimit = 5;
    dna->fissionFactor = 0.5f;
    dna->photosynthesisIncrement = 0.2f;
    dna->maxAge = 50000;
    dna->metabolicRate = 0;
    dna->emaciationQuotient = 5;
  }

  g->dna = *dna;
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
  appearance->points = (Vector2 *)malloc(100 * sizeof(Vector2));
  if (g->species == SPECIES_SNAKE) {
    appearance->kind = APPEARANCE_KIND_SNAKE;
  } else if (g->species == SPECIES_FISH) {
    appearance->kind = APPEARANCE_KIND_FISH;
  } else if (g->species == SPECIES_ALGAE) {
    appearance->kind = APPEARANCE_KIND_ALGAE;
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
