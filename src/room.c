#include "room.h"
#include "tuning.h"

/* Tile letters -> tileset index (column = i % 8, row = i / 8).
 * Order from tools/generate_assets.py:
 * 0 G  1 G2  2 J  3 J2  4 D  5 D2  6 A  7 A2  8 S  9 N  10 R  11 W0  12 W1  13 T */
enum { T_G = 0, T_J = 2, T_D = 4, T_A = 6, T_S = 8, T_N = 9, T_R = 10, T_W0 = 11, T_W1 = 12 };

/* G grass, J jungle floor, D dirt path, A sand, W water,
 * S shrine stone, N cliff face, R ramp/stairs. */
static const char *LAYOUT[ROOM_ROWS] = {
    "JJGGJJJSSSSSSJJGGGJJ",
    "JGGGGJJSSSSSSJJGGGGJ",
    "JGGGGJJNNRNNNJGGGGGJ",
    "GGGGGGGGGDDGGGGGGGGG",
    "GGGGGGGGGDDGGGGGGGGG",
    "GGGGGGGDDDDDDGGGGGGG",
    "GGGGGGGDDGGGDDGGGGAA",
    "GGGGGGGDDGGGGDDGGAAW",
    "GGGGGGGGDDGGGGDDAAWW",
    "GGGGGGGGGDDGGGGAAWWW",
    "GGGGGGGGGGDDGGAAWWWW"
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

void Room_Draw(const Room *r, float time)
{
    for (int y = 0; y < ROOM_ROWS; y++) {
        for (int x = 0; x < ROOM_COLS; x++) {
            int idx = TileIndex(LAYOUT[y][x], x, y, time);
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

static bool IsSolid(char tile)
{
    return tile == 'S' || tile == 'N' || tile == 'W';
}

bool Room_Collides(Rectangle collider)
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
            char tile = LAYOUT[y][x];
            if (!IsSolid(tile)) continue;

            float tileLeft = (float)(x * TILE_SIZE);
            float tileTop = (float)(y * TILE_SIZE);
            if (collider.x < tileLeft + TILE_SIZE && right > tileLeft &&
                collider.y < tileTop + TILE_SIZE && bottom > tileTop) return true;
        }
    }
    return false;
}

Vector2 Room_SpawnPoint(void)
{
    /* Centre of the dirt path in front of the shrine stairs. */
    return (Vector2){(float)(9 * TILE_SIZE + TILE_SIZE / 2), (float)(5 * TILE_SIZE + TILE_SIZE / 2)};
}
