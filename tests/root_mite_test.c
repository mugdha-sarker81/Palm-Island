#include "root_mite.h"
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

static Vector2 NestHome(void)
{
    return (Vector2){ROOT_MITE_HOME_X, ROOT_MITE_HOME_Y};
}

static Vector2 EastOfNest(float distance)
{
    return (Vector2){ROOT_MITE_HOME_X + distance, ROOT_MITE_HOME_Y};
}

/* Wakes the mite, triggers the pause tell, and enters the lunge. */
static void ReachLunge(RootMite *mite, Vector2 playerPosition)
{
    RootMite_Init(mite, false);
    RootMite_Update(mite, playerPosition, SIM_DT);
    CHECK(mite->state == ROOT_MITE_STATE_APPROACH);
    RootMite_Update(mite, playerPosition, SIM_DT);
    CHECK(mite->state == ROOT_MITE_STATE_PAUSE);
    RootMite_Update(mite, playerPosition, ROOT_MITE_PAUSE_TIME);
    CHECK(mite->state == ROOT_MITE_STATE_LUNGE);
}

static void TestInitSeedsDefeatedFromRewardClaim(void)
{
    RootMite mite;

    RootMite_Init(&mite, false);
    CHECK(mite.state == ROOT_MITE_STATE_DORMANT);
    CHECK(mite.health == ROOT_MITE_HEALTH);

    RootMite_Init(&mite, true);
    CHECK(mite.state == ROOT_MITE_STATE_DEFEATED);
    CHECK(mite.health == 0);

    /* A claimed reward keeps the mite out of the fight for good. */
    Vector2 home = NestHome();
    RootMite_Update(&mite, home, SIM_DT);
    CHECK(mite.state == ROOT_MITE_STATE_DEFEATED);
    CHECK(mite.position.x == home.x && mite.position.y == home.y);
    CHECK(!RootMite_TryPlayerContact(&mite, home, ROOM_ELEVATION_GROUND));
    CHECK(!RootMite_TryHit(&mite, (Vector2){home.x - 20.0f, home.y},
                           (Vector2){1.0f, 0.0f}, ROOM_ELEVATION_GROUND,
                           INVENTORY_WEAPON_PALMWOOD_CLUB));
}

static void TestDormantWakesOnlyAtAlertBoundary(void)
{
    RootMite mite;
    Vector2 home = NestHome();

    RootMite_Init(&mite, false);
    RootMite_Update(&mite, EastOfNest(ROOT_MITE_ALERT_RANGE + 1.0f), SIM_DT);
    CHECK(mite.state == ROOT_MITE_STATE_DORMANT);
    CHECK(mite.position.x == home.x && mite.position.y == home.y);

    RootMite_Update(&mite, EastOfNest(ROOT_MITE_ALERT_RANGE), SIM_DT);
    CHECK(mite.state == ROOT_MITE_STATE_APPROACH);
    CHECK(mite.position.x == home.x && mite.position.y == home.y);
}

static void TestApproachIsSlowPauseTellsAndLocksDirection(void)
{
    RootMite mite;
    Vector2 home = NestHome();

    CHECK(ROOT_MITE_APPROACH_SPEED < WALK_SPEED);

    RootMite_Init(&mite, false);
    RootMite_Update(&mite, EastOfNest(80.0f), SIM_DT);
    CHECK(mite.state == ROOT_MITE_STATE_APPROACH);

    RootMite_Update(&mite, EastOfNest(80.0f), SIM_DT);
    float step = mite.position.x - home.x;
    CHECK(fabsf(step - ROOT_MITE_APPROACH_SPEED * SIM_DT) < 1.0e-4f);
    CHECK(mite.position.y == home.y);

    /* Inside the pause trigger: the tell locks toward the player. */
    RootMite_Update(&mite, EastOfNest(40.0f), SIM_DT);
    CHECK(mite.state == ROOT_MITE_STATE_PAUSE);
    float lockedLength =
        sqrtf(mite.lungeDirection.x * mite.lungeDirection.x +
              mite.lungeDirection.y * mite.lungeDirection.y);
    CHECK(fabsf(lockedLength - 1.0f) < 1.0e-4f);
    CHECK(mite.lungeDirection.x > 0.99f);

    /* The pause lasts its full fairness window before the lunge. */
    RootMite_Update(&mite, (Vector2){ROOT_MITE_HOME_X,
                                     ROOT_MITE_HOME_Y - 40.0f},
                    ROOT_MITE_PAUSE_TIME - SIM_DT);
    CHECK(mite.state == ROOT_MITE_STATE_PAUSE);
    RootMite_Update(&mite, (Vector2){ROOT_MITE_HOME_X,
                                     ROOT_MITE_HOME_Y - 40.0f},
                    2.0f * SIM_DT);
    CHECK(mite.state == ROOT_MITE_STATE_LUNGE);
    CHECK(mite.lungeDirection.x > 0.99f && mite.lungeDirection.y < 1.0e-4f);
}

static void TestLungeContactHitsOnceAndStopsAtTerritoryEdge(void)
{
    RootMite mite;
    ReachLunge(&mite, EastOfNest(40.0f));
    CHECK(mite.position.x == ROOT_MITE_HOME_X &&
          mite.position.y == ROOT_MITE_HOME_Y);

    Vector2 onMite = mite.position;
    CHECK(RootMite_TryPlayerContact(&mite, onMite, ROOM_ELEVATION_GROUND));
    CHECK(!RootMite_TryPlayerContact(&mite, onMite, ROOM_ELEVATION_GROUND));

    RootMite_Update(&mite, onMite, 0.11f);
    CHECK(mite.state == ROOT_MITE_STATE_RECOVERY);
    CHECK(mite.position.x == ROOT_MITE_TERRITORY_MAX_X);

    /* Contact is a lunge-only threat. */
    CHECK(!RootMite_TryPlayerContact(&mite, mite.position,
                                     ROOM_ELEVATION_GROUND));
}

static void TestContactRequiresGroundElevation(void)
{
    RootMite mite;
    ReachLunge(&mite, EastOfNest(40.0f));

    Vector2 onMite = mite.position;
    CHECK(!RootMite_TryPlayerContact(&mite, onMite, ROOM_ELEVATION_UPPER));
    CHECK(RootMite_TryPlayerContact(&mite, onMite, ROOM_ELEVATION_GROUND));
}

static void TestHitWindowNeedsFacingRangeWeaponAndRecovery(void)
{
    RootMite mite;
    RootMite_Init(&mite, false);
    RootMite_Update(&mite, EastOfNest(40.0f), SIM_DT);
    RootMite_Update(&mite, EastOfNest(40.0f), SIM_DT);
    RootMite_Update(&mite, EastOfNest(40.0f), ROOT_MITE_PAUSE_TIME);
    RootMite_Update(&mite, EastOfNest(40.0f), 0.11f);
    CHECK(mite.state == ROOT_MITE_STATE_RECOVERY);
    Vector2 nest = mite.position; /* clamped to the east territory edge */

    /* Each gate of the hit window fails closed. */
    CHECK(!RootMite_TryHit(&mite, (Vector2){nest.x - 20.0f, nest.y},
                           (Vector2){-1.0f, 0.0f}, ROOM_ELEVATION_GROUND,
                           INVENTORY_WEAPON_PALMWOOD_CLUB));
    CHECK(!RootMite_TryHit(&mite, (Vector2){nest.x - 30.0f, nest.y},
                           (Vector2){1.0f, 0.0f}, ROOM_ELEVATION_GROUND,
                           INVENTORY_WEAPON_PALMWOOD_CLUB));
    CHECK(!RootMite_TryHit(&mite, (Vector2){nest.x - 20.0f, nest.y},
                           (Vector2){1.0f, 0.0f}, ROOM_ELEVATION_GROUND,
                           INVENTORY_WEAPON_NONE));
    CHECK(!RootMite_TryHit(&mite, (Vector2){nest.x - 20.0f, nest.y},
                           (Vector2){1.0f, 0.0f}, ROOM_ELEVATION_UPPER,
                           INVENTORY_WEAPON_PALMWOOD_CLUB));
    CHECK(mite.health == ROOT_MITE_HEALTH);

    /* One clean hit lands; the same window cannot be struck twice. */
    CHECK(RootMite_TryHit(&mite, (Vector2){nest.x - 20.0f, nest.y},
                          (Vector2){1.0f, 0.0f}, ROOM_ELEVATION_GROUND,
                          INVENTORY_WEAPON_PALMWOOD_CLUB));
    CHECK(mite.health == ROOT_MITE_HEALTH - 1);
    CHECK(!RootMite_TryHit(&mite, (Vector2){nest.x - 20.0f, nest.y},
                           (Vector2){1.0f, 0.0f}, ROOM_ELEVATION_GROUND,
                           INVENTORY_WEAPON_THORNFANG));
}

static void TestSecondRecoveryWindowDefeatsAndDefeatedStopsAll(void)
{
    RootMite mite;
    RootMite_Init(&mite, false);

    /* First recovery window: land one hit. */
    RootMite_Update(&mite, EastOfNest(40.0f), SIM_DT);
    RootMite_Update(&mite, EastOfNest(40.0f), SIM_DT);
    RootMite_Update(&mite, EastOfNest(40.0f), ROOT_MITE_PAUSE_TIME);
    RootMite_Update(&mite, EastOfNest(40.0f), 0.11f);
    CHECK(mite.state == ROOT_MITE_STATE_RECOVERY);
    CHECK(RootMite_TryHit(&mite, (Vector2){mite.position.x - 20.0f,
                                           mite.position.y},
                          (Vector2){1.0f, 0.0f}, ROOM_ELEVATION_GROUND,
                          INVENTORY_WEAPON_PALMWOOD_CLUB));
    CHECK(mite.health == ROOT_MITE_HEALTH - 1);

    /* Second cycle: approach, pause, lunge west, recover at the far edge. */
    Vector2 westPlayer = {mite.position.x - 40.0f, mite.position.y};
    RootMite_Update(&mite, westPlayer, ROOT_MITE_RECOVERY_TIME);
    CHECK(mite.state == ROOT_MITE_STATE_APPROACH);
    RootMite_Update(&mite, westPlayer, SIM_DT);
    CHECK(mite.state == ROOT_MITE_STATE_PAUSE);
    RootMite_Update(&mite, westPlayer, ROOT_MITE_PAUSE_TIME);
    CHECK(mite.state == ROOT_MITE_STATE_LUNGE);
    RootMite_Update(&mite, westPlayer, 0.22f);
    CHECK(mite.state == ROOT_MITE_STATE_RECOVERY);
    CHECK(mite.position.x == ROOT_MITE_TERRITORY_MIN_X);

    CHECK(RootMite_TryHit(&mite, (Vector2){mite.position.x + 20.0f,
                                           mite.position.y},
                          (Vector2){-1.0f, 0.0f}, ROOM_ELEVATION_GROUND,
                          INVENTORY_WEAPON_THORNFANG));
    CHECK(mite.state == ROOT_MITE_STATE_DEFEATED);
    CHECK(mite.health <= 0);

    /* The defeated mite never moves, chases, or deals damage again. */
    Vector2 frozen = mite.position;
    RootMite_Update(&mite, frozen, 1.0f);
    CHECK(mite.position.x == frozen.x && mite.position.y == frozen.y);
    CHECK(!RootMite_TryPlayerContact(&mite, frozen, ROOM_ELEVATION_GROUND));
    CHECK(!RootMite_TryHit(&mite, frozen, (Vector2){1.0f, 0.0f},
                           ROOM_ELEVATION_GROUND,
                           INVENTORY_WEAPON_THORNFANG));
}

static void TestLeashReturnsToNestAndSleeps(void)
{
    RootMite mite;
    Vector2 home = NestHome();

    RootMite_Init(&mite, false);
    RootMite_Update(&mite, EastOfNest(80.0f), SIM_DT);
    CHECK(mite.state == ROOT_MITE_STATE_APPROACH);
    for (int i = 0; i < 10; i++) {
        RootMite_Update(&mite, EastOfNest(80.0f), SIM_DT);
    }
    CHECK(mite.state == ROOT_MITE_STATE_APPROACH);
    CHECK(mite.position.x > home.x);

    Vector2 farPlayer = EastOfNest(ROOT_MITE_LEASH_RANGE + 24.0f);
    for (int i = 0; i < 400; i++) {
        RootMite_Update(&mite, farPlayer, SIM_DT);
    }
    CHECK(mite.state == ROOT_MITE_STATE_DORMANT);
    CHECK(mite.position.x == home.x && mite.position.y == home.y);
}

int main(void)
{
    TestInitSeedsDefeatedFromRewardClaim();
    TestDormantWakesOnlyAtAlertBoundary();
    TestApproachIsSlowPauseTellsAndLocksDirection();
    TestLungeContactHitsOnceAndStopsAtTerritoryEdge();
    TestContactRequiresGroundElevation();
    TestHitWindowNeedsFacingRangeWeaponAndRecovery();
    TestSecondRecoveryWindowDefeatsAndDefeatedStopsAll();
    TestLeashReturnsToNestAndSleeps();

    if (failures != 0) {
        fprintf(stderr, "%d root-mite check(s) failed.\n", failures);
        return 1;
    }

    puts("All root-mite checks passed.");
    return 0;
}

