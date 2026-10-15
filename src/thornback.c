#include "thornback.h"
#include "tuning.h"
#include <math.h>

static void EnterState(Thornback *thornback, ThornbackState state)
{
    thornback->state = state;
    thornback->stateTime = 0.0f;
}

void Thornback_Init(Thornback *thornback)
{
    thornback->position = (Vector2){THORNBACK_START_X, THORNBACK_LANE_Y};
    thornback->state = THORNBACK_STATE_DORMANT;
    thornback->stateTime = 0.0f;
    thornback->direction = 1;
    thornback->nextAttackIsSweep = false;
    thornback->health = THORNBACK_HEALTH;
    thornback->hitThisRecovery = false;
    thornback->playerHitThisAttack = false;
}

bool Thornback_Start(Thornback *thornback, Vector2 playerPosition)
{
    if (thornback->state != THORNBACK_STATE_DORMANT) return false;
    thornback->direction = playerPosition.x < thornback->position.x ? -1 : 1;
    EnterState(thornback, THORNBACK_STATE_TELL);
    return true;
}

void Thornback_Update(Thornback *thornback, float dt)
{
    if (dt <= 0.0f || thornback->state == THORNBACK_STATE_DORMANT) return;

    if (thornback->state == THORNBACK_STATE_TELL) {
        thornback->stateTime += dt;
        if (thornback->stateTime >= THORNBACK_TELL_TIME) {
            thornback->playerHitThisAttack = false;
            EnterState(thornback, THORNBACK_STATE_CHARGE);
        }
        return;
    }

    if (thornback->state == THORNBACK_STATE_SWEEP_TELL) {
        thornback->stateTime += dt;
        if (thornback->stateTime >= THORNBACK_SWEEP_TELL_TIME) {
            thornback->playerHitThisAttack = false;
            EnterState(thornback, THORNBACK_STATE_SWEEP);
        }
        return;
    }

    if (thornback->state == THORNBACK_STATE_CHARGE) {
        float nextX = thornback->position.x +
                      (float)thornback->direction * THORNBACK_CHARGE_SPEED * dt;
        if (thornback->direction < 0 && nextX <= THORNBACK_LANE_MIN_X) {
            thornback->position.x = THORNBACK_LANE_MIN_X;
            thornback->nextAttackIsSweep = true;
            thornback->hitThisRecovery = false;
            EnterState(thornback, THORNBACK_STATE_RECOVERY);
        } else if (thornback->direction > 0 &&
                   nextX >= THORNBACK_LANE_MAX_X) {
            thornback->position.x = THORNBACK_LANE_MAX_X;
            thornback->nextAttackIsSweep = true;
            thornback->hitThisRecovery = false;
            EnterState(thornback, THORNBACK_STATE_RECOVERY);
        } else {
            thornback->position.x = nextX;
        }
        return;
    }

    if (thornback->state == THORNBACK_STATE_SWEEP) {
        float nextX = thornback->position.x +
                      (float)thornback->direction * THORNBACK_SWEEP_SPEED * dt;
        if (thornback->direction < 0 && nextX <= THORNBACK_LANE_MIN_X) {
            thornback->position.x = THORNBACK_LANE_MIN_X;
            thornback->nextAttackIsSweep = false;
            thornback->hitThisRecovery = false;
            EnterState(thornback, THORNBACK_STATE_RECOVERY);
        } else if (thornback->direction > 0 &&
                   nextX >= THORNBACK_LANE_MAX_X) {
            thornback->position.x = THORNBACK_LANE_MAX_X;
            thornback->nextAttackIsSweep = false;
            thornback->hitThisRecovery = false;
            EnterState(thornback, THORNBACK_STATE_RECOVERY);
        } else {
            thornback->position.x = nextX;
        }
        return;
    }

    if (thornback->state == THORNBACK_STATE_RECOVERY) {
        thornback->stateTime += dt;
        if (thornback->stateTime >= THORNBACK_RECOVERY_TIME) {
            thornback->direction = -thornback->direction;
            EnterState(thornback, thornback->nextAttackIsSweep
                                      ? THORNBACK_STATE_SWEEP_TELL
                                      : THORNBACK_STATE_TELL);
        }
    }
}

bool Thornback_IsInSweepLane(const Thornback *thornback, Vector2 position)
{
    return thornback->state == THORNBACK_STATE_SWEEP &&
           position.x >= THORNBACK_LANE_MIN_X &&
           position.x <= THORNBACK_LANE_MAX_X &&
           fabsf(position.y - THORNBACK_LANE_Y) <=
               THORNBACK_SWEEP_HALF_HEIGHT;
}

bool Thornback_IsPlayerInAttackZone(const Thornback *thornback,
                                    Vector2 playerPosition)
{
    if (thornback->state != THORNBACK_STATE_CHARGE &&
        thornback->state != THORNBACK_STATE_SWEEP) {
        return false;
    }
    float halfWidth = thornback->state == THORNBACK_STATE_SWEEP
                          ? (THORNBACK_LANE_MAX_X - THORNBACK_LANE_MIN_X) *
                                0.5f
                          : THORNBACK_CHARGE_HALF_WIDTH;
    float halfHeight = thornback->state == THORNBACK_STATE_SWEEP
                           ? THORNBACK_SWEEP_HALF_HEIGHT
                           : THORNBACK_CHARGE_HALF_HEIGHT;
    float centerX = thornback->state == THORNBACK_STATE_SWEEP
                        ? (THORNBACK_LANE_MAX_X + THORNBACK_LANE_MIN_X) * 0.5f
                        : thornback->position.x;
    return fabsf(playerPosition.x - centerX) <=
               halfWidth + PLAYER_COLLIDER_W * 0.5f &&
           fabsf(playerPosition.y - THORNBACK_LANE_Y) <=
               halfHeight + PLAYER_COLLIDER_H * 0.5f;
}

bool Thornback_TryPlayerContact(Thornback *thornback,
                                Vector2 playerPosition)
{
    if (thornback->playerHitThisAttack ||
        !Thornback_IsPlayerInAttackZone(thornback, playerPosition)) {
        return false;
    }
    thornback->playerHitThisAttack = true;
    return true;
}

bool Thornback_TryHit(Thornback *thornback, Vector2 playerPosition,
                      Vector2 facingDirection)
{
    if (thornback->state != THORNBACK_STATE_RECOVERY ||
        thornback->hitThisRecovery) {
        return false;
    }

    Vector2 toThornback = {
        thornback->position.x - playerPosition.x,
        thornback->position.y - playerPosition.y
    };
    float distance = sqrtf(toThornback.x * toThornback.x +
                           toThornback.y * toThornback.y);
    if (distance > THORNBACK_PLAYER_HIT_RANGE || distance == 0.0f) {
        return false;
    }
    float facingDot = (facingDirection.x * toThornback.x +
                       facingDirection.y * toThornback.y) / distance;
    if (facingDot < 0.5f) return false;

    thornback->hitThisRecovery = true;
    thornback->health--;
    if (thornback->health <= 0) {
        EnterState(thornback, THORNBACK_STATE_DEFEATED);
    }
    return true;
}

static void DrawChargeLane(Color color)
{
    float y = THORNBACK_LANE_Y;
    DrawLineEx((Vector2){THORNBACK_LANE_MIN_X, y},
               (Vector2){THORNBACK_LANE_MAX_X, y}, 3.0f, color);
    for (int x = (int)THORNBACK_LANE_MIN_X + 8;
         x < (int)THORNBACK_LANE_MAX_X; x += 24) {
        DrawLineEx((Vector2){(float)x, y - 5.0f},
                   (Vector2){(float)x + 8.0f, y - 5.0f}, 1.0f, color);
    }
}

static void DrawSweepLane(Color color)
{
    const int left = (int)THORNBACK_LANE_MIN_X;
    const int top = (int)(THORNBACK_LANE_Y - THORNBACK_SWEEP_HALF_HEIGHT);
    const int width = (int)(THORNBACK_LANE_MAX_X - THORNBACK_LANE_MIN_X);
    const int height = (int)(THORNBACK_SWEEP_HALF_HEIGHT * 2.0f);

    DrawRectangle(left, top, width, height, color);
    DrawLineEx((Vector2){(float)left, (float)top},
               (Vector2){(float)(left + width), (float)top}, 1.0f,
               (Color){255, 220, 147, 230});
    DrawLineEx((Vector2){(float)left, (float)(top + height)},
               (Vector2){(float)(left + width), (float)(top + height)}, 1.0f,
               (Color){255, 220, 147, 230});
    for (int x = left + 12; x < left + width; x += 32) {
        DrawLineEx((Vector2){(float)x, (float)top},
                   (Vector2){(float)x, (float)(top + height)}, 1.0f,
                   (Color){255, 220, 147, 180});
    }
}

static void DrawThorns(Vector2 position, int direction)
{
    const float x = position.x;
    const float y = position.y;
    const Color shell = {57, 82, 59, 255};
    const Color shellLight = {105, 137, 83, 255};
    const Color thorn = {197, 125, 69, 255};
    const Color shadow = {8, 24, 28, 140};

    DrawEllipse((int)x, (int)(y + 3.0f), 15.0f, 5.0f, shadow);
    DrawTriangle((Vector2){x - 13.0f, y - 2.0f},
                 (Vector2){x - 19.0f, y - 8.0f},
                 (Vector2){x - 11.0f, y - 7.0f}, thorn);
    DrawTriangle((Vector2){x - 5.0f, y - 7.0f},
                 (Vector2){x - 3.0f, y - 15.0f},
                 (Vector2){x + 1.0f, y - 7.0f}, thorn);
    DrawTriangle((Vector2){x + 5.0f, y - 7.0f},
                 (Vector2){x + 11.0f, y - 14.0f},
                 (Vector2){x + 11.0f, y - 4.0f}, thorn);
    DrawTriangle((Vector2){x + 13.0f, y - 1.0f},
                 (Vector2){x + 20.0f, y - 3.0f},
                 (Vector2){x + 13.0f, y + 3.0f}, thorn);
    DrawEllipse((int)x, (int)(y - 3.0f), 13.0f, 7.0f, shell);
    DrawEllipse((int)x, (int)(y - 5.0f), 8.0f, 3.0f, shellLight);
    DrawCircle((int)(x + (float)direction * 10.0f),
               (int)(y - 2.0f), 2.0f, (Color){245, 218, 145, 255});
}

void Thornback_DrawWarning(const Thornback *thornback)
{
    if (thornback->state == THORNBACK_STATE_DORMANT ||
        thornback->state == THORNBACK_STATE_DEFEATED) return;

    if (thornback->state == THORNBACK_STATE_TELL) {
        DrawChargeLane((Color){222, 116, 70, 185});
        DrawLineEx(
            (Vector2){thornback->position.x, thornback->position.y - 13.0f},
            (Vector2){thornback->position.x +
                          (float)thornback->direction * 18.0f,
                      thornback->position.y - 13.0f},
            2.0f, (Color){255, 225, 150, 255});
    } else if (thornback->state == THORNBACK_STATE_CHARGE) {
        DrawChargeLane((Color){227, 77, 59, 170});
    } else if (thornback->state == THORNBACK_STATE_SWEEP_TELL) {
        DrawSweepLane((Color){222, 164, 69, 100});
        DrawLineEx(
            (Vector2){thornback->position.x, thornback->position.y - 15.0f},
            (Vector2){thornback->position.x +
                          (float)thornback->direction * 18.0f,
                      thornback->position.y - 15.0f},
            2.0f, (Color){255, 225, 150, 255});
    } else if (thornback->state == THORNBACK_STATE_SWEEP) {
        DrawSweepLane((Color){227, 77, 59, 105});
    }
}

void Thornback_Draw(const Thornback *thornback)
{
    if (thornback->state == THORNBACK_STATE_DORMANT ||
        thornback->state == THORNBACK_STATE_DEFEATED) return;

    if (thornback->state == THORNBACK_STATE_RECOVERY) {
        DrawCircleLines((int)thornback->position.x,
                        (int)thornback->position.y - 3, 17,
                        (Color){127, 196, 192, 210});
    }

    DrawThorns(thornback->position, thornback->direction);
}
