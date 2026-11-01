#include "pond/appearance.h"
#include "pond/log.h"
#include "pond/organism.h"
#include "pond/types.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdlib.h>

Appearance *appearance_create(Organism *organism, Color colour,
                              AppearanceKind kind) {
  Appearance *a = (Appearance *)calloc(1, sizeof(Appearance));
  a->self = organism;
  a->colour = colour;
  a->kind = kind;

  if (kind == APPEARANCE_KIND_FISH || kind == APPEARANCE_KIND_SNAKE) {
    float growthRate = (kind == APPEARANCE_KIND_FISH) ? 0.4f : 0.3f;
    a->pointCapacity = (int)(organism->mass / growthRate) + 2;
    a->points = (Vector2 *)malloc(
        sizeof(Vector2) * (a->pointCapacity > 0 ? a->pointCapacity : 1));
    float i = 0;
    while (i <= organism->mass) {
      a->points[a->pointCount].x = organism_displacement(organism).x;
      a->points[a->pointCount].y = organism_displacement(organism).y;
      a->pointCount++;
      i += growthRate;
    }
  }

  return a;
}

void appearance_show(Appearance *a) {
  Organism *self = a->self;

  if (a->kind == APPEARANCE_KIND_FISH) {
    float growthRate = 0.4f;
    int newPointCount = (int)(self->mass / growthRate) + 1;
    int currentPointCount = a->pointCount;

    if (currentPointCount >= newPointCount) {
      a->pointCount--;
    }
    if (currentPointCount > newPointCount) {
      a->pointCount--;
    }

    for (int i = a->pointCount; i > 0; i--) {
      a->points[i] = a->points[i - 1];
    }
    a->points[0].x = organism_displacement(self).x;
    a->points[0].y = organism_displacement(self).y;
    a->pointCount++;

    {
      float r = self->mass / 2.0f;
      Vector2 lastPoint = a->points[a->pointCount - 1];
      Vector2 finHeadingVector =
          pond_vsub(a->points[a->pointCount - 2], lastPoint);
      float heading = pond_vheading(finHeadingVector);

      rlPushMatrix();
      rlTranslatef(lastPoint.x, lastPoint.y, 0.0f);
      rlRotatef(heading, 0.0f, 0.0f, 1.0f);
      DrawTriangle((Vector2){r, 0.0f}, (Vector2){0.0f, r}, (Vector2){0.0f, -r},
                   self->colour);
      rlPopMatrix();
    }

    {
      int i = a->pointCount - 1;
      float taper = self->mass;
      while (i >= 0 && taper > 0) {
        Vector2 point = a->points[i];
        float size = self->mass - taper;
        DrawEllipse((int)point.x, (int)point.y, size / 2.0f, size / 2.0f,
                    self->colour);
        i--;
        taper -= self->mass / (float)a->pointCount;
      }
    }
  } else if (a->kind == APPEARANCE_KIND_SNAKE) {
    float growthRate = 0.3f;
    int newPointCount = (int)(self->mass / growthRate) + 1;
    int currentPointCount = a->pointCount;

    if (currentPointCount >= newPointCount) {
      a->pointCount--;
    }
    if (currentPointCount > newPointCount) {
      a->pointCount--;
    }

    for (int i = a->pointCount; i > 0; i--) {
      a->points[i] = a->points[i - 1];
    }
    a->points[0].x = organism_displacement(self).x;
    a->points[0].y = organism_displacement(self).y;
    a->pointCount++;

    int i = a->pointCount - 1;
    float taper = self->mass;
    while (i >= 0 && taper > 0) {
      Vector2 point = a->points[i];
      float size = self->mass - taper;
      DrawEllipse((int)point.x, (int)point.y, size / 2.0f, size / 2.0f,
                  self->colour);

      i--;
      taper -= self->mass / (float)a->pointCount;
    }
  } else {

    float growthRate = 0.3f;
    Vector2 d = organism_displacement(self);
    DrawEllipse((int)d.x, (int)d.y, self->mass / 2.0f, self->mass / 2.0f,
                self->colour);
  }
}

void appearance_free(Appearance *a) {
  free(a->points);
  free(a);
}
