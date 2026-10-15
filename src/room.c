#include "room.h"
#include "tuning.h"
#include <math.h>
#include <stddef.h>

enum {
    T_G = 0, T_J = 2, T_D = 4, T_S = 8, T_N = 9, T_R = 10,
    T_W0 = 11, T_W1 = 12, T_THORN = 13
};

#define RAMP_TILE_X 28
#define RAMP_TILE_Y 12

typedef struct RoomRamp {
    Rectangle bounds;
    Rectangle upperLanding;
    float transitionY;
} RoomRamp;

typedef enum RoomExitSide {
    ROOM_EXIT_TOP,
    ROOM_EXIT_BOTTOM
} RoomExitSide;

typedef struct RoomTransition {
    RoomArea source;
    RoomArea destination;
    RoomExitSide side;
    float thresholdY;
    Vector2 spawnPoint;
} RoomTransition;

static const Vector2 ROOM_SPAWN_POINTS[] = {
    {320.0f, 336.0f},
    {320.0f, 336.0f},
    {320.0f, 336.0f},
    {320.0f, 336.0f},
    {320.0f, 336.0f}
};

static const RoomTransition ROOM_TRANSITIONS[] = {
    {ROOM_AREA_JUNGLE_ENTRANCE, ROOM_AREA_ROOT_PATH,
     ROOM_EXIT_TOP, 56.0f, {320.0f, 336.0f}},
    {ROOM_AREA_ROOT_PATH, ROOM_AREA_JUNGLE_ENTRANCE,
     ROOM_EXIT_BOTTOM, 352.0f, {320.0f, 336.0f}},
    {ROOM_AREA_ROOT_PATH, ROOM_AREA_OVERGROWN_CLEARING,
     ROOM_EXIT_TOP, 16.0f, {320.0f, 336.0f}},
    {ROOM_AREA_OVERGROWN_CLEARING, ROOM_AREA_ROOT_PATH,
     ROOM_EXIT_BOTTOM, 352.0f, {320.0f, 32.0f}},
    {ROOM_AREA_OVERGROWN_CLEARING, ROOM_AREA_SHRINE_APPROACH,
     ROOM_EXIT_TOP, 16.0f, {320.0f, 336.0f}},
    {ROOM_AREA_SHRINE_APPROACH, ROOM_AREA_OVERGROWN_CLEARING,
     ROOM_EXIT_BOTTOM, 352.0f, {320.0f, 32.0f}},
    {ROOM_AREA_SHRINE_APPROACH, ROOM_AREA_THORNBACK_ARENA,
     ROOM_EXIT_TOP, 16.0f, {320.0f, 336.0f}},
    {ROOM_AREA_THORNBACK_ARENA, ROOM_AREA_SHRINE_APPROACH,
     ROOM_EXIT_BOTTOM, 352.0f, {320.0f, 32.0f}}
};

static const RoomRamp ENTRANCE_RAMP = {
    .bounds = {RAMP_TILE_X * TILE_SIZE, RAMP_TILE_Y * TILE_SIZE,
               2.0f * TILE_SIZE, TILE_SIZE},
    .upperLanding = {RAMP_TILE_X * TILE_SIZE, (RAMP_TILE_Y - 1.0f) * TILE_SIZE,
                     2.0f * TILE_SIZE, TILE_SIZE},
    .transitionY = (RAMP_TILE_Y + 0.5f) * TILE_SIZE
};

static const RoomRamp ROOT_PATH_RAMP = {
    .bounds = {8.0f * TILE_SIZE, 12.0f * TILE_SIZE,
               2.0f * TILE_SIZE, TILE_SIZE},
    .upperLanding = {8.0f * TILE_SIZE, 11.0f * TILE_SIZE,
                     2.0f * TILE_SIZE, TILE_SIZE},
    .transitionY = 12.5f * TILE_SIZE
};

static const RoomProp ENTRANCE_PROPS[ROOM_PROP_COUNT] = {
    {{112.0f, 72.0f}, 128.0f, {129.0f, 116.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{464.0f, 80.0f}, 136.0f, {481.0f, 124.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{96.0f, 208.0f}, 264.0f, {113.0f, 252.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{480.0f, 216.0f}, 272.0f, {497.0f, 260.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{240.0f, 160.0f}, 176.0f, {240.0f, 160.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{384.0f, 160.0f}, 176.0f, {384.0f, 160.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{224.0f, 112.0f}, 144.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_MARKER},
    {{176.0f, 256.0f}, 276.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, true, ROOM_SPRITE_TALL_GRASS},
    {{416.0f, 256.0f}, 276.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, true, ROOM_SPRITE_TALL_GRASS},
    {{308.0f, 16.0f}, 48.0f, {316.0f, 40.0f, 8.0f, 6.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_SHRINE_DOOR}
};

static const RoomProp ROOT_PATH_PROPS[ROOM_PROP_COUNT] = {
    {{112.0f, 112.0f}, 168.0f, {129.0f, 156.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{480.0f, 112.0f}, 168.0f, {497.0f, 156.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{176.0f, 224.0f}, 280.0f, {193.0f, 268.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{448.0f, 224.0f}, 280.0f, {465.0f, 268.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{208.0f, 136.0f}, 168.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_MARKER},
    {{272.0f, 200.0f}, 216.0f, {272.0f, 200.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{352.0f, 200.0f}, 216.0f, {352.0f, 200.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{400.0f, 112.0f}, 136.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROOT_BRIDGE},
    {{240.0f, 272.0f}, 292.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, true, ROOM_SPRITE_TALL_GRASS},
    {{304.0f, 64.0f}, 96.0f, {304.0f, 96.0f, 32.0f, 32.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_THORN_GATE}
};

static const RoomProp CLEARING_PROPS[ROOM_PROP_COUNT] = {
    {{112.0f, 80.0f}, 136.0f, {129.0f, 124.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{480.0f, 80.0f}, 136.0f, {497.0f, 124.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{112.0f, 224.0f}, 280.0f, {129.0f, 268.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{480.0f, 224.0f}, 280.0f, {497.0f, 268.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{208.0f, 112.0f}, 144.0f, {224.0f, 136.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{384.0f, 112.0f}, 144.0f, {400.0f, 136.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{208.0f, 224.0f}, 256.0f, {224.0f, 248.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{384.0f, 224.0f}, 256.0f, {400.0f, 248.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{64.0f, 176.0f}, 192.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_MARKER},
    {{320.0f, 256.0f}, 276.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, true, ROOM_SPRITE_TALL_GRASS}
};

static const RoomProp SHRINE_APPROACH_PROPS[ROOM_PROP_COUNT] = {
    {{112.0f, 80.0f}, 136.0f, {129.0f, 124.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{480.0f, 80.0f}, 136.0f, {497.0f, 124.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{112.0f, 256.0f}, 312.0f, {129.0f, 300.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{480.0f, 256.0f}, 312.0f, {497.0f, 300.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_PALM},
    {{208.0f, 176.0f}, 208.0f, {224.0f, 200.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{384.0f, 176.0f}, 208.0f, {400.0f, 200.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{304.0f, 88.0f}, 112.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_MARKER},
    {{176.0f, 296.0f}, 316.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, true, ROOM_SPRITE_TALL_GRASS},
    {{432.0f, 296.0f}, 316.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, true, ROOM_SPRITE_TALL_GRASS},
    {{304.0f, 16.0f}, 48.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_SHRINE_DOOR_OPEN}
};

static const RoomProp THORNBACK_ARENA_PROPS[ROOM_PROP_COUNT] = {
    {{112.0f, 88.0f}, 144.0f, {129.0f, 132.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{480.0f, 88.0f}, 144.0f, {497.0f, 132.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{112.0f, 256.0f}, 312.0f, {129.0f, 300.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{480.0f, 256.0f}, 312.0f, {497.0f, 300.0f, 8.0f, 8.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{208.0f, 112.0f}, 144.0f, {224.0f, 136.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{384.0f, 112.0f}, 144.0f, {400.0f, 136.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{208.0f, 240.0f}, 272.0f, {224.0f, 264.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{384.0f, 240.0f}, 272.0f, {400.0f, 264.0f, 16.0f, 16.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_ROCK},
    {{176.0f, 176.0f}, 200.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_MARKER},
    {{432.0f, 176.0f}, 200.0f, {0.0f, 0.0f, 0.0f, 0.0f},
     ROOM_ELEVATION_GROUND, false, ROOM_SPRITE_MARKER}
};

static const RoomInteractable ROOT_MARKINGS = {
    .id = "jungle.root_markings_seen",
    .title = "Living Thorn Weave",
    .text = "Roots curl away from the thorn wall.\nThey trace a route around the gate.",
    .interactBox = {304.0f, 144.0f, 32.0f, 16.0f},
    .kind = ROOM_INTERACTABLE_ROOT_WEAVE
};

static const RoomInteractable ROOT_MARKER = {
    .id = "jungle.root_marker_activated",
    .title = "Root Marker",
    .text = "A pulse follows the roots, tracing the safe bypass.",
    .activatedText = "The roots still glow along the bypass.",
    .interactBox = {208.0f, 136.0f, 16.0f, 16.0f},
    .kind = ROOM_INTERACTABLE_ROOT_MARKER
};

static const RoomInteractable CLEARING_NOTE = {
    .id = "jungle.clearing_camp_note_seen",
    .title = "Explorer's Note",
    .text = "The roots curl west around the thorn wall.\nSomeone marked the quiet path here.",
    .interactBox = {56.0f, 176.0f, 16.0f, 16.0f},
    .kind = ROOM_INTERACTABLE_CLEARING_NOTE
};

static const RoomInteractable PALM_FRUIT = {
    .id = "jungle.palm_fruit_collected",
    .title = "Palm Fruit",
    .text = "A ripe palm fruit. It should restore a little strength.",
    .interactBox = {96.0f, 176.0f, 16.0f, 16.0f},
    .kind = ROOM_INTERACTABLE_PALM_FRUIT
};

static const RoomInteractable SHRINE_PALM_FRUIT = {
    .id = "jungle.shrine_palm_fruit_collected",
    .title = "Palm Fruit",
    .text = "A ripe palm fruit. It should restore a little strength.",
    .interactBox = {144.0f, 176.0f, 16.0f, 16.0f},
    .kind = ROOM_INTERACTABLE_PALM_FRUIT
};

static const RoomInteractable ROOTS_MURAL = {
    .id = "jungle.map_of_roots_seen",
    .title = "Map of Roots",
    .text = "The roots bend, they do not break.\nTheir curve marks a safe way through.",
    .interactBox = {296.0f, 96.0f, 48.0f, 16.0f},
    .kind = ROOM_INTERACTABLE_ROOTS_MURAL
};

static const RoomInteractable THORNBACK_TRIAL = {
    .id = "jungle.thornback_trial_started",
    .title = "Trial Marker",
    .text = "The stone answers with a low rumble.",
    .interactBox = {168.0f, 160.0f, 16.0f, 32.0f},
    .kind = ROOM_INTERACTABLE_THORNBACK_TRIAL
};

static const RoomRamp *RampForArea(RoomArea area)
{
    if (area == ROOM_AREA_JUNGLE_ENTRANCE) return &ENTRANCE_RAMP;
    if (area == ROOM_AREA_ROOT_PATH) return &ROOT_PATH_RAMP;
    return NULL;
}

static const RoomProp *PropsForArea(RoomArea area)
{
    if (area == ROOM_AREA_ROOT_PATH) return ROOT_PATH_PROPS;
    if (area == ROOM_AREA_OVERGROWN_CLEARING) return CLEARING_PROPS;
    if (area == ROOM_AREA_SHRINE_APPROACH) return SHRINE_APPROACH_PROPS;
    if (area == ROOM_AREA_THORNBACK_ARENA) return THORNBACK_ARENA_PROPS;
    return ENTRANCE_PROPS;
}

bool Room_Load(Room *r, const char *tilesetPath)
{
    r->tileset = LoadTexture(tilesetPath);
    r->area = ROOM_AREA_JUNGLE_ENTRANCE;
    return r->tileset.id != 0;
}

void Room_SetArea(Room *r, RoomArea area)
{
    r->area = area;
}

void Room_Unload(Room *r)
{
    if (r->tileset.id != 0) UnloadTexture(r->tileset);
    r->tileset.id = 0;
}

static int TileIndex(char tile, int x, int y, float time)
{
    int variant = (x * 7 + y * 13) % 2;
    switch (tile) {
        case 'G': return T_G + variant;
        case 'J': return T_J + variant;
        case 'D': return T_D + variant;
        case 'S': return T_S;
        case 'N': return T_N;
        case 'R': return T_R;
        case 'W': return ((int)(time * 2.0f) % 2) ? T_W1 : T_W0;
        case 'T': return T_THORN;
        default: return T_G;
    }
}

static bool EntrancePath(int x, int y)
{
    bool northTrail = x >= 18 && x <= 21 && y >= 3 && y <= 10;
    bool southTrail = x >= 18 && x <= 21 && y >= 15;
    bool loopFloor = y >= 15 && y <= 16 && x >= 14 && x <= 25;
    bool westTrail = x >= 10 && x <= 14 && y >= 9 && y <= 15;
    bool eastTrail = x >= 25 && x <= 30 && y >= 9 && y <= 15;
    bool northLoop = y >= 8 && y <= 9 && x >= 14 && x <= 25;
    return northTrail || southTrail || loopFloor || westTrail || eastTrail ||
           northLoop;
}

static char EntranceTileAt(int x, int y)
{
    if (x >= 18 && x <= 21 && y <= 2) return 'S';
    if (x >= 15 && x <= 24 && y >= 11 && y <= 14) return 'N';
    if (x >= RAMP_TILE_X && x < RAMP_TILE_X + 2 && y == RAMP_TILE_Y - 1) return 'S';
    if (x >= RAMP_TILE_X && x < RAMP_TILE_X + 2 && y == RAMP_TILE_Y) return 'R';
    if (EntrancePath(x, y)) return 'D';
    return ((x * 7 + y * 13) % 5 == 0) ? 'G' : 'J';
}

static bool RootPathFloor(int x, int y)
{
    bool southTrail = x >= 18 && x <= 21 && y >= 11;
    bool southJunction = y >= 10 && y <= 12 && x >= 12 && x <= 27;
    bool sideWest = x >= 12 && x <= 14 && y >= 4 && y <= 11;
    bool sideEast = x >= 25 && x <= 27 && y >= 4 && y <= 11;
    bool upperJunction = y >= 3 && y <= 4 && x >= 12 && x <= 27;
    bool northTrail = x >= 18 && x <= 21 && y <= 3;
    bool rampWest = x >= 8 && x <= 9 && y >= 12 && y <= 13;
    return southTrail || southJunction || sideWest || sideEast ||
           upperJunction || northTrail || rampWest;
}

static char RootPathTileAt(int x, int y, bool thornGateOpen)
{
    if (x >= 17 && x <= 22 && y >= 6 && y <= 7) {
        return thornGateOpen ? 'D' : 'T';
    }
    if (thornGateOpen && x >= 17 && x <= 22 && y >= 5 && y <= 10) {
        return 'D';
    }
    if (x >= 15 && x <= 24 && y >= 14 && y <= 17) return 'N';
    if (x >= 8 && x <= 9 && y == 10) return 'S';
    if (x >= 8 && x <= 9 && y == 11) return 'R';
    if (RootPathFloor(x, y)) return 'D';
    if (x >= 15 && x <= 24 && y >= 5 && y <= 10) return 'N';
    return ((x * 11 + y * 5) % 4 == 0) ? 'G' : 'J';
}

static bool ClearingPath(int x, int y)
{
    bool southTrail = x >= 18 && x <= 21 && y >= 18;
    bool lowerJunction = y >= 17 && y <= 20 && x >= 8 && x <= 31;
    bool westTrail = x >= 8 && x <= 10 && y >= 5 && y <= 17;
    bool eastTrail = x >= 29 && x <= 31 && y >= 5 && y <= 17;
    bool upperJunction = y >= 4 && y <= 5 && x >= 8 && x <= 31;
    bool northTrail = x >= 18 && x <= 21 && y <= 4;
    bool exposedTrail = x >= 18 && x <= 21 && y >= 5 && y <= 17;
    bool nookTrail = x >= 4 && x <= 8 && y >= 12 && y <= 14;
    return southTrail || lowerJunction || westTrail || eastTrail ||
           upperJunction || northTrail || exposedTrail || nookTrail;
}

static char ClearingTileAt(int x, int y)
{
    if ((x >= 13 && x <= 16 && y >= 8 && y <= 14) ||
        (x >= 23 && x <= 26 && y >= 8 && y <= 14)) return 'N';
    if (ClearingPath(x, y)) {
        bool outerPath = (x >= 8 && x <= 10) || (x >= 29 && x <= 31) ||
                         (y >= 17 && y <= 20 && x >= 8 && x <= 31);
        return outerPath ? 'G' : 'D';
    }
    if (x >= 2 && x <= 3 && y >= 11 && y <= 15) return 'N';
    return ((x * 7 + y * 13) % 5 == 0) ? 'G' : 'J';
}

static bool ShrineApproachPath(int x, int y)
{
    bool southTrail = x >= 18 && x <= 21 && y >= 18;
    bool lowerPlaza = y >= 16 && y <= 19 && x >= 10 && x <= 29;
    bool westWalk = x >= 10 && x <= 12 && y >= 7 && y <= 16;
    bool eastWalk = x >= 27 && x <= 29 && y >= 7 && y <= 16;
    bool upperPlaza = y >= 6 && y <= 9 && x >= 10 && x <= 29;
    bool shrineWalk = x >= 18 && x <= 21 && y >= 3 && y <= 6;
    bool muralCourt = x >= 16 && x <= 23 && y >= 10 && y <= 15;
    return southTrail || lowerPlaza || westWalk || eastWalk ||
           upperPlaza || shrineWalk || muralCourt;
}

static char ShrineApproachTileAt(int x, int y)
{
    if (y <= 2) return x >= 18 && x <= 21 ? 'D' : 'S';
    if ((x >= 14 && x <= 16 && y >= 11 && y <= 14) ||
        (x >= 23 && x <= 25 && y >= 11 && y <= 14)) return 'N';
    if (ShrineApproachPath(x, y)) {
        bool courtyard = y >= 6 && y <= 19;
        return courtyard ? 'D' : 'G';
    }
    return ((x * 5 + y * 11) % 4 == 0) ? 'G' : 'J';
}

static bool ThornbackArenaPath(int x, int y)
{
    bool westPerimeter = x >= 4 && x <= 8 && y >= 3 && y <= 19;
    bool eastPerimeter = x >= 31 && x <= 35 && y >= 3 && y <= 19;
    bool southPerimeter = y >= 16 && y <= 19 && x >= 4 && x <= 35;
    bool northPerimeter = y >= 3 && y <= 6 && x >= 4 && x <= 35;
    bool centerBowl = x >= 12 && x <= 27 && y >= 7 && y <= 15;
    bool entrance = x >= 18 && x <= 21 && y >= 19;
    return westPerimeter || eastPerimeter || southPerimeter ||
           northPerimeter || centerBowl || entrance;
}

static char ThornbackArenaTileAt(int x, int y)
{
    if (ThornbackArenaPath(x, y)) {
        return (x >= 12 && x <= 27 && y >= 7 && y <= 15) ? 'D' : 'G';
    }
    return ((x * 11 + y * 7) % 4 == 0) ? 'J' : 'G';
}

static char RoomTileAt(RoomArea area, int x, int y, bool thornGateOpen)
{
    if (area == ROOM_AREA_ROOT_PATH) {
        return RootPathTileAt(x, y, thornGateOpen);
    }
    if (area == ROOM_AREA_OVERGROWN_CLEARING) return ClearingTileAt(x, y);
    if (area == ROOM_AREA_SHRINE_APPROACH) return ShrineApproachTileAt(x, y);
    if (area == ROOM_AREA_THORNBACK_ARENA) return ThornbackArenaTileAt(x, y);
    return EntranceTileAt(x, y);
}

void Room_Draw(const Room *r, float time, bool thornGateOpen)
{
    for (int y = 0; y < ROOM_ROWS; y++) {
        for (int x = 0; x < ROOM_COLS; x++) {
            int idx = TileIndex(RoomTileAt(r->area, x, y, thornGateOpen),
                                x, y, time);
            Rectangle src = {
                (float)((idx % 8) * TILE_SIZE),
                (float)((idx / 8) * TILE_SIZE),
                (float)TILE_SIZE, (float)TILE_SIZE
            };
            Vector2 dst = {(float)(x * TILE_SIZE), (float)(y * TILE_SIZE)};
            DrawTextureRec(r->tileset, src, dst, WHITE);
        }
    }
    if (r->area == ROOM_AREA_ROOT_PATH && thornGateOpen) {
        DrawLineEx((Vector2){272.0f, 112.0f}, (Vector2){296.0f, 112.0f},
                   2.0f, (Color){100, 219, 143, 220});
        DrawLineEx((Vector2){344.0f, 112.0f}, (Vector2){368.0f, 112.0f},
                   2.0f, (Color){100, 219, 143, 220});
    }
}

Rectangle Room_Bounds(void)
{
    return (Rectangle){0.0f, 0.0f, (float)(ROOM_COLS * TILE_SIZE),
                       (float)(ROOM_ROWS * TILE_SIZE)};
}

const RoomProp *Room_Props(RoomArea area)
{
    return PropsForArea(area);
}

uint32_t Room_FoodNodeForArea(RoomArea area)
{
    if (area == ROOM_AREA_OVERGROWN_CLEARING) {
        return ROOM_FOOD_NODE_CLEARING;
    }
    if (area == ROOM_AREA_SHRINE_APPROACH) {
        return ROOM_FOOD_NODE_SHRINE_APPROACH;
    }
    return ROOM_FOOD_NODE_NONE;
}

Vector2 Room_PalmFruitPosition(RoomArea area)
{
    if (area == ROOM_AREA_SHRINE_APPROACH) {
        return (Vector2){152.0f, 184.0f};
    }
    return (Vector2){104.0f, 184.0f};
}

const RoomInteractable *Room_FindInteractable(RoomArea area, Vector2 feet,
                                               Vector2 facingDirection,
                                               float maxDistance)
{
    return Room_FindInteractableWithFruitState(
        area, feet, facingDirection, maxDistance, ROOM_FOOD_NODE_NONE);
}

const RoomInteractable *Room_FindInteractableWithFruitState(
    RoomArea area, Vector2 feet, Vector2 facingDirection,
                                               float maxDistance,
                                               uint32_t collectedFoodNodes)
{
    if (maxDistance < 0.0f) return NULL;
    const RoomInteractable *targets[2];
    size_t targetCount = 0;
    if (area == ROOM_AREA_ROOT_PATH) {
        targets[targetCount++] = &ROOT_MARKINGS;
        targets[targetCount++] = &ROOT_MARKER;
    } else if (area == ROOM_AREA_OVERGROWN_CLEARING) {
        targets[targetCount++] = &CLEARING_NOTE;
        if ((collectedFoodNodes & ROOM_FOOD_NODE_CLEARING) == 0) {
            targets[targetCount++] = &PALM_FRUIT;
        }
    } else if (area == ROOM_AREA_SHRINE_APPROACH) {
        targets[targetCount++] = &ROOTS_MURAL;
        if ((collectedFoodNodes & ROOM_FOOD_NODE_SHRINE_APPROACH) == 0) {
            targets[targetCount++] = &SHRINE_PALM_FRUIT;
        }
    } else if (area == ROOM_AREA_THORNBACK_ARENA) {
        targets[targetCount++] = &THORNBACK_TRIAL;
    } else {
        return NULL;
    }
    const RoomInteractable *nearest = NULL;
    float nearestDistanceSquared = maxDistance * maxDistance;
    for (size_t i = 0; i < targetCount; i++) {
        const RoomInteractable *target = targets[i];
        float closestX = fmaxf(target->interactBox.x,
                               fminf(feet.x, target->interactBox.x +
                                               target->interactBox.width));
        float closestY = fmaxf(target->interactBox.y,
                               fminf(feet.y, target->interactBox.y +
                                               target->interactBox.height));
        Vector2 delta = {closestX - feet.x, closestY - feet.y};
        float distanceSquared = delta.x * delta.x + delta.y * delta.y;
        if (distanceSquared > nearestDistanceSquared) continue;

        Vector2 targetCenter = {
            target->interactBox.x + target->interactBox.width * 0.5f,
            target->interactBox.y + target->interactBox.height * 0.5f
        };
        Vector2 facingDelta = {targetCenter.x - feet.x,
                               targetCenter.y - feet.y};
        float facingDistance = sqrtf(facingDelta.x * facingDelta.x +
                                     facingDelta.y * facingDelta.y);
        float facingLength = sqrtf(facingDirection.x * facingDirection.x +
                                   facingDirection.y * facingDirection.y);
        if (facingDistance > 0.0f && facingLength > 0.0f) {
            float facingDot = (facingDirection.x * facingDelta.x +
                               facingDirection.y * facingDelta.y) /
                              (facingLength * facingDistance);
            if (facingDot < INTERACT_FACING_DOT) continue;
        }

        if (nearest == NULL || distanceSquared < nearestDistanceSquared) {
            nearest = target;
            nearestDistanceSquared = distanceSquared;
        }
    }
    return nearest;
}

Vector2 Room_SpawnPoint(RoomArea area)
{
    if (area >= ROOM_AREA_JUNGLE_ENTRANCE &&
        area <= ROOM_AREA_THORNBACK_ARENA) {
        return ROOM_SPAWN_POINTS[area];
    }
    return (Vector2){320.0f, 336.0f};
}

bool Room_GetTransition(const Room *r, Vector2 feet, RoomArea *destination,
                        Vector2 *spawnPoint)
{
    if (r == NULL || destination == NULL || spawnPoint == NULL) return false;
    if (feet.x < 288.0f || feet.x > 352.0f) return false;

    for (size_t i = 0; i < sizeof(ROOM_TRANSITIONS) /
                            sizeof(ROOM_TRANSITIONS[0]); i++) {
        const RoomTransition *transition = &ROOM_TRANSITIONS[i];
        if (transition->source != r->area) continue;
        bool reachedExit = transition->side == ROOM_EXIT_TOP
                               ? feet.y <= transition->thresholdY
                               : feet.y >= transition->thresholdY;
        if (!reachedExit) continue;

        *destination = transition->destination;
        *spawnPoint = transition->spawnPoint;
        return true;
    }
    return false;
}

bool Room_IsFacingThornGate(RoomArea area, Vector2 feet,
                            Vector2 facingDirection, bool gateOpen)
{
    if (area != ROOM_AREA_ROOT_PATH || gateOpen) return false;

    const Rectangle gate = {304.0f, 96.0f, 32.0f, 32.0f};
    float closestX = fmaxf(gate.x, fminf(feet.x, gate.x + gate.width));
    float closestY = fmaxf(gate.y, fminf(feet.y, gate.y + gate.height));
    Vector2 delta = {closestX - feet.x, closestY - feet.y};
    float distance = sqrtf(delta.x * delta.x + delta.y * delta.y);
    if (distance > THORNFANG_CUT_RANGE) return false;

    Vector2 centerDelta = {gate.x + gate.width * 0.5f - feet.x,
                           gate.y + gate.height * 0.5f - feet.y};
    float centerDistance = sqrtf(centerDelta.x * centerDelta.x +
                                 centerDelta.y * centerDelta.y);
    float facingLength = sqrtf(facingDirection.x * facingDirection.x +
                               facingDirection.y * facingDirection.y);
    if (centerDistance == 0.0f || facingLength == 0.0f) return false;
    float facingDot = (facingDirection.x * centerDelta.x +
                       facingDirection.y * centerDelta.y) /
                      (facingLength * centerDistance);
    return facingDot >= INTERACT_FACING_DOT;
}

bool Room_CanCutThornGate(RoomArea area, Vector2 feet,
                          Vector2 facingDirection, bool hasThornfang,
                          bool gateOpen)
{
    return hasThornfang &&
           Room_IsFacingThornGate(area, feet, facingDirection, gateOpen);
}

bool Room_TryCutThornGate(RoomArea area, Vector2 feet,
                          Vector2 facingDirection, bool hasThornfang,
                          bool *gateOpen)
{
    if (gateOpen == NULL ||
        !Room_CanCutThornGate(area, feet, facingDirection, hasThornfang,
                              *gateOpen)) {
        return false;
    }
    *gateOpen = true;
    return true;
}

static bool IsSolid(char tile)
{
    return tile == 'S' || tile == 'N' || tile == 'W' || tile == 'T';
}

static bool IsUpperLandingTile(RoomArea area, int x, int y)
{
    const RoomRamp *ramp = RampForArea(area);
    if (ramp == NULL) return false;
    int left = (int)(ramp->upperLanding.x / TILE_SIZE);
    int top = (int)(ramp->upperLanding.y / TILE_SIZE);
    int right = (int)((ramp->upperLanding.x + ramp->upperLanding.width) / TILE_SIZE);
    int bottom = (int)((ramp->upperLanding.y + ramp->upperLanding.height) / TILE_SIZE);
    return x >= left && x < right && y >= top && y < bottom;
}

bool Room_CollidesAtElevationWithGate(RoomArea area, Rectangle collider,
                                      RoomElevation elevation,
                                      bool thornGateOpen)
{
    float roomWidth = (float)(ROOM_COLS * TILE_SIZE);
    float roomHeight = (float)(ROOM_ROWS * TILE_SIZE);
    float right = collider.x + collider.width;
    float bottom = collider.y + collider.height;
    if (collider.width <= 0.0f || collider.height <= 0.0f ||
        collider.x < 0.0f || collider.y < 0.0f ||
        right > roomWidth || bottom > roomHeight) return true;

    int minX = (int)(collider.x / TILE_SIZE);
    int minY = (int)(collider.y / TILE_SIZE);
    int maxX = (int)((right - 0.001f) / TILE_SIZE);
    int maxY = (int)((bottom - 0.001f) / TILE_SIZE);
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            char tile = RoomTileAt(area, x, y, thornGateOpen);
            if (!IsSolid(tile)) continue;
            if (elevation == ROOM_ELEVATION_UPPER &&
                IsUpperLandingTile(area, x, y)) continue;
            float tileLeft = (float)(x * TILE_SIZE);
            float tileTop = (float)(y * TILE_SIZE);
            if (collider.x < tileLeft + TILE_SIZE && right > tileLeft &&
                collider.y < tileTop + TILE_SIZE && bottom > tileTop) return true;
        }
    }

    const RoomProp *props = PropsForArea(area);
    for (int i = 0; i < ROOM_PROP_COUNT; i++) {
        if (thornGateOpen && area == ROOM_AREA_ROOT_PATH &&
            props[i].sprite == ROOM_SPRITE_THORN_GATE) continue;
        if (props[i].elevation != elevation) continue;
        Rectangle obstacle = props[i].collider;
        if (obstacle.width <= 0.0f || obstacle.height <= 0.0f) continue;
        if (collider.x < obstacle.x + obstacle.width &&
            right > obstacle.x && collider.y < obstacle.y + obstacle.height &&
            bottom > obstacle.y) return true;
    }
    return false;
}

bool Room_CollidesAtElevation(RoomArea area, Rectangle collider,
                              RoomElevation elevation)
{
    return Room_CollidesAtElevationWithGate(area, collider, elevation, false);
}

bool Room_CollidesInArea(RoomArea area, Rectangle collider)
{
    return Room_CollidesAtElevation(area, collider, ROOM_ELEVATION_GROUND);
}

bool Room_Collides(Rectangle collider)
{
    return Room_CollidesInArea(ROOM_AREA_JUNGLE_ENTRANCE, collider);
}

void Room_TryRampTransition(RoomArea area, Vector2 from, Vector2 to,
                            RoomElevation *elevation)
{
    if (elevation == NULL) return;
    const RoomRamp *ramp = RampForArea(area);
    if (ramp == NULL) return;
    float minFeetX = ramp->bounds.x + PLAYER_COLLIDER_W * 0.5f;
    float maxFeetX = ramp->bounds.x + ramp->bounds.width -
                     PLAYER_COLLIDER_W * 0.5f;
    bool remainsInOpening = from.x >= minFeetX && from.x <= maxFeetX &&
                            to.x >= minFeetX && to.x <= maxFeetX;
    if (!remainsInOpening) return;
    if (*elevation == ROOM_ELEVATION_GROUND &&
        from.y >= ramp->transitionY && to.y < ramp->transitionY) {
        *elevation = ROOM_ELEVATION_UPPER;
    } else if (*elevation == ROOM_ELEVATION_UPPER &&
               from.y <= ramp->transitionY && to.y > ramp->transitionY) {
        *elevation = ROOM_ELEVATION_GROUND;
    }
}
