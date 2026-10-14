#include "raylib.h"
#include "tuning.h"
#include "input.h"
#include "player.h"
#include "room.h"
#include "camera.h"
#include "render.h"
#include <math.h>
#include <stdio.h>

/* ---- asset lookup: works from the repo root, build/, or next to the exe ---- */
static char gAssetRoot[512] = "";

static bool FindAssetRoot(void)
{
    char candidate[512];
    const char *app = GetApplicationDirectory();
    const char *prefixes[] = { "", "../", "../../", NULL };

    for (int i = 0; prefixes[i]; i++) {
        snprintf(candidate, sizeof(candidate), "%sassets/tilesets/tileset_16.png", prefixes[i]);
        if (FileExists(candidate)) {
            snprintf(gAssetRoot, sizeof(gAssetRoot), "%sassets", prefixes[i]);
            return true;
        }
    }
    for (int i = 0; prefixes[i]; i++) {
        snprintf(candidate, sizeof(candidate), "%s%sassets/tilesets/tileset_16.png", app, prefixes[i]);
        if (FileExists(candidate)) {
            snprintf(gAssetRoot, sizeof(gAssetRoot), "%s%sassets", app, prefixes[i]);
            return true;
        }
    }
    return false;
}

static const char *AssetPath(const char *relative)
{
    return TextFormat("%s/%s", gAssetRoot, relative);
}

/* ---- integer-scaled, letterboxed destination for the 320x180 canvas ---- */
static Rectangle CanvasDestination(void)
{
    int sw = GetScreenWidth(), sh = GetScreenHeight();
    int scale = sw / VIRTUAL_W;
    int scaleY = sh / VIRTUAL_H;
    if (scaleY < scale) scale = scaleY;
    if (scale < 1) scale = 1;
    float w = (float)(VIRTUAL_W * scale), h = (float)(VIRTUAL_H * scale);
    return (Rectangle){ floorf((sw - w) * 0.5f), floorf((sh - h) * 0.5f), w, h };
}

static void BuildRenderProps(RenderItem renderProps[RENDER_PROP_COUNT],
                             RoomArea area)
{
    const RoomProp *roomProps = Room_Props(area);
    for (int i = 0; i < RENDER_PROP_COUNT; i++) {
        renderProps[i] = (RenderItem){
            .type = RENDER_ITEM_PROP,
            .position = roomProps[i].drawPosition,
            .feetY = roomProps[i].feetY,
            .elevation = roomProps[i].elevation,
            .stableOrder = (unsigned int)(i + 1),
            .foreground = roomProps[i].foreground,
            .texture = (Texture2D){0}
        };
    }
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Palm Island");
    SetWindowMinSize(VIRTUAL_W, VIRTUAL_H);
    SetTargetFPS(240); /* safety cap if vsync is unavailable */
    SetExitKey(KEY_ESCAPE);

    Room room = {0};
    Player player;
    Texture2D sheet = {0};
    Texture2D propTextures[ROOM_SPRITE_COUNT] = {0};
    bool assetsOk = FindAssetRoot();
    if (assetsOk) {
        assetsOk = Room_Load(&room, AssetPath("tilesets/tileset_16.png"));
        sheet = LoadTexture(AssetPath("sprites/player_sheet_14x20.png"));
        if (sheet.id == 0) assetsOk = false;
        const char *propPaths[ROOM_SPRITE_COUNT] = {
            "sprites/props/prop_palm.png",
            "sprites/props/prop_rock.png",
            "sprites/props/prop_marker.png",
            "sprites/props/prop_tall_grass.png",
            "sprites/props/prop_shrine_door_closed.png",
            "sprites/props/prop_root_bridge.png",
            "sprites/props/prop_thorn_gate_closed.png"
        };
        for (int i = 0; i < ROOM_SPRITE_COUNT; i++) {
            propTextures[i] = LoadTexture(AssetPath(propPaths[i]));
            if (propTextures[i].id == 0) assetsOk = false;
        }
    }

    RenderItem props[RENDER_PROP_COUNT];
    BuildRenderProps(props, room.area);
    const RoomProp *roomProps = Room_Props(room.area);
    for (int i = 0; i < RENDER_PROP_COUNT; i++) {
        props[i].texture = propTextures[roomProps[i].sprite];
    }

    RenderTexture2D canvas = LoadRenderTexture(VIRTUAL_W, VIRTUAL_H);
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_POINT);
    if (room.tileset.id != 0) SetTextureFilter(room.tileset, TEXTURE_FILTER_POINT);
    if (sheet.id != 0)        SetTextureFilter(sheet, TEXTURE_FILTER_POINT);
    for (int i = 0; i < ROOM_SPRITE_COUNT; i++) {
        if (propTextures[i].id != 0) SetTextureFilter(propTextures[i], TEXTURE_FILTER_POINT);
    }

    Player_Init(&player, Room_SpawnPoint(room.area));
    CameraFollow camera;
    Camera_Init(&camera, player.pos, Room_Bounds(),
                (Vector2){(float)VIRTUAL_W, (float)VIRTUAL_H});
    Input input = {0};
    bool debug = false;
    double accumulator = 0.0;
    float worldTime = 0.0f;
    enum { TRANSITION_NONE, TRANSITION_FADE_OUT, TRANSITION_FADE_IN };
    int transitionPhase = TRANSITION_NONE;
    float transitionTime = 0.0f;
    RoomArea destinationArea = room.area;
    Vector2 destinationSpawn = player.pos;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F3)) debug = !debug;
        if (IsKeyPressed(KEY_F11)) ToggleBorderlessWindowed();

        /* Fixed-timestep simulation. Input is polled once per rendered frame. */
        float frame = GetFrameTime();
        if (frame > MAX_FRAME_TIME) frame = MAX_FRAME_TIME;
        accumulator += frame;
        Input_Poll(&input);

        int steps = 0;
        while (accumulator >= SIM_DT && steps < MAX_SIM_STEPS) {
            if (assetsOk) {
                if (transitionPhase == TRANSITION_NONE) {
                    Player_Update(&player, &input, SIM_DT, room.area);
                    Camera_Update(&camera, player.pos, player.vel, SIM_DT);
                    if (Room_GetTransition(&room, player.pos, &destinationArea,
                                           &destinationSpawn)) {
                        transitionPhase = TRANSITION_FADE_OUT;
                        transitionTime = 0.0f;
                    }
                } else {
                    transitionTime += SIM_DT;
                    if (transitionTime >= 0.2f) {
                        transitionTime = 0.0f;
                        if (transitionPhase == TRANSITION_FADE_OUT) {
                            Room_SetArea(&room, destinationArea);
                            Player_Init(&player, destinationSpawn);
                            Camera_Init(&camera, player.pos, Room_Bounds(),
                                        (Vector2){(float)VIRTUAL_W, (float)VIRTUAL_H});
                            BuildRenderProps(props, room.area);
                            roomProps = Room_Props(room.area);
                            for (int i = 0; i < RENDER_PROP_COUNT; i++) {
                                props[i].texture = propTextures[roomProps[i].sprite];
                            }
                            transitionPhase = TRANSITION_FADE_IN;
                        } else {
                            transitionPhase = TRANSITION_NONE;
                        }
                    }
                }
            }
            worldTime += SIM_DT;
            accumulator -= SIM_DT;
            steps++;
        }
        if (steps == MAX_SIM_STEPS) accumulator = 0.0; /* drop leftover after a stall */

        /* ---- draw the world into the 320x180 canvas ---- */
        BeginTextureMode(canvas);
        ClearBackground((Color){11, 26, 34, 255});
        if (assetsOk) {
            BeginMode2D(Camera_Render(&camera));
            Room_Draw(&room, worldTime);
            Render_DrawDepthScene(&player, sheet, props, debug);
            EndMode2D();
            if (transitionPhase != TRANSITION_NONE) {
                float alpha = transitionTime / 0.2f;
                if (transitionPhase == TRANSITION_FADE_IN) alpha = 1.0f - alpha;
                if (alpha < 0.0f) alpha = 0.0f;
                if (alpha > 1.0f) alpha = 1.0f;
                DrawRectangle(0, 0, VIRTUAL_W, VIRTUAL_H,
                              Fade(BLACK, alpha));
            }
        } else {
            DrawText("Assets not found.", 8, 8, 10, WHITE);
            DrawText("Run from the repo root or keep", 8, 24, 10, WHITE);
            DrawText("the assets/ folder next to build/.", 8, 36, 10, WHITE);
        }
        EndTextureMode();

        /* ---- present: integer scale, point filter, letterbox ---- */
        BeginDrawing();
        ClearBackground(BLACK);
        Rectangle src = {0.0f, 0.0f, (float)VIRTUAL_W, -(float)VIRTUAL_H}; /* render textures are flipped */
        DrawTexturePro(canvas.texture, src, CanvasDestination(), (Vector2){0, 0}, 0.0f, WHITE);
        if (debug) {
            DrawText(TextFormat("FPS %d  frame %.2f ms", GetFPS(), GetFrameTime() * 1000.0f), 10, 10, 20, RAYWHITE);
            DrawText(TextFormat("speed %.2f px/s  (walk %.0f)", Player_Speed(&player), WALK_SPEED), 10, 34, 20, RAYWHITE);
            DrawText(TextFormat("input %.2f %s", input.magnitude, input.usingGamepad ? "gamepad" : "keys"), 10, 58, 20, RAYWHITE);
            DrawText(TextFormat("feet %.1f, %.1f", player.pos.x, player.pos.y), 10, 82, 20, RAYWHITE);
            DrawText(player.elevation == ROOM_ELEVATION_UPPER ? "elevation upper" : "elevation ground",
                     10, 106, 20, RAYWHITE);
        }
        EndDrawing();
    }

    if (sheet.id != 0) UnloadTexture(sheet);
    for (int i = 0; i < ROOM_SPRITE_COUNT; i++) {
        if (propTextures[i].id != 0) UnloadTexture(propTextures[i]);
    }
    Room_Unload(&room);
    UnloadRenderTexture(canvas);
    CloseWindow();
    return 0;
}
