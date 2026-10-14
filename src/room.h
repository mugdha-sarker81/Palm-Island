#ifndef PALM_ROOM_H
#define PALM_ROOM_H

#include "raylib.h"

#define ROOM_COLS 20
#define ROOM_ROWS 11

typedef struct Room {
    Texture2D tileset;   /* assets/tilesets/tileset_16.png (8 columns x 2 rows) */
} Room;

/* Returns false if the tileset could not be loaded. */
bool      Room_Load(Room *r, const char *tilesetPath);
void      Room_Unload(Room *r);
void      Room_Draw(const Room *r, float time);
Rectangle Room_Bounds(void);
Vector2   Room_SpawnPoint(void);

#endif
