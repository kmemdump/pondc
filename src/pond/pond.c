#include "pond/pond.h"
#include "pond/population.h"
#include "pond/painter.h"
#include "pond/log.h"
#include "pond/types.h"
#include <stdlib.h>

Color g_background = WHITE;
float g_clock = 0;
float g_clockMax = CLOCK_MAX;
float g_clockIncrement = CLOCK_INCREMENT;
float g_entropy = ENTROPY;
float g_width = 0;
float g_height = 0;
bool g_biomassLock = false;
int g_populationSize = POPULATION_SIZE;
float g_biomassLimit = BIOMASS_LIMIT;
int g_snakeCountInit = SNAKE_COUNT_INIT;
int g_fishCountInit = FISH_COUNT_INIT;
int g_algaeCountInit = ALGAE_COUNT_INIT;

void pond_init(Pond *p, int width, int height)
{
    p->background = WHITE;
    p->clock = 0;
    p->clockMax = CLOCK_MAX;
    p->clockIncrement = CLOCK_INCREMENT;
    p->entropy = ENTROPY;

    g_width = width;
    g_height = height;
    g_biomassLock = false;
    g_populationSize = POPULATION_SIZE;
    g_biomassLimit = BIOMASS_LIMIT;
    g_snakeCountInit = SNAKE_COUNT_INIT;
    g_fishCountInit = FISH_COUNT_INIT;
    g_algaeCountInit = ALGAE_COUNT_INIT;

    painter_init(&p->painter);
    pangenome_init(&p->population.pangenome);
    population_init(&p->population, &p->painter, g_populationSize);
}

void pond_update(Pond *p)
{
    if (!p->painter.blur && !p->painter.pretty)
    {
        ClearBackground(p->background);
    }
    population_update(&p->population, &p->painter);
    pond_drawMouse(p);
    p->clock += p->clockIncrement;
}

void pond_drawMouse(Pond *p)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        DrawEllipse(GetMouseX(), GetMouseY(), 25, 25, WHITE);
    }
}
