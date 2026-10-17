#include "raylib.h"
#include "tuning.h"
#include "input.h"
#include "player.h"
#include "room.h"
#include "interaction.h"
#include "camera.h"
#include "render.h"
#include "thornback.h"
#include "ash_skitter.h"
#include "jungle_task.h"
#include "save.h"
#include "world_map.h"
#include "root_mite.h"
#include "game_ui.h"
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
                                bool thornGateOpen,
                                uint32_t collectedFoodNodes)
{
    return (SaveData){
        .checkpointArea = checkpointArea,
        .health = health,
        .inventory = *inventory,
        .thornGateOpen = thornGateOpen,
        .collectedFoodNodes = collectedFoodNodes
    };
}

static bool PersistProgress(const char *savePath, RoomArea checkpointArea,
                            int health, const Inventory *inventory,
                            bool thornGateOpen,
                            uint32_t collectedFoodNodes,
                            char *notice, size_t noticeCapacity,
                            float *noticeTime)
{
    SaveData data = CurrentSaveData(checkpointArea, health, inventory,
                                    thornGateOpen, collectedFoodNodes);
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

static bool HasCombatWeapon(const Inventory *inventory)
{
    return Inventory_HasWeapon(inventory, inventory->selectedWeapon) &&
           (inventory->selectedWeapon == INVENTORY_WEAPON_PALMWOOD_CLUB ||
            inventory->selectedWeapon == INVENTORY_WEAPON_THORNFANG);
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
        case ROOM_INTERACTABLE_CRACKED_MINERAL_SEAL:
            Inventory_AddKeyItem(
                inventory, INVENTORY_KEY_ITEM_CRACKED_MINERAL_SEAL);
            break;
        case ROOM_INTERACTABLE_COOLED_MINERAL_SEAM:
            Inventory_AddKeyItem(
                inventory, INVENTORY_KEY_ITEM_COOLED_MINERAL_SEAM);
            break;
        case ROOM_INTERACTABLE_THORNBACK_TRIAL:
        case ROOM_INTERACTABLE_PALM_FRUIT:
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

static void DrawWeaponHitEffect(Vector2 position, Facing facing,
                                InventoryWeapon weapon, float remainingTime)
{
    const float effectDuration = 0.25f;
    if (remainingTime <= 0.0f || remainingTime > effectDuration) return;

    float alpha = remainingTime / effectDuration;
    Vector2 forward = FacingDirection(facing);
    Vector2 side = {-forward.y, forward.x};
    Vector2 start = {
        position.x + forward.x * 5.0f - side.x * 6.0f,
        position.y + forward.y * 5.0f - side.y * 6.0f
    };
    Vector2 tip = {
        position.x + forward.x * 13.0f,
        position.y + forward.y * 13.0f
    };
    Vector2 end = {
        position.x + forward.x * 5.0f + side.x * 6.0f,
        position.y + forward.y * 5.0f + side.y * 6.0f
    };
    Color color = weapon == INVENTORY_WEAPON_THORNFANG
                      ? (Color){114, 220, 133, 255}
                      : (Color){245, 214, 124, 255};
    color.a = (unsigned char)(255.0f * alpha);

    DrawLineEx(start, tip, 3.0f, Fade(color, 0.4f));
    DrawLineEx(tip, end, 3.0f, Fade(color, 0.4f));
    DrawLineEx(start, tip, 1.0f, color);
    DrawLineEx(tip, end, 1.0f, color);
}

static ZoneId CampaignZoneForRoom(RoomArea area)
{
    switch (area) {
        case ROOM_AREA_JUNGLE_ENTRANCE: return Z_JUNGLE_ENTRANCE;
        case ROOM_AREA_ROOT_PATH: return Z_ROOT_PATH;
        case ROOM_AREA_OVERGROWN_CLEARING: return Z_OVERGROWN_CLEARING;
        case ROOM_AREA_SHRINE_APPROACH:
        case ROOM_AREA_THORNBACK_ARENA:
            return Z_SHRINE_APPROACH;
        case ROOM_AREA_ASHFALL_DESCENT: return Z_ASHFALL_DESCENT;
        case ROOM_AREA_COUNT:
        default:
            return Z_WRECKSHORE;
    }
}

static RenderItem PalmFruitRenderItem(RoomArea area)
{
    Vector2 position = Room_PalmFruitPosition(area);
    return (RenderItem){
        .type = RENDER_ITEM_FOOD,
        .position = position,
        .feetY = position.y,
        .elevation = ROOM_ELEVATION_GROUND,
        .stableOrder = RENDER_PROP_COUNT + 2,
        .foreground = false
    };
}

int main(void)
{
    if (!world_validate()) {
        fputs("Campaign world map validation failed; refusing to start.\n",
              stderr);
        return EXIT_FAILURE;
    }

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
    uint32_t collectedFoodNodes = ROOM_FOOD_NODE_NONE;
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
            collectedFoodNodes = loaded.collectedFoodNodes;
            interaction.rootWeaveSeen = Inventory_HasKeyItem(
                &inventory, INVENTORY_KEY_ITEM_ROOT_WEAVE);
            interaction.rootMarkerActivated = Inventory_HasKeyItem(
                &inventory, INVENTORY_KEY_ITEM_ROOT_MARKER_TRACE);
            interaction.clearingNoteSeen = Inventory_HasKeyItem(
                &inventory, INVENTORY_KEY_ITEM_CLEARING_NOTE);
            interaction.rootsMuralSeen = Inventory_HasKeyItem(
                &inventory, INVENTORY_KEY_ITEM_ROOTS_MURAL);
            interaction.crackedMineralSealSeen = Inventory_HasKeyItem(
                &inventory, INVENTORY_KEY_ITEM_CRACKED_MINERAL_SEAL);
            interaction.cooledMineralSeamSeen = Inventory_HasKeyItem(
                &inventory, INVENTORY_KEY_ITEM_COOLED_MINERAL_SEAM);
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
                collectedFoodNodes = ROOM_FOOD_NODE_NONE;
                interaction = (InteractionState){0};
                Player_Init(&player, Room_SpawnPoint(checkpointArea));
            }
        }
    }

    bool starterWeaponGranted =
        !Inventory_HasWeapon(&inventory, INVENTORY_WEAPON_PALMWOOD_CLUB);
    if (starterWeaponGranted) {
        Inventory_AddWeapon(&inventory, INVENTORY_WEAPON_PALMWOOD_CLUB);
    }
    if (saveAvailable &&
        (loadResult == SAVE_LOAD_MISSING || starterWeaponGranted) &&
        !PersistProgress(savePath, checkpointArea, player.health, &inventory,
                         thornGateOpen, collectedFoodNodes, saveNotice,
                         sizeof(saveNotice), &saveNoticeTime)) {
        saveAvailable = false;
        saveError = true;
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
    bool helpOpen = false;
    bool mapOpen = false;
    enum { TRANSITION_NONE, TRANSITION_FADE_OUT, TRANSITION_FADE_IN };
    int transitionPhase = TRANSITION_NONE;
    float transitionTime = 0.0f;
    RoomArea destinationArea = room.area;
    Vector2 destinationSpawn = player.pos;
    float interactBuffer = 0.0f;
    float attackBuffer = 0.0f;
    float foodUseBuffer = 0.0f;
    const char *pickupNotice = NULL;
    float pickupNoticeTime = 0.0f;
    float attackFeedbackTime = 0.0f;
    float thornGateFeedbackTime = 0.0f;
    float retryFeedbackTime = 0.0f;
    float ashfallRetryFeedbackTime = 0.0f;
    float rootPathRetryFeedbackTime = 0.0f;
    float promptFade = 0.0f;
    Thornback thornback;
    Thornback_Init(&thornback);
    AshSkitter ashSkitter;
    AshSkitter_Init(&ashSkitter);
    RootMite rootMite;
    RootMite_Init(&rootMite,
                  (collectedFoodNodes & ROOM_FOOD_NODE_ROOT_PATH) != 0);
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
            if (inventoryOpen) {
                helpOpen = false;
                mapOpen = false;
            }
        }
        if (input.helpPressed) {
            helpOpen = !helpOpen;
            if (helpOpen) {
                inventoryOpen = false;
                mapOpen = false;
            }
        }
        if (input.mapPressed) {
            mapOpen = !mapOpen;
            if (mapOpen) {
                inventoryOpen = false;
                helpOpen = false;
            }
        }
        bool overlayOpen = inventoryOpen || helpOpen || mapOpen;
        if (overlayOpen) {
            interactBuffer = 0.0f;
            attackBuffer = 0.0f;
            foodUseBuffer = 0.0f;
        }
        if (overlayOpen) {
            accumulator = 0.0;
        } else {
            accumulator += frame;
        }
        if (!overlayOpen && transitionPhase == TRANSITION_NONE &&
            input.interactPressed) {
            interactBuffer = INTERACT_INPUT_BUFFER;
        } else if (transitionPhase != TRANSITION_NONE) {
            interactBuffer = 0.0f;
        }
        if (!overlayOpen && transitionPhase == TRANSITION_NONE &&
            input.attackPressed) {
            attackBuffer = THORNFANG_CUT_BUFFER;
        } else if (transitionPhase != TRANSITION_NONE) {
            attackBuffer = 0.0f;
        }
        if (!overlayOpen && transitionPhase == TRANSITION_NONE &&
            input.useFoodPressed) {
            foodUseBuffer = INTERACT_INPUT_BUFFER;
        } else if (transitionPhase != TRANSITION_NONE) {
            foodUseBuffer = 0.0f;
        }

        int steps = 0;
        while (!overlayOpen && accumulator >= SIM_DT &&
               steps < MAX_SIM_STEPS) {
            if (assetsOk) {
                if (transitionPhase == TRANSITION_NONE) {
                    player.thornfangOwned = HasThornfang(&inventory);
                    player.thornGateOpen = thornGateOpen;
                    Player_Update(&player, &input, SIM_DT, room.area);
                    Camera_Update(&camera, player.pos, player.vel, SIM_DT);
                    Vector2 ventSource;
                    if (Room_VentHazardAt(room.area, player.pos, worldTime,
                                          player.elevation, &ventSource)) {
                        (void)Player_TakeDamage(&player, ventSource,
                                               room.area);
                        if (player.health <= 0) {
                            Player_Init(&player, Room_SpawnPoint(room.area));
                            player.thornfangOwned =
                                HasThornfang(&inventory);
                            player.thornGateOpen = thornGateOpen;
                            AshSkitter_Init(&ashSkitter);
                            Camera_Init(&camera, player.pos, Room_Bounds(),
                                        (Vector2){(float)VIRTUAL_W,
                                                  (float)VIRTUAL_H});
                            interactBuffer = 0.0f;
                            attackBuffer = 0.0f;
                            foodUseBuffer = 0.0f;
                            ashfallRetryFeedbackTime = 1.5f;
                            if (saveAvailable &&
                                !PersistProgress(
                                    savePath, checkpointArea, player.health,
                                    &inventory, thornGateOpen,
                                    collectedFoodNodes, saveNotice,
                                    sizeof(saveNotice), &saveNoticeTime)) {
                                saveAvailable = false;
                                saveError = true;
                            }
                        }
                    }
                    if (foodUseBuffer > 0.0f) {
                        if (Inventory_ConsumeFood(
                                &inventory, &player.health,
                                PLAYER_START_HEALTH)) {
                            pickupNotice = "ATE PALM FRUIT";
                            pickupNoticeTime = 1.5f;
                            if (saveAvailable &&
                                !PersistProgress(
                                    savePath, checkpointArea, player.health,
                                    &inventory, thornGateOpen,
                                    collectedFoodNodes, saveNotice,
                                    sizeof(saveNotice), &saveNoticeTime)) {
                                saveAvailable = false;
                                saveError = true;
                            }
                        } else {
                            pickupNotice = inventory.foodServings > 0
                                               ? "HEALTH ALREADY FULL"
                                               : "NO FOOD TO EAT";
                            pickupNoticeTime = 1.2f;
                        }
                        foodUseBuffer = 0.0f;
                    }
                    uint32_t visibleFoodNodes = collectedFoodNodes;
                    if (room.area == ROOM_AREA_ROOT_PATH &&
                        rootMite.state != ROOT_MITE_STATE_DEFEATED) {
                        /* The drop stays hidden while the Root Mite lives. */
                        visibleFoodNodes |= ROOM_FOOD_NODE_ROOT_PATH;
                    }
                    Vector2 facingDirection = FacingDirection(player.facing);
                    activeInteractable =
                        room.area == ROOM_AREA_THORNBACK_ARENA &&
                                (thornback.state != THORNBACK_STATE_DORMANT ||
                                 HasThornfang(&inventory))
                            ? NULL
                            : Room_FindInteractableWithFruitState(
                                  room.area, player.pos, facingDirection,
                                  INTERACT_PROMPT_RANGE,
                                  visibleFoodNodes);
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
                        const RoomInteractable *inRange =
                            Room_FindInteractableWithFruitState(
                                room.area, player.pos, facingDirection,
                                INTERACT_RANGE, visibleFoodNodes);
                        if (inRange != NULL) {
                            if (inRange->kind ==
                                ROOM_INTERACTABLE_PALM_FRUIT) {
                                if (Inventory_AddFood(&inventory)) {
                                    collectedFoodNodes |=
                                        Room_FoodNodeForArea(room.area);
                                    pickupNotice = "PALM FRUIT +1";
                                    pickupNoticeTime = 1.5f;
                                    if (saveAvailable &&
                                        !PersistProgress(
                                            savePath, checkpointArea,
                                            player.health, &inventory,
                                            thornGateOpen,
                                            collectedFoodNodes,
                                            saveNotice, sizeof(saveNotice),
                                            &saveNoticeTime)) {
                                        saveAvailable = false;
                                        saveError = true;
                                    }
                                } else {
                                    pickupNotice = "FOOD PACK FULL";
                                    pickupNoticeTime = 1.5f;
                                }
                            } else if (inRange->kind ==
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
                                        thornGateOpen, collectedFoodNodes,
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
                            RootMite_Init(&rootMite,
                                          (collectedFoodNodes &
                                           ROOM_FOOD_NODE_ROOT_PATH) != 0);
                            Thornback_Init(&thornback);
                            AshSkitter_Init(&ashSkitter);
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
                                    thornGateOpen, collectedFoodNodes,
                                    saveNotice,
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
                            &inventory, thornGateOpen, collectedFoodNodes,
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
                                thornGateOpen, collectedFoodNodes, saveNotice,
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
                RootMite_Update(&rootMite, player.pos, SIM_DT);
                if (RootMite_TryPlayerContact(&rootMite, player.pos,
                                              player.elevation)) {
                    (void)Player_TakeDamage(&player, rootMite.position,
                                            room.area);
                    if (player.health <= 0) {
                        Player_Init(&player, Room_SpawnPoint(room.area));
                        player.thornfangOwned = HasThornfang(&inventory);
                        player.thornGateOpen = thornGateOpen;
                        RootMite_Init(&rootMite,
                                      (collectedFoodNodes &
                                       ROOM_FOOD_NODE_ROOT_PATH) != 0);
                        Camera_Init(&camera, player.pos, Room_Bounds(),
                                    (Vector2){(float)VIRTUAL_W,
                                              (float)VIRTUAL_H});
                        interactBuffer = 0.0f;
                        attackBuffer = 0.0f;
                        foodUseBuffer = 0.0f;
                        rootPathRetryFeedbackTime = 1.5f;
                        if (saveAvailable &&
                            !PersistProgress(
                                savePath, checkpointArea, player.health,
                                &inventory, thornGateOpen,
                                collectedFoodNodes, saveNotice,
                                sizeof(saveNotice), &saveNoticeTime)) {
                            saveAvailable = false;
                            saveError = true;
                        }
                    }
                }
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
                                thornGateOpen, collectedFoodNodes, saveNotice,
                                sizeof(saveNotice), &saveNoticeTime)) {
                            saveAvailable = false;
                            saveError = true;
                        }
                        attackBuffer = 0.0f;
                    } else if (RootMite_TryHit(
                                   &rootMite, player.pos,
                                   FacingDirection(player.facing),
                                   player.elevation,
                                   inventory.selectedWeapon)) {
                        attackFeedbackTime = 0.25f;
                        attackBuffer = 0.0f;
                    } else {
                        attackBuffer -= SIM_DT;
                        if (attackBuffer < 0.0f) attackBuffer = 0.0f;
                    }
                }
            } else if (room.area == ROOM_AREA_ASHFALL_DESCENT &&
                       transitionPhase == TRANSITION_NONE) {
                AshSkitter_Update(&ashSkitter, player.pos, SIM_DT);
                if (AshSkitter_TryPlayerContact(
                        &ashSkitter, player.pos, player.elevation)) {
                    (void)Player_TakeDamage(&player, ashSkitter.position,
                                            room.area);
                    if (player.health <= 0) {
                        Player_Init(&player, Room_SpawnPoint(room.area));
                        player.thornfangOwned = HasThornfang(&inventory);
                        player.thornGateOpen = thornGateOpen;
                        AshSkitter_Init(&ashSkitter);
                        Camera_Init(&camera, player.pos, Room_Bounds(),
                                    (Vector2){(float)VIRTUAL_W,
                                              (float)VIRTUAL_H});
                        interactBuffer = 0.0f;
                        attackBuffer = 0.0f;
                        foodUseBuffer = 0.0f;
                        ashfallRetryFeedbackTime = 1.5f;
                        if (saveAvailable &&
                            !PersistProgress(
                                savePath, checkpointArea, player.health,
                                &inventory, thornGateOpen,
                                collectedFoodNodes, saveNotice,
                                sizeof(saveNotice), &saveNoticeTime)) {
                            saveAvailable = false;
                            saveError = true;
                        }
                    }
                }
                if (attackBuffer > 0.0f) {
                    if (AshSkitter_TryHit(
                            &ashSkitter, player.pos,
                            FacingDirection(player.facing),
                            player.elevation,
                            inventory.selectedWeapon)) {
                        attackFeedbackTime = 0.25f;
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
            if (ashfallRetryFeedbackTime > 0.0f) {
                ashfallRetryFeedbackTime -= SIM_DT;
                if (ashfallRetryFeedbackTime < 0.0f) {
                    ashfallRetryFeedbackTime = 0.0f;
                }
            }
            if (rootPathRetryFeedbackTime > 0.0f) {
                rootPathRetryFeedbackTime -= SIM_DT;
                if (rootPathRetryFeedbackTime < 0.0f) {
                    rootPathRetryFeedbackTime = 0.0f;
                }
            }
            if (pickupNoticeTime > 0.0f) {
                pickupNoticeTime -= SIM_DT;
                if (pickupNoticeTime < 0.0f) pickupNoticeTime = 0.0f;
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
                GameUI_DrawRootMarkerRoute();
            }
            if (room.area == ROOM_AREA_THORNBACK_ARENA) {
                Thornback_DrawWarning(&thornback);
            }
            if (room.area == ROOM_AREA_ASHFALL_DESCENT) {
                GameUI_DrawAshfallSeal(
                    worldTime,
                    Inventory_HasKeyItem(
                        &inventory,
                        INVENTORY_KEY_ITEM_CRACKED_MINERAL_SEAL));
                GameUI_DrawCooledMineralSeam(
                    worldTime,
                    Inventory_HasKeyItem(
                        &inventory,
                        INVENTORY_KEY_ITEM_COOLED_MINERAL_SEAM));
            }
            if (room.area == ROOM_AREA_ASHFALL_DESCENT) {
                AshSkitter_DrawWarning(&ashSkitter);
            }
            if (room.area == ROOM_AREA_ROOT_PATH) {
                RootMite_DrawWarning(&rootMite);
            }
            RenderItem foodPickup;
            const RenderItem *foodPickupPtr = NULL;
            uint32_t roomFoodNode = Room_FoodNodeForArea(room.area);
            bool rootPathRewardHidden =
                room.area == ROOM_AREA_ROOT_PATH &&
                rootMite.state != ROOT_MITE_STATE_DEFEATED;
            if (roomFoodNode != ROOM_FOOD_NODE_NONE &&
                (collectedFoodNodes & roomFoodNode) == 0 &&
                !rootPathRewardHidden) {
                foodPickup = PalmFruitRenderItem(room.area);
                foodPickupPtr = &foodPickup;
            }
            Render_DrawDepthScene(
                &player, sheet, props,
                room.area == ROOM_AREA_THORNBACK_ARENA ? &thornback : NULL,
                room.area == ROOM_AREA_ASHFALL_DESCENT ? &ashSkitter : NULL,
                room.area == ROOM_AREA_ROOT_PATH ? &rootMite : NULL,
                foodPickupPtr, debug);
            DrawWeaponHitEffect(player.pos, player.facing,
                                inventory.selectedWeapon,
                                attackFeedbackTime);
            if (room.area == ROOM_AREA_THORNBACK_ARENA &&
                !HasThornfang(&inventory) &&
                thornback.state == THORNBACK_STATE_DORMANT) {
                GameUI_DrawArenaTrialLocator(worldTime);
            }
            EndMode2D();
            if (transitionPhase != TRANSITION_NONE) {
                float alpha = transitionTime / 0.2f;
                if (transitionPhase == TRANSITION_FADE_IN) alpha = 1.0f - alpha;
                if (alpha < 0.0f) alpha = 0.0f;
                if (alpha > 1.0f) alpha = 1.0f;
                DrawRectangle(0, 0, VIRTUAL_W, VIRTUAL_H,
                              Fade(BLACK, alpha));
            }
            GameUI_DrawInteraction(promptInteractable, promptFade,
                                   &interaction);
            bool objectiveCueVisible =
                room.area == ROOM_AREA_ASHFALL_DESCENT ||
                (room.area == ROOM_AREA_THORNBACK_ARENA &&
                 !HasThornfang(&inventory) &&
                 thornback.state == THORNBACK_STATE_DORMANT &&
                 retryFeedbackTime <= 0.0f) ||
                (room.area == ROOM_AREA_ROOT_PATH &&
                 thornGateOpen && thornGateFeedbackTime > 0.0f) ||
                (room.area == ROOM_AREA_ROOT_PATH &&
                 rootMite.state != ROOT_MITE_STATE_DEFEATED) ||
                (room.area == ROOM_AREA_ROOT_PATH &&
                 rootPathRetryFeedbackTime > 0.0f) ||
                (room.area == ROOM_AREA_THORNBACK_ARENA &&
                 (retryFeedbackTime > 0.0f ||
                  HasThornfang(&inventory) ||
                  thornback.state != THORNBACK_STATE_DORMANT));
            if (room.area == ROOM_AREA_ASHFALL_DESCENT) {
                const char *ashfallCue;
                if (ashSkitter.state == ASH_SKITTER_STATE_DORMANT) {
                    ashfallCue =
                        "ASHFALL: AMBER TELLS - ORANGE VENTS HARM";
                } else if (ashSkitter.state == ASH_SKITTER_STATE_TELL) {
                    ashfallCue = "SKITTER TELL - CLEAR THE GLOWING LINE";
                } else if (ashSkitter.state == ASH_SKITTER_STATE_DASH) {
                    ashfallCue = "SKITTER DASH - MOVE CLEAR";
                } else if (ashSkitter.state ==
                           ASH_SKITTER_STATE_RECOVERY) {
                    ashfallCue = attackFeedbackTime > 0.0f
                                     ? "SKITTER HIT!"
                                     : HasCombatWeapon(&inventory)
                                           ? "RECOVERY - FACE + SPACE / X"
                                           : "RECOVERY - WEAPON NEEDED";
                } else if (ashSkitter.state ==
                           ASH_SKITTER_STATE_DEFEATED) {
                    ashfallCue = "ASH SKITTER DOWN - ROUTE CLEAR";
                }
                DrawRectangle(6, 81, 194, 15, (Color){11, 26, 34, 220});
                DrawText(ashfallCue, 10, 85, 8,
                         (Color){245, 214, 124, 255});
                if (ashSkitter.state != ASH_SKITTER_STATE_DORMANT) {
                    DrawText(TextFormat("SKITTER %d/%d", ashSkitter.health,
                                        ASH_SKITTER_HEALTH),
                             234, 98, 8, RAYWHITE);
                }
            } else if (room.area == ROOM_AREA_THORNBACK_ARENA &&
                       !HasThornfang(&inventory) &&
                       thornback.state == THORNBACK_STATE_DORMANT &&
                       retryFeedbackTime <= 0.0f) {
                DrawRectangle(6, 81, 194, 15, (Color){11, 26, 34, 220});
                DrawText("ARENA TRIAL: WEST-CENTER GLOWING MARKER",
                         10, 85, 8, (Color){245, 214, 124, 255});
            } else if (room.area == ROOM_AREA_ROOT_PATH &&
                       rootMite.state != ROOT_MITE_STATE_DEFEATED &&
                       thornGateFeedbackTime <= 0.0f &&
                       rootPathRetryFeedbackTime <= 0.0f) {
                const char *miteCue;
                if (rootMite.state == ROOT_MITE_STATE_DORMANT) {
                    miteCue = "ROOT MITE NEST - GIVE IT A WIDE BERTH";
                } else if (rootMite.state == ROOT_MITE_STATE_APPROACH) {
                    miteCue = "MITE APPROACH - KEEP YOUR DISTANCE";
                } else if (rootMite.state == ROOT_MITE_STATE_PAUSE) {
                    miteCue = "MITE PAUSE - DODGE THE MARKED LINE";
                } else if (rootMite.state == ROOT_MITE_STATE_LUNGE) {
                    miteCue = "LUNGE! MOVE CLEAR";
                } else {
                    miteCue = attackFeedbackTime > 0.0f
                                  ? "MITE HIT!"
                                  : HasCombatWeapon(&inventory)
                                        ? "RECOVERY - FACE + SPACE / X"
                                        : "RECOVERY - WEAPON NEEDED";
                }
                DrawRectangle(6, 81, 236, 15, (Color){11, 26, 34, 220});
                DrawText(miteCue, 10, 85, 8, (Color){245, 214, 124, 255});
                if (rootMite.state != ROOT_MITE_STATE_DORMANT) {
                    DrawText(TextFormat("MITE %d/%d", rootMite.health,
                                        ROOT_MITE_HEALTH),
                             234, 98, 8, RAYWHITE);
                }
            } else if (!objectiveCueVisible) {
                JungleTaskProgress taskProgress = {
                    .rootWeaveSeen = interaction.rootWeaveSeen,
                    .rootMarkerActivated =
                        interaction.rootMarkerActivated,
                    .rootsMuralSeen = interaction.rootsMuralSeen,
                    .thornfangAcquired = HasThornfang(&inventory),
                    .thornGateOpen = thornGateOpen,
                    .returnedToJungleEntrance =
                        room.area == ROOM_AREA_JUNGLE_ENTRANCE
                };
                DrawRectangle(6, 81, 308, 15,
                              (Color){11, 26, 34, 220});
                DrawText(TextFormat("OBJECTIVE: %s",
                                    JungleTask_CurrentObjective(taskProgress)),
                         10, 85, 8, (Color){245, 214, 124, 255});
            }
            world_map_draw_minimap(
                CampaignZoneForRoom(room.area),
                room.area == ROOM_AREA_THORNBACK_ARENA &&
                    !HasThornfang(&inventory) &&
                    thornback.state == THORNBACK_STATE_DORMANT);
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
            GameUI_DrawHealth(player.health);
            DrawText(TextFormat("FRUIT %d/%d [H/B]",
                                inventory.foodServings,
                                FOOD_SERVING_CAPACITY),
                     142, 8, 8, RAYWHITE);
            if (pickupNoticeTime > 0.0f && pickupNotice != NULL) {
                DrawText(pickupNotice, 8, 154, 8,
                         (Color){245, 214, 124, 255});
            }
            if (saveError || saveNoticeTime > 0.0f) {
                DrawText(saveNotice, 8, 170, 8,
                         saveError ? (Color){255, 139, 115, 255}
                                   : (Color){127, 196, 192, 255});
            }
            if (room.area == ROOM_AREA_ROOT_PATH && thornGateOpen &&
                thornGateFeedbackTime > 0.0f) {
                DrawRectangle(6, 81, 136, 15, (Color){11, 26, 34, 220});
                DrawText("THORNS CUT - PATH OPEN", 10, 85, 8,
                         (Color){127, 196, 192, 255});
            }
            if (room.area == ROOM_AREA_THORNBACK_ARENA &&
                retryFeedbackTime > 0.0f) {
                DrawRectangle(6, 81, 210, 15, (Color){11, 26, 34, 220});
                DrawText("TRIAL RESET - RETURN TO MARKER", 10, 85, 8,
                         (Color){245, 214, 124, 255});
            } else if (room.area == ROOM_AREA_ASHFALL_DESCENT &&
                       ashfallRetryFeedbackTime > 0.0f) {
                DrawRectangle(6, 81, 218, 15, (Color){11, 26, 34, 220});
                DrawText("STEAM RESET - ROOM ENTRY", 10, 85, 8,
                         (Color){255, 139, 115, 255});
            } else if (room.area == ROOM_AREA_ROOT_PATH &&
                       rootPathRetryFeedbackTime > 0.0f) {
                DrawRectangle(6, 81, 218, 15, (Color){11, 26, 34, 220});
                DrawText("MITE RESET - THE NEST STIRS", 10, 85, 8,
                         (Color){255, 139, 115, 255});
            } else if (room.area == ROOM_AREA_THORNBACK_ARENA &&
                HasThornfang(&inventory)) {
                DrawRectangle(6, 81, 146, 15, (Color){11, 26, 34, 220});
                DrawText("THORNFANG CLAIMED", 10, 85, 8,
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
                DrawRectangle(6, 81, 210, 15, (Color){11, 26, 34, 220});
                DrawText(cue, 10, 85, 8, (Color){245, 214, 124, 255});
                DrawText(TextFormat("THORNBACK %d/%d", thornback.health,
                                    THORNBACK_HEALTH),
                         234, 71, 8, (Color){245, 214, 124, 255});
            }
            if (inventoryOpen) GameUI_DrawInventory(&inventory);
            if (helpOpen) GameUI_DrawHelp();
            if (mapOpen) world_map_draw(CampaignZoneForRoom(room.area));
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
