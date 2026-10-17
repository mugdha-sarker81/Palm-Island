#ifndef PALM_ROOT_MITE_H
#define PALM_ROOT_MITE_H

#include "raylib.h"
#include "inventory.h"
#include "room.h"
#include <stdbool.h>

typedef enum RootMiteState {
    ROOT_MITE_STATE_DORMANT,
    ROOT_MITE_STATE_APPROACH,
    ROOT_MITE_STATE_PAUSE,
    ROOT_MITE_STATE_LUNGE,
    ROOT_MITE_STATE_RECOVERY,
    ROOT_MITE_STATE_DEFEATED
} RootMiteState;

typedef struct RootMite {
    Vector2 position;
    Vector2 facing;         /* last movement/aim direction, unit length */
    Vector2 lungeDirection; /* locked at pause entry so the tell is honest */
    RootMiteState state;
    float stateTime;
    int health;
    bool hitThisRecovery;
    bool playerHitThisLunge;
} RootMite;

/* rewardClaimed seeds the defeated state for saves that already collected
 * the Root Path palm-fruit drop; otherwise the mite starts dormant. */
void RootMite_Init(RootMite *mite, bool rewardClaimed);
void RootMite_Update(RootMite *mite, Vector2 playerPosition, float dt);
bool RootMite_TryPlayerContact(RootMite *mite, Vector2 playerPosition,
                               RoomElevation playerElevation);
bool RootMite_TryHit(RootMite *mite, Vector2 playerPosition,
                     Vector2 facingDirection, RoomElevation playerElevation,
                     InventoryWeapon weapon);
void RootMite_DrawWarning(const RootMite *mite);
void RootMite_Draw(const RootMite *mite);

#endif