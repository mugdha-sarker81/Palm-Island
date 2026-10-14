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

static Vector2 CorePoint(float x, float y)
{
    return (Vector2){
        x + ROOM_CORE_OFFSET_X * TILE_SIZE,
        y + ROOM_CORE_OFFSET_Y * TILE_SIZE
    };
}

static float CoreX(float x)
{
    return x - ROOM_CORE_OFFSET_X * TILE_SIZE;
}

static float CoreY(float y)
{
    return y - ROOM_CORE_OFFSET_Y * TILE_SIZE;
}

static Rectangle FeetAtWorld(float x, float y)
{
    return (Rectangle){
        x - PLAYER_COLLIDER_W * 0.5f,
        y - PLAYER_COLLIDER_H * 0.5f,
        PLAYER_COLLIDER_W,
        PLAYER_COLLIDER_H
    };
}

static Rectangle FeetAt(float x, float y)
{
    Vector2 world = CorePoint(x, y);
    return FeetAtWorld(world.x, world.y);
}

static void Advance(Player *player, Vector2 direction, int ticks)
{
    Input input = {0};
    input.move = direction;
    for (int i = 0; i < ticks; i++) Player_Update(player, &input, SIM_DT);
}

static void TestRoomCollisionMap(void)
{
    CHECK(Room_Collides(FeetAt(120.0f, 24.0f)));  /* shrine stone */
    CHECK(Room_Collides(FeetAt(120.0f, 40.0f)));  /* cliff face */
    CHECK(Room_Collides(FeetAt(264.0f, 168.0f))); /* water */
    CHECK(!Room_Collides(FeetAt(160.0f, 40.0f))); /* original 32 px stair opening */
    CHECK(Room_Collides(FeetAt(147.0f, 40.0f)));  /* just left of the opening */
    CHECK(!Room_Collides(FeetAt(149.0f, 40.0f))); /* collider fits at its left edge */
    CHECK(!Room_Collides(FeetAt(171.0f, 40.0f)));
    CHECK(Room_Collides(FeetAt(173.0f, 40.0f)));  /* just right of the opening */
    CHECK(!Room_Collides(FeetAt(152.0f, 88.0f))); /* dirt path remains open */
    CHECK(Room_Collides(FeetAtWorld(4.9f, Room_SpawnPoint().y))); /* room boundary */
    CHECK(!Room_Collides(FeetAtWorld(5.0f, Room_SpawnPoint().y)));
    Rectangle bounds = Room_Bounds();
    CHECK(bounds.width > VIRTUAL_W);
    CHECK(bounds.height > VIRTUAL_H);
    CHECK(Room_CollidesAtElevation(FeetAt(160.0f, 24.0f), ROOM_ELEVATION_GROUND));
    CHECK(!Room_CollidesAtElevation(FeetAt(160.0f, 24.0f), ROOM_ELEVATION_UPPER));
    CHECK(Room_CollidesAtElevation(FeetAt(120.0f, 24.0f), ROOM_ELEVATION_UPPER));
    CHECK(Room_CollidesAtElevation(FeetAt(144.0f, 24.0f), ROOM_ELEVATION_UPPER));
}

static void TestPalmTrunkAndPillarBaseAreSolid(void)
{
    CHECK(Room_Collides(FeetAt(90.0f, 108.0f))); /* palm trunk */
    CHECK(Room_Collides(FeetAt(232.0f, 76.0f))); /* pillar base */
    CHECK(!Room_CollidesAtElevation(FeetAt(90.0f, 108.0f), ROOM_ELEVATION_UPPER));
    CHECK(!Room_Collides(FeetAt(80.0f, 108.0f))); /* beside palm */
    CHECK(!Room_Collides(FeetAt(232.0f, 68.0f))); /* above pillar base */

    Player palm;
    Player_Init(&palm, CorePoint(90.0f, 128.0f));
    Advance(&palm, (Vector2){0.0f, -1.0f}, SIM_HZ);
    CHECK(CoreY(palm.pos.y) >= 114.0f);
    CHECK(fabsf(palm.vel.y) < 0.001f);
    CHECK(!Room_Collides(Player_FeetCollider(&palm)));

    Player pillar;
    Player_Init(&pillar, CorePoint(232.0f, 96.0f));
    Advance(&pillar, (Vector2){0.0f, -1.0f}, SIM_HZ);
    CHECK(CoreY(pillar.pos.y) >= 83.0f);
    CHECK(fabsf(pillar.vel.y) < 0.001f);
    CHECK(!Room_Collides(Player_FeetCollider(&pillar)));
}

static void TestPlayerCanWalkBehindPalmCanopy(void)
{
    Player player;
    Player_Init(&player, CorePoint(60.0f, 85.0f));
    Advance(&player, (Vector2){1.0f, 0.0f}, SIM_HZ);

    CHECK(CoreX(player.pos.x) > 100.0f);
    CHECK(fabsf(CoreY(player.pos.y) - 85.0f) < 0.01f);
    CHECK(!Room_Collides(Player_FeetCollider(&player)));
}

static void TestShrineRampLeadsToUpperWall(void)
{
    Player player;
    Player_Init(&player, CorePoint(160.0f, 60.0f));
    Advance(&player, (Vector2){0.0f, -1.0f}, SIM_HZ);

    CHECK(player.elevation == ROOM_ELEVATION_UPPER);
    CHECK(CoreY(player.pos.y) >= 19.0f);
    CHECK(!Room_CollidesAtElevation(Player_FeetCollider(&player), player.elevation));
    CHECK(fabsf(player.vel.y) < 0.001f);
}

static void TestCliffAndWaterBlockPlayer(void)
{
    Player cliff;
    Player_Init(&cliff, CorePoint(120.0f, 56.0f));
    Advance(&cliff, (Vector2){0.0f, -1.0f}, SIM_HZ);
    CHECK(CoreY(cliff.pos.y) >= 51.0f);
    CHECK(!Room_Collides(Player_FeetCollider(&cliff)));

    Player water;
    Player_Init(&water, CorePoint(248.0f, 168.0f));
    Advance(&water, (Vector2){1.0f, 0.0f}, SIM_HZ);
    CHECK(CoreX(water.pos.x) <= 251.0f);
    CHECK(!Room_Collides(Player_FeetCollider(&water)));
}

static void TestWallSlide(void)
{
    Player player;
    Player_Init(&player, CorePoint(107.0f, 24.0f));
    Vector2 diagonal = {0.70710678f, 0.70710678f};

    Advance(&player, diagonal, 10);
    CHECK(CoreY(player.pos.y) > 24.0f);
    CHECK(CoreX(player.pos.x) <= 107.01f);
    CHECK(player.vel.y >= WALK_SPEED * 0.70f);
    CHECK(!Room_Collides(Player_FeetCollider(&player)));
}

static void TestCornerAssistClearsCliffEdge(void)
{
    Player player;
    Player_Init(&player, CorePoint(106.99f, 48.01f));
    CHECK(!Room_Collides(Player_FeetCollider(&player)));
    Advance(&player, (Vector2){0.70710678f, 0.70710678f}, 1);

    CHECK(CoreY(player.pos.y) >= 51.0f);
    CHECK(CoreY(player.pos.y) - 48.01f <= (float)CORNER_ASSIST_MAX);
    CHECK(CoreX(player.pos.x) > 106.99f);
    CHECK(!Room_Collides(Player_FeetCollider(&player)));
}

static void TestStairRampTransitionsBothWays(void)
{
    Player player;
    Player_Init(&player, CorePoint(147.0f, 56.0f));
    Advance(&player, (Vector2){0.0f, -1.0f}, SIM_HZ);

    CHECK(player.elevation == ROOM_ELEVATION_UPPER);
    CHECK(CoreY(player.pos.y) < 32.0f);
    CHECK(CoreY(player.pos.y) >= 19.0f);
    CHECK(CoreX(player.pos.x) >= 149.0f && CoreX(player.pos.x) <= 171.0f);
    CHECK(!Room_CollidesAtElevation(Player_FeetCollider(&player), player.elevation));

    Player_Init(&player, CorePoint(173.0f, 56.0f));
    Advance(&player, (Vector2){0.0f, -1.0f}, SIM_HZ);

    CHECK(player.elevation == ROOM_ELEVATION_UPPER);
    CHECK(CoreY(player.pos.y) < 32.0f);
    CHECK(CoreX(player.pos.x) >= 149.0f && CoreX(player.pos.x) <= 171.0f);
    CHECK(!Room_CollidesAtElevation(Player_FeetCollider(&player), player.elevation));

    Advance(&player, (Vector2){0.0f, 1.0f}, SIM_HZ);
    CHECK(player.elevation == ROOM_ELEVATION_GROUND);
    CHECK(CoreY(player.pos.y) > 48.0f);
    CHECK(!Room_CollidesAtElevation(Player_FeetCollider(&player), player.elevation));
}

static void TestRampCannotBeBypassedAtItsSides(void)
{
    Player player;
    Player_Init(&player, CorePoint(160.0f, 40.0f));
    Advance(&player, (Vector2){-1.0f, 0.0f}, 30);

    CHECK(player.elevation == ROOM_ELEVATION_GROUND);
    CHECK(CoreX(player.pos.x) >= 149.0f);
    CHECK(!Room_CollidesAtElevation(Player_FeetCollider(&player), player.elevation));

    Player_Init(&player, CorePoint(160.0f, 24.0f));
    player.elevation = ROOM_ELEVATION_UPPER;
    Advance(&player, (Vector2){1.0f, 0.0f}, 30);

    CHECK(player.elevation == ROOM_ELEVATION_UPPER);
    CHECK(CoreX(player.pos.x) <= 171.0f);
    CHECK(!Room_CollidesAtElevation(Player_FeetCollider(&player), player.elevation));
}

static void TestRoomEdgeBlocksPlayer(void)
{
    Player player;
    Player_Init(&player, (Vector2){8.0f, Room_SpawnPoint().y});
    Advance(&player, (Vector2){-1.0f, 0.0f}, SIM_HZ);

    CHECK(player.pos.x >= PLAYER_COLLIDER_W * 0.5f);
    CHECK(!Room_Collides(Player_FeetCollider(&player)));
}

static void TestPlayerCanExploreTheExtendedRoom(void)
{
    Player player;
    Player_Init(&player, Room_SpawnPoint());
    Advance(&player, (Vector2){-1.0f, 0.0f}, SIM_HZ * 4);
    CHECK(player.pos.x < ROOM_CORE_OFFSET_X * TILE_SIZE);
    CHECK(!Room_Collides(Player_FeetCollider(&player)));

    Player_Init(&player, Room_SpawnPoint());
    Advance(&player, (Vector2){1.0f, 0.0f}, SIM_HZ * 4);
    CHECK(player.pos.x > (ROOM_CORE_OFFSET_X + ROOM_CORE_COLS) * TILE_SIZE);
    CHECK(!Room_Collides(Player_FeetCollider(&player)));

    Player_Init(&player, CorePoint(96.0f, 80.0f));
    Advance(&player, (Vector2){0.0f, -1.0f}, SIM_HZ * 4);
    CHECK(player.pos.y < ROOM_CORE_OFFSET_Y * TILE_SIZE);
    CHECK(!Room_Collides(Player_FeetCollider(&player)));

    Player_Init(&player, Room_SpawnPoint());
    Advance(&player, (Vector2){0.0f, 1.0f}, SIM_HZ * 4);
    CHECK(player.pos.y > (ROOM_CORE_OFFSET_Y + ROOM_CORE_ROWS) * TILE_SIZE);
    CHECK(!Room_Collides(Player_FeetCollider(&player)));
}

int main(void)
{
    TestRoomCollisionMap();
    TestPalmTrunkAndPillarBaseAreSolid();
    TestPlayerCanWalkBehindPalmCanopy();
    TestShrineRampLeadsToUpperWall();
    TestCliffAndWaterBlockPlayer();
    TestWallSlide();
    TestCornerAssistClearsCliffEdge();
    TestStairRampTransitionsBothWays();
    TestRampCannotBeBypassedAtItsSides();
    TestRoomEdgeBlocksPlayer();
    TestPlayerCanExploreTheExtendedRoom();

    if (failures != 0) {
        fprintf(stderr, "%d movement collision check(s) failed.\n", failures);
        return 1;
    }

    puts("All movement collision checks passed.");
    return 0;
}
