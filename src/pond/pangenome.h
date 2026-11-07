#ifndef POND_PANGENOME_H
#define POND_PANGENOME_H

#include "pond/types.h"

typedef struct Pangenome Pangenome;
typedef struct Genome Genome;
typedef struct Dna Dna;

struct Dna {
  Color colour;
  float predatorFactor;
  float fertility;
  float maxFoodCapacityCoefficient;
  float lowerMassLimit;
  float upperMassLimit;
  float fissionFactor;
  float photosynthesisIncrement;
  float maxAge;
  float metabolicRate;
  float emaciationQuotient;
};

struct Genome {
  Species species;
  Species diet[2];
  Dna dna;
  int initCountMax;
  int dietCount;
};

struct Pangenome {
  Genome snake;
  Genome fish;
  Genome algae;
  Species speciesList[3];
  int speciesCount;
};

void pangenome_init(Pangenome *p);
Genome *pangenome_getGenome(Pangenome *p, Species species);
void genome_initDna(Genome *g);
Motor *genome_createMotor(Genome *g, Organism *organism,
                          Vector2 startDisplacement, Vector2 startVelocity);
Appearance *genome_createAppearance(Genome *g, Organism *organism,
                                    Color colour);
Senses *genome_createSenses(Genome *g, Organism *organism);
bool genome_diet_contains(Genome *g, Species species);

#endif
