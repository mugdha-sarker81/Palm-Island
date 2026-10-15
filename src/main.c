#include "raylib.h"
#include "tuning.h"
#include "input.h"
#include "player.h"
#include "room.h"
#include "interaction.h"
#include "camera.h"
#include "render.h"
#include "thornback.h"
#include "save.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
                             RoomArea area, bool thornGateOpen,
                             const Texture2D propTextures[ROOM_SPRITE_COUNT])
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
            .texture = thornGateOpen && area == ROOM_AREA_ROOT_PATH &&
                               roomProps[i].sprite == ROOM_SPRITE_THORN_GATE
                           ? (Texture2D){0}
                           : propTextures[roomProps[i].sprite]
        };
    }
}

static SaveData CurrentSaveData(RoomArea checkpointArea, int health,
                                const Inventory *inventory,
                                bool thornGateOpen)
{
    return (SaveData){
        .checkpointArea = checkpointArea,
        .health = health,
        .inventory = *inventory,
        .thornGateOpen = thornGateOpen
    };
}

static bool PersistProgress(const char *savePath, RoomArea checkpointArea,
                            int health, const Inventory *inventory,
                            bool thornGateOpen,
                            char *notice, size_t noticeCapacity,
                            float *noticeTime)
{
    SaveData data = CurrentSaveData(checkpointArea, health, inventory,
                                    thornGateOpen);
    SaveWriteResult result = Save_Write(savePath, &data);
    if (result == SAVE_WRITE_OK) {
        snprintf(notice, noticeCapacity, "GAME SAVED");
        *noticeTime = 1.5f;
        return true;
    }

    const char *reason = result == SAVE_WRITE_INVALID_DATA
                             ? "SAVE REJECTED - INVALID STATE"
                             : "SAVE FAILED - CHECK STORAGE";
    snprintf(notice, noticeCapacity, "%s", reason);
    *noticeTime = 0.0f;
    fprintf(stderr, "%s: %s\n", reason, savePath);
    return false;
}

static bool HasThornfang(const Inventory *inventory)
{
    return Inventory_HasWeapon(inventory, INVENTORY_WEAPON_THORNFANG);
}

static void RecordKeyItem(Inventory *inventory,
                          RoomInteractableKind interactionKind)
{
    switch (interactionKind) {
        case ROOM_INTERACTABLE_ROOT_WEAVE:
            Inventory_AddKeyItem(inventory, INVENTORY_KEY_ITEM_ROOT_WEAVE);
            break;
        case ROOM_INTERACTABLE_ROOT_MARKER:
            Inventory_AddKeyItem(
                inventory, INVENTORY_KEY_ITEM_ROOT_MARKER_TRACE);
            break;
        case ROOM_INTERACTABLE_CLEARING_NOTE:
            Inventory_AddKeyItem(
                inventory, INVENTORY_KEY_ITEM_CLEARING_NOTE);
            break;
        case ROOM_INTERACTABLE_ROOTS_MURAL:
            Inventory_AddKeyItem(inventory, INVENTORY_KEY_ITEM_ROOTS_MURAL);
            break;
        case ROOM_INTERACTABLE_THORNBACK_TRIAL:
        default:
            break;
    }
}

static bool FindSavePath(char *path, size_t capacity)
{
    const char *directory = getenv("APPDATA");
    if (directory == NULL || directory[0] == '\0') {
        directory = GetApplicationDirectory();
    }
    int length = snprintf(path, capacity, "%s/PalmIsland.sav", directory);
    return length >= 0 && (size_t)length < capacity;
}

static Vector2 FacingDirection(Facing facing)
{
    switch (facing) {
        case FACING_LEFT: return (Vector2){-1.0f, 0.0f};
        case FACING_RIGHT: return (Vector2){1.0f, 0.0f};
        case FACING_UP: return (Vector2){0.0f, -1.0f};
        case FACING_DOWN:
        default: return (Vector2){0.0f, 1.0f};
    }
}

static bool InteractionWasSeen(const InteractionState *state,
                               const RoomInteractable *target)
{
    switch (target->kind) {
        case ROOM_INTERACTABLE_ROOT_WEAVE: return state->rootWeaveSeen;
        case ROOM_INTERACTABLE_ROOT_MARKER: return state->rootMarkerActivated;
        case ROOM_INTERACTABLE_CLEARING_NOTE: return state->clearingNoteSeen;
        case ROOM_INTERACTABLE_ROOTS_MURAL: return state->rootsMuralSeen;
        case ROOM_INTERACTABLE_THORNBACK_TRIAL: return false;
        default: return false;
    }
}

static void DrawInteractionUI(const RoomInteractable *target, float promptFade,
                              const InteractionState *state)
{
    const Color panelColor = {11, 26, 34, 245};
    const Color edgeColor = {127, 196, 192, 255};

    if (state->panelOpen && target != NULL) {
        DrawRectangle(8, 124, 304, 48, Fade(panelColor, 0.96f));
        DrawRectangleLines(8, 124, 304, 48, edgeColor);
        DrawText(target->title, 14, 129, 10, (Color){245, 214, 124, 255});
        const char *text = target->kind == ROOM_INTERACTABLE_ROOT_MARKER &&
                           state->rootMarkerActivated &&
                           target->activatedText != NULL
                               ? target->activatedText : target->text;
        DrawText(text, 14, 142, 8, RAYWHITE);
        DrawText("E / A Close", 254, 161, 8,
                 (Color){127, 196, 192, 255});
    } else if (target != NULL && promptFade > 0.0f) {
        DrawRectangle(8, 154, 304, 18, Fade(panelColor, 0.94f * promptFade));
        DrawRectangleLines(8, 154, 304, 18,
                           Fade(edgeColor, promptFade));
        bool alreadyUsed = InteractionWasSeen(state, target);
        const char *action;
        if (target->kind == ROOM_INTERACTABLE_THORNBACK_TRIAL) {
            action = "Begin";
        } else if (target->kind == ROOM_INTERACTABLE_ROOT_MARKER) {
            action = alreadyUsed ? "Review" : "Activate";
        } else {
            action = alreadyUsed ? "Review" : "Read";
        }
        DrawText(TextFormat("E / A %s %s", action,
                            target->title),
                 14, 158, 8, Fade(RAYWHITE, promptFade));
    }
}

static void DrawInventoryUI(const Inventory *inventory)
{
    const Color panelColor = {11, 26, 34, 250};
    const Color edgeColor = {127, 196, 192, 255};
    const Color headingColor = {245, 214, 124, 255};
    const Color mutedColor = {164, 184, 181, 255};
    const char *keyItemNames[INVENTORY_KEY_ITEM_COUNT] = {
        "Living Thorn Weave",
        "Root Marker Trace",
        "Explorer's Note",
        "Map of Roots"
    };

    DrawRectangle(12, 12, 296, 156, panelColor);
    DrawRectangleLines(12, 12, 296, 156, edgeColor);
    DrawText("INVENTORY", 20, 18, 12, headingColor);
    DrawText("WEAPONS", 20, 38, 8, headingColor);
    if (Inventory_HasWeapon(inventory, INVENTORY_WEAPON_THORNFANG)) {
        DrawText(inventory->selectedWeapon == INVENTORY_WEAPON_THORNFANG
                     ? "> Thornfang  ACTIVE"
                     : "  Thornfang  OWNED",
                 24, 50, 8, RAYWHITE);
        DrawText("Cuts marked thorn barriers", 32, 61, 8, mutedColor);
    } else {
        DrawText("No weapons discovered", 24, 50, 8, mutedColor);
    }

    DrawText("KEY ITEMS / DISCOVERIES", 164, 38, 8, headingColor);
    for (int i = 0; i < INVENTORY_KEY_ITEM_COUNT; i++) {
        bool found = Inventory_HasKeyItem(inventory, (InventoryKeyItem)i);
        DrawText(TextFormat("%s %s", found ? "[x]" : "[ ]",
                            keyItemNames[i]),
                 168, 52 + i * 16, 8, found ? RAYWHITE : mutedColor);
    }
    DrawLine(20, 136, 300, 136, edgeColor);
    DrawText("I / TAB / Y  CLOSE", 20, 145, 8, RAYWHITE);
    DrawText("World paused", 226, 145, 8, mutedColor);
}

static void DrawRootMarkerRoute(void)
{
    const Color routeColor = {100, 219, 143, 220};
    DrawLineEx((Vector2){216.0f, 144.0f}, (Vector2){216.0f, 56.0f},
               2.0f, routeColor);
    DrawLineEx((Vector2){216.0f, 56.0f}, (Vector2){320.0f, 56.0f},
               2.0f, routeColor);
    DrawLineEx((Vector2){320.0f, 56.0f}, (Vector2){320.0f, 24.0f},
               2.0f, routeColor);
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
            "sprites/props/prop_shrine_door_open.png",
            "sprites/props/prop_root_bridge.png",
            "sprites/props/prop_thorn_gate_closed.png"
        };
        for (int i = 0; i < ROOM_SPRITE_COUNT; i++) {
            propTextures[i] = LoadTexture(AssetPath(propPaths[i]));
            if (propTextures[i].id == 0) assetsOk = false;
        }
    }

    InteractionState interaction = {0};
    Inventory inventory = {0};
    bool thornGateOpen = false;
    RoomArea checkpointArea = room.area;
    bool saveAvailable = false;
    bool saveError = false;
    char savePath[1024] = "";
    char saveNotice[64] = "";
    float saveNoticeTime = 0.0f;
    SaveData loaded = {0};
    SaveLoadResult loadResult = SAVE_LOAD_MISSING;
    if (FindSavePath(savePath, sizeof(savePath))) {
        saveAvailable = true;
        loadResult = Save_Load(savePath, &loaded);
        if (loadResult == SAVE_LOAD_LOADED ||
            loadResult == SAVE_LOAD_RECOVERED_BACKUP) {
            checkpointArea = loaded.checkpointArea;
            Room_SetArea(&room, checkpointArea);
            inventory = loaded.inventory;
            thornGateOpen = loaded.thornGateOpen;
            interaction.rootWeaveSeen = Inventory_HasKeyItem(
                &inventory, INVENTORY_KEY_ITEM_ROOT_WEAVE);
            interaction.rootMarkerActivated = Inventory_HasKeyItem(
                &inventory, INVENTORY_KEY_ITEM_ROOT_MARKER_TRACE);
            interaction.clearingNoteSeen = Inventory_HasKeyItem(
                &inventory, INVENTORY_KEY_ITEM_CLEARING_NOTE);
            interaction.rootsMuralSeen = Inventory_HasKeyItem(
                &inventory, INVENTORY_KEY_ITEM_ROOTS_MURAL);
            if (loadResult == SAVE_LOAD_RECOVERED_BACKUP) {
                snprintf(saveNotice, sizeof(saveNotice),
                         "SAVE RESTORED FROM BACKUP");
                saveNoticeTime = 3.0f;
                fprintf(stderr, "Recovered Palm Island progress from backup.\n");
            }
        } else if (loadResult == SAVE_LOAD_INVALID ||
                   loadResult == SAVE_LOAD_IO_ERROR) {
            saveAvailable = false;
            saveError = true;
            snprintf(saveNotice, sizeof(saveNotice),
                     loadResult == SAVE_LOAD_INVALID
                         ? "SAVE INVALID - NOT OVERWRITTEN"
                         : "SAVE LOAD FAILED - CHECK STORAGE");
            fprintf(stderr, "%s: %s\n", saveNotice, savePath);
        }
    } else {
        saveError = true;
        snprintf(saveNotice, sizeof(saveNotice), "SAVE PATH UNAVAILABLE");
        fputs("Could not create a path for Palm Island save data.\n", stderr);
    }

    Player_Init(&player, Room_SpawnPoint(checkpointArea));
    if (saveAvailable) {
        if (loadResult == SAVE_LOAD_LOADED ||
            loadResult == SAVE_LOAD_RECOVERED_BACKUP) {
            player.health = loaded.health;
            player.thornfangOwned = HasThornfang(&inventory);
            player.thornGateOpen = thornGateOpen;
            if (Room_CollidesAtElevationWithGate(
                    checkpointArea, Player_FeetCollider(&player),
                    player.elevation, thornGateOpen)) {
                saveAvailable = false;
                saveError = true;
                snprintf(saveNotice, sizeof(saveNotice),
                         "SAVE CHECKPOINT BLOCKED");
                fprintf(stderr, "%s: %s\n", saveNotice, savePath);
                checkpointArea = ROOM_AREA_JUNGLE_ENTRANCE;
                Room_SetArea(&room, checkpointArea);
                inventory = (Inventory){0};
                thornGateOpen = false;
                interaction = (InteractionState){0};
                Player_Init(&player, Room_SpawnPoint(checkpointArea));
            }
        } else if (loadResult == SAVE_LOAD_MISSING) {
            if (!PersistProgress(savePath, checkpointArea, player.health,
                                 &inventory,
                                 thornGateOpen, saveNotice,
                                 sizeof(saveNotice), &saveNoticeTime)) {
                saveAvailable = false;
                saveError = true;
            }
        }
    }

    RenderItem props[RENDER_PROP_COUNT];
    BuildRenderProps(props, room.area, thornGateOpen, propTextures);

    RenderTexture2D canvas = LoadRenderTexture(VIRTUAL_W, VIRTUAL_H);
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_POINT);
    if (room.tileset.id != 0) SetTextureFilter(room.tileset, TEXTURE_FILTER_POINT);
    if (sheet.id != 0)        SetTextureFilter(sheet, TEXTURE_FILTER_POINT);
    for (int i = 0; i < ROOM_SPRITE_COUNT; i++) {
        if (propTextures[i].id != 0) SetTextureFilter(propTextures[i], TEXTURE_FILTER_POINT);
    }

    CameraFollow camera;
    Camera_Init(&camera, player.pos, Room_Bounds(),
                (Vector2){(float)VIRTUAL_W, (float)VIRTUAL_H});
    Input input = {0};
    bool debug = false;
    double accumulator = 0.0;
    float worldTime = 0.0f;
    bool inventoryOpen = false;
    enum { TRANSITION_NONE, TRANSITION_FADE_OUT, TRANSITION_FADE_IN };
    int transitionPhase = TRANSITION_NONE;
    float transitionTime = 0.0f;
    RoomArea destinationArea = room.area;
    Vector2 destinationSpawn = player.pos;
    float interactBuffer = 0.0f;
    float attackBuffer = 0.0f;
    float attackFeedbackTime = 0.0f;
    float thornGateFeedbackTime = 0.0f;
    float retryFeedbackTime = 0.0f;
    float promptFade = 0.0f;
    Thornback thornback;
    Thornback_Init(&thornback);
    const RoomInteractable *activeInteractable = NULL;
    const RoomInteractable *promptInteractable = NULL;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F3)) debug = !debug;
        if (IsKeyPressed(KEY_F11)) ToggleBorderlessWindowed();

        /* Fixed-timestep simulation. Input is polled once per rendered frame. */
        float frame = GetFrameTime();
        if (frame > MAX_FRAME_TIME) frame = MAX_FRAME_TIME;
        Input_Poll(&input);
        if (input.inventoryPressed) {
            inventoryOpen = !inventoryOpen;
            interactBuffer = 0.0f;
            attackBuffer = 0.0f;
        }
        if (inventoryOpen) {
            accumulator = 0.0;
        } else {
            accumulator += frame;
        }
        if (!inventoryOpen && transitionPhase == TRANSITION_NONE &&
            input.interactPressed) {
            interactBuffer = INTERACT_INPUT_BUFFER;
        } else if (transitionPhase != TRANSITION_NONE) {
            interactBuffer = 0.0f;
        }
        if (!inventoryOpen && transitionPhase == TRANSITION_NONE &&
            input.attackPressed) {
            attackBuffer = THORNFANG_CUT_BUFFER;
        } else if (transitionPhase != TRANSITION_NONE) {
            attackBuffer = 0.0f;
        }

        int steps = 0;
        while (!inventoryOpen && accumulator >= SIM_DT &&
               steps < MAX_SIM_STEPS) {
            if (assetsOk) {
                if (transitionPhase == TRANSITION_NONE) {
                    player.thornfangOwned = HasThornfang(&inventory);
                    player.thornGateOpen = thornGateOpen;
                    Player_Update(&player, &input, SIM_DT, room.area);
                    Camera_Update(&camera, player.pos, player.vel, SIM_DT);
                    Vector2 facingDirection = FacingDirection(player.facing);
                    activeInteractable =
                        room.area == ROOM_AREA_THORNBACK_ARENA &&
                                (thornback.state != THORNBACK_STATE_DORMANT ||
                                 HasThornfang(&inventory))
                            ? NULL
                            : Room_FindInteractable(
                                  room.area, player.pos, facingDirection,
                                  INTERACT_PROMPT_RANGE);
                    if (activeInteractable != NULL) {
                        promptInteractable = activeInteractable;
                        promptFade += SIM_DT / INTERACT_PROMPT_FADE;
                        if (promptFade > 1.0f) promptFade = 1.0f;
                    } else {
                        promptFade -= SIM_DT / INTERACT_PROMPT_FADE;
                        if (promptFade <= 0.0f) {
                            promptFade = 0.0f;
                            promptInteractable = NULL;
                        }
                        InteractionState_Close(&interaction);
                    }

                    if (interactBuffer > 0.0f) {
                        const RoomInteractable *inRange = Room_FindInteractable(
                            room.area, player.pos, facingDirection,
                            INTERACT_RANGE);
                        if (inRange != NULL) {
                            if (inRange->kind ==
                                ROOM_INTERACTABLE_THORNBACK_TRIAL) {
                                if (!HasThornfang(&inventory)) {
                                    Thornback_Start(&thornback, player.pos);
                                }
                            } else {
                                InteractionState_Activate(&interaction,
                                                          inRange);
                                RecordKeyItem(&inventory, inRange->kind);
                                if (saveAvailable &&
                                    !PersistProgress(
                                        savePath, checkpointArea,
                                        player.health, &inventory,
                                        thornGateOpen,
                                        saveNotice, sizeof(saveNotice),
                                        &saveNoticeTime)) {
                                    saveAvailable = false;
                                    saveError = true;
                                }
                            }
                            interactBuffer = 0.0f;
                        } else {
                            interactBuffer -= SIM_DT;
                            if (interactBuffer < 0.0f) interactBuffer = 0.0f;
                        }
                    }

                    if (Room_GetTransition(&room, player.pos, &destinationArea,
                                           &destinationSpawn)) {
                        transitionPhase = TRANSITION_FADE_OUT;
                        transitionTime = 0.0f;
                        InteractionState_Close(&interaction);
                        activeInteractable = NULL;
                        promptInteractable = NULL;
                        promptFade = 0.0f;
                    }
                } else {
                    InteractionState_Close(&interaction);
                    activeInteractable = NULL;
                    promptInteractable = NULL;
                    promptFade = 0.0f;
                    transitionTime += SIM_DT;
                    if (transitionTime >= 0.2f) {
                        transitionTime = 0.0f;
                        if (transitionPhase == TRANSITION_FADE_OUT) {
                            int health = player.health;
                            float damageInvulnerability =
                                player.damageInvulnerability;
                            Room_SetArea(&room, destinationArea);
                            checkpointArea = destinationArea;
                            Thornback_Init(&thornback);
                            Player_Init(&player, destinationSpawn);
                            player.health = health;
                            player.damageInvulnerability =
                                damageInvulnerability;
                            player.thornfangOwned = HasThornfang(&inventory);
                            player.thornGateOpen = thornGateOpen;
                            if (saveAvailable &&
                                !PersistProgress(
                                    savePath, checkpointArea, player.health,
                                    &inventory,
                                    thornGateOpen, saveNotice,
                                    sizeof(saveNotice), &saveNoticeTime)) {
                                saveAvailable = false;
                                saveError = true;
                            }
                            Camera_Init(&camera, player.pos, Room_Bounds(),
                                        (Vector2){(float)VIRTUAL_W, (float)VIRTUAL_H});
                            BuildRenderProps(props, room.area, thornGateOpen,
                                             propTextures);
                            transitionPhase = TRANSITION_FADE_IN;
                        } else {
                            transitionPhase = TRANSITION_NONE;
                        }
                    }
                }
            }
            if (room.area == ROOM_AREA_THORNBACK_ARENA &&
                transitionPhase == TRANSITION_NONE) {
                Thornback_Update(&thornback, SIM_DT);
                if (thornback.state == THORNBACK_STATE_DEFEATED &&
                    !HasThornfang(&inventory)) {
                    Inventory_AddWeapon(&inventory,
                                        INVENTORY_WEAPON_THORNFANG);
                    player.thornfangOwned = true;
                    if (saveAvailable &&
                        !PersistProgress(
                            savePath, checkpointArea, player.health,
                            &inventory, thornGateOpen,
                            saveNotice, sizeof(saveNotice),
                            &saveNoticeTime)) {
                        saveAvailable = false;
                        saveError = true;
                    }
                } else if (Thornback_TryPlayerContact(&thornback,
                                                     player.pos)) {
                    Player_TakeDamage(&player, thornback.position,
                                      room.area);
                    if (player.health <= 0) {
                        Player_Init(&player, Room_SpawnPoint(
                                                 ROOM_AREA_THORNBACK_ARENA));
                        player.thornfangOwned = HasThornfang(&inventory);
                        player.thornGateOpen = thornGateOpen;
                        Thornback_Init(&thornback);
                        Camera_Init(&camera, player.pos, Room_Bounds(),
                                    (Vector2){(float)VIRTUAL_W,
                                              (float)VIRTUAL_H});
                        attackBuffer = 0.0f;
                        attackFeedbackTime = 0.0f;
                        retryFeedbackTime = 1.5f;
                        if (saveAvailable &&
                            !PersistProgress(
                                savePath, checkpointArea, player.health,
                                &inventory,
                                thornGateOpen, saveNotice,
                                sizeof(saveNotice), &saveNoticeTime)) {
                            saveAvailable = false;
                            saveError = true;
                        }
                    }
                }
                if (attackBuffer > 0.0f) {
                    if (Thornback_TryHit(&thornback, player.pos,
                                         FacingDirection(player.facing))) {
                        attackFeedbackTime = 0.25f;
                        attackBuffer = 0.0f;
                    } else {
                        attackBuffer -= SIM_DT;
                        if (attackBuffer < 0.0f) attackBuffer = 0.0f;
                    }
                }
            } else if (room.area == ROOM_AREA_ROOT_PATH &&
                       transitionPhase == TRANSITION_NONE) {
                if (attackBuffer > 0.0f) {
                    if (Room_TryCutThornGate(
                            room.area, player.pos,
                            FacingDirection(player.facing),
                            HasThornfang(&inventory),
                            &thornGateOpen)) {
                        player.thornGateOpen = true;
                        BuildRenderProps(props, room.area, thornGateOpen,
                                         propTextures);
                        thornGateFeedbackTime =
                            THORNFANG_CUT_FEEDBACK_TIME;
                        if (saveAvailable &&
                            !PersistProgress(
                                savePath, checkpointArea, player.health,
                                &inventory,
                                thornGateOpen, saveNotice,
                                sizeof(saveNotice), &saveNoticeTime)) {
                            saveAvailable = false;
                            saveError = true;
                        }
                        attackBuffer = 0.0f;
                    } else {
                        attackBuffer -= SIM_DT;
                        if (attackBuffer < 0.0f) attackBuffer = 0.0f;
                    }
                }
            } else {
                attackBuffer = 0.0f;
            }
            if (attackFeedbackTime > 0.0f) {
                attackFeedbackTime -= SIM_DT;
                if (attackFeedbackTime < 0.0f) attackFeedbackTime = 0.0f;
            }
            if (thornGateFeedbackTime > 0.0f) {
                thornGateFeedbackTime -= SIM_DT;
                if (thornGateFeedbackTime < 0.0f) {
                    thornGateFeedbackTime = 0.0f;
                }
            }
            if (retryFeedbackTime > 0.0f) {
                retryFeedbackTime -= SIM_DT;
                if (retryFeedbackTime < 0.0f) retryFeedbackTime = 0.0f;
            }
            if (saveNoticeTime > 0.0f) {
                saveNoticeTime -= SIM_DT;
                if (saveNoticeTime < 0.0f) saveNoticeTime = 0.0f;
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
            Room_Draw(&room, worldTime, thornGateOpen);
            if (room.area == ROOM_AREA_ROOT_PATH &&
                interaction.rootMarkerActivated) {
                DrawRootMarkerRoute();
            }
            if (room.area == ROOM_AREA_THORNBACK_ARENA) {
                Thornback_DrawWarning(&thornback);
            }
            Render_DrawDepthScene(
                &player, sheet, props,
                room.area == ROOM_AREA_THORNBACK_ARENA ? &thornback : NULL,
                debug);
            EndMode2D();
            if (transitionPhase != TRANSITION_NONE) {
                float alpha = transitionTime / 0.2f;
                if (transitionPhase == TRANSITION_FADE_IN) alpha = 1.0f - alpha;
                if (alpha < 0.0f) alpha = 0.0f;
                if (alpha > 1.0f) alpha = 1.0f;
                DrawRectangle(0, 0, VIRTUAL_W, VIRTUAL_H,
                              Fade(BLACK, alpha));
            }
            DrawInteractionUI(promptInteractable, promptFade,
                              &interaction);
            bool thornGatePrompt = Room_IsFacingThornGate(
                room.area, player.pos, FacingDirection(player.facing),
                thornGateOpen);
            if (thornGatePrompt) {
                DrawRectangle(8, 154, 304, 18, (Color){11, 26, 34, 240});
                DrawRectangleLines(8, 154, 304, 18,
                                   (Color){127, 196, 192, 255});
                DrawText(HasThornfang(&inventory) ? "SPACE / X CUT THORNS"
                                           : "MARKED VINES - THORNFANG NEEDED",
                         14, 158, 8, RAYWHITE);
            }
            DrawText(TextFormat("HP %d/%d", player.health,
                                PLAYER_START_HEALTH),
                     8, 8, 8, (Color){245, 214, 124, 255});
            if (saveError || saveNoticeTime > 0.0f) {
                DrawText(saveNotice, 8, 170, 8,
                         saveError ? (Color){255, 139, 115, 255}
                                   : (Color){127, 196, 192, 255});
            }
            if (room.area == ROOM_AREA_ROOT_PATH && thornGateOpen &&
                thornGateFeedbackTime > 0.0f) {
                DrawRectangle(6, 22, 136, 15, (Color){11, 26, 34, 220});
                DrawText("THORNS CUT - PATH OPEN", 10, 26, 8,
                         (Color){127, 196, 192, 255});
            }
            if (room.area == ROOM_AREA_THORNBACK_ARENA &&
                retryFeedbackTime > 0.0f) {
                DrawRectangle(6, 22, 210, 15, (Color){11, 26, 34, 220});
                DrawText("TRIAL RESET - RETURN TO MARKER", 10, 26, 8,
                         (Color){245, 214, 124, 255});
            } else if (room.area == ROOM_AREA_THORNBACK_ARENA &&
                HasThornfang(&inventory)) {
                DrawRectangle(6, 22, 146, 15, (Color){11, 26, 34, 220});
                DrawText("THORNFANG CLAIMED", 10, 26, 8,
                         (Color){127, 196, 192, 255});
            } else if (room.area == ROOM_AREA_THORNBACK_ARENA &&
                       thornback.state != THORNBACK_STATE_DORMANT) {
                const char *cue = "WATCH THE LINE - MOVE CLEAR";
                if (thornback.state == THORNBACK_STATE_CHARGE) {
                    cue = "CHARGE!";
                } else if (thornback.state ==
                           THORNBACK_STATE_SWEEP_TELL) {
                    cue = "SWEEP TELL - STEP CLEAR";
                } else if (thornback.state == THORNBACK_STATE_SWEEP) {
                    cue = "SWEEP - MOVE CLEAR";
                } else if (thornback.state == THORNBACK_STATE_RECOVERY) {
                    cue = attackFeedbackTime > 0.0f
                              ? "THORNBACK HIT!"
                              : "RECOVERY - FACE + SPACE / X";
                }
                DrawRectangle(6, 22, 210, 15, (Color){11, 26, 34, 220});
                DrawText(cue, 10, 26, 8, (Color){245, 214, 124, 255});
                DrawText(TextFormat("THORNBACK %d/%d", thornback.health,
                                    THORNBACK_HEALTH),
                         224, 26, 8, (Color){245, 214, 124, 255});
            }
            if (inventoryOpen) DrawInventoryUI(&inventory);
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
