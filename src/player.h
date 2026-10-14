#ifndef PALM_PLAYER_H
#define PALM_PLAYER_H

#include "raylib.h"
#include "input.h"

/* Row order in assets/sprites/player_sheet_14x20.png */
typedef enum Facing {
    FACING_DOWN = 0,
    FACING_LEFT = 1,
    FACING_RIGHT = 2,
    FACING_UP = 3
} Facing;

typedef struct Player {
    Vector2 pos;     /* feet point (bottom-centre), the world anchor */
    Vector2 vel;     /* px/s */
    Facing facing;
    float animTime;  /* seconds spent walking since last stop */
} Player;

void  Player_Init(Player *p, Vector2 feet);
/* Advance by one fixed simulation step. bounds limits the feet point. */
void  Player_Update(Player *p, const Input *in, float dt, Rectangle bounds);
float Player_Speed(const Player *p);
/* Draw shadow and sprite. Snaps to whole pixels only here. */
void  Player_Draw(const Player *p, Texture2D sheet);

#endif
