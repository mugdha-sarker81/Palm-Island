#ifndef PALM_ROOM_H
#define PALM_ROOM_H

#include "raylib.h"

#define ROOM_COLS 40
#define ROOM_ROWS 23
#define ROOM_CORE_COLS 20
#define ROOM_CORE_ROWS 11
#define ROOM_CORE_OFFSET_X 10
#define ROOM_CORE_OFFSET_Y 6
#define ROOM_PROP_COUNT 3

typedef enum RoomElevation {
    ROOM_ELEVATION_GROUND,
    ROOM_ELEVATION_UPPER
} RoomElevation;

typedef enum RoomPropId {
    ROOM_PROP_PALM,
    ROOM_PROP_PILLAR,
    ROOM_PROP_BUSH
} RoomPropId;

typedef struct RoomProp {
    Vector2 drawPosition;
    float feetY;
    Rectangle collider;
    RoomElevation elevation;
    bool foreground;
} RoomProp;

typedef struct Room {
    Texture2D tileset;   /* assets/tilesets/tileset_16.png (8 columns x 2 rows) */
} Room;

/* Returns false if the tileset could not be loaded. */
bool      Room_Load(Room *r, const char *tilesetPath);
void      Room_Unload(Room *r);
void      Room_Draw(const Room *r, float time);
Rectangle Room_Bounds(void);
Vector2   Room_SpawnPoint(void);
const RoomProp *Room_Props(void);
/* Tests terrain, solid prop bases, and room bounds against a feet collider. */
bool      Room_Collides(Rectangle collider);
/* Tests collision against the requested side of the authored stair connector. */
bool      Room_CollidesAtElevation(Rectangle collider, RoomElevation elevation);
/* Changes elevation only when a move crosses the midpoint of the stair connector. */
void      Room_TryRampTransition(Vector2 from, Vector2 to, RoomElevation *elevation);

#endif
