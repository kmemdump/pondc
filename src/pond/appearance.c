#include "pond/appearance.h"
#include "pond/log.h"
#include "pond/motor.h"
#include "pond/organism.h"
#include "pond/types.h"
#include "raylib.h"
#include "rlgl.h"
#include <assert.h>
#include <stdlib.h>

static void updatePoints(Appearance *appearance) {
  Organism *organism = appearance->organism;
  assert(organism->mass);
  int newPointCount = (int)(organism->mass / appearance->growthRate) + 2;
  if (newPointCount > appearance->pointCapacity) {
    appearance->pointCapacity = newPointCount;
    appearance->points =
        realloc(appearance->points, sizeof(Vector2) * newPointCount);
  }

  appearance->points[0] = organism->motor->displacement;

  int currentPointCount = appearance->pointCount;
  for (int i = currentPointCount - 1; i > 0; i--) {
    appearance->points[i] = appearance->points[i - 1];
  }

  appearance->pointCount = newPointCount;
  for (int i = currentPointCount; i < newPointCount; i++) {
    appearance->points[i] = appearance->points[currentPointCount];
  }
}

static void drawTaperedBody(Appearance *appearance) {
  Organism *organism = appearance->organism;
  int i = appearance->pointCount - 1;
  float taper = organism->mass;
  while (i >= 0 && taper > 0) {
    Vector2 point = appearance->points[i];
    float size = organism->mass - taper;
    DrawEllipse((int)point.x, (int)point.y, size / 2.0f, size / 2.0f,
                organism->colour);
    i--;
    taper -= organism->mass / (float)appearance->pointCount;
  }
}

void appearance_show(Appearance *appearance) {
  assert(appearance);
  Organism *organism = appearance->organism;
  if (appearance->kind == APPEARANCE_KIND_FISH) {
    updatePoints(appearance);
    drawTaperedBody(appearance);

    assert(appearance->pointCount > 1);

    float radius = organism->mass / 2.0f;
    Vector2 lastPoint = appearance->points[appearance->pointCount - 1];
    Vector2 finHeadingVector =
        pond_vsub(appearance->points[appearance->pointCount - 2], lastPoint);
    float heading = pond_vheading(finHeadingVector);

    rlPushMatrix();
    rlTranslatef(lastPoint.x, lastPoint.y, 0.0f);
    rlRotatef(heading, 0.0f, 0.0f, 1.0f);
    DrawTriangle((Vector2){radius, 0.0f}, (Vector2){0.0f, radius},
                 (Vector2){0.0f, -radius}, organism->colour);
    rlPopMatrix();
  }

  if (appearance->kind == APPEARANCE_KIND_SNAKE) {
    updatePoints(appearance);
    drawTaperedBody(appearance);
  }

  if (appearance->kind == APPEARANCE_KIND_ALGAE) {
    Vector2 d = organism->motor->displacement;
    DrawEllipse((int)d.x, (int)d.y, organism->mass / 2.0f,
                organism->mass / 2.0f, organism->randomColour);
  }
}

void appearance_free(Appearance *a) {
  free(a->points);
  free(a);
}
