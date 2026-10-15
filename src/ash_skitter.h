#ifndef PALM_ASH_SKITTER_H
#define PALM_ASH_SKITTER_H

#include "raylib.h"
#include "inventory.h"
#include "room.h"
#include <stdbool.h>

typedef enum AshSkitterState {
    ASH_SKITTER_STATE_DORMANT,
    ASH_SKITTER_STATE_TELL,
    ASH_SKITTER_STATE_DASH,
    ASH_SKITTER_STATE_RECOVERY,
    ASH_SKITTER_STATE_DEFEATED
} AshSkitterState;

typedef struct AshSkitter {
    Vector2 position;
    AshSkitterState state;
    float stateTime;
    int direction;
    int health;
    bool hitThisRecovery;
    bool playerHitThisDash;
} AshSkitter;

void AshSkitter_Init(AshSkitter *skitter);
void AshSkitter_Update(AshSkitter *skitter, Vector2 playerPosition, float dt);
bool AshSkitter_TryPlayerContact(AshSkitter *skitter,
                                 Vector2 playerPosition,
                                 RoomElevation playerElevation);
bool AshSkitter_TryHit(AshSkitter *skitter, Vector2 playerPosition,
                       Vector2 facingDirection, RoomElevation playerElevation,
                       InventoryWeapon weapon);
void AshSkitter_DrawWarning(const AshSkitter *skitter);
void AshSkitter_Draw(const AshSkitter *skitter);

#endif
