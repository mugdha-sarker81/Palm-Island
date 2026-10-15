#ifndef PALM_THORNBACK_H
#define PALM_THORNBACK_H

#include "raylib.h"
#include <stdbool.h>

typedef enum ThornbackState {
    THORNBACK_STATE_DORMANT,
    THORNBACK_STATE_TELL,
    THORNBACK_STATE_CHARGE,
    THORNBACK_STATE_SWEEP_TELL,
    THORNBACK_STATE_SWEEP,
    THORNBACK_STATE_RECOVERY,
    THORNBACK_STATE_DEFEATED
} ThornbackState;

typedef struct Thornback {
    Vector2 position;
    ThornbackState state;
    float stateTime;
    int direction;
    bool nextAttackIsSweep;
    int health;
    bool hitThisRecovery;
    bool playerHitThisAttack;
} Thornback;

void Thornback_Init(Thornback *thornback);
bool Thornback_Start(Thornback *thornback, Vector2 playerPosition);
void Thornback_Update(Thornback *thornback, float dt);
bool Thornback_IsInSweepLane(const Thornback *thornback, Vector2 position);
bool Thornback_IsPlayerInAttackZone(const Thornback *thornback,
                                    Vector2 playerPosition);
bool Thornback_TryPlayerContact(Thornback *thornback,
                                Vector2 playerPosition);
bool Thornback_TryHit(Thornback *thornback, Vector2 playerPosition,
                      Vector2 facingDirection);
void Thornback_DrawWarning(const Thornback *thornback);
void Thornback_Draw(const Thornback *thornback);

#endif
