#include "player.h"
#include "tuning.h"
#include "room.h"
#include <math.h>

static const Vector2 FACING_VEC[4] = {
    { 0.0f,  1.0f},  /* down  */
    {-1.0f,  0.0f},  /* left  */
    { 1.0f,  0.0f},  /* right */
    { 0.0f, -1.0f}   /* up    */
};

static float Dot(Vector2 a, Vector2 b) { return a.x * b.x + a.y * b.y; }
static float Length(Vector2 v)         { return sqrtf(v.x * v.x + v.y * v.y); }

static Vector2 Approach(Vector2 current, Vector2 target, float maxStep)
{
    Vector2 delta = {target.x - current.x, target.y - current.y};
    float length = Length(delta);
    if (length <= maxStep) return target;
    return (Vector2){
        current.x + delta.x / length * maxStep,
        current.y + delta.y / length * maxStep
    };
}

void Player_Init(Player *p, Vector2 feet)
{
    p->pos = feet;
    p->vel = (Vector2){0.0f, 0.0f};
    p->elevation = ROOM_ELEVATION_GROUND;
    p->facing = FACING_DOWN;
    p->animTime = 0.0f;
    p->health = PLAYER_START_HEALTH;
    p->damageInvulnerability = 0.0f;
    p->thornfangOwned = false;
    p->thornGateOpen = false;
}

/* Keep the current facing while the input stays within FACING_KEEP_DEG of it.
 * This stops the sprite flickering on diagonals. */
static Facing PickFacing(Facing current, Vector2 dir)
{
    const float keepCos = cosf(FACING_KEEP_DEG * (PI / 180.0f));
    if (Dot(dir, FACING_VEC[current]) >= keepCos) return current;

    /* Otherwise choose the closest cardinal; on an exact tie prefer horizontal. */
    if (fabsf(dir.x) >= fabsf(dir.y)) return dir.x < 0.0f ? FACING_LEFT : FACING_RIGHT;
    return dir.y < 0.0f ? FACING_UP : FACING_DOWN;
}

Rectangle Player_FeetCollider(const Player *p)
{
    return (Rectangle){
        p->pos.x - PLAYER_COLLIDER_W * 0.5f,
        p->pos.y - PLAYER_COLLIDER_H * 0.5f,
        PLAYER_COLLIDER_W,
        PLAYER_COLLIDER_H
    };
}

static bool TryMoveTo(Player *p, Vector2 feet, RoomArea area)
{
    RoomElevation candidateElevation = p->elevation;
    Room_TryRampTransition(area, p->pos, feet, &candidateElevation);

    Player candidate = *p;
    candidate.pos = feet;
    candidate.elevation = candidateElevation;
    if (Room_CollidesAtElevationWithGate(area,
                                         Player_FeetCollider(&candidate),
                                         candidateElevation,
                                         p->thornGateOpen)) return false;

    p->pos = feet;
    p->elevation = candidateElevation;
    return true;
}

static bool TryCornerAssist(Player *p, float blockedStep, float perpendicularStep,
                            bool blockedAxisIsX, float *assistOffset, RoomArea area)
{
    float preferredDirection = perpendicularStep > 0.0f ? 1.0f : -1.0f;
    int directionCount = perpendicularStep == 0.0f ? 2 : 1;

    for (int offset = 1; offset <= CORNER_ASSIST_MAX; offset++) {
        for (int directionIndex = 0; directionIndex < directionCount; directionIndex++) {
            float direction = directionIndex == 0 ? preferredDirection : -preferredDirection;
            Vector2 candidate = p->pos;
            if (blockedAxisIsX) candidate.y += direction * (float)offset;
            else                candidate.x += direction * (float)offset;
            if (blockedAxisIsX) candidate.x += blockedStep;
            else                candidate.y += blockedStep;

            if (TryMoveTo(p, candidate, area)) {
                *assistOffset = direction * (float)offset;
                return true;
            }
        }
    }
    return false;
}

static float RemainingStep(float step, float assist)
{
    if (step == 0.0f || assist == 0.0f || step * assist <= 0.0f) return step;
    if (fabsf(assist) >= fabsf(step)) return 0.0f;
    return step - assist;
}

void Player_Update(Player *p, const Input *in, float dt, RoomArea area)
{
    if (dt > 0.0f && p->damageInvulnerability > 0.0f) {
        p->damageInvulnerability -= dt;
        if (p->damageInvulnerability < 0.0f) {
            p->damageInvulnerability = 0.0f;
        }
    }

    float inLen = Length(in->move);
    bool moving = inLen > 0.0f;

    Vector2 desired = {in->move.x * WALK_SPEED, in->move.y * WALK_SPEED};
    float accel = WALK_DECEL;
    if (moving) {
        accel = WALK_ACCEL;
        /* Snappier turns when pushing against current motion. */
        if (Dot(p->vel, desired) < 0.0f) accel *= TURN_ACCEL_MULT;
    }

    /* Move velocity toward desired by at most accel*dt (vector approach). */
    Vector2 startingVelocity = p->vel;
    float maxStep = accel * dt;
    p->vel = Approach(p->vel, desired, maxStep);

    if (moving) {
        Vector2 dir = {in->move.x / inLen, in->move.y / inLen};
        p->facing = PickFacing(p->facing, dir);
    }

    float moveX = p->vel.x * dt;
    float moveY = p->vel.y * dt;
    float longestMove = fmaxf(fabsf(moveX), fabsf(moveY));
    int substeps = (int)ceilf(longestMove / COLLISION_MAX_STEP);
    if (substeps < 1) substeps = 1;
    float stepX = moveX / (float)substeps;
    float stepY = moveY / (float)substeps;
    bool blockedX = false;
    bool blockedY = false;

    for (int i = 0; i < substeps; i++) {
        float xStep = stepX;
        float yStep = stepY;
        float assist = 0.0f;

        Vector2 candidate = {p->pos.x + xStep, p->pos.y};
        if (!TryMoveTo(p, candidate, area)) {
            if (TryCornerAssist(p, xStep, yStep, true, &assist, area)) {
                yStep = RemainingStep(yStep, assist);
            } else {
                blockedX = true;
                stepX = 0.0f;
                xStep = 0.0f;
            }
        }

        candidate = (Vector2){p->pos.x, p->pos.y + yStep};
        if (!TryMoveTo(p, candidate, area)) {
            if (TryCornerAssist(p, yStep, xStep, false, &assist, area)) {
                xStep = RemainingStep(xStep, assist);
            } else {
                blockedY = true;
                stepY = 0.0f;
                yStep = 0.0f;
            }
        }
    }

    if (blockedX || blockedY) {
        Vector2 slideTarget = desired;
        if (blockedX) slideTarget.x = 0.0f;
        if (blockedY) slideTarget.y = 0.0f;
        p->vel = Approach(startingVelocity, slideTarget, maxStep);
        if (blockedX) p->vel.x = 0.0f;
        if (blockedY) p->vel.y = 0.0f;
    }

    /* Walk animation advances with speed, resets when standing still. */
    float speedFrac = Length(p->vel) / WALK_SPEED;
    if (speedFrac > 0.05f) p->animTime += dt * speedFrac;
    else                   p->animTime = 0.0f;
}

float Player_Speed(const Player *p) { return Length(p->vel); }

bool Player_TakeDamage(Player *p, Vector2 source, RoomArea area)
{
    if (p->health <= 0 || p->damageInvulnerability > 0.0f) return false;

    p->health--;
    p->damageInvulnerability = PLAYER_HIT_INVULNERABILITY;
    Vector2 away = {p->pos.x - source.x, p->pos.y - source.y};
    float distance = Length(away);
    if (distance > 0.0f) {
        Vector2 destination = {
            p->pos.x + away.x / distance * PLAYER_HIT_KNOCKBACK,
            p->pos.y + away.y / distance * PLAYER_HIT_KNOCKBACK
        };
        (void)TryMoveTo(p, destination, area);
    }
    p->vel = (Vector2){0.0f, 0.0f};
    return true;
}

void Player_Draw(const Player *p, Texture2D sheet, bool debug)
{
    /* Whole-pixel snapping happens only at draw time. */
    int fx = (int)floorf(p->pos.x);
    int fy = (int)floorf(p->pos.y);

    DrawEllipse(fx, fy - 1, 6.0f, 2.5f, (Color){8, 24, 28, 110});
    if (debug) {
        Rectangle collider = Player_FeetCollider(p);
        DrawRectangleLinesEx(collider, 1.0f, (Color){255, 64, 64, 255});
    }

    int frame = 0; /* idle */
    if (p->animTime > 0.0f) {
        /* Sequence: walk A, idle, walk B, idle (sheet columns 1,2,3,0). */
        frame = (1 + (int)(p->animTime / WALK_FRAME_TIME)) % 4;
    }
    Rectangle src = {
        (float)(frame * PLAYER_SPRITE_W),
        (float)((int)p->facing * PLAYER_SPRITE_H),
        (float)PLAYER_SPRITE_W,
        (float)PLAYER_SPRITE_H
    };
    Vector2 dst = {(float)(fx - PLAYER_SPRITE_W / 2), (float)(fy - PLAYER_SPRITE_H)};
    Color tint = WHITE;
    if (p->damageInvulnerability > 0.0f &&
        ((int)(p->damageInvulnerability * 20.0f) % 2) == 0) {
        tint = Fade(WHITE, 0.25f);
    }
    DrawTextureRec(sheet, src, dst, tint);
}
