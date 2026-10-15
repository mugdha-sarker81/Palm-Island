#ifndef PALM_INPUT_H
#define PALM_INPUT_H

#include "raylib.h"

/* Movement intent for one frame. Magnitude is 0..1 (keyboard is always 0 or 1). */
typedef struct Input {
    Vector2 move;
    float magnitude;
    bool usingGamepad;
    bool interactPressed;
    bool attackPressed;
    bool inventoryPressed;
} Input;

/* Poll keyboard and gamepad once per rendered frame. */
void Input_Poll(Input *in);

#endif
