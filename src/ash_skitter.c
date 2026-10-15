#include "ash_skitter.h"
#include "tuning.h"
#include <math.h>

static void EnterState(AshSkitter *skitter, AshSkitterState state)
{
    skitter->state = state;
    skitter->stateTime = 0.0f;
}

static float TargetX(const AshSkitter *skitter)
{
    return skitter->direction > 0 ? ASH_SKITTER_PATCH_RIGHT_X
                                  : ASH_SKITTER_PATCH_LEFT_X;
}

void AshSkitter_Init(AshSkitter *skitter)
{
    skitter->position = (Vector2){ASH_SKITTER_PATCH_LEFT_X,
                                  ASH_SKITTER_LANE_Y};
    skitter->state = ASH_SKITTER_STATE_DORMANT;
    skitter->stateTime = 0.0f;
    skitter->direction = 1;
    skitter->health = ASH_SKITTER_HEALTH;
    skitter->hitThisRecovery = false;
    skitter->playerHitThisDash = false;
}

void AshSkitter_Update(AshSkitter *skitter, Vector2 playerPosition, float dt)
{
    if (dt <= 0.0f || skitter->state == ASH_SKITTER_STATE_DEFEATED) return;

    if (skitter->state == ASH_SKITTER_STATE_DORMANT) {
        float dx = playerPosition.x - skitter->position.x;
        float dy = playerPosition.y - skitter->position.y;
        if (dx * dx + dy * dy <=
            ASH_SKITTER_ALERT_RANGE * ASH_SKITTER_ALERT_RANGE) {
            EnterState(skitter, ASH_SKITTER_STATE_TELL);
        }
        return;
    }

    skitter->stateTime += dt;
    if (skitter->state == ASH_SKITTER_STATE_TELL) {
        if (skitter->stateTime >= ASH_SKITTER_TELL_TIME) {
            skitter->playerHitThisDash = false;
            EnterState(skitter, ASH_SKITTER_STATE_DASH);
        }
        return;
    }

    if (skitter->state == ASH_SKITTER_STATE_DASH) {
        float nextX = skitter->position.x +
                      (float)skitter->direction * ASH_SKITTER_DASH_SPEED * dt;
        float targetX = TargetX(skitter);
        if ((skitter->direction < 0 && nextX <= targetX) ||
            (skitter->direction > 0 && nextX >= targetX)) {
            skitter->position.x = targetX;
            skitter->hitThisRecovery = false;
            EnterState(skitter, ASH_SKITTER_STATE_RECOVERY);
        } else {
            skitter->position.x = nextX;
        }
        return;
    }

    if (skitter->state == ASH_SKITTER_STATE_RECOVERY &&
        skitter->stateTime >= ASH_SKITTER_RECOVERY_TIME) {
        skitter->direction = -skitter->direction;
        EnterState(skitter, ASH_SKITTER_STATE_TELL);
    }
}

bool AshSkitter_TryPlayerContact(AshSkitter *skitter,
                                 Vector2 playerPosition,
                                 RoomElevation playerElevation)
{
    if (playerElevation != ROOM_ELEVATION_GROUND ||
        skitter->state != ASH_SKITTER_STATE_DASH ||
        skitter->playerHitThisDash ||
        fabsf(playerPosition.x - skitter->position.x) >
            ASH_SKITTER_CONTACT_HALF_WIDTH + PLAYER_COLLIDER_W * 0.5f ||
        fabsf(playerPosition.y - skitter->position.y) >
            ASH_SKITTER_CONTACT_HALF_HEIGHT + PLAYER_COLLIDER_H * 0.5f) {
        return false;
    }
    skitter->playerHitThisDash = true;
    return true;
}

bool AshSkitter_TryHit(AshSkitter *skitter, Vector2 playerPosition,
                       Vector2 facingDirection, RoomElevation playerElevation,
                       InventoryWeapon weapon)
{
    bool canStrike = weapon == INVENTORY_WEAPON_PALMWOOD_CLUB ||
                     weapon == INVENTORY_WEAPON_THORNFANG;
    if (playerElevation != ROOM_ELEVATION_GROUND || !canStrike ||
        skitter->state != ASH_SKITTER_STATE_RECOVERY ||
        skitter->hitThisRecovery) {
        return false;
    }

    Vector2 toSkitter = {
        skitter->position.x - playerPosition.x,
        skitter->position.y - playerPosition.y
    };
    float distance = sqrtf(toSkitter.x * toSkitter.x +
                           toSkitter.y * toSkitter.y);
    if (distance > ASH_SKITTER_PLAYER_HIT_RANGE || distance == 0.0f) {
        return false;
    }
    float facingDot = (facingDirection.x * toSkitter.x +
                       facingDirection.y * toSkitter.y) / distance;
    if (facingDot < 0.5f) return false;

    skitter->hitThisRecovery = true;
    skitter->health--;
    if (skitter->health <= 0) {
        EnterState(skitter, ASH_SKITTER_STATE_DEFEATED);
    }
    return true;
}

void AshSkitter_DrawWarning(const AshSkitter *skitter)
{
    Vector2 leftPatch = {ASH_SKITTER_PATCH_LEFT_X, ASH_SKITTER_LANE_Y};
    Vector2 rightPatch = {ASH_SKITTER_PATCH_RIGHT_X, ASH_SKITTER_LANE_Y};
    DrawCircleV(leftPatch, 11.0f, (Color){62, 61, 53, 210});
    DrawCircleV(rightPatch, 11.0f, (Color){62, 61, 53, 210});
    DrawCircleLines((int)leftPatch.x, (int)leftPatch.y, 11.0f,
                    (Color){156, 127, 77, 220});
    DrawCircleLines((int)rightPatch.x, (int)rightPatch.y, 11.0f,
                    (Color){156, 127, 77, 220});
    if (skitter->state != ASH_SKITTER_STATE_TELL) return;

    float targetX = TargetX(skitter);
    DrawLineEx((Vector2){skitter->position.x, skitter->position.y},
               (Vector2){targetX, skitter->position.y}, 4.0f,
               (Color){255, 183, 74, 110});
    DrawLineEx((Vector2){skitter->position.x, skitter->position.y},
               (Vector2){targetX, skitter->position.y}, 1.0f,
               (Color){255, 226, 145, 240});
    DrawCircleLines((int)targetX, (int)skitter->position.y, 9.0f,
                    (Color){255, 226, 145, 230});
}

void AshSkitter_Draw(const AshSkitter *skitter)
{
    int x = (int)floorf(skitter->position.x);
    int y = (int)floorf(skitter->position.y);
    Color body = {232, 105, 46, 255};
    Color shell = {82, 52, 43, 255};
    if (skitter->state == ASH_SKITTER_STATE_TELL) {
        body = (Color){255, 220, 105, 255};
        shell = (Color){120, 73, 39, 255};
    } else if (skitter->state == ASH_SKITTER_STATE_RECOVERY) {
        body = (Color){255, 177, 101, 255};
        shell = (Color){101, 70, 48, 255};
    } else if (skitter->state == ASH_SKITTER_STATE_DEFEATED) {
        body = (Color){116, 91, 77, 255};
        shell = (Color){65, 57, 53, 255};
    }

    DrawEllipse(x, y + 2, 11.0f, 4.0f, (Color){8, 24, 28, 115});
    for (int side = -1; side <= 1; side += 2) {
        DrawLineEx((Vector2){(float)x + side * 4.0f, (float)y + 1.0f},
                   (Vector2){(float)x + side * 8.0f, (float)y + 5.0f},
                   2.0f, shell);
        DrawLineEx((Vector2){(float)x + side * 4.0f, (float)y - 1.0f},
                   (Vector2){(float)x + side * 8.0f, (float)y - 5.0f},
                   2.0f, shell);
    }
    DrawEllipse(x, y, 8.0f, 5.0f, shell);
    DrawEllipse(x, y - 1, 5.0f, 3.0f, body);
    DrawPixel(x + skitter->direction * 3, y - 2, (Color){255, 244, 191, 255});
    if (skitter->state == ASH_SKITTER_STATE_TELL) {
        DrawCircleLines(x, y, 10.0f, (Color){255, 226, 145, 220});
    } else if (skitter->state == ASH_SKITTER_STATE_RECOVERY) {
        DrawLineEx((Vector2){(float)x - 4.0f, (float)y + 1.0f},
                   (Vector2){(float)x + 4.0f, (float)y + 1.0f}, 1.0f,
                   (Color){255, 238, 178, 255});
    }
}
