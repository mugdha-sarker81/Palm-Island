#include "raylib.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Palm Island");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground((Color){20, 31, 39, 255});
        DrawText("PALM ISLAND", 48, 48, 32, (Color){213, 232, 222, 255});
        DrawText("Top-down movement room not implemented yet.",
                 48, 100, 20, (Color){180, 196, 188, 255});
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
