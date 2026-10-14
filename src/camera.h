#ifndef PALM_CAMERA_H
#define PALM_CAMERA_H

#include "raylib.h"

typedef struct CameraFollow {
    Vector2 center;
    Vector2 lookAhead;
    Rectangle roomBounds;
    Vector2 viewport;
} CameraFollow;

/* Initializes at the bounded player target without a camera sweep. */
void Camera_Init(CameraFollow *camera, Vector2 playerPosition, Rectangle roomBounds,
                 Vector2 viewport);
/* Smooths camera center and movement look-ahead; call once per fixed step. */
void Camera_Update(CameraFollow *camera, Vector2 playerPosition, Vector2 playerVelocity,
                   float dt);
/* Returns a camera with whole-pixel target coordinates for rendering. */
Camera2D Camera_Render(const CameraFollow *camera);

#endif
