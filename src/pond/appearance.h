#ifndef POND_APPEARANCE_H
#define POND_APPEARANCE_H

#include "pond/types.h"

typedef enum AppearanceKind {
  APPEARANCE_KIND_NULL = 0,
  APPEARANCE_KIND_ALGAE,
  APPEARANCE_KIND_FISH,
  APPEARANCE_KIND_SNAKE
} AppearanceKind;

typedef struct Appearance Appearance;
struct Appearance {
  Organism *organism;
  Color colour;
  AppearanceKind kind;
  Vector2 *points;
  float growthRate;
  int pointCount;
  int pointCapacity;
};

void appearance_show(Appearance *appearance);
void appearance_free(Appearance *appearance);

#endif
