#ifndef POND_TYPES_H
#define POND_TYPES_H

#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum Species {
  SPECIES_NULL = 0,
  SPECIES_ALGAE,
  SPECIES_FISH,
  SPECIES_SNAKE
} Species;

typedef struct Organism Organism;
typedef struct Genome Genome;
typedef struct Motor Motor;
typedef struct Appearance Appearance;

extern float g_clock;
extern bool g_biomassLock;
extern int g_width;
extern int g_height;

#define CLOCK_MAX 360.0f
#define CLOCK_INCREMENT 0.05f

static inline Vector2 pond_vadd(Vector2 a, Vector2 b) {
  return (Vector2){a.x + b.x, a.y + b.y};
}
static inline Vector2 pond_vsub(Vector2 a, Vector2 b) {
  return (Vector2){a.x - b.x, a.y - b.y};
}
static inline Vector2 pond_vmult(Vector2 v, float s) {
  return (Vector2){v.x * s, v.y * s};
}
static inline Vector2 pond_vdiv(Vector2 v, float s) {
  return (Vector2){v.x / s, v.y / s};
}
static inline float pond_vmag(Vector2 v) {
  return sqrtf(v.x * v.x + v.y * v.y);
}
static inline Vector2 pond_vnormalize(Vector2 v) {
  float m = pond_vmag(v);
  if (m == 0.0f) {
    return v;
  }
  return pond_vmult(v, 1.0f / m);
}
static inline float pond_vdot(Vector2 a, Vector2 b) {
  return a.x * b.x + a.y * b.y;
}
static inline float pond_vheading(Vector2 v) { return atan2f(v.y, v.x); }
static inline float pond_random(float min, float max) {
  return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}
static inline Vector2 pond_vrandom2D(void) {
  return pond_vmult(
      (Vector2){pond_random(-100.0f, 100.0f), pond_random(-100.0f, 100.0f)},
      1.0f);
}

#endif
