#include "camera.h"
#include "tuning.h"
#include <math.h>

static float ClampCenter(float target, float roomStart, float roomSize, float viewSize)
{
    if (roomSize <= viewSize) return roomStart + roomSize * 0.5f;

    float minCenter = roomStart + viewSize * 0.5f;
    float maxCenter = roomStart + roomSize - viewSize * 0.5f;
    if (target < minCenter) return minCenter;
    if (target > maxCenter) return maxCenter;
    return target;
}

static Vector2 ClampTarget(const CameraFollow *camera, Vector2 target)
{
    return (Vector2){
        ClampCenter(target.x, camera->roomBounds.x, camera->roomBounds.width, camera->viewport.x),
        ClampCenter(target.y, camera->roomBounds.y, camera->roomBounds.height, camera->viewport.y)
    };
}

static Vector2 Smooth(Vector2 current, Vector2 target, float dt)
{
    float blend = 1.0f - expf(-CAMERA_FOLLOW_RATE * dt);
    return (Vector2){
        current.x + (target.x - current.x) * blend,
        current.y + (target.y - current.y) * blend
    };
}

void Camera_Init(CameraFollow *camera, Vector2 playerPosition, Rectangle roomBounds,
                 Vector2 viewport)
{
    camera->roomBounds = roomBounds;
    camera->viewport = viewport;
    camera->lookAhead = (Vector2){0.0f, 0.0f};
    camera->center = ClampTarget(camera, playerPosition);
}

void Camera_Update(CameraFollow *camera, Vector2 playerPosition, Vector2 playerVelocity,
                   float dt)
{
    float speed = sqrtf(playerVelocity.x * playerVelocity.x +
                        playerVelocity.y * playerVelocity.y);
    Vector2 desiredLookAhead = {0.0f, 0.0f};
    if (speed > 0.0f) {
        float strength = speed / WALK_SPEED;
        if (strength > 1.0f) strength = 1.0f;
        float scale = CAMERA_LOOK_AHEAD * strength / speed;
        desiredLookAhead = (Vector2){playerVelocity.x * scale, playerVelocity.y * scale};
    }

    camera->lookAhead = Smooth(camera->lookAhead, desiredLookAhead, dt);
    Vector2 desiredCenter = {
        playerPosition.x + camera->lookAhead.x,
        playerPosition.y + camera->lookAhead.y
    };
    desiredCenter = ClampTarget(camera, desiredCenter);
    camera->center = ClampTarget(camera, Smooth(camera->center, desiredCenter, dt));
}

Camera2D Camera_Render(const CameraFollow *camera)
{
    Camera2D renderCamera = {0};
    renderCamera.target = (Vector2){
        roundf(camera->center.x),
        roundf(camera->center.y)
    };
    renderCamera.offset = (Vector2){
        camera->viewport.x * 0.5f,
        camera->viewport.y * 0.5f
    };
    renderCamera.rotation = 0.0f;
    renderCamera.zoom = 1.0f;
    return renderCamera;
}
