#include "pond/painter.h"
#include "pond/organism.h"
#include "pond/types.h"
#include "raylib.h"
#include <string.h>

void painter_init(Painter *p) { memset(p, 0, sizeof(Painter)); }

Color painter_randomColour(Painter *p) {
  return (Color){(unsigned char)pond_random(0, 255),
                 (unsigned char)pond_random(0, 255),
                 (unsigned char)pond_random(0, 255), 255};
}

void painter_show_organism(Painter *p, Organism *organism) {
  if (!p->drawOrganism) {
    return;
  }

  Color colour = p->pretty ? organism->randomColour : organism->colour;
  Color fillColour = colour;
  fillColour.a = p->blur ? 20 : 200;

  Vector2 d = organism_displacement(organism);
  DrawEllipse((int)d.x, (int)d.y, organism->mass / 2.0f, organism->mass / 2.0f,
              fillColour);
}

void painter_show_flag(Painter *p, Organism *organism, Color colour,
                       bool flag) {
  if (!flag) {
    return;
  }

  if (p->debugEllipse) {
    Vector2 d = organism_displacement(organism);
    DrawEllipse((int)d.x, (int)d.y, organism->mass / 2.0f,
                organism->mass / 2.0f, colour);
  }

  if (p->debugRect) {
    Vector2 d = organism_displacement(organism);
    Rectangle rect;
    rect.x = d.x - organism->mass / 2.0f;
    rect.y = d.y - organism->mass / 2.0f;
    rect.width = organism->mass;
    rect.height = organism->mass;
    DrawRectangleRec(rect, colour);
  }
}
