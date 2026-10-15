#include "input.h"
#include "tuning.h"
#include <math.h>

static Vector2 KeyboardMove(void)
{
    Vector2 v = {0.0f, 0.0f};
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  v.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) v.x += 1.0f;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    v.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  v.y += 1.0f;

    /* Normalize so diagonals are not faster. */
    float len = sqrtf(v.x * v.x + v.y * v.y);
    if (len > 0.0f) { v.x /= len; v.y /= len; }
    return v;
}

static float RemapAnalogMagnitude(float magnitude)
{
    if (magnitude <= STICK_DEADZONE) return 0.0f;
    float scaled = (magnitude - STICK_DEADZONE) / (1.0f - STICK_DEADZONE);
    if (scaled > 1.0f) scaled = 1.0f;
    return scaled;
}

static Vector2 GamepadMove(void)
{
    Vector2 v = {0.0f, 0.0f};
    if (!IsGamepadAvailable(0)) return v;

    /* Analog stick with a radial dead zone, remapped so the edge of the
     * dead zone is 0 and a full push is 1 (keeps gentle pushes slow). */
    float x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    float y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
    float len = sqrtf(x * x + y * y);
    float magnitude = RemapAnalogMagnitude(len);
    if (magnitude > 0.0f) {
        v.x = (x / len) * magnitude;
        v.y = (y / len) * magnitude;
        return v;
    }

    /* D-pad fallback (digital, normalized). */
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))  v.x -= 1.0f;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) v.x += 1.0f;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP))    v.y -= 1.0f;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))  v.y += 1.0f;
    len = sqrtf(v.x * v.x + v.y * v.y);
    if (len > 0.0f) { v.x /= len; v.y /= len; }
    return v;
}

void Input_Poll(Input *in)
{
    Vector2 pad = GamepadMove();
    Vector2 key = KeyboardMove();
    bool keyboardInteract = IsKeyPressed(KEY_E);
    bool gamepadInteract = IsGamepadAvailable(0) &&
                           IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    bool keyboardAttack = IsKeyPressed(KEY_SPACE);
    bool gamepadAttack = IsGamepadAvailable(0) &&
                         IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
    bool keyboardInventory = IsKeyPressed(KEY_I) || IsKeyPressed(KEY_TAB);
    bool gamepadInventory = IsGamepadAvailable(0) &&
                            IsGamepadButtonPressed(
                                0, GAMEPAD_BUTTON_RIGHT_FACE_UP);
    bool keyboardUseFood = IsKeyPressed(KEY_H);
    bool gamepadUseFood = IsGamepadAvailable(0) &&
                          IsGamepadButtonPressed(
                              0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
    in->move = (Vector2){0.0f, 0.0f};
    in->magnitude = 0.0f;
    in->usingGamepad = false;
    in->interactPressed = keyboardInteract || gamepadInteract;
    in->attackPressed = keyboardAttack || gamepadAttack;
    in->inventoryPressed = keyboardInventory || gamepadInventory;
    in->useFoodPressed = keyboardUseFood || gamepadUseFood;

    /* Whichever device is actually being pushed wins; gamepad first. */
    if (pad.x != 0.0f || pad.y != 0.0f) {
        in->move = pad;
        in->usingGamepad = true;
    } else {
        in->move = key;
    }

    in->magnitude = sqrtf(in->move.x * in->move.x + in->move.y * in->move.y);
}
