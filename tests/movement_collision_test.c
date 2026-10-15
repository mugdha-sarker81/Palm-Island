#include "player.h"
#include "room.h"
#include "interaction.h"
#include "tuning.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

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

static void TestAreaDefinitionsHaveSafeSpawnsAndProps(void)
{
    for (RoomArea area = ROOM_AREA_JUNGLE_ENTRANCE;
         area < ROOM_AREA_COUNT;
         area = (RoomArea)(area + 1)) {
        Vector2 spawn = Room_SpawnPoint(area);
        CHECK(spawn.x == 320.0f && spawn.y == 336.0f);
        CHECK(Room_Props(area) != NULL);
        CHECK(!Room_CollidesInArea(area, FeetAt(spawn.x, spawn.y)));
    }
    CHECK(Room_Props((RoomArea)-1) ==
          Room_Props(ROOM_AREA_JUNGLE_ENTRANCE));
    Vector2 fallback = Room_SpawnPoint((RoomArea)-1);
    CHECK(fallback.x == 320.0f && fallback.y == 336.0f);
}

static void TestAreaDefinitionsHaveFruitMetadata(void)
{
    CHECK(Room_FoodNodeForArea(ROOM_AREA_JUNGLE_ENTRANCE) ==
          ROOM_FOOD_NODE_NONE);
    CHECK(Room_FoodNodeForArea(ROOM_AREA_ROOT_PATH) ==
          ROOM_FOOD_NODE_NONE);
    CHECK(Room_FoodNodeForArea(ROOM_AREA_OVERGROWN_CLEARING) ==
          ROOM_FOOD_NODE_CLEARING);
    CHECK(Room_FoodNodeForArea(ROOM_AREA_SHRINE_APPROACH) ==
          ROOM_FOOD_NODE_SHRINE_APPROACH);
    CHECK(Room_FoodNodeForArea(ROOM_AREA_THORNBACK_ARENA) ==
          ROOM_FOOD_NODE_NONE);

    Vector2 clearingFruit =
        Room_PalmFruitPosition(ROOM_AREA_OVERGROWN_CLEARING);
    CHECK(clearingFruit.x == 104.0f && clearingFruit.y == 184.0f);
    Vector2 shrineFruit = Room_PalmFruitPosition(ROOM_AREA_SHRINE_APPROACH);
    CHECK(shrineFruit.x == 152.0f && shrineFruit.y == 184.0f);
    Vector2 fallbackFruit = Room_PalmFruitPosition((RoomArea)-1);
    CHECK(fallbackFruit.x == 104.0f && fallbackFruit.y == 184.0f);
    CHECK(Room_FoodNodeForArea((RoomArea)-1) == ROOM_FOOD_NODE_NONE);
}

static void TestTemplateRoomUsesSharedMovementAndTransitions(void)
{
    Room room = {0};
    RoomArea destination;
    Vector2 spawn;

    Room_SetArea(&room, ROOM_AREA_THORNBACK_ARENA);
    CHECK(Room_GetTransition(&room, (Vector2){320.0f, 16.0f},
                             &destination, &spawn));
    CHECK(destination == ROOM_AREA_TEMPLATE_TEST);
    CHECK(spawn.x == 320.0f && spawn.y == 336.0f);
    CHECK(!Room_CollidesInArea(destination, FeetAt(spawn.x, spawn.y)));

    Player player;
    Player_Init(&player, spawn);
    AdvanceInArea(&player, (Vector2){0.0f, -1.0f}, SIM_HZ * 5,
                  ROOM_AREA_TEMPLATE_TEST);
    CHECK(player.pos.y <= 16.0f);
    CHECK(!Room_CollidesInArea(ROOM_AREA_TEMPLATE_TEST,
                               Player_FeetCollider(&player)));
    AdvanceInArea(&player, (Vector2){0.0f, 1.0f}, SIM_HZ * 5,
                  ROOM_AREA_TEMPLATE_TEST);
    Room_SetArea(&room, ROOM_AREA_TEMPLATE_TEST);
    CHECK(Room_GetTransition(&room, player.pos, &destination, &spawn));
    CHECK(destination == ROOM_AREA_THORNBACK_ARENA);
    CHECK(spawn.x == 320.0f && spawn.y == 32.0f);
    CHECK(!Room_CollidesInArea(destination, FeetAt(spawn.x, spawn.y)));
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

static void TestPlayerDamageHasInvulnerabilityAndKnockback(void)
{
    Player player;
    Player_Init(&player, (Vector2){320.0f, 184.0f});
    CHECK(player.health == PLAYER_START_HEALTH);
    CHECK(Player_TakeDamage(&player, (Vector2){312.0f, 184.0f},
                            ROOM_AREA_THORNBACK_ARENA));
    CHECK(player.health == PLAYER_START_HEALTH - 1);
    CHECK(fabsf(player.pos.x - (320.0f + PLAYER_HIT_KNOCKBACK)) < 0.01f);
    CHECK(player.damageInvulnerability == PLAYER_HIT_INVULNERABILITY);
    CHECK(!Player_TakeDamage(&player, (Vector2){312.0f, 184.0f},
                             ROOM_AREA_THORNBACK_ARENA));
    CHECK(player.health == PLAYER_START_HEALTH - 1);

    Input released = {0};
    for (int i = 0; i < SIM_HZ; i++) {
        Player_Update(&player, &released, SIM_DT, ROOM_AREA_THORNBACK_ARENA);
    }
    CHECK(player.damageInvulnerability == 0.0f);
    CHECK(Player_TakeDamage(&player, (Vector2){312.0f, 184.0f},
                            ROOM_AREA_THORNBACK_ARENA));
    CHECK(player.health == PLAYER_START_HEALTH - 2);

    while (player.health > 0) {
        for (int i = 0; i < SIM_HZ; i++) {
            Player_Update(&player, &released, SIM_DT,
                          ROOM_AREA_THORNBACK_ARENA);
        }
        CHECK(Player_TakeDamage(&player, (Vector2){312.0f, 184.0f},
                                ROOM_AREA_THORNBACK_ARENA));
    }
    CHECK(player.health == 0);
    CHECK(!Player_TakeDamage(&player, (Vector2){312.0f, 184.0f},
                             ROOM_AREA_THORNBACK_ARENA));

    Player_Init(&player, (Vector2){320.0f, 184.0f});
    CHECK(player.health == PLAYER_START_HEALTH);
    CHECK(player.damageInvulnerability == 0.0f);
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

static void TestThornfangOpensRootPathShortcut(void)
{
    Vector2 gateApproach = {320.0f, 76.0f};
    Vector2 facingGate = {0.0f, 1.0f};
    bool gateOpen = false;

    CHECK(Room_IsFacingThornGate(ROOM_AREA_ROOT_PATH, gateApproach,
                                 facingGate, gateOpen));
    CHECK(!Room_CanCutThornGate(ROOM_AREA_ROOT_PATH, gateApproach,
                                facingGate, false, gateOpen));
    CHECK(!Room_CanCutThornGate(ROOM_AREA_ROOT_PATH, gateApproach,
                                (Vector2){0.0f, -1.0f}, true, gateOpen));
    CHECK(!Room_CanCutThornGate(ROOM_AREA_ROOT_PATH,
                                (Vector2){320.0f, 64.0f}, facingGate,
                                true, gateOpen));
    CHECK(Room_CollidesAtElevationWithGate(
        ROOM_AREA_ROOT_PATH, FeetAt(320.0f, 112.0f),
        ROOM_ELEVATION_GROUND, false));
    CHECK(!Room_CollidesAtElevationWithGate(
        ROOM_AREA_ROOT_PATH, FeetAt(320.0f, 112.0f),
        ROOM_ELEVATION_GROUND, true));

    CHECK(!Room_TryCutThornGate(ROOM_AREA_ROOT_PATH, gateApproach,
                                facingGate, false, &gateOpen));
    CHECK(!gateOpen);
    CHECK(Room_TryCutThornGate(ROOM_AREA_ROOT_PATH, gateApproach,
                               facingGate, true, &gateOpen));
    CHECK(gateOpen);
    CHECK(!Room_IsFacingThornGate(ROOM_AREA_ROOT_PATH, gateApproach,
                                  facingGate, gateOpen));
    CHECK(!Room_TryCutThornGate(ROOM_AREA_ROOT_PATH, gateApproach,
                                facingGate, true, &gateOpen));

    Player player;
    Player_Init(&player, gateApproach);
    player.thornfangOwned = true;
    player.thornGateOpen = gateOpen;
    AdvanceInArea(&player, facingGate, SIM_HZ * 2,
                  ROOM_AREA_ROOT_PATH);
    CHECK(player.pos.y > 144.0f);
    CHECK(!Room_CollidesAtElevationWithGate(
        ROOM_AREA_ROOT_PATH, Player_FeetCollider(&player),
        player.elevation, gateOpen));
}

static void TestRootPathTransitionPairsAndRamp(void)
{
    Room room = {0};
    Room_SetArea(&room, ROOM_AREA_JUNGLE_ENTRANCE);
    RoomArea destination;
    Vector2 spawn;
    CHECK(Room_GetTransition(&room, (Vector2){288.0f, 56.0f},
                             &destination, &spawn));
    CHECK(destination == ROOM_AREA_ROOT_PATH);
    CHECK(spawn.x == 320.0f && spawn.y == 336.0f);
    CHECK(Room_GetTransition(&room, (Vector2){352.0f, 56.0f},
                             &destination, &spawn));
    CHECK(destination == ROOM_AREA_ROOT_PATH);
    CHECK(!Room_GetTransition(&room, (Vector2){200.0f, 56.0f},
                              &destination, &spawn));
    Room_SetArea(&room, (RoomArea)-1);
    CHECK(!Room_GetTransition(&room, (Vector2){320.0f, 56.0f},
                              &destination, &spawn));

    Room_SetArea(&room, ROOM_AREA_ROOT_PATH);
    CHECK(Room_GetTransition(&room, (Vector2){320.0f, 352.0f},
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

static void TestOvergrownClearingRoutesAndTransition(void)
{
    Rectangle bounds = Room_Bounds();
    CHECK(bounds.width == 640.0f && bounds.height == 368.0f);
    CHECK(!Room_CollidesInArea(ROOM_AREA_OVERGROWN_CLEARING,
                               FeetAt(320.0f, 336.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_OVERGROWN_CLEARING,
                               FeetAt(144.0f, 192.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_OVERGROWN_CLEARING,
                               FeetAt(496.0f, 192.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_OVERGROWN_CLEARING,
                               FeetAt(320.0f, 192.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_OVERGROWN_CLEARING,
                               FeetAt(96.0f, 208.0f)));
    CHECK(Room_CollidesInArea(ROOM_AREA_OVERGROWN_CLEARING,
                              FeetAt(240.0f, 176.0f)));
    CHECK(Room_CollidesInArea(ROOM_AREA_OVERGROWN_CLEARING,
                              FeetAt(4.9f, 192.0f)));

    const RoomProp *props = Room_Props(ROOM_AREA_OVERGROWN_CLEARING);
    bool foundMarker = false;
    bool foundForegroundGrass = false;
    for (int i = 0; i < ROOM_PROP_COUNT; i++) {
        if (props[i].sprite == ROOM_SPRITE_MARKER) foundMarker = true;
        if (props[i].sprite == ROOM_SPRITE_TALL_GRASS &&
            props[i].foreground) foundForegroundGrass = true;
    }
    CHECK(foundMarker);
    CHECK(foundForegroundGrass);

    Player exposedRoute;
    Player_Init(&exposedRoute, Room_SpawnPoint(ROOM_AREA_OVERGROWN_CLEARING));
    AdvanceInArea(&exposedRoute, (Vector2){0.0f, -1.0f}, SIM_HZ * 4,
                  ROOM_AREA_OVERGROWN_CLEARING);
    CHECK(exposedRoute.pos.y < 64.0f);
    CHECK(!Room_CollidesAtElevation(ROOM_AREA_OVERGROWN_CLEARING,
                                    Player_FeetCollider(&exposedRoute),
                                    exposedRoute.elevation));

    Player outerRoute;
    Player_Init(&outerRoute, Room_SpawnPoint(ROOM_AREA_OVERGROWN_CLEARING));
    AdvanceInArea(&outerRoute, (Vector2){-1.0f, 0.0f}, SIM_HZ * 2,
                  ROOM_AREA_OVERGROWN_CLEARING);
    CHECK(outerRoute.pos.x > 120.0f && outerRoute.pos.x < 240.0f);
    AdvanceInArea(&outerRoute, (Vector2){0.0f, -1.0f}, SIM_HZ * 7 / 2,
                  ROOM_AREA_OVERGROWN_CLEARING);
    CHECK(outerRoute.pos.y < 96.0f);
    CHECK(!Room_CollidesAtElevation(ROOM_AREA_OVERGROWN_CLEARING,
                                    Player_FeetCollider(&outerRoute),
                                    outerRoute.elevation));
    AdvanceInArea(&outerRoute, (Vector2){1.0f, 0.0f}, SIM_HZ * 2,
                  ROOM_AREA_OVERGROWN_CLEARING);
    CHECK(outerRoute.pos.x > 280.0f);
    CHECK(!Room_CollidesAtElevation(ROOM_AREA_OVERGROWN_CLEARING,
                                    Player_FeetCollider(&outerRoute),
                                    outerRoute.elevation));

    Player nookRoute;
    Player_Init(&nookRoute, Room_SpawnPoint(ROOM_AREA_OVERGROWN_CLEARING));
    AdvanceInArea(&nookRoute, (Vector2){-1.0f, 0.0f}, SIM_HZ * 2,
                  ROOM_AREA_OVERGROWN_CLEARING);
    AdvanceInArea(&nookRoute, (Vector2){0.0f, -1.0f}, SIM_HZ * 17 / 10,
                  ROOM_AREA_OVERGROWN_CLEARING);
    AdvanceInArea(&nookRoute, (Vector2){-1.0f, 0.0f}, SIM_HZ,
                  ROOM_AREA_OVERGROWN_CLEARING);
    CHECK(nookRoute.pos.x > 55.0f && nookRoute.pos.x < 128.0f);
    CHECK(nookRoute.pos.y > 192.0f && nookRoute.pos.y < 240.0f);
    CHECK(!Room_CollidesAtElevation(ROOM_AREA_OVERGROWN_CLEARING,
                                    Player_FeetCollider(&nookRoute),
                                    nookRoute.elevation));

    Room room = {0};
    Room_SetArea(&room, ROOM_AREA_ROOT_PATH);
    RoomArea destination;
    Vector2 spawn;
    CHECK(Room_GetTransition(&room, (Vector2){320.0f, 16.0f},
                             &destination, &spawn));
    CHECK(destination == ROOM_AREA_OVERGROWN_CLEARING);
    CHECK(spawn.x == 320.0f && spawn.y == 336.0f);
    CHECK(!Room_CollidesInArea(ROOM_AREA_OVERGROWN_CLEARING,
                               FeetAt(spawn.x, spawn.y)));
    CHECK(!Room_GetTransition(&room, (Vector2){240.0f, 16.0f},
                              &destination, &spawn));

    Room_SetArea(&room, ROOM_AREA_OVERGROWN_CLEARING);
    CHECK(Room_GetTransition(&room, (Vector2){320.0f, 352.0f},
                             &destination, &spawn));
    CHECK(destination == ROOM_AREA_ROOT_PATH);
    CHECK(spawn.x == 320.0f && spawn.y == 32.0f);
    CHECK(!Room_CollidesInArea(ROOM_AREA_ROOT_PATH,
                               FeetAt(spawn.x, spawn.y)));
    CHECK(!Room_GetTransition(&room, (Vector2){240.0f, 352.0f},
                              &destination, &spawn));
}

static void TestShrineApproachLayoutAndConnections(void)
{
    CHECK(!Room_CollidesInArea(ROOM_AREA_SHRINE_APPROACH,
                               FeetAt(320.0f, 336.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_SHRINE_APPROACH,
                               FeetAt(320.0f, 132.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_SHRINE_APPROACH,
                               FeetAt(176.0f, 192.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_SHRINE_APPROACH,
                               FeetAt(464.0f, 192.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_SHRINE_APPROACH,
                               FeetAt(320.0f, 48.0f)));
    CHECK(Room_CollidesInArea(ROOM_AREA_SHRINE_APPROACH,
                              FeetAt(4.9f, 192.0f)));

    Player approach;
    Player_Init(&approach, Room_SpawnPoint(ROOM_AREA_SHRINE_APPROACH));
    AdvanceInArea(&approach, (Vector2){0.0f, -1.0f}, SIM_HZ * 5,
                  ROOM_AREA_SHRINE_APPROACH);
    CHECK(approach.pos.y >= 3.0f && approach.pos.y < 20.0f);
    CHECK(!Room_CollidesInArea(ROOM_AREA_SHRINE_APPROACH,
                               Player_FeetCollider(&approach)));

    Room room = {0};
    Room_SetArea(&room, ROOM_AREA_OVERGROWN_CLEARING);
    RoomArea destination;
    Vector2 spawn;
    CHECK(Room_GetTransition(&room, (Vector2){320.0f, 16.0f},
                             &destination, &spawn));
    CHECK(destination == ROOM_AREA_SHRINE_APPROACH);
    CHECK(spawn.x == 320.0f && spawn.y == 336.0f);
    CHECK(!Room_CollidesInArea(ROOM_AREA_SHRINE_APPROACH,
                               FeetAt(spawn.x, spawn.y)));
    CHECK(!Room_GetTransition(&room, (Vector2){240.0f, 16.0f},
                              &destination, &spawn));

    Room_SetArea(&room, ROOM_AREA_SHRINE_APPROACH);
    CHECK(Room_GetTransition(&room, (Vector2){320.0f, 352.0f},
                             &destination, &spawn));
    CHECK(destination == ROOM_AREA_OVERGROWN_CLEARING);
    CHECK(spawn.x == 320.0f && spawn.y == 32.0f);
    CHECK(!Room_CollidesInArea(ROOM_AREA_OVERGROWN_CLEARING,
                               FeetAt(spawn.x, spawn.y)));
    CHECK(!Room_GetTransition(&room, (Vector2){240.0f, 352.0f},
                              &destination, &spawn));
    CHECK(Room_GetTransition(&room, (Vector2){320.0f, 16.0f},
                             &destination, &spawn));
    CHECK(destination == ROOM_AREA_THORNBACK_ARENA);
    CHECK(spawn.x == 320.0f && spawn.y == 336.0f);
    CHECK(!Room_CollidesInArea(ROOM_AREA_THORNBACK_ARENA,
                               FeetAt(spawn.x, spawn.y)));
    CHECK(!Room_GetTransition(&room, (Vector2){240.0f, 16.0f},
                              &destination, &spawn));

    CHECK(!Room_CollidesInArea(ROOM_AREA_THORNBACK_ARENA,
                               FeetAt(320.0f, 184.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_THORNBACK_ARENA,
                               FeetAt(80.0f, 192.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_THORNBACK_ARENA,
                               FeetAt(560.0f, 192.0f)));
    for (float x = 200.0f; x <= 440.0f; x += 8.0f) {
        CHECK(!Room_CollidesInArea(ROOM_AREA_THORNBACK_ARENA,
                                   FeetAt(x, 184.0f)));
    }
    CHECK(Room_CollidesInArea(ROOM_AREA_THORNBACK_ARENA,
                              FeetAt(4.9f, 192.0f)));
    CHECK(!Room_CollidesInArea(ROOM_AREA_THORNBACK_ARENA,
                               FeetAt(5.0f, 192.0f)));

    Room_SetArea(&room, ROOM_AREA_THORNBACK_ARENA);
    CHECK(Room_GetTransition(&room, (Vector2){320.0f, 352.0f},
                             &destination, &spawn));
    CHECK(destination == ROOM_AREA_SHRINE_APPROACH);
    CHECK(spawn.x == 320.0f && spawn.y == 32.0f);
    CHECK(!Room_CollidesInArea(ROOM_AREA_SHRINE_APPROACH,
                               FeetAt(spawn.x, spawn.y)));
    CHECK(!Room_GetTransition(&room, (Vector2){240.0f, 352.0f},
                              &destination, &spawn));

    const RoomInteractable *trial = Room_FindInteractable(
        ROOM_AREA_THORNBACK_ARENA, (Vector2){150.0f, 176.0f},
        (Vector2){1.0f, 0.0f}, INTERACT_PROMPT_RANGE);
    CHECK(trial != NULL);
    CHECK(trial != NULL &&
          trial->kind == ROOM_INTERACTABLE_THORNBACK_TRIAL);
    CHECK(Room_FindInteractable(ROOM_AREA_THORNBACK_ARENA,
                                (Vector2){150.0f, 176.0f},
                                (Vector2){-1.0f, 0.0f},
                                INTERACT_PROMPT_RANGE) == NULL);
    CHECK(Room_FindInteractable(ROOM_AREA_THORNBACK_ARENA,
                                (Vector2){100.0f, 176.0f},
                                (Vector2){1.0f, 0.0f},
                                INTERACT_PROMPT_RANGE) == NULL);
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

static void TestRootMarkingsTargetRangeAndFacing(void)
{
    const RoomInteractable *target = Room_FindInteractable(
        ROOM_AREA_ROOT_PATH, (Vector2){320.0f, 180.0f},
        (Vector2){0.0f, -1.0f}, INTERACT_RANGE);
    CHECK(target != NULL);
    if (target != NULL) {
        CHECK(strcmp(target->id, "jungle.root_markings_seen") == 0);
        CHECK(strcmp(target->title, "Living Thorn Weave") == 0);
        CHECK(target->kind == ROOM_INTERACTABLE_ROOT_WEAVE);
    }

    CHECK(Room_FindInteractable(ROOM_AREA_ROOT_PATH,
                               (Vector2){320.0f, 180.0f},
                               (Vector2){0.0f, -1.0f},
                               INTERACT_RANGE - 0.01f) == NULL);
    CHECK(Room_FindInteractable(ROOM_AREA_ROOT_PATH,
                                (Vector2){320.0f, 188.0f},
                                (Vector2){0.0f, -1.0f},
                                INTERACT_PROMPT_RANGE) != NULL);
    CHECK(Room_FindInteractable(ROOM_AREA_ROOT_PATH,
                                (Vector2){320.0f, 188.01f},
                                (Vector2){0.0f, -1.0f},
                                INTERACT_PROMPT_RANGE) == NULL);
    CHECK(Room_FindInteractable(ROOM_AREA_ROOT_PATH,
                                (Vector2){320.0f, 180.0f},
                                (Vector2){-1.0f, 0.0f},
                                INTERACT_RANGE) == NULL);
    CHECK(Room_FindInteractable(ROOM_AREA_ROOT_PATH,
                                (Vector2){320.0f, 180.0f},
                                (Vector2){0.0f, 1.0f},
                                INTERACT_RANGE) == NULL);
    CHECK(Room_FindInteractable(ROOM_AREA_ROOT_PATH,
                                (Vector2){320.0f, 180.0f},
                                (Vector2){0.559193f, -0.829038f},
                                INTERACT_RANGE) != NULL);
    CHECK(Room_FindInteractable(ROOM_AREA_ROOT_PATH,
                                (Vector2){320.0f, 180.0f},
                                (Vector2){0.587785f, -0.809017f},
                                INTERACT_RANGE) == NULL);
    CHECK(Room_FindInteractable(ROOM_AREA_JUNGLE_ENTRANCE,
                                (Vector2){320.0f, 180.0f},
                                (Vector2){0.0f, -1.0f},
                                INTERACT_RANGE) == NULL);
    CHECK(Room_FindInteractable(ROOM_AREA_ROOT_PATH,
                                (Vector2){320.0f, 180.0f},
                                (Vector2){0.0f, -1.0f}, -1.0f) == NULL);

    target = Room_FindInteractable(ROOM_AREA_ROOT_PATH,
                                   (Vector2){216.0f, 172.0f},
                                   (Vector2){0.0f, -1.0f}, INTERACT_RANGE);
    CHECK(!Room_CollidesInArea(ROOM_AREA_ROOT_PATH, FeetAt(216.0f, 172.0f)));
    CHECK(target != NULL);
    if (target != NULL) {
        CHECK(strcmp(target->id, "jungle.root_marker_activated") == 0);
        CHECK(target->kind == ROOM_INTERACTABLE_ROOT_MARKER);
    }
    CHECK(Room_FindInteractable(ROOM_AREA_ROOT_PATH,
                                (Vector2){216.0f, 172.0f},
                                (Vector2){0.0f, -1.0f},
                                INTERACT_RANGE - 0.01f) == NULL);
    CHECK(Room_FindInteractable(ROOM_AREA_ROOT_PATH,
                                (Vector2){216.0f, 180.0f},
                                (Vector2){0.0f, -1.0f},
                                INTERACT_PROMPT_RANGE) != NULL);
    CHECK(Room_FindInteractable(ROOM_AREA_ROOT_PATH,
                                (Vector2){216.0f, 151.0f},
                                (Vector2){0.0f, 1.0f},
                                INTERACT_PROMPT_RANGE) == NULL);

    target = Room_FindInteractable(ROOM_AREA_OVERGROWN_CLEARING,
                                   (Vector2){84.0f, 184.0f},
                                   (Vector2){-1.0f, 0.0f}, INTERACT_RANGE);
    CHECK(!Room_CollidesInArea(ROOM_AREA_OVERGROWN_CLEARING,
                               FeetAt(84.0f, 184.0f)));
    CHECK(target != NULL);
    if (target != NULL) {
        CHECK(strcmp(target->id, "jungle.clearing_camp_note_seen") == 0);
        CHECK(target->kind == ROOM_INTERACTABLE_CLEARING_NOTE);
    }
    CHECK(Room_FindInteractable(ROOM_AREA_OVERGROWN_CLEARING,
                                (Vector2){92.0f, 184.0f},
                                (Vector2){-1.0f, 0.0f},
                                INTERACT_RANGE - 0.01f) == NULL);
    CHECK(Room_FindInteractable(ROOM_AREA_OVERGROWN_CLEARING,
                                (Vector2){100.0f, 184.0f},
                                (Vector2){-1.0f, 0.0f},
                                INTERACT_PROMPT_RANGE) != NULL);

    const RoomInteractable *fruit = Room_FindInteractableWithFruitState(
        ROOM_AREA_OVERGROWN_CLEARING, (Vector2){76.0f, 184.0f},
        (Vector2){1.0f, 0.0f}, INTERACT_RANGE, ROOM_FOOD_NODE_NONE);
    CHECK(!Room_CollidesInArea(ROOM_AREA_OVERGROWN_CLEARING,
                               FeetAt(76.0f, 184.0f)));
    CHECK(fruit != NULL);
    if (fruit != NULL) {
        CHECK(strcmp(fruit->id, "jungle.palm_fruit_collected") == 0);
        CHECK(fruit->kind == ROOM_INTERACTABLE_PALM_FRUIT);
    }
    CHECK(Room_FindInteractableWithFruitState(
              ROOM_AREA_OVERGROWN_CLEARING, (Vector2){76.0f, 184.0f},
              (Vector2){1.0f, 0.0f}, INTERACT_RANGE,
              ROOM_FOOD_NODE_CLEARING) == NULL);

    fruit = Room_FindInteractableWithFruitState(
        ROOM_AREA_SHRINE_APPROACH, (Vector2){124.0f, 184.0f},
        (Vector2){1.0f, 0.0f}, INTERACT_RANGE, ROOM_FOOD_NODE_NONE);
    CHECK(!Room_CollidesInArea(ROOM_AREA_SHRINE_APPROACH,
                               FeetAt(124.0f, 184.0f)));
    CHECK(fruit != NULL);
    if (fruit != NULL) {
        CHECK(strcmp(fruit->id, "jungle.shrine_palm_fruit_collected") == 0);
        CHECK(fruit->kind == ROOM_INTERACTABLE_PALM_FRUIT);
    }
    CHECK(Room_FindInteractableWithFruitState(
              ROOM_AREA_SHRINE_APPROACH, (Vector2){124.0f, 184.0f},
              (Vector2){1.0f, 0.0f}, INTERACT_RANGE,
              ROOM_FOOD_NODE_SHRINE_APPROACH) == NULL);
}

static void TestRootMarkingsCanBeReviewedAndClosed(void)
{
    InteractionState state = {0};
    CHECK(!Room_CollidesInArea(ROOM_AREA_ROOT_PATH, FeetAt(320.0f, 180.0f)));
    const RoomInteractable *clue = Room_FindInteractable(
        ROOM_AREA_ROOT_PATH, (Vector2){320.0f, 180.0f},
        (Vector2){0.0f, -1.0f}, INTERACT_RANGE);
    CHECK(clue != NULL);
    if (clue == NULL) return;

    InteractionState_Activate(&state, clue);
    CHECK(state.panelOpen);
    CHECK(state.rootWeaveSeen);
    CHECK(!state.rootMarkerActivated);
    CHECK(!state.clearingNoteSeen);

    InteractionState_Activate(&state, clue);
    CHECK(!state.panelOpen);
    CHECK(state.rootWeaveSeen);

    InteractionState_Activate(&state, clue);
    CHECK(state.panelOpen);
    CHECK(state.rootWeaveSeen);
    InteractionState_Close(&state);
    CHECK(!state.panelOpen);
    CHECK(state.rootWeaveSeen);
}

static void TestRootMarkerActivationIsRepeatable(void)
{
    InteractionState state = {0};
    const RoomInteractable *marker = Room_FindInteractable(
        ROOM_AREA_ROOT_PATH, (Vector2){216.0f, 172.0f},
        (Vector2){0.0f, -1.0f}, INTERACT_RANGE);
    CHECK(marker != NULL);
    if (marker == NULL) return;

    InteractionState_Activate(&state, marker);
    CHECK(state.panelOpen);
    CHECK(state.rootMarkerActivated);
    CHECK(!state.rootWeaveSeen);

    InteractionState_Activate(&state, marker);
    CHECK(!state.panelOpen);
    CHECK(state.rootMarkerActivated);
    InteractionState_Activate(&state, marker);
    CHECK(state.panelOpen);
    CHECK(state.rootMarkerActivated);
}

static void TestClearingNoteCanBeReadAndReopened(void)
{
    InteractionState state = {0};
    const RoomInteractable *note = Room_FindInteractable(
        ROOM_AREA_OVERGROWN_CLEARING, (Vector2){84.0f, 184.0f},
        (Vector2){-1.0f, 0.0f}, INTERACT_RANGE);
    CHECK(note != NULL);
    if (note == NULL) return;

    InteractionState_Activate(&state, note);
    CHECK(state.panelOpen);
    CHECK(state.clearingNoteSeen);
    CHECK(!state.rootWeaveSeen);
    CHECK(!state.rootMarkerActivated);

    InteractionState_Activate(&state, note);
    CHECK(!state.panelOpen);
    CHECK(state.clearingNoteSeen);
    InteractionState_Activate(&state, note);
    CHECK(state.panelOpen);
    CHECK(state.clearingNoteSeen);
}

static void TestMapOfRootsMuralIsReachableAndIndependent(void)
{
    InteractionState state = {0};
    const RoomInteractable *mural = Room_FindInteractable(
        ROOM_AREA_SHRINE_APPROACH, (Vector2){320.0f, 132.0f},
        (Vector2){0.0f, -1.0f}, INTERACT_RANGE);
    CHECK(!Room_CollidesInArea(ROOM_AREA_SHRINE_APPROACH,
                               FeetAt(320.0f, 132.0f)));
    CHECK(mural != NULL);
    if (mural == NULL) return;

    CHECK(strcmp(mural->id, "jungle.map_of_roots_seen") == 0);
    CHECK(strcmp(mural->title, "Map of Roots") == 0);
    CHECK(mural->kind == ROOM_INTERACTABLE_ROOTS_MURAL);
    InteractionState_Activate(&state, mural);
    CHECK(state.panelOpen);
    CHECK(state.rootsMuralSeen);
    CHECK(!state.rootWeaveSeen);
    CHECK(!state.rootMarkerActivated);
    CHECK(!state.clearingNoteSeen);
    InteractionState_Activate(&state, mural);
    CHECK(!state.panelOpen);
    CHECK(state.rootsMuralSeen);
    InteractionState_Activate(&state, mural);
    CHECK(state.panelOpen);
    CHECK(state.rootsMuralSeen);
}

int main(void)
{
    TestEntranceBoundsAndRouteLayout();
    TestAreaDefinitionsHaveSafeSpawnsAndProps();
    TestAreaDefinitionsHaveFruitMetadata();
    TestTemplateRoomUsesSharedMovementAndTransitions();
    TestPlayerCanLoopAroundRootCluster();
    TestTreeBasesBlockAndCanopiesStayWalkable();
    TestWallSlideAndDiagonalSpeed();
    TestStoppingMeetsFeelGate();
    TestPlayerDamageHasInvulnerabilityAndKnockback();
    TestRampTransitionsAndSideBlocking();
    TestRootPathGateHasAReconnectingSideRoute();
    TestThornfangOpensRootPathShortcut();
    TestRootPathTransitionPairsAndRamp();
    TestOvergrownClearingRoutesAndTransition();
    TestShrineApproachLayoutAndConnections();
    TestFrameTimeStallClampsAndRecovers();
    TestRootMarkingsTargetRangeAndFacing();
    TestRootMarkingsCanBeReviewedAndClosed();
    TestRootMarkerActivationIsRepeatable();
    TestClearingNoteCanBeReadAndReopened();
    TestMapOfRootsMuralIsReachableAndIndependent();

    if (failures != 0) {
        fprintf(stderr, "%d movement collision check(s) failed.\n", failures);
        return 1;
    }

    puts("All movement collision checks passed.");
    return 0;
}
