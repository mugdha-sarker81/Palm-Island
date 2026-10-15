#include "ash_skitter.h"
#include "tuning.h"
#include <stdio.h>

static int failures;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static void StartSkitter(AshSkitter *skitter)
{
    AshSkitter_Init(skitter);
    AshSkitter_Update(skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      SIM_DT);
}

static void ReachRecovery(AshSkitter *skitter)
{
    AshSkitter_Update(skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      ASH_SKITTER_TELL_TIME);
    CHECK(skitter->state == ASH_SKITTER_STATE_DASH);
    AshSkitter_Update(skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      1.0f);
    CHECK(skitter->state == ASH_SKITTER_STATE_RECOVERY);
}

static void TestSkitterWakesWithVisibleTellOnlyWhenApproached(void)
{
    AshSkitter skitter;
    AshSkitter_Init(&skitter);
    CHECK(skitter.state == ASH_SKITTER_STATE_DORMANT);

    AshSkitter_Update(&skitter, (Vector2){159.0f, ASH_SKITTER_LANE_Y},
                      SIM_DT);
    CHECK(skitter.state == ASH_SKITTER_STATE_DORMANT);
    AshSkitter_Update(&skitter, (Vector2){160.0f, ASH_SKITTER_LANE_Y},
                      SIM_DT);
    CHECK(skitter.state == ASH_SKITTER_STATE_TELL);

    AshSkitter_Init(&skitter);
    AshSkitter_Update(&skitter, (Vector2){320.0f, 340.0f}, SIM_DT);
    CHECK(skitter.state == ASH_SKITTER_STATE_DORMANT);
    AshSkitter_Update(&skitter, (Vector2){320.0f, 280.0f}, SIM_DT);
    CHECK(skitter.state == ASH_SKITTER_STATE_TELL);
    CHECK(skitter.position.x == ASH_SKITTER_PATCH_LEFT_X);
    CHECK(skitter.position.y == ASH_SKITTER_LANE_Y);
    CHECK(skitter.direction == 1);

    AshSkitter_Update(&skitter, (Vector2){320.0f, 280.0f},
                      ASH_SKITTER_TELL_TIME - SIM_DT);
    CHECK(skitter.state == ASH_SKITTER_STATE_TELL);
    AshSkitter_Update(&skitter, (Vector2){320.0f, 280.0f}, SIM_DT);
    CHECK(skitter.state == ASH_SKITTER_STATE_DASH);
}

static void TestSkitterDashesBetweenPatchesAndReverses(void)
{
    AshSkitter skitter;
    StartSkitter(&skitter);
    ReachRecovery(&skitter);
    CHECK(skitter.position.x == ASH_SKITTER_PATCH_RIGHT_X);
    CHECK(!AshSkitter_TryPlayerContact(
        &skitter, (Vector2){ASH_SKITTER_PATCH_RIGHT_X,
                            ASH_SKITTER_LANE_Y}, ROOM_ELEVATION_GROUND));

    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      ASH_SKITTER_RECOVERY_TIME);
    CHECK(skitter.state == ASH_SKITTER_STATE_TELL);
    CHECK(skitter.direction == -1);
    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      ASH_SKITTER_TELL_TIME);
    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      1.0f);
    CHECK(skitter.state == ASH_SKITTER_STATE_RECOVERY);
    CHECK(skitter.position.x == ASH_SKITTER_PATCH_LEFT_X);
}

static void TestDashContactOnlyHitsOncePerPass(void)
{
    AshSkitter skitter;
    StartSkitter(&skitter);
    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      ASH_SKITTER_TELL_TIME);
    CHECK(skitter.state == ASH_SKITTER_STATE_DASH);
    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      0.34f);
    CHECK(skitter.position.x > 310.0f && skitter.position.x < 330.0f);
    CHECK(!AshSkitter_TryPlayerContact(
        &skitter, (Vector2){skitter.position.x,
                            ASH_SKITTER_LANE_Y + 20.0f},
        ROOM_ELEVATION_GROUND));
    CHECK(!AshSkitter_TryPlayerContact(&skitter, skitter.position,
                                       ROOM_ELEVATION_UPPER));
    CHECK(AshSkitter_TryPlayerContact(&skitter, skitter.position,
                                      ROOM_ELEVATION_GROUND));
    CHECK(!AshSkitter_TryPlayerContact(&skitter, skitter.position,
                                       ROOM_ELEVATION_GROUND));

    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      1.0f);
    CHECK(skitter.state == ASH_SKITTER_STATE_RECOVERY);
    CHECK(!AshSkitter_TryPlayerContact(&skitter, skitter.position,
                                       ROOM_ELEVATION_GROUND));
}

static void TestContactCanHitAgainOnNextDash(void)
{
    AshSkitter skitter;
    StartSkitter(&skitter);
    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      ASH_SKITTER_TELL_TIME);
    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      0.34f);
    CHECK(AshSkitter_TryPlayerContact(&skitter, skitter.position,
                                      ROOM_ELEVATION_GROUND));

    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      1.0f);
    CHECK(skitter.state == ASH_SKITTER_STATE_RECOVERY);
    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      ASH_SKITTER_RECOVERY_TIME);
    CHECK(skitter.state == ASH_SKITTER_STATE_TELL);
    CHECK(skitter.direction == -1);
    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      ASH_SKITTER_TELL_TIME);
    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      0.34f);
    CHECK(skitter.state == ASH_SKITTER_STATE_DASH);
    CHECK(AshSkitter_TryPlayerContact(&skitter, skitter.position,
                                      ROOM_ELEVATION_GROUND));
}

static void TestCombatWeaponsCanHitOnceInEachRecovery(void)
{
    AshSkitter skitter;
    StartSkitter(&skitter);
    ReachRecovery(&skitter);

    Vector2 inRange = {skitter.position.x - 16.0f, skitter.position.y};
    CHECK(!AshSkitter_TryHit(&skitter, inRange, (Vector2){1.0f, 0.0f},
                             ROOM_ELEVATION_GROUND,
                             INVENTORY_WEAPON_NONE));
    CHECK(!AshSkitter_TryHit(&skitter, inRange, (Vector2){1.0f, 0.0f},
                             ROOM_ELEVATION_GROUND,
                             INVENTORY_WEAPON_COUNT));
    CHECK(!AshSkitter_TryHit(&skitter, inRange, (Vector2){-1.0f, 0.0f},
                             ROOM_ELEVATION_GROUND,
                             INVENTORY_WEAPON_PALMWOOD_CLUB));
    CHECK(!AshSkitter_TryHit(&skitter, inRange, (Vector2){1.0f, 0.0f},
                             ROOM_ELEVATION_UPPER,
                             INVENTORY_WEAPON_THORNFANG));
    CHECK(!AshSkitter_TryHit(
        &skitter, (Vector2){skitter.position.x - 40.0f, skitter.position.y},
        (Vector2){1.0f, 0.0f}, ROOM_ELEVATION_GROUND,
        INVENTORY_WEAPON_PALMWOOD_CLUB));
    CHECK(AshSkitter_TryHit(&skitter, inRange, (Vector2){1.0f, 0.0f},
                            ROOM_ELEVATION_GROUND,
                            INVENTORY_WEAPON_PALMWOOD_CLUB));
    CHECK(skitter.health == ASH_SKITTER_HEALTH - 1);
    CHECK(!AshSkitter_TryHit(&skitter, inRange, (Vector2){1.0f, 0.0f},
                             ROOM_ELEVATION_GROUND,
                             INVENTORY_WEAPON_PALMWOOD_CLUB));

    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y},
                      ASH_SKITTER_RECOVERY_TIME);
    ReachRecovery(&skitter);
    Vector2 oppositeSide = {skitter.position.x + 16.0f, skitter.position.y};
    CHECK(skitter.direction == -1);
    CHECK(AshSkitter_TryHit(&skitter, oppositeSide, (Vector2){-1.0f, 0.0f},
                            ROOM_ELEVATION_GROUND,
                            INVENTORY_WEAPON_THORNFANG));
    CHECK(skitter.health == 0);
    CHECK(skitter.state == ASH_SKITTER_STATE_DEFEATED);
    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y}, 10.0f);
    CHECK(skitter.state == ASH_SKITTER_STATE_DEFEATED);

    AshSkitter_Init(&skitter);
    CHECK(skitter.state == ASH_SKITTER_STATE_DORMANT);
    CHECK(skitter.health == ASH_SKITTER_HEALTH);
    CHECK(skitter.position.x == ASH_SKITTER_PATCH_LEFT_X);
    CHECK(skitter.position.y == ASH_SKITTER_LANE_Y);
    CHECK(skitter.direction == 1);
    CHECK(!skitter.hitThisRecovery);
    CHECK(!skitter.playerHitThisDash);
}

static void TestNonPositiveUpdatesDoNothing(void)
{
    AshSkitter skitter;
    StartSkitter(&skitter);
    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y}, 0.0f);
    CHECK(skitter.state == ASH_SKITTER_STATE_TELL);
    AshSkitter_Update(&skitter, (Vector2){320.0f, ASH_SKITTER_LANE_Y}, -1.0f);
    CHECK(skitter.state == ASH_SKITTER_STATE_TELL);
    CHECK(skitter.stateTime == 0.0f);
}

int main(void)
{
    TestSkitterWakesWithVisibleTellOnlyWhenApproached();
    TestSkitterDashesBetweenPatchesAndReverses();
    TestDashContactOnlyHitsOncePerPass();
    TestContactCanHitAgainOnNextDash();
    TestCombatWeaponsCanHitOnceInEachRecovery();
    TestNonPositiveUpdatesDoNothing();

    if (failures != 0) {
        fprintf(stderr, "%d Ash Skitter check(s) failed.\n", failures);
        return 1;
    }

    puts("All Ash Skitter checks passed.");
    return 0;
}
