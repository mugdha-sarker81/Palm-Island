#include "camera.h"
#include "room.h"
#include "tuning.h"
#include <math.h>
#include <stdio.h>

static int failures;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static const Vector2 VIEWPORT = {VIRTUAL_W, VIRTUAL_H};
static const Rectangle LARGE_ROOM = {0.0f, 0.0f, 640.0f, 360.0f};
static const float EPSILON = 0.01f;

static void TestSmallRoomCentersAndStaysCentered(void)
{
    CameraFollow camera;
    Camera_Init(&camera, (Vector2){40.0f, 30.0f},
                (Rectangle){0.0f, 0.0f, 320.0f, 176.0f}, VIEWPORT);

    CHECK(fabsf(camera.center.x - 160.0f) < EPSILON);
    CHECK(fabsf(camera.center.y - 88.0f) < EPSILON);

    for (int i = 0; i < SIM_HZ; i++) {
        Camera_Update(&camera, (Vector2){280.0f, 150.0f},
                      (Vector2){WALK_SPEED, WALK_SPEED}, SIM_DT);
    }
    CHECK(fabsf(camera.center.x - 160.0f) < EPSILON);
    CHECK(fabsf(camera.center.y - 88.0f) < EPSILON);
}

static void TestFollowSmoothsTowardPlayer(void)
{
    CameraFollow camera;
    Camera_Init(&camera, (Vector2){250.0f, 150.0f}, LARGE_ROOM, VIEWPORT);
    Camera_Update(&camera, (Vector2){300.0f, 200.0f},
                  (Vector2){0.0f, 0.0f}, SIM_DT);

    CHECK(camera.center.x > 250.0f && camera.center.x < 300.0f);
    CHECK(camera.center.y > 150.0f && camera.center.y < 200.0f);
}

static void TestLookAheadEasesInAndOut(void)
{
    CameraFollow camera;
    Camera_Init(&camera, (Vector2){250.0f, 150.0f}, LARGE_ROOM, VIEWPORT);
    Camera_Update(&camera, (Vector2){250.0f, 150.0f},
                  (Vector2){WALK_SPEED, 0.0f}, SIM_DT);

    CHECK(camera.lookAhead.x > 0.0f);
    CHECK(camera.lookAhead.x < CAMERA_LOOK_AHEAD);

    float lookAheadDuringMotion = camera.lookAhead.x;
    for (int i = 0; i < SIM_HZ; i++) {
        Camera_Update(&camera, (Vector2){250.0f, 150.0f},
                      (Vector2){0.0f, 0.0f}, SIM_DT);
    }
    CHECK(camera.lookAhead.x >= 0.0f);
    CHECK(camera.lookAhead.x < lookAheadDuringMotion);
    CHECK(camera.lookAhead.x < EPSILON);
}

static void TestRoomBoundsClampCamera(void)
{
    CameraFollow camera;
    Camera_Init(&camera, (Vector2){10.0f, 10.0f}, LARGE_ROOM, VIEWPORT);
    CHECK(fabsf(camera.center.x - 160.0f) < EPSILON);
    CHECK(fabsf(camera.center.y - 90.0f) < EPSILON);

    for (int i = 0; i < SIM_HZ * 3; i++) {
        Camera_Update(&camera, (Vector2){1000.0f, 1000.0f},
                      (Vector2){WALK_SPEED, WALK_SPEED}, SIM_DT);
    }
    CHECK(fabsf(camera.center.x - 480.0f) < EPSILON);
    CHECK(fabsf(camera.center.y - 270.0f) < EPSILON);

    for (int i = 0; i < SIM_HZ * 3; i++) {
        Camera_Update(&camera, (Vector2){-1000.0f, -1000.0f},
                      (Vector2){-WALK_SPEED, -WALK_SPEED}, SIM_DT);
    }
    CHECK(fabsf(camera.center.x - 160.0f) < EPSILON);
    CHECK(fabsf(camera.center.y - 90.0f) < EPSILON);
}

static void TestActualRoomAllowsCameraScrolling(void)
{
    Rectangle bounds = Room_Bounds();
    Vector2 spawn = Room_SpawnPoint(ROOM_AREA_JUNGLE_ENTRANCE);
    CameraFollow camera;

    CHECK(bounds.width > VIEWPORT.x);
    CHECK(bounds.height > VIEWPORT.y);
    Camera_Init(&camera, spawn, bounds, VIEWPORT);
    CHECK(fabsf(camera.center.x - spawn.x) < EPSILON);
    CHECK(fabsf(camera.center.y - (bounds.height - VIEWPORT.y * 0.5f)) < EPSILON);

    Vector2 distantPlayer = {spawn.x + 180.0f, spawn.y - 100.0f};
    for (int i = 0; i < SIM_HZ * 2; i++) {
        Camera_Update(&camera, distantPlayer, (Vector2){WALK_SPEED, WALK_SPEED}, SIM_DT);
    }
    CHECK(camera.center.x > spawn.x);
    CHECK(camera.center.y < spawn.y);
    CHECK(camera.center.x <= bounds.width - VIEWPORT.x * 0.5f);
    CHECK(camera.center.y <= bounds.height - VIEWPORT.y * 0.5f);
}

static void TestInitializationHardSnapsAndRenderRounds(void)
{
    CameraFollow camera;
    Camera_Init(&camera, (Vector2){1000.0f, 1000.0f}, LARGE_ROOM, VIEWPORT);
    CHECK(fabsf(camera.center.x - 480.0f) < EPSILON);
    CHECK(fabsf(camera.center.y - 270.0f) < EPSILON);

    camera.center = (Vector2){160.4f, 90.6f};
    Camera2D renderCamera = Camera_Render(&camera);
    CHECK(renderCamera.target.x == 160.0f);
    CHECK(renderCamera.target.y == 91.0f);
    CHECK(renderCamera.offset.x == VIRTUAL_W * 0.5f);
    CHECK(renderCamera.offset.y == VIRTUAL_H * 0.5f);
    CHECK(renderCamera.zoom == 1.0f);
}

int main(void)
{
    TestSmallRoomCentersAndStaysCentered();
    TestFollowSmoothsTowardPlayer();
    TestLookAheadEasesInAndOut();
    TestRoomBoundsClampCamera();
    TestActualRoomAllowsCameraScrolling();
    TestInitializationHardSnapsAndRenderRounds();

    if (failures != 0) {
        fprintf(stderr, "%d camera check(s) failed.\n", failures);
        return 1;
    }

    puts("All camera follow checks passed.");
    return 0;
}
