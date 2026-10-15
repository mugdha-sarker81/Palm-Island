#ifndef PALM_ROOM_H
#define PALM_ROOM_H

#include "raylib.h"

#define ROOM_COLS 40
#define ROOM_ROWS 23
#define ROOM_PROP_COUNT 10

typedef enum RoomArea {
    ROOM_AREA_JUNGLE_ENTRANCE,
    ROOM_AREA_ROOT_PATH,
    ROOM_AREA_OVERGROWN_CLEARING,
    ROOM_AREA_SHRINE_APPROACH,
    ROOM_AREA_THORNBACK_ARENA
} RoomArea;

typedef enum RoomSprite {
    ROOM_SPRITE_PALM,
    ROOM_SPRITE_ROCK,
    ROOM_SPRITE_MARKER,
    ROOM_SPRITE_TALL_GRASS,
    ROOM_SPRITE_SHRINE_DOOR,
    ROOM_SPRITE_SHRINE_DOOR_OPEN,
    ROOM_SPRITE_ROOT_BRIDGE,
    ROOM_SPRITE_THORN_GATE,
    ROOM_SPRITE_COUNT
} RoomSprite;

typedef enum RoomElevation {
    ROOM_ELEVATION_GROUND,
    ROOM_ELEVATION_UPPER
} RoomElevation;

typedef enum RoomPropId {
    ROOM_PROP_PALM,
    ROOM_PROP_PILLAR,
    ROOM_PROP_BUSH
} RoomPropId;

typedef enum RoomInteractableKind {
    ROOM_INTERACTABLE_ROOT_WEAVE,
    ROOM_INTERACTABLE_ROOT_MARKER,
    ROOM_INTERACTABLE_CLEARING_NOTE,
    ROOM_INTERACTABLE_ROOTS_MURAL,
    ROOM_INTERACTABLE_THORNBACK_TRIAL,
    ROOM_INTERACTABLE_PALM_FRUIT
} RoomInteractableKind;

typedef struct RoomProp {
    Vector2 drawPosition;
    float feetY;
    Rectangle collider;
    RoomElevation elevation;
    bool foreground;
    RoomSprite sprite;
} RoomProp;

typedef struct RoomInteractable {
    const char *id;
    const char *title;
    const char *text;
    const char *activatedText;
    Rectangle interactBox;
    RoomInteractableKind kind;
} RoomInteractable;

typedef struct Room {
    Texture2D tileset;   /* assets/tilesets/tileset_16.png (8 columns x 2 rows) */
    RoomArea area;
} Room;

/* Returns false if the tileset could not be loaded. */
bool      Room_Load(Room *r, const char *tilesetPath);
void      Room_SetArea(Room *r, RoomArea area);
void      Room_Unload(Room *r);
void      Room_Draw(const Room *r, float time, bool thornGateOpen);
Rectangle Room_Bounds(void);
Vector2   Room_SpawnPoint(RoomArea area);
const RoomProp *Room_Props(RoomArea area);
/* Returns the area's inspect target when it is in front and within range. */
const RoomInteractable *Room_FindInteractable(RoomArea area, Vector2 feet,
                                               Vector2 facingDirection,
                                               float maxDistance);
const RoomInteractable *Room_FindInteractableWithFruitState(
    RoomArea area, Vector2 feet, Vector2 facingDirection, float maxDistance,
    bool palmFruitCollected);
Vector2   Room_PalmFruitPosition(void);
/* Returns the paired spawn for a simple in-room connection, if reached. */
bool      Room_GetTransition(const Room *r, Vector2 feet, RoomArea *destination,
                             Vector2 *spawnPoint);
bool      Room_CanCutThornGate(RoomArea area, Vector2 feet,
                               Vector2 facingDirection, bool hasThornfang,
                               bool gateOpen);
bool      Room_TryCutThornGate(RoomArea area, Vector2 feet,
                               Vector2 facingDirection, bool hasThornfang,
                               bool *gateOpen);
bool      Room_IsFacingThornGate(RoomArea area, Vector2 feet,
                                 Vector2 facingDirection, bool gateOpen);
/* Tests terrain, solid prop bases, and room bounds against a feet collider. */
bool      Room_Collides(Rectangle collider);
/* Tests collision against the requested side of the authored stair connector. */
bool      Room_CollidesInArea(RoomArea area, Rectangle collider);
bool      Room_CollidesAtElevation(RoomArea area, Rectangle collider,
                                   RoomElevation elevation);
bool      Room_CollidesAtElevationWithGate(RoomArea area, Rectangle collider,
                                           RoomElevation elevation,
                                           bool thornGateOpen);
/* Changes elevation only when a move crosses the midpoint of the stair connector. */
void      Room_TryRampTransition(RoomArea area, Vector2 from, Vector2 to,
                                 RoomElevation *elevation);

#endif
