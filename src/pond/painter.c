#include "pond/painter.h"
#include "pond/motor.h"
#include "pond/organism.h"
#include "pond/types.h"
#include "raylib.h"

static Painter painter_instance;

void painter_init() {
  // painter_instance.drawPostChompGrowth = true;
  // painter_instance.drawDead = true;
  painter_instance.drawPredation = true;
  // painter_instance.drawBaby = true;
  painter_instance.drawOrganism = true;
  // painter_instance.drawBirth = true;
  // painter_instance.drawCollision = true;
  // painter_instance.drawDaddy = true;
  // painter_instance.drawOverlap = true;
  // painter_instance.debugEllipse = true;

  painter_instance.background = WHITE;
}

void painter_clearBackground() {
  if (!painter_instance.blur && !painter_instance.pretty) {
    ClearBackground(painter_instance.background);
  }
}

Color painter_randomColour() {
  return (Color){(unsigned char)pond_random(0, 255),
                 (unsigned char)pond_random(0, 255),
                 (unsigned char)pond_random(0, 255), 255};
}

void painter_showOrganism(Organism *organism) {
  if (!painter_instance.drawOrganism) {
    return;
  }

  Color colour =
      painter_instance.pretty ? organism->randomColour : organism->colour;
  Color fillColour = colour;
  fillColour.a = painter_instance.blur ? 20 : 200;

  Vector2 d = organism->motor->displacement;
  DrawEllipse((int)d.x, (int)d.y, organism->mass / 2.0f, organism->mass / 2.0f,
              fillColour);
}

void painter_debugOrganism(Organism *organism, Color colour) {
  if (painter_instance.debugEllipse) {
    Vector2 d = organism->motor->displacement;
    DrawEllipse((int)d.x, (int)d.y, organism->mass / 2.0f,
                organism->mass / 2.0f, colour);
  }

  if (painter_instance.debugRect) {
    Vector2 d = organism->motor->displacement;
    Rectangle rect;
    rect.x = d.x - organism->mass / 2.0f;
    rect.y = d.y - organism->mass / 2.0f;
    rect.width = organism->mass;
    rect.height = organism->mass;
    DrawRectangleRec(rect, colour);
  }
}

void painter_debugCollisions(Organism *organism, Organism *other) {
  if (painter_instance.drawCollision) {
    // debug collision lines
  }
}
