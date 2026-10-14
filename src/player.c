#include "player.h"
#include "tuning.h"
#include <math.h>

static const Vector2 FACING_VEC[4] = {
    { 0.0f,  1.0f},  /* down  */
    {-1.0f,  0.0f},  /* left  */
    { 1.0f,  0.0f},  /* right */
    { 0.0f, -1.0f}   /* up    */
};

static float Dot(Vector2 a, Vector2 b) { return a.x * b.x + a.y * b.y; }
static float Length(Vector2 v)         { return sqrtf(v.x * v.x + v.y * v.y); }

void Player_Init(Player *p, Vector2 feet)
{
    p->pos = feet;
    p->vel = (Vector2){0.0f, 0.0f};
    p->facing = FACING_DOWN;
    p->animTime = 0.0f;
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

void Player_Update(Player *p, const Input *in, float dt, Rectangle bounds)
{
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
    Vector2 delta = {desired.x - p->vel.x, desired.y - p->vel.y};
    float dl = Length(delta);
    float maxStep = accel * dt;
    if (dl <= maxStep) {
        p->vel = desired;
    } else {
        p->vel.x += delta.x / dl * maxStep;
        p->vel.y += delta.y / dl * maxStep;
    }

    if (moving) {
        Vector2 dir = {in->move.x / inLen, in->move.y / inLen};
        p->facing = PickFacing(p->facing, dir);
    }

    p->pos.x += p->vel.x * dt;
    p->pos.y += p->vel.y * dt;

    /* Temporary: stay inside the room until the feet collider arrives (M3). */
    float minX = bounds.x + PLAYER_WALL_MARGIN, maxX = bounds.x + bounds.width  - PLAYER_WALL_MARGIN;
    float minY = bounds.y + PLAYER_SPRITE_H, maxY = bounds.y + bounds.height;
    if (p->pos.x < minX) { p->pos.x = minX; if (p->vel.x < 0.0f) p->vel.x = 0.0f; }
    if (p->pos.x > maxX) { p->pos.x = maxX; if (p->vel.x > 0.0f) p->vel.x = 0.0f; }
    if (p->pos.y < minY) { p->pos.y = minY; if (p->vel.y < 0.0f) p->vel.y = 0.0f; }
    if (p->pos.y > maxY) { p->pos.y = maxY; if (p->vel.y > 0.0f) p->vel.y = 0.0f; }

    /* Walk animation advances with speed, resets when standing still. */
    float speedFrac = Length(p->vel) / WALK_SPEED;
    if (speedFrac > 0.05f) p->animTime += dt * speedFrac;
    else                   p->animTime = 0.0f;
}

float Player_Speed(const Player *p) { return Length(p->vel); }

void Player_Draw(const Player *p, Texture2D sheet)
{
    /* Whole-pixel snapping happens only at draw time. */
    int fx = (int)floorf(p->pos.x);
    int fy = (int)floorf(p->pos.y);

    DrawEllipse(fx, fy - 1, 6.0f, 2.5f, (Color){8, 24, 28, 110});

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
    DrawTextureRec(sheet, src, dst, WHITE);
}
