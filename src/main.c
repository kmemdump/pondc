#include "raylib.h"
#include "pond/pond.h"

int main(void)
{
    const int screenWidth = 1200;
    const int screenHeight = 400;

    InitWindow(screenWidth, screenHeight, "Pond");
    g_width = screenWidth;
    g_height = screenHeight;
    g_background = WHITE;

    Pond pond;
    pond_init(&pond, screenWidth, screenHeight);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        pond_update(&pond);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
