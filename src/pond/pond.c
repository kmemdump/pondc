#include "pond/pond.h"
#include "pond/log.h"
#include "pond/painter.h"
#include "pond/population.h"
#include "pond/types.h"

Color g_background = WHITE;
float g_clock = 0;
float g_clockMax = CLOCK_MAX;
float g_clockIncrement = CLOCK_INCREMENT;
float g_width = 0;
float g_height = 0;
bool g_biomassLock = false;

void pond_init(Pond *pond, int width, int height) {
  pond->clock = 0;
  pond->clockMax = CLOCK_MAX;
  pond->clockIncrement = CLOCK_INCREMENT;

  g_width = width;
  g_height = height;

  painter_init();
  pangenome_init(&pond->population.pangenome);
  population_init(&pond->population);
}

void pond_update(Pond *pond) {
  painter_clearBackground();
  population_update(&pond->population);
  pond_drawMouse();
  pond->clock += pond->clockIncrement;
}

void pond_drawMouse() {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    DrawEllipse(GetMouseX(), GetMouseY(), 25, 25, WHITE);
  }
}

void pond_free(Pond *pond) { population_free(&pond->population); }
