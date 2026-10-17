#include "root_mite.h"
#include "tuning.h"
#include <math.h>

static void EnterState(RootMite *mite, RootMiteState state)
{
    mite->state = state;
    mite->stateTime = 0.0f;
}

static float Clampf(float value, float minimum, float maximum)
{
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

static void MoveToward(RootMite *mite, Vector2 target, float speed, float dt)
{
    float dx = target.x - mite->position.x;
    float dy = target.y - mite->position.y;
    float distance = sqrtf(dx * dx + dy * dy);
    if (distance < 0.001f) return;

    float step = speed * dt;
    Vector2 direction = {dx / distance, dy / distance};
    if (step >= distance) {
        mite->position.x = target.x;
        mite->position.y = target.y;
    } else {
        mite->position.x += direction.x * step;
        mite->position.y += direction.y * step;
    }
    mite->position.x = Clampf(mite->position.x, ROOT_MITE_TERRITORY_MIN_X,
                              ROOT_MITE_TERRITORY_MAX_X);
    mite->position.y = Clampf(mite->position.y, ROOT_MITE_TERRITORY_MIN_Y,
                              ROOT_MITE_TERRITORY_MAX_Y);
    mite->facing = direction;
}

void RootMite_Init(RootMite *mite, bool rewardClaimed)
{
    mite->position = (Vector2){ROOT_MITE_HOME_X, ROOT_MITE_HOME_Y};
    mite->facing = (Vector2){0.0f, 1.0f};
    mite->lungeDirection = (Vector2){0.0f, 1.0f};
    mite->state = rewardClaimed ? ROOT_MITE_STATE_DEFEATED
                                : ROOT_MITE_STATE_DORMANT;
    mite->stateTime = 0.0f;
    mite->health = rewardClaimed ? 0 : ROOT_MITE_HEALTH;
    mite->hitThisRecovery = false;
    mite->playerHitThisLunge = false;
}

void RootMite_Update(RootMite *mite, Vector2 playerPosition, float dt)
{
    if (dt <= 0.0f || mite->state == ROOT_MITE_STATE_DEFEATED) return;

    float dx = playerPosition.x - mite->position.x;
    float dy = playerPosition.y - mite->position.y;
    float distance = sqrtf(dx * dx + dy * dy);

    if (mite->state == ROOT_MITE_STATE_DORMANT) {
        if (distance <= ROOT_MITE_ALERT_RANGE) {
            EnterState(mite, ROOT_MITE_STATE_APPROACH);
        }
        return;
    }

    mite->stateTime += dt;
    if (mite->state == ROOT_MITE_STATE_APPROACH) {
        if (distance <= ROOT_MITE_PAUSE_TRIGGER && distance > 0.0f) {
            mite->lungeDirection = (Vector2){dx / distance, dy / distance};
            mite->facing = mite->lungeDirection;
            EnterState(mite, ROOT_MITE_STATE_PAUSE);
            return;
        }

        if (distance > ROOT_MITE_LEASH_RANGE) {
            Vector2 home = {ROOT_MITE_HOME_X, ROOT_MITE_HOME_Y};
            float homeX = home.x - mite->position.x;
            float homeY = home.y - mite->position.y;
            if (homeX * homeX + homeY * homeY <= 2.25f &&
                distance > ROOT_MITE_ALERT_RANGE) {
                mite->position = home;
                EnterState(mite, ROOT_MITE_STATE_DORMANT);
                return;
            }
            MoveToward(mite, home, ROOT_MITE_APPROACH_SPEED, dt);
            return;
        }

        MoveToward(mite, playerPosition, ROOT_MITE_APPROACH_SPEED, dt);
        return;
    }

    if (mite->state == ROOT_MITE_STATE_PAUSE) {
        if (mite->stateTime >= ROOT_MITE_PAUSE_TIME) {
            mite->playerHitThisLunge = false;
            EnterState(mite, ROOT_MITE_STATE_LUNGE);
        }
        return;
    }

    if (mite->state == ROOT_MITE_STATE_LUNGE) {
        float nextX = mite->position.x +
                      mite->lungeDirection.x * ROOT_MITE_LUNGE_SPEED * dt;
        float nextY = mite->position.y +
                      mite->lungeDirection.y * ROOT_MITE_LUNGE_SPEED * dt;
        bool hitBounds =
            nextX < ROOT_MITE_TERRITORY_MIN_X ||
            nextX > ROOT_MITE_TERRITORY_MAX_X ||
            nextY < ROOT_MITE_TERRITORY_MIN_Y ||
            nextY > ROOT_MITE_TERRITORY_MAX_Y;
        mite->position.x = Clampf(nextX, ROOT_MITE_TERRITORY_MIN_X,
                                  ROOT_MITE_TERRITORY_MAX_X);
        mite->position.y = Clampf(nextY, ROOT_MITE_TERRITORY_MIN_Y,
                                  ROOT_MITE_TERRITORY_MAX_Y);
        if (hitBounds || mite->stateTime >= ROOT_MITE_LUNGE_TIME) {
            mite->hitThisRecovery = false;
            EnterState(mite, ROOT_MITE_STATE_RECOVERY);
        }
        return;
    }

    if (mite->state == ROOT_MITE_STATE_RECOVERY &&
        mite->stateTime >= ROOT_MITE_RECOVERY_TIME) {
        EnterState(mite, distance <= ROOT_MITE_ALERT_RANGE
                              ? ROOT_MITE_STATE_APPROACH
                              : ROOT_MITE_STATE_DORMANT);
    }
}

bool RootMite_TryPlayerContact(RootMite *mite, Vector2 playerPosition,
                               RoomElevation playerElevation)
{
    if (playerElevation != ROOM_ELEVATION_GROUND ||
        mite->state != ROOT_MITE_STATE_LUNGE ||
        mite->playerHitThisLunge ||
        fabsf(playerPosition.x - mite->position.x) >
            ROOT_MITE_CONTACT_HALF_WIDTH + PLAYER_COLLIDER_W * 0.5f ||
        fabsf(playerPosition.y - mite->position.y) >
            ROOT_MITE_CONTACT_HALF_HEIGHT + PLAYER_COLLIDER_H * 0.5f) {
        return false;
    }
    mite->playerHitThisLunge = true;
    return true;
}

bool RootMite_TryHit(RootMite *mite, Vector2 playerPosition,
                     Vector2 facingDirection, RoomElevation playerElevation,
                     InventoryWeapon weapon)
{
    bool canStrike = weapon == INVENTORY_WEAPON_PALMWOOD_CLUB ||
                     weapon == INVENTORY_WEAPON_THORNFANG;
    if (playerElevation != ROOM_ELEVATION_GROUND || !canStrike ||
        mite->state != ROOT_MITE_STATE_RECOVERY || mite->hitThisRecovery) {
        return false;
    }

    Vector2 toMite = {
        mite->position.x - playerPosition.x,
        mite->position.y - playerPosition.y
    };
    float distance = sqrtf(toMite.x * toMite.x + toMite.y * toMite.y);
    if (distance > ROOT_MITE_PLAYER_HIT_RANGE || distance == 0.0f) {
        return false;
    }
    float facingDot = (facingDirection.x * toMite.x +
                       facingDirection.y * toMite.y) / distance;
    if (facingDot < 0.5f) return false;

    mite->hitThisRecovery = true;
    mite->health--;
    if (mite->health <= 0) {
        EnterState(mite, ROOT_MITE_STATE_DEFEATED);
    }
    return true;
}

void RootMite_DrawWarning(const RootMite *mite)
{
    if (mite->state == ROOT_MITE_STATE_DEFEATED) return;

    /* Nest patch: shows the territory even while the mite sleeps. */
    Vector2 home = {ROOT_MITE_HOME_X, ROOT_MITE_HOME_Y};
    DrawCircleV(home, 12.0f, (Color){47, 38, 26, 200});
    DrawCircleLines((int)home.x, (int)home.y, 12.0f,
                    (Color){141, 116, 74, 210});
    if (mite->state != ROOT_MITE_STATE_PAUSE) return;

    /* Honest tell: the line points where the locked lunge will travel. */
    float travel = ROOT_MITE_LUNGE_SPEED * ROOT_MITE_LUNGE_TIME;
    Vector2 target = {
        mite->position.x + mite->lungeDirection.x * travel,
        mite->position.y + mite->lungeDirection.y * travel
    };
    target.x = Clampf(target.x, ROOT_MITE_TERRITORY_MIN_X,
                      ROOT_MITE_TERRITORY_MAX_X);
    target.y = Clampf(target.y, ROOT_MITE_TERRITORY_MIN_Y,
                      ROOT_MITE_TERRITORY_MAX_Y);
    DrawLineEx(mite->position, target, 4.0f, (Color){230, 158, 66, 110});
    DrawLineEx(mite->position, target, 1.0f, (Color){255, 226, 145, 240});
    DrawCircleLines((int)target.x, (int)target.y, 9.0f,
                    (Color){255, 226, 145, 230});
}

void RootMite_Draw(const RootMite *mite)
{
    int x = (int)floorf(mite->position.x);
    int y = (int)floorf(mite->position.y);
    Color body = {166, 92, 40, 255};
    Color shell = {56, 42, 32, 255};
    if (mite->state == ROOT_MITE_STATE_PAUSE) {
        body = (Color){255, 220, 105, 255};
        shell = (Color){112, 74, 40, 255};
    } else if (mite->state == ROOT_MITE_STATE_RECOVERY) {
        body = (Color){255, 177, 101, 255};
        shell = (Color){96, 68, 50, 255};
    } else if (mite->state == ROOT_MITE_STATE_DEFEATED) {
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
        DrawLineEx((Vector2){(float)x + side * 4.0f, (float)y + 3.0f},
                   (Vector2){(float)x + side * 7.0f, (float)y + 7.0f},
                   2.0f, shell);
    }
    DrawEllipse(x, y, 8.0f, 5.0f, shell);
    DrawEllipse(x, y - 1, 5.0f, 3.0f, body);
    int eyeX = x + (int)(mite->facing.x * 3.0f);
    int eyeY = y + (int)(mite->facing.y * 2.0f) - 2;
    DrawPixel(eyeX, eyeY, (Color){255, 244, 191, 255});
    if (mite->state == ROOT_MITE_STATE_PAUSE) {
        DrawCircleLines(x, y, 10.0f, (Color){255, 226, 145, 220});
    } else if (mite->state == ROOT_MITE_STATE_RECOVERY) {
        DrawLineEx((Vector2){(float)x - 4.0f, (float)y + 1.0f},
                   (Vector2){(float)x + 4.0f, (float)y + 1.0f},
                   1.0f, (Color){255, 238, 178, 255});
    }
}