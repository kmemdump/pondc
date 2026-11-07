#include "pond/pangenome.h"
#include "pond/appearance.h"
#include "pond/log.h"
#include "pond/motor.h"
#include "pond/senses.h"
#include "pond/types.h"
#include <assert.h>
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
