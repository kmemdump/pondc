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

  Color background;
};

void painter_init();
void painter_clearBackground();
Color painter_randomColour();
void painter_showOrganism(Organism *organism);
void painter_debugOrganism(Organism *organism, Color colour);
void painter_debugCollisions(Organism *organism, Organism *other);

#endif
