#include "room.h"
#include "tuning.h"
#include <stddef.h>

/* Tile letters -> tileset index (column = i % 8, row = i / 8).
 * Order from tools/generate_assets.py:
 * 0 G  1 G2  2 J  3 J2  4 D  5 D2  6 A  7 A2  8 S  9 N  10 R  11 W0  12 W1  13 T */
enum { T_G = 0, T_J = 2, T_D = 4, T_A = 6, T_S = 8, T_N = 9, T_R = 10, T_W0 = 11, T_W1 = 12 };

/* G grass, J jungle floor, D dirt path, A sand, W water,
 * S shrine stone, N cliff face, R ramp/stairs. */
static const char *LAYOUT[ROOM_CORE_ROWS] = {
    "JJGGJJJSSSSSSJJGGGJJ",
    "JGGGGJJSSSSSSJJGGGGJ",
    "JGGGGJJNNRRNNJGGGGGJ",
    "GGGGGGGGGDDGGGGGGGGG",
    "GGGGGGGGGDDGGGGGGGGG",
    "GGGGGGGDDDDDDGGGGGGG",
    "GGGGGGGDDGGGDDGGGGAA",
    "GGGGGGGDDGGGGDDGGAAW",
    "GGGGGGGGDDGGGGDDAAWW",
    "GGGGGGGGGDDGGGGAAWWW",
    "GGGGGGGGGGDDGGAAWWWW"
};

typedef struct RoomRamp {
    Rectangle bounds;
    Rectangle upperLanding;
    float transitionY;
} RoomRamp;

static const RoomRamp SHRINE_RAMP = {
    .bounds = {(ROOM_CORE_OFFSET_X + 9.0f) * TILE_SIZE,
               (ROOM_CORE_OFFSET_Y + 2.0f) * TILE_SIZE,
               2.0f * TILE_SIZE, TILE_SIZE},
    .upperLanding = {(ROOM_CORE_OFFSET_X + 9.0f) * TILE_SIZE,
                     (ROOM_CORE_OFFSET_Y + 1.0f) * TILE_SIZE,
                     2.0f * TILE_SIZE, TILE_SIZE},
    .transitionY = (ROOM_CORE_OFFSET_Y + 2.5f) * TILE_SIZE
};

static const RoomProp PROPS[ROOM_PROP_COUNT] = {
    {
        .drawPosition = {(ROOM_CORE_OFFSET_X * TILE_SIZE) + 69.0f,
                         (ROOM_CORE_OFFSET_Y * TILE_SIZE) + 55.0f},
        .feetY = (ROOM_CORE_OFFSET_Y * TILE_SIZE) + 111.0f,
        .collider = {(ROOM_CORE_OFFSET_X * TILE_SIZE) + 86.0f,
                     (ROOM_CORE_OFFSET_Y * TILE_SIZE) + 104.0f, 8.0f, 7.0f},
        .elevation = ROOM_ELEVATION_GROUND
    },
    {
        .drawPosition = {(ROOM_CORE_OFFSET_X * TILE_SIZE) + 224.0f,
                         (ROOM_CORE_OFFSET_Y * TILE_SIZE) + 40.0f},
        .feetY = (ROOM_CORE_OFFSET_Y * TILE_SIZE) + 80.0f,
        .collider = {(ROOM_CORE_OFFSET_X * TILE_SIZE) + 228.0f,
                     (ROOM_CORE_OFFSET_Y * TILE_SIZE) + 73.0f, 8.0f, 7.0f},
        .elevation = ROOM_ELEVATION_GROUND
    },
    {
        .drawPosition = {(ROOM_CORE_OFFSET_X * TILE_SIZE) + 171.0f,
                         (ROOM_CORE_OFFSET_Y * TILE_SIZE) + 84.0f},
        .feetY = (ROOM_CORE_OFFSET_Y * TILE_SIZE) + 100.0f,
        .elevation = ROOM_ELEVATION_GROUND,
        .foreground = true
    }
};

bool Room_Load(Room *r, const char *tilesetPath)
{
    r->tileset = LoadTexture(tilesetPath);
    return r->tileset.id != 0;
}

void Room_Unload(Room *r)
{
    if (r->tileset.id != 0) UnloadTexture(r->tileset);
    r->tileset.id = 0;
}

static int TileIndex(char c, int x, int y, float time)
{
    /* Two look-alike variants per ground type, chosen by position (no randomness per frame). */
    int variant = ((x * 7 + y * 13) % 2);
    switch (c) {
        case 'G': return T_G + variant;
        case 'J': return T_J + variant;
        case 'D': return T_D + variant;
        case 'A': return T_A + variant;
        case 'S': return T_S;
        case 'N': return T_N;
        case 'R': return T_R;
        case 'W': return ((int)(time * 2.0f) % 2) ? T_W1 : T_W0; /* 2 frames, 0.5 s each */
        default:  return T_G;
    }
}

static char RoomTileAt(int x, int y)
{
    int coreX = x - ROOM_CORE_OFFSET_X;
    int coreY = y - ROOM_CORE_OFFSET_Y;
    if (coreX >= 0 && coreX < ROOM_CORE_COLS &&
        coreY >= 0 && coreY < ROOM_CORE_ROWS) {
        return LAYOUT[coreY][coreX];
    }

    if ((x == ROOM_COLS / 2 - 1 || x == ROOM_COLS / 2) ||
        (y == ROOM_ROWS / 2 || y == ROOM_ROWS / 2 + 1)) return 'D';
    return 'G';
}

void Room_Draw(const Room *r, float time)
{
    for (int y = 0; y < ROOM_ROWS; y++) {
        for (int x = 0; x < ROOM_COLS; x++) {
            int idx = TileIndex(RoomTileAt(x, y), x, y, time);
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
    return (Rectangle){0.0f, 0.0f, (float)(ROOM_COLS * TILE_SIZE), (float)(ROOM_ROWS * TILE_SIZE)};
}

const RoomProp *Room_Props(void)
{
    return PROPS;
}

static bool IsSolid(char tile)
{
    return tile == 'S' || tile == 'N' || tile == 'W';
}

static bool IsUpperLandingTile(int x, int y)
{
    int landingLeft = (int)(SHRINE_RAMP.upperLanding.x / TILE_SIZE);
    int landingTop = (int)(SHRINE_RAMP.upperLanding.y / TILE_SIZE);
    int landingRight = (int)((SHRINE_RAMP.upperLanding.x +
                              SHRINE_RAMP.upperLanding.width) / TILE_SIZE);
    int landingBottom = (int)((SHRINE_RAMP.upperLanding.y +
                               SHRINE_RAMP.upperLanding.height) / TILE_SIZE);
    return x >= landingLeft && x < landingRight &&
           y >= landingTop && y < landingBottom;
}

bool Room_CollidesAtElevation(Rectangle collider, RoomElevation elevation)
{
    const float roomWidth = (float)(ROOM_COLS * TILE_SIZE);
    const float roomHeight = (float)(ROOM_ROWS * TILE_SIZE);
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
            char tile = RoomTileAt(x, y);
            if (!IsSolid(tile)) continue;
            if (elevation == ROOM_ELEVATION_UPPER &&
                IsUpperLandingTile(x, y)) continue;

            float tileLeft = (float)(x * TILE_SIZE);
            float tileTop = (float)(y * TILE_SIZE);
            if (collider.x < tileLeft + TILE_SIZE && right > tileLeft &&
                collider.y < tileTop + TILE_SIZE && bottom > tileTop) return true;
        }
    }

    for (int i = 0; i < ROOM_PROP_COUNT; i++) {
        if (PROPS[i].elevation != elevation) continue;
        Rectangle obstacle = PROPS[i].collider;
        if (obstacle.width <= 0.0f || obstacle.height <= 0.0f) continue;
        if (collider.x < obstacle.x + obstacle.width &&
            right > obstacle.x &&
            collider.y < obstacle.y + obstacle.height &&
            bottom > obstacle.y) return true;
    }
    return false;
}

bool Room_Collides(Rectangle collider)
{
    return Room_CollidesAtElevation(collider, ROOM_ELEVATION_GROUND);
}

void Room_TryRampTransition(Vector2 from, Vector2 to, RoomElevation *elevation)
{
    if (elevation == NULL) return;

    float minFeetX = SHRINE_RAMP.bounds.x + PLAYER_COLLIDER_W * 0.5f;
    float maxFeetX = SHRINE_RAMP.bounds.x + SHRINE_RAMP.bounds.width -
                     PLAYER_COLLIDER_W * 0.5f;
    bool remainsInOpening = from.x >= minFeetX && from.x <= maxFeetX &&
                            to.x >= minFeetX && to.x <= maxFeetX;
    if (!remainsInOpening) return;

    if (*elevation == ROOM_ELEVATION_GROUND &&
        from.y >= SHRINE_RAMP.transitionY && to.y < SHRINE_RAMP.transitionY) {
        *elevation = ROOM_ELEVATION_UPPER;
    } else if (*elevation == ROOM_ELEVATION_UPPER &&
               from.y <= SHRINE_RAMP.transitionY && to.y > SHRINE_RAMP.transitionY) {
        *elevation = ROOM_ELEVATION_GROUND;
    }
}

Vector2 Room_SpawnPoint(void)
{
    /* Centre of the dirt path in front of the shrine stairs. */
    return (Vector2){
        (float)((ROOM_CORE_OFFSET_X + 10) * TILE_SIZE),
        (float)((ROOM_CORE_OFFSET_Y + 5) * TILE_SIZE + TILE_SIZE / 2)
    };
}
