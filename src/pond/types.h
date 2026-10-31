#ifndef POND_TYPES_H
#define POND_TYPES_H

#include "raylib.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

typedef enum Species {
    SPECIES_ALGAE = 0,
    SPECIES_FISH = 1,
    SPECIES_SNAKE = 2
} Species;

typedef struct Organism Organism;
typedef struct Genome Genome;
typedef struct Motor Motor;
typedef struct Appearance Appearance;
typedef struct Senses Senses;

extern Color g_background;
extern float g_clock;
extern float g_clockMax;
extern float g_clockIncrement;
extern float g_entropy;
extern float g_width;
extern float g_height;
extern bool g_biomassLock;
extern int g_populationSize;
extern float g_biomassLimit;
extern int g_snakeCountInit;
extern int g_fishCountInit;
extern int g_algaeCountInit;

#define CLOCK_MAX 360.0f
#define CLOCK_INCREMENT 0.05f
#define ENTROPY 0.99f
#define POPULATION_SIZE 100
#define BIOMASS_LIMIT 4000.0f
#define SNAKE_COUNT_INIT 2
#define FISH_COUNT_INIT 6
#define ALGAE_COUNT_INIT 9999999
#define SENS_DISTANCE 100.0f
#define ATTRACTION_RATE 2.0f
#define REPULSION_RATE 3.0f

#define TERMINAL_VELOCITY 1.0f
#define STAGNANT_VELOCITY 0.5f
#define COLLISION_DAMPING_FACTOR -0.9f
#define ACCELERATION_NOISE_FACTOR 15.0f
#define ACCELERATION_GROWTH_FACTOR 0.45f
#define SLITHER_FACTOR 1.0f

/* PVector stand-ins implemented on top of raylib Vector2. */
static inline Vector2 pond_vcopy(Vector2 v) { return v; }
static inline Vector2 pond_vadd(Vector2 a, Vector2 b) { return (Vector2){a.x + b.x, a.y + b.y}; }
static inline Vector2 pond_vsub(Vector2 a, Vector2 b) { return (Vector2){a.x - b.x, a.y - b.y}; }
static inline Vector2 pond_vmult(Vector2 v, float s) { return (Vector2){v.x * s, v.y * s}; }
static inline Vector2 pond_vdiv(Vector2 v, float s) { return (Vector2){v.x / s, v.y / s}; }
static inline float pond_vmag(Vector2 v) { return sqrtf(v.x * v.x + v.y * v.y); }
static inline Vector2 pond_vnormalize(Vector2 v) { float m = pond_vmag(v); if (m == 0.0f) { return v; } return pond_vmult(v, 1.0f / m); }
static inline float pond_vdot(Vector2 a, Vector2 b) { return a.x * b.x + a.y * b.y; }
static inline float pond_vheading(Vector2 v) { return atan2f(v.y, v.x); }
static inline float pond_random(float min, float max) { return min + ((float)rand() / (float)RAND_MAX) * (max - min); }
static inline Vector2 pond_vrandom2D(void) { return pond_vmult((Vector2){pond_random(-100.0f, 100.0f), pond_random(-100.0f, 100.0f)}, 1.0f); }

#endif
