#include "render.h"
#include <math.h>

static bool ComesAfter(const RenderItem *left, const RenderItem *right)
{
    if (left->foreground != right->foreground) return left->foreground;
    if (left->elevation != right->elevation) {
        return left->elevation > right->elevation;
    }
    if (left->feetY != right->feetY) return left->feetY > right->feetY;
    return left->stableOrder > right->stableOrder;
}

void Render_SortItems(RenderItem *items, size_t count)
{
    for (size_t i = 1; i < count; i++) {
        RenderItem item = items[i];
        size_t j = i;
        while (j > 0 && ComesAfter(&items[j - 1], &item)) {
            items[j] = items[j - 1];
            j--;
        }
        items[j] = item;
    }
}

static void DrawItem(const RenderItem *item, const Player *player,
                     Texture2D playerSheet, const Thornback *thornback,
                     const AshSkitter *ashSkitter, bool debug)
{
    if (item->type == RENDER_ITEM_PLAYER) {
        Player_Draw(player, playerSheet, debug);
    } else if (item->type == RENDER_ITEM_THORNBACK) {
        Thornback_Draw(thornback);
    } else if (item->type == RENDER_ITEM_ASH_SKITTER) {
        AshSkitter_Draw(ashSkitter);
    } else if (item->type == RENDER_ITEM_FOOD) {
        int x = (int)floorf(item->position.x);
        int y = (int)floorf(item->position.y);
        DrawRectangle(x - 5, y + 1, 10, 3, (Color){38, 49, 31, 220});
        DrawRectangle(x - 3, y - 5, 6, 6, (Color){220, 114, 68, 255});
        DrawRectangle(x - 1, y - 6, 3, 2, (Color){127, 196, 104, 255});
        DrawPixel(x - 1, y - 3, (Color){255, 211, 137, 255});
    } else if (item->texture.id != 0) {
        DrawTexture(item->texture, (int)floorf(item->position.x),
                    (int)floorf(item->position.y), WHITE);
    }
}

void Render_DrawDepthScene(const Player *player, Texture2D playerSheet,
                           const RenderItem props[RENDER_PROP_COUNT],
                           const Thornback *thornback,
                           const AshSkitter *ashSkitter,
                           const RenderItem *foodPickup, bool debug)
{
    RenderItem items[RENDER_PROP_COUNT + 4];
                           size_t itemCount = 1;
    items[0] = (RenderItem){
        .type = RENDER_ITEM_PLAYER,
        .position = player->pos,
        .feetY = player->pos.y,
        .elevation = player->elevation,
        .stableOrder = 0,
        .foreground = false
    };
    if (thornback != NULL &&
        thornback->state != THORNBACK_STATE_DORMANT) {
        items[itemCount++] = (RenderItem){
            .type = RENDER_ITEM_THORNBACK,
            .position = thornback->position,
            .feetY = thornback->position.y,
            .elevation = ROOM_ELEVATION_GROUND,
            .stableOrder = RENDER_PROP_COUNT + 1,
            .foreground = false
        };
    }
    if (ashSkitter != NULL &&
        ashSkitter->state != ASH_SKITTER_STATE_DORMANT) {
        items[itemCount++] = (RenderItem){
            .type = RENDER_ITEM_ASH_SKITTER,
            .position = ashSkitter->position,
            .feetY = ashSkitter->position.y,
            .elevation = ROOM_ELEVATION_GROUND,
            .stableOrder = RENDER_PROP_COUNT + 2,
            .foreground = false
        };
    }
    if (foodPickup != NULL) items[itemCount++] = *foodPickup;
    for (size_t i = 0; i < RENDER_PROP_COUNT; i++) {
        items[itemCount++] = props[i];
    }

    Render_SortItems(items, itemCount);
    for (size_t i = 0; i < itemCount; i++) {
        DrawItem(&items[i], player, playerSheet, thornback, ashSkitter, debug);
    }
}
