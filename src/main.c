#include "raylib.h"

#include <stdbool.h>
#include <stddef.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define WORLD_WIDTH 3200.0f
#define PLAYER_WIDTH 34.0f
#define PLAYER_HEIGHT 58.0f
#define PLAYER_SPEED 300.0f
#define PLAYER_ACCELERATION 2200.0f
#define AIR_ACCELERATION 1400.0f
#define FRICTION 2400.0f
#define GRAVITY 1650.0f
#define JUMP_SPEED 590.0f
#define COYOTE_TIME 0.10f
#define JUMP_BUFFER_TIME 0.12f

typedef struct Player {
    Rectangle bounds;
    Vector2 velocity;
    float coyoteTimer;
    float jumpBufferTimer;
    int facing;
    bool grounded;
} Player;

static const Rectangle platforms[] = {
    { 0.0f, 625.0f, 620.0f, 180.0f },
    { 475.0f, 520.0f, 190.0f, 28.0f },
    { 790.0f, 455.0f, 200.0f, 28.0f },
    { 1050.0f, 570.0f, 360.0f, 180.0f },
    { 1430.0f, 495.0f, 205.0f, 28.0f },
    { 1690.0f, 625.0f, 600.0f, 180.0f },
    { 1875.0f, 515.0f, 195.0f, 28.0f },
    { 2210.0f, 445.0f, 230.0f, 28.0f },
    { 2490.0f, 570.0f, 710.0f, 180.0f }
};

static const int platformCount = (int)(sizeof(platforms) / sizeof(platforms[0]));

static float MoveTowards(float current, float target, float maxDelta)
{
    if (current < target) {
        return current + maxDelta < target ? current + maxDelta : target;
    }
    return current - maxDelta > target ? current - maxDelta : target;
}

static void ResetPlayer(Player *player)
{
    player->bounds = (Rectangle){ 80.0f, 625.0f - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT };
    player->velocity = (Vector2){ 0.0f, 0.0f };
    player->coyoteTimer = 0.0f;
    player->jumpBufferTimer = 0.0f;
    player->facing = 1;
    player->grounded = true;
}

static float ReadHorizontalInput(void)
{
    float input = 0.0f;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) input -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) input += 1.0f;

    if (IsGamepadAvailable(0)) {
        float axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        if (axis < -0.2f || axis > 0.2f) input = axis;
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) input = -1.0f;
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) input = 1.0f;
    }

    return input;
}

static bool JumpPressed(void)
{
    bool keyboardPressed =
        IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP);
    bool gamepadPressed = IsGamepadAvailable(0) &&
        IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);

    return keyboardPressed || gamepadPressed;
}

static void UpdatePlayer(Player *player, float deltaTime)
{
    float horizontalInput = ReadHorizontalInput();
    float acceleration = player->grounded ? PLAYER_ACCELERATION : AIR_ACCELERATION;

    if (horizontalInput != 0.0f) {
        player->velocity.x = MoveTowards(
            player->velocity.x,
            horizontalInput * PLAYER_SPEED,
            acceleration * deltaTime
        );
        player->facing = horizontalInput > 0.0f ? 1 : -1;
    } else {
        player->velocity.x = MoveTowards(
            player->velocity.x,
            0.0f,
            FRICTION * deltaTime
        );
    }

    if (JumpPressed()) player->jumpBufferTimer = JUMP_BUFFER_TIME;
    else player->jumpBufferTimer -= deltaTime;

    if (player->grounded) player->coyoteTimer = COYOTE_TIME;
    else player->coyoteTimer -= deltaTime;

    if (player->jumpBufferTimer > 0.0f && player->coyoteTimer > 0.0f) {
        player->velocity.y = -JUMP_SPEED;
        player->grounded = false;
        player->coyoteTimer = 0.0f;
        player->jumpBufferTimer = 0.0f;
    }

    if (!(IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W) || IsKeyDown(KEY_UP) ||
          (IsGamepadAvailable(0) &&
           IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))) &&
        player->velocity.y < -180.0f) {
        player->velocity.y = -180.0f;
    }

    float previousX = player->bounds.x;
    player->bounds.x += player->velocity.x * deltaTime;

    if (player->bounds.x < 0.0f) {
        player->bounds.x = 0.0f;
        player->velocity.x = 0.0f;
    } else if (player->bounds.x + player->bounds.width > WORLD_WIDTH) {
        player->bounds.x = WORLD_WIDTH - player->bounds.width;
        player->velocity.x = 0.0f;
    }

    for (int i = 0; i < platformCount; i++) {
        Rectangle platform = platforms[i];
        bool overlapsVertically =
            player->bounds.y < platform.y + platform.height &&
            player->bounds.y + player->bounds.height > platform.y;

        if (!overlapsVertically || !CheckCollisionRecs(player->bounds, platform)) continue;

        if (player->velocity.x > 0.0f &&
            previousX + player->bounds.width <= platform.x) {
            player->bounds.x = platform.x - player->bounds.width;
            player->velocity.x = 0.0f;
        } else if (player->velocity.x < 0.0f &&
                   previousX >= platform.x + platform.width) {
            player->bounds.x = platform.x + platform.width;
            player->velocity.x = 0.0f;
        }
    }

    float previousY = player->bounds.y;
    player->velocity.y += GRAVITY * deltaTime;
    player->bounds.y += player->velocity.y * deltaTime;
    player->grounded = false;

    for (int i = 0; i < platformCount; i++) {
        Rectangle platform = platforms[i];
        bool overlapsHorizontally =
            player->bounds.x < platform.x + platform.width &&
            player->bounds.x + player->bounds.width > platform.x;

        if (!overlapsHorizontally) continue;

        float previousBottom = previousY + player->bounds.height;
        float currentBottom = player->bounds.y + player->bounds.height;

        if (player->velocity.y >= 0.0f &&
            previousBottom <= platform.y &&
            currentBottom >= platform.y) {
            player->bounds.y = platform.y - player->bounds.height;
            player->velocity.y = 0.0f;
            player->grounded = true;
        } else if (player->velocity.y < 0.0f &&
                   previousY >= platform.y + platform.height &&
                   player->bounds.y <= platform.y + platform.height) {
            player->bounds.y = platform.y + platform.height;
            player->velocity.y = 0.0f;
        }
    }
}

static void DrawBackground(void)
{
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                           (Color){ 102, 190, 190, 255 },
                           (Color){ 215, 222, 166, 255 });
    DrawCircle(1040, 118, 52.0f, (Color){ 255, 224, 144, 190 });
}

static void DrawDistantJungle(Camera2D camera)
{
    camera.target.x *= 0.30f;
    BeginMode2D(camera);

    for (int i = 0; i < 14; i++) {
        float x = (float)i * 290.0f - 80.0f;
        float height = 150.0f + (float)((i * 47) % 90);
        DrawTriangle(
            (Vector2){ x, 625.0f },
            (Vector2){ x + 150.0f, 625.0f - height },
            (Vector2){ x + 300.0f, 625.0f },
            (Color){ 45, 111, 93, 255 }
        );
    }

    EndMode2D();
}

static void DrawPlatforms(void)
{
    for (int i = 0; i < platformCount; i++) {
        Rectangle platform = platforms[i];
        Rectangle soil = { platform.x, platform.y + 12.0f, platform.width, platform.height - 12.0f };
        Rectangle grass = { platform.x, platform.y, platform.width, 14.0f };

        DrawRectangleRec(soil, (Color){ 103, 76, 54, 255 });
        DrawRectangleRec(grass, (Color){ 81, 145, 75, 255 });
        DrawRectangle((int)platform.x, (int)platform.y + 12,
                      (int)platform.width, 5, (Color){ 122, 177, 92, 255 });

        if (platform.height > 40.0f) {
            for (float x = platform.x + 24.0f; x < platform.x + platform.width; x += 48.0f) {
                DrawLine((int)x, (int)platform.y + 30,
                         (int)x - 7, (int)platform.y + 61,
                         (Color){ 85, 61, 47, 255 });
            }
        }
    }
}

static void DrawPlayer(Player player)
{
    float centerX = player.bounds.x + player.bounds.width * 0.5f;
    float top = player.bounds.y;
    Color tunic = (Color){ 227, 113, 79, 255 };

    DrawEllipse((int)centerX, (int)(player.bounds.y + player.bounds.height),
                23.0f, 5.0f, (Color){ 25, 53, 46, 80 });
    DrawRectangle((int)(centerX - 13.0f), (int)(top + 21.0f), 26, 29, tunic);
    DrawCircle((int)centerX, (int)(top + 13.0f), 11.0f,
               (Color){ 235, 194, 145, 255 });
    DrawRectangle((int)(centerX - 13.0f), (int)(top + 5.0f), 26, 7,
                  (Color){ 63, 57, 52, 255 });
    DrawRectangle((int)(centerX - 16.0f), (int)(top + 45.0f), 12, 13,
                  (Color){ 67, 74, 68, 255 });
    DrawRectangle((int)(centerX + 4.0f), (int)(top + 45.0f), 12, 13,
                  (Color){ 67, 74, 68, 255 });
    DrawRectangle((int)(centerX + (float)(player.facing * 13)), (int)(top + 25.0f),
                  7, 18, (Color){ 228, 206, 144, 255 });
}

static void DrawHud(Player player)
{
    DrawRectangleRounded((Rectangle){ 22.0f, 20.0f, 398.0f, 92.0f },
                         0.16f, 8, (Color){ 24, 46, 43, 220 });
    DrawText("PALM ISLAND  |  MOVEMENT PROTOTYPE", 40, 37, 18, RAYWHITE);
    DrawText("Move: A/D or arrows    Jump: Space / W / Up", 40, 65, 15,
             (Color){ 223, 232, 203, 255 });
    DrawText("Gamepad: left stick / D-pad + A    R: restart", 40, 88, 14,
             (Color){ 197, 213, 185, 255 });

    if (player.grounded) {
        DrawText("Grounded", SCREEN_WIDTH - 125, 28, 16,
                 (Color){ 40, 77, 57, 255 });
    } else {
        DrawText("Airborne", SCREEN_WIDTH - 125, 28, 16,
                 (Color){ 40, 77, 57, 255 });
    }
}

int main(void)
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Palm Island - Movement Prototype");
    SetTargetFPS(60);

    Player player = { 0 };
    ResetPlayer(&player);

    Camera2D camera = { 0 };
    camera.offset = (Vector2){ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    camera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) ResetPlayer(&player);

        float deltaTime = GetFrameTime();
        if (deltaTime > 1.0f / 30.0f) deltaTime = 1.0f / 30.0f;

        UpdatePlayer(&player, deltaTime);
        if (player.bounds.y > SCREEN_HEIGHT + 260.0f) ResetPlayer(&player);

        float cameraX = player.bounds.x + player.bounds.width * 0.5f;
        if (cameraX < SCREEN_WIDTH * 0.5f) cameraX = SCREEN_WIDTH * 0.5f;
        if (cameraX > WORLD_WIDTH - SCREEN_WIDTH * 0.5f) {
            cameraX = WORLD_WIDTH - SCREEN_WIDTH * 0.5f;
        }
        camera.target = (Vector2){ cameraX, SCREEN_HEIGHT * 0.5f };

        BeginDrawing();
        DrawBackground();
        DrawDistantJungle(camera);
        BeginMode2D(camera);
        DrawPlatforms();
        DrawPlayer(player);
        EndMode2D();
        DrawHud(player);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
