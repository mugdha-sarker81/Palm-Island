#include "player.h"
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

static Rectangle FeetAt(float x, float y)
{
    return (Rectangle){
        x - PLAYER_COLLIDER_W * 0.5f,
        y - PLAYER_COLLIDER_H * 0.5f,
        PLAYER_COLLIDER_W,
        PLAYER_COLLIDER_H
    };
}

static void AdvanceInArea(Player *player, Vector2 direction, int ticks, RoomArea area)
{
    Input input = {0};
    input.move = direction;
    for (int i = 0; i < ticks; i++) Player_Update(player, &input, SIM_DT, area);
}

static void Advance(Player *player, Vector2 direction, int ticks)
{
    AdvanceInArea(player, direction, ticks, ROOM_AREA_JUNGLE_ENTRANCE);
}

static void TestEntranceBoundsAndRouteLayout(void)
{
    Rectangle bounds = Room_Bounds();
    Vector2 spawn = Room_SpawnPoint(ROOM_AREA_JUNGLE_ENTRANCE);
    CHECK(bounds.width == 640.0f);
    CHECK(bounds.height == 368.0f);
    CHECK(!Room_Collides(FeetAt(spawn.x, spawn.y)));
    CHECK(!Room_Collides(FeetAt(320.0f, 96.0f)));  /* shrine approach */
    CHECK(!Room_Collides(FeetAt(176.0f, 176.0f))); /* west side of loop */
    CHECK(!Room_Collides(FeetAt(464.0f, 160.0f))); /* east side of loop */
    CHECK(Room_Collides(FeetAt(320.0f, 208.0f)));  /* root cluster */
    CHECK(Room_Collides(FeetAt(320.0f, 42.0f)));   /* distant shrine door */
    CHECK(Room_Collides(FeetAt(4.9f, spawn.y)));
    CHECK(!Room_Collides(FeetAt(5.0f, spawn.y)));
}

static void TestPlayerCanLoopAroundRootCluster(void)
{
    Player player;
    Player_Init(&player, Room_SpawnPoint(ROOM_AREA_JUNGLE_ENTRANCE));

    Advance(&player, (Vector2){0.0f, -1.0f}, SIM_HZ * 2);
    CHECK(player.pos.y < 250.0f);
    Advance(&player, (Vector2){-1.0f, 0.0f}, SIM_HZ * 3 / 2);
    CHECK(player.pos.x < 235.0f);
    Advance(&player, (Vector2){0.0f, -1.0f}, SIM_HZ * 3 / 2);
    CHECK(player.pos.y < 160.0f);
    Advance(&player, (Vector2){1.0f, 0.0f}, SIM_HZ * 3 / 2);
    CHECK(player.pos.x > 260.0f);
    CHECK(!Room_Collides(Player_FeetCollider(&player)));
}

static void TestTreeBasesBlockAndCanopiesStayWalkable(void)
{
    const RoomProp *props = Room_Props(ROOM_AREA_JUNGLE_ENTRANCE);
    bool foundPalm = false;
    bool foundUpperCanopySpace = false;
    for (int i = 0; i < ROOM_PROP_COUNT; i++) {
        if (props[i].sprite != ROOM_SPRITE_PALM) continue;
        foundPalm = true;
        CHECK(Room_Collides(props[i].collider));
        Rectangle canopy = {
            props[i].drawPosition.x + 2.0f,
            props[i].drawPosition.y + 4.0f,
            8.0f,
            8.0f
        };
        if (!Room_Collides(canopy)) foundUpperCanopySpace = true;
    }
    CHECK(foundPalm);
    CHECK(foundUpperCanopySpace);
}

static void TestWallSlideAndDiagonalSpeed(void)
{
    Player sliding;
    Player_Init(&sliding, (Vector2){230.0f, 196.0f});
    Advance(&sliding, (Vector2){0.70710678f, 0.70710678f}, 10);
    CHECK(sliding.pos.y > 196.0f);
    CHECK(sliding.pos.x <= 235.01f);
    CHECK(sliding.vel.y >= WALK_SPEED * 0.70f);
    CHECK(!Room_Collides(Player_FeetCollider(&sliding)));

    Player cardinal;
    Player diagonal;
    Player_Init(&cardinal, Room_SpawnPoint(ROOM_AREA_JUNGLE_ENTRANCE));
    Player_Init(&diagonal, Room_SpawnPoint(ROOM_AREA_JUNGLE_ENTRANCE));
    Advance(&cardinal, (Vector2){1.0f, 0.0f}, SIM_HZ);
    Advance(&diagonal, (Vector2){0.70710678f, -0.70710678f}, SIM_HZ);

    float difference = fabsf(Player_Speed(&diagonal) - Player_Speed(&cardinal));
    float percent = difference / Player_Speed(&cardinal) * 100.0f;
    printf("Feel Gate diagonal speed: %.3f%% difference\n", percent);
    CHECK(percent <= 1.0f);
}

static void TestStoppingMeetsFeelGate(void)
{
    Player player;
    Player_Init(&player, Room_SpawnPoint(ROOM_AREA_JUNGLE_ENTRANCE));
    Advance(&player, (Vector2){1.0f, 0.0f}, SIM_HZ);

    float startX = player.pos.x;
    Input released = {0};
    int ticks = 0;
    while (Player_Speed(&player) > 0.001f && ticks <= SIM_HZ) {
        Player_Update(&player, &released, SIM_DT, ROOM_AREA_JUNGLE_ENTRANCE);
        ticks++;
    }
    float stopTime = (float)ticks * SIM_DT;
    float drift = player.pos.x - startX;
    printf("Feel Gate stopping: %.4f s, %.3f px drift\n", stopTime, drift);
    CHECK(ticks <= (int)ceilf(0.08f * SIM_HZ));
    CHECK(drift <= WALK_SPEED * DECEL_TIME * 0.5f + 0.05f);
}

static void TestRampTransitionsAndSideBlocking(void)
{
    Player player;
    Player_Init(&player, (Vector2){464.0f, 224.0f});
    Advance(&player, (Vector2){0.0f, -1.0f}, SIM_HZ);
    CHECK(player.elevation == ROOM_ELEVATION_UPPER);
    CHECK(player.pos.y < 184.0f);
    CHECK(!Room_CollidesAtElevation(ROOM_AREA_JUNGLE_ENTRANCE,
                                    Player_FeetCollider(&player), player.elevation));

    Advance(&player, (Vector2){0.0f, 1.0f}, SIM_HZ);
    CHECK(player.elevation == ROOM_ELEVATION_GROUND);
    CHECK(player.pos.y > 208.0f);

    Player_Init(&player, (Vector2){440.0f, 184.0f});
    player.elevation = ROOM_ELEVATION_UPPER;
    Advance(&player, (Vector2){-1.0f, 0.0f}, SIM_HZ / 2);
    CHECK(player.elevation == ROOM_ELEVATION_UPPER);
    CHECK(!Room_CollidesAtElevation(ROOM_AREA_JUNGLE_ENTRANCE,
                                    Player_FeetCollider(&player), player.elevation));
}

static void TestRootPathGateHasAReconnectingSideRoute(void)
{
    const RoomProp *props = Room_Props(ROOM_AREA_ROOT_PATH);
    bool foundGate = false;
    for (int i = 0; i < ROOM_PROP_COUNT; i++) {
        if (props[i].sprite == ROOM_SPRITE_THORN_GATE) {
            foundGate = true;
            CHECK(Room_CollidesInArea(ROOM_AREA_ROOT_PATH, props[i].collider));
        }
    }
    CHECK(foundGate);
    CHECK(Room_CollidesInArea(ROOM_AREA_ROOT_PATH, FeetAt(320.0f, 104.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_ROOT_PATH, FeetAt(216.0f, 112.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_ROOT_PATH, FeetAt(424.0f, 112.0f)));

    Player player;
    Player_Init(&player, Room_SpawnPoint(ROOM_AREA_ROOT_PATH));
    AdvanceInArea(&player, (Vector2){0.0f, -1.0f}, SIM_HZ * 2,
                  ROOM_AREA_ROOT_PATH);
    AdvanceInArea(&player, (Vector2){-1.0f, 0.0f}, SIM_HZ * 3 / 2,
                  ROOM_AREA_ROOT_PATH);
    AdvanceInArea(&player, (Vector2){0.0f, -1.0f}, SIM_HZ * 3,
                  ROOM_AREA_ROOT_PATH);
    AdvanceInArea(&player, (Vector2){1.0f, 0.0f}, SIM_HZ,
                  ROOM_AREA_ROOT_PATH);
    AdvanceInArea(&player, (Vector2){0.0f, -1.0f}, SIM_HZ,
                  ROOM_AREA_ROOT_PATH);
    CHECK(player.pos.y < 64.0f);
    CHECK(player.pos.x > 280.0f && player.pos.x < 360.0f);
    CHECK(!Room_CollidesAtElevation(ROOM_AREA_ROOT_PATH,
                                    Player_FeetCollider(&player), player.elevation));
}

static void TestRootPathTransitionPairsAndRamp(void)
{
    Room room = {0};
    Room_SetArea(&room, ROOM_AREA_JUNGLE_ENTRANCE);
    RoomArea destination;
    Vector2 spawn;
    CHECK(Room_GetTransition(&room, (Vector2){320.0f, 55.0f},
                             &destination, &spawn));
    CHECK(destination == ROOM_AREA_ROOT_PATH);
    CHECK(spawn.x == 320.0f && spawn.y == 336.0f);
    CHECK(!Room_GetTransition(&room, (Vector2){200.0f, 55.0f},
                              &destination, &spawn));

    Room_SetArea(&room, ROOM_AREA_ROOT_PATH);
    CHECK(Room_GetTransition(&room, (Vector2){320.0f, 353.0f},
                             &destination, &spawn));
    CHECK(destination == ROOM_AREA_JUNGLE_ENTRANCE);
    CHECK(spawn.x == 320.0f && spawn.y == 336.0f);

    Player player;
    Player_Init(&player, (Vector2){144.0f, 224.0f});
    AdvanceInArea(&player, (Vector2){0.0f, -1.0f}, SIM_HZ,
                  ROOM_AREA_ROOT_PATH);
    CHECK(player.elevation == ROOM_ELEVATION_UPPER);
    CHECK(!Room_CollidesAtElevation(ROOM_AREA_ROOT_PATH,
                                    Player_FeetCollider(&player), player.elevation));
}

static void SimulateFixedStepFrame(float frameTime, float *accumulator, int *stepsTaken)
{
    float clamped = frameTime;
    if (clamped > MAX_FRAME_TIME) clamped = MAX_FRAME_TIME;
    *accumulator += clamped;

    int steps = 0;
    while (*accumulator >= SIM_DT && steps < MAX_SIM_STEPS) {
        *accumulator -= SIM_DT;
        steps++;
    }
    if (steps == MAX_SIM_STEPS) *accumulator = 0.0f;
    *stepsTaken = steps;
}

static void TestFrameTimeStallClampsAndRecovers(void)
{
    float accumulator = 0.0f;
    int steps = 0;
    SimulateFixedStepFrame(0.2f, &accumulator, &steps);
    CHECK(steps == MAX_SIM_STEPS);
    CHECK(accumulator == 0.0f);

    SimulateFixedStepFrame(SIM_DT, &accumulator, &steps);
    CHECK(steps == 1);
    CHECK(accumulator < SIM_DT);
}

int main(void)
{
    TestEntranceBoundsAndRouteLayout();
    TestPlayerCanLoopAroundRootCluster();
    TestTreeBasesBlockAndCanopiesStayWalkable();
    TestWallSlideAndDiagonalSpeed();
    TestStoppingMeetsFeelGate();
    TestRampTransitionsAndSideBlocking();
    TestRootPathGateHasAReconnectingSideRoute();
    TestRootPathTransitionPairsAndRamp();
    TestFrameTimeStallClampsAndRecovers();

    if (failures != 0) {
        fprintf(stderr, "%d movement collision check(s) failed.\n", failures);
        return 1;
    }

    puts("All movement collision checks passed.");
    return 0;
}
