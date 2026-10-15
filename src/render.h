#ifndef PALM_RENDER_H
#define PALM_RENDER_H

#include "player.h"
#include "room.h"
#include "thornback.h"
#include <stddef.h>

typedef enum RenderItemType {
    RENDER_ITEM_PLAYER,
    RENDER_ITEM_THORNBACK,
    RENDER_ITEM_PROP,
    RENDER_ITEM_FOOD
} RenderItemType;

typedef struct RenderItem {
    RenderItemType type;
    Vector2 position;
    float feetY;
    RoomElevation elevation;
    unsigned int stableOrder;
    bool foreground;
    Texture2D texture;
} RenderItem;

#define RENDER_PROP_COUNT ROOM_PROP_COUNT

/* Sorts world items back-to-front, with foreground items last and stable ties. */
void Render_SortItems(RenderItem *items, size_t count);
void Render_DrawDepthScene(const Player *player, Texture2D playerSheet,
                           const RenderItem props[RENDER_PROP_COUNT],
                           const Thornback *thornback,
                           const RenderItem *foodPickup, bool debug);

#endif
