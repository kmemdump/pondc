#ifndef POND_PAINTER_H
#define POND_PAINTER_H

#include "pond/types.h"

typedef struct Painter Painter;

struct Painter {
  bool blur;
  bool pretty;
  bool outline;
  bool drawOrganism;
  bool drawBirth;
  bool drawDaddy;
  bool drawBaby;
  bool drawPredation;
  bool drawDead;
  bool drawPredator;
  bool drawPostChompGrowth;
  bool debugRect;
  bool debugEllipse;
  bool drawOverlap;
  bool drawCollision;
};

void painter_init(Painter *p);
Color painter_randomColour(Painter *p);
void painter_show_organism(Painter *p, Organism *organism);
void painter_show_flag(Painter *p, Organism *organism, Color colour, bool flag);

#endif
