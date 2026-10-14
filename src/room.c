#include "room.h"
#include "tuning.h"
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

static const RoomProp *PropsForArea(RoomArea area)
{
    return area == ROOM_AREA_ROOT_PATH ? ROOT_PATH_PROPS : ENTRANCE_PROPS;
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

static char RootPathTileAt(int x, int y)
{
    if (x >= 17 && x <= 22 && y >= 6 && y <= 7) return 'T';
    if (x >= 15 && x <= 24 && y >= 14 && y <= 17) return 'N';
    if (x >= 8 && x <= 9 && y == 10) return 'S';
    if (x >= 8 && x <= 9 && y == 11) return 'R';
    if (RootPathFloor(x, y)) return 'D';
    if (x >= 15 && x <= 24 && y >= 5 && y <= 10) return 'N';
    return ((x * 11 + y * 5) % 4 == 0) ? 'G' : 'J';
}

static char RoomTileAt(RoomArea area, int x, int y)
{
    if (area == ROOM_AREA_ROOT_PATH) return RootPathTileAt(x, y);
    return EntranceTileAt(x, y);
}

void Room_Draw(const Room *r, float time)
{
    for (int y = 0; y < ROOM_ROWS; y++) {
        for (int x = 0; x < ROOM_COLS; x++) {
            int idx = TileIndex(RoomTileAt(r->area, x, y), x, y, time);
            Rectangle src = {
                (float)((idx % 8) * TILE_SIZE),
                (float)((idx / 8) * TILE_SIZE),
                (float)TILE_SIZE, (float)TILE_SIZE
            };
            Vector2 dst = {(float)(x * TILE_SIZE), (float)(y * TILE_SIZE)};
            DrawTextureRec(r->tileset, src, dst, WHITE);
        }
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

Vector2 Room_SpawnPoint(RoomArea area)
{
    if (area == ROOM_AREA_ROOT_PATH) return (Vector2){320.0f, 336.0f};
    return (Vector2){320.0f, 336.0f};
}

bool Room_GetTransition(const Room *r, Vector2 feet, RoomArea *destination,
                        Vector2 *spawnPoint)
{
    if (destination == NULL || spawnPoint == NULL) return false;
    if (r->area == ROOM_AREA_JUNGLE_ENTRANCE &&
        feet.x >= 288.0f && feet.x <= 352.0f && feet.y <= 56.0f) {
        *destination = ROOM_AREA_ROOT_PATH;
        *spawnPoint = Room_SpawnPoint(*destination);
        return true;
    }
    if (r->area == ROOM_AREA_ROOT_PATH &&
        feet.x >= 288.0f && feet.x <= 352.0f && feet.y >= 352.0f) {
        *destination = ROOM_AREA_JUNGLE_ENTRANCE;
        *spawnPoint = Room_SpawnPoint(*destination);
        return true;
    }
    return false;
}

static bool IsSolid(char tile)
{
    return tile == 'S' || tile == 'N' || tile == 'W' || tile == 'T';
}

static bool IsUpperLandingTile(RoomArea area, int x, int y)
{
    const RoomRamp *ramp = area == ROOM_AREA_ROOT_PATH
                               ? &ROOT_PATH_RAMP : &ENTRANCE_RAMP;
    int left = (int)(ramp->upperLanding.x / TILE_SIZE);
    int top = (int)(ramp->upperLanding.y / TILE_SIZE);
    int right = (int)((ramp->upperLanding.x + ramp->upperLanding.width) / TILE_SIZE);
    int bottom = (int)((ramp->upperLanding.y + ramp->upperLanding.height) / TILE_SIZE);
    return x >= left && x < right && y >= top && y < bottom;
}

bool Room_CollidesAtElevation(RoomArea area, Rectangle collider,
                             RoomElevation elevation)
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
            char tile = RoomTileAt(area, x, y);
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
        if (props[i].elevation != elevation) continue;
        Rectangle obstacle = props[i].collider;
        if (obstacle.width <= 0.0f || obstacle.height <= 0.0f) continue;
        if (collider.x < obstacle.x + obstacle.width &&
            right > obstacle.x && collider.y < obstacle.y + obstacle.height &&
            bottom > obstacle.y) return true;
    }
    return false;
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
    const RoomRamp *ramp = area == ROOM_AREA_ROOT_PATH
                               ? &ROOT_PATH_RAMP : &ENTRANCE_RAMP;
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
