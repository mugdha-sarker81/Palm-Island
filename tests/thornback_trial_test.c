#include "thornback.h"
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

static void TestTrialStartsTowardPlayerAndShowsTell(void)
{
    Thornback thornback;
    Thornback_Init(&thornback);

    CHECK(thornback.state == THORNBACK_STATE_DORMANT);
    CHECK(thornback.position.x == THORNBACK_START_X);
    CHECK(thornback.position.y == THORNBACK_LANE_Y);
    CHECK(Thornback_Start(&thornback, (Vector2){100.0f, 180.0f}));
    CHECK(thornback.state == THORNBACK_STATE_TELL);
    CHECK(thornback.direction == -1);
    CHECK(!Thornback_Start(&thornback, (Vector2){500.0f, 180.0f}));

    Thornback_Update(&thornback, THORNBACK_TELL_TIME * 0.5f);
    CHECK(thornback.state == THORNBACK_STATE_TELL);
    CHECK(thornback.position.x == THORNBACK_START_X);
    Thornback_Update(&thornback, THORNBACK_TELL_TIME);
    CHECK(thornback.state == THORNBACK_STATE_CHARGE);
    CHECK(thornback.position.x == THORNBACK_START_X);
}

static void TestStraightChargeStopsAtLaneEdgeAndRecovers(void)
{
    Thornback thornback;
    Thornback_Init(&thornback);
    CHECK(Thornback_Start(&thornback, (Vector2){100.0f, THORNBACK_LANE_Y}));
    Thornback_Update(&thornback, THORNBACK_TELL_TIME);

    Thornback_Update(&thornback, 0.25f);
    CHECK(thornback.state == THORNBACK_STATE_CHARGE);
    CHECK(fabsf(thornback.position.x -
                (THORNBACK_START_X - THORNBACK_CHARGE_SPEED * 0.25f)) < 0.01f);
    CHECK(thornback.position.y == THORNBACK_LANE_Y);

    Thornback_Update(&thornback, 1.0f);
    CHECK(thornback.state == THORNBACK_STATE_RECOVERY);
    CHECK(thornback.position.x == THORNBACK_LANE_MIN_X);
    CHECK(thornback.position.y == THORNBACK_LANE_Y);
    Thornback_Update(&thornback, THORNBACK_RECOVERY_TIME * 0.5f);
    CHECK(thornback.state == THORNBACK_STATE_RECOVERY);
    Thornback_Update(&thornback, THORNBACK_RECOVERY_TIME);
    CHECK(thornback.state == THORNBACK_STATE_SWEEP_TELL);
    CHECK(thornback.direction == 1);
}

static void TestSweepTellAndSweepLeaveARepositioningLane(void)
{
    Thornback thornback;
    Thornback_Init(&thornback);
    CHECK(Thornback_Start(&thornback, (Vector2){100.0f, THORNBACK_LANE_Y}));
    Thornback_Update(&thornback, THORNBACK_TELL_TIME);
    Thornback_Update(&thornback, 1.0f);
    CHECK(thornback.state == THORNBACK_STATE_RECOVERY);
    CHECK(thornback.position.x == THORNBACK_LANE_MIN_X);

    Thornback_Update(&thornback, THORNBACK_RECOVERY_TIME);
    CHECK(thornback.state == THORNBACK_STATE_SWEEP_TELL);
    CHECK(thornback.position.x == THORNBACK_LANE_MIN_X);
    CHECK(thornback.position.y == THORNBACK_LANE_Y);
    CHECK(!Thornback_IsInSweepLane(
        &thornback, (Vector2){320.0f, THORNBACK_LANE_Y}));
    Thornback_Update(&thornback, THORNBACK_SWEEP_TELL_TIME * 0.5f);
    CHECK(thornback.state == THORNBACK_STATE_SWEEP_TELL);

    Thornback_Update(&thornback, THORNBACK_SWEEP_TELL_TIME);
    CHECK(thornback.state == THORNBACK_STATE_SWEEP);
    CHECK(Thornback_IsInSweepLane(
        &thornback, (Vector2){320.0f, THORNBACK_LANE_Y}));
    CHECK(!Thornback_IsInSweepLane(
        &thornback, (Vector2){320.0f, THORNBACK_LANE_Y +
                                        THORNBACK_SWEEP_HALF_HEIGHT + 6.0f}));
    CHECK(!Thornback_IsInSweepLane(
        &thornback, (Vector2){THORNBACK_LANE_MIN_X - 1.0f,
                              THORNBACK_LANE_Y}));

    Thornback_Update(&thornback, 0.5f);
    CHECK(thornback.state == THORNBACK_STATE_SWEEP);
    CHECK(fabsf(thornback.position.x -
                (THORNBACK_LANE_MIN_X + THORNBACK_SWEEP_SPEED * 0.5f)) <
          0.01f);
    CHECK(thornback.position.y == THORNBACK_LANE_Y);

    Thornback_Update(&thornback, 2.0f);
    CHECK(thornback.state == THORNBACK_STATE_RECOVERY);
    CHECK(thornback.position.x == THORNBACK_LANE_MAX_X);
    CHECK(!Thornback_IsInSweepLane(
        &thornback, (Vector2){320.0f, THORNBACK_LANE_Y}));
    Thornback_Update(&thornback, THORNBACK_RECOVERY_TIME);
    CHECK(thornback.state == THORNBACK_STATE_TELL);
    CHECK(thornback.direction == -1);
}

static void TestAttackZonesMatchTheirTelegraphedAreas(void)
{
    Thornback thornback;
    Thornback_Init(&thornback);
    CHECK(Thornback_Start(&thornback, (Vector2){100.0f, THORNBACK_LANE_Y}));
    CHECK(!Thornback_IsPlayerInAttackZone(
        &thornback, (Vector2){320.0f, THORNBACK_LANE_Y}));
    Thornback_Update(&thornback, THORNBACK_TELL_TIME - SIM_DT);
    CHECK(!Thornback_IsPlayerInAttackZone(
        &thornback, (Vector2){320.0f, THORNBACK_LANE_Y}));
    Thornback_Update(&thornback, SIM_DT);
    CHECK(Thornback_IsPlayerInAttackZone(
        &thornback, (Vector2){320.0f, THORNBACK_LANE_Y}));
    CHECK(!Thornback_IsPlayerInAttackZone(
        &thornback, (Vector2){360.0f, THORNBACK_LANE_Y}));
    Thornback_Update(&thornback, 0.25f);
    CHECK(Thornback_IsPlayerInAttackZone(
        &thornback, (Vector2){270.0f, THORNBACK_LANE_Y}));
    CHECK(!Thornback_IsPlayerInAttackZone(
        &thornback, (Vector2){270.0f, THORNBACK_LANE_Y +
                                        THORNBACK_CHARGE_HALF_HEIGHT +
                                        PLAYER_COLLIDER_H * 0.5f + 0.1f}));
    CHECK(Thornback_TryPlayerContact(
        &thornback, (Vector2){270.0f, THORNBACK_LANE_Y}));
    CHECK(!Thornback_TryPlayerContact(
        &thornback, (Vector2){270.0f, THORNBACK_LANE_Y}));

    Thornback_Update(&thornback, 2.0f);
    CHECK(thornback.state == THORNBACK_STATE_RECOVERY);
    Thornback_Update(&thornback, THORNBACK_RECOVERY_TIME);
    Thornback_Update(&thornback, THORNBACK_SWEEP_TELL_TIME);
    CHECK(thornback.state == THORNBACK_STATE_SWEEP);
    CHECK(Thornback_IsPlayerInAttackZone(
        &thornback, (Vector2){320.0f, THORNBACK_LANE_Y +
                                        THORNBACK_SWEEP_HALF_HEIGHT}));
    CHECK(!Thornback_IsPlayerInAttackZone(
        &thornback, (Vector2){320.0f, THORNBACK_LANE_Y +
                                        THORNBACK_SWEEP_HALF_HEIGHT +
                                        PLAYER_COLLIDER_H * 0.5f + 0.1f}));
}

static void TestRecoveryHitsRequireRangeFacingAndOnlyLandOnce(void)
{
    Thornback thornback;
    Thornback_Init(&thornback);
    CHECK(Thornback_Start(&thornback, (Vector2){100.0f, THORNBACK_LANE_Y}));
    Thornback_Update(&thornback, THORNBACK_TELL_TIME);
    Thornback_Update(&thornback, 2.0f);
    CHECK(thornback.state == THORNBACK_STATE_RECOVERY);
    CHECK(thornback.health == THORNBACK_HEALTH);

    CHECK(!Thornback_TryHit(&thornback, (Vector2){220.0f, 220.0f},
                            (Vector2){-1.0f, 0.0f}));
    CHECK(!Thornback_TryHit(&thornback, (Vector2){160.0f, THORNBACK_LANE_Y},
                            (Vector2){1.0f, 0.0f}));
    CHECK(!Thornback_TryHit(&thornback, (Vector2){220.0f, THORNBACK_LANE_Y},
                            (Vector2){1.0f, 0.0f}));
    CHECK(Thornback_TryHit(&thornback, (Vector2){220.0f, THORNBACK_LANE_Y},
                           (Vector2){-1.0f, 0.0f}));
    CHECK(thornback.health == THORNBACK_HEALTH - 1);
    CHECK(!Thornback_TryHit(&thornback, (Vector2){220.0f, THORNBACK_LANE_Y},
                            (Vector2){-1.0f, 0.0f}));

    Thornback_Update(&thornback, THORNBACK_RECOVERY_TIME);
    Thornback_Update(&thornback, THORNBACK_SWEEP_TELL_TIME);
    Thornback_Update(&thornback, 3.0f);
    CHECK(thornback.state == THORNBACK_STATE_RECOVERY);
    CHECK(Thornback_TryHit(&thornback, (Vector2){420.0f, THORNBACK_LANE_Y},
                           (Vector2){1.0f, 0.0f}));
    CHECK(thornback.health == THORNBACK_HEALTH - 2);

    Thornback_Update(&thornback, THORNBACK_RECOVERY_TIME);
    Thornback_Update(&thornback, THORNBACK_TELL_TIME);
    Thornback_Update(&thornback, 3.0f);
    CHECK(thornback.state == THORNBACK_STATE_RECOVERY);
    CHECK(Thornback_TryHit(&thornback, (Vector2){220.0f, THORNBACK_LANE_Y},
                           (Vector2){-1.0f, 0.0f}));
    CHECK(thornback.health == 0);
    CHECK(thornback.state == THORNBACK_STATE_DEFEATED);
    Thornback_Update(&thornback, 10.0f);
    CHECK(thornback.state == THORNBACK_STATE_DEFEATED);
}

static void TestRepeatedPhaseOneChargeReversesDirection(void)
{
    Thornback thornback;
    Thornback_Init(&thornback);
    CHECK(Thornback_Start(&thornback, (Vector2){500.0f, THORNBACK_LANE_Y}));
    Thornback_Update(&thornback, THORNBACK_TELL_TIME);
    Thornback_Update(&thornback, 1.0f);
    CHECK(thornback.state == THORNBACK_STATE_RECOVERY);
    CHECK(thornback.position.x == THORNBACK_LANE_MAX_X);

    Thornback_Update(&thornback, THORNBACK_RECOVERY_TIME);
    CHECK(thornback.state == THORNBACK_STATE_SWEEP_TELL);
    CHECK(thornback.direction == -1);
    Thornback_Update(&thornback, THORNBACK_SWEEP_TELL_TIME);
    CHECK(thornback.state == THORNBACK_STATE_SWEEP);
    Thornback_Update(&thornback, 3.0f);
    CHECK(thornback.state == THORNBACK_STATE_RECOVERY);
    CHECK(thornback.position.x == THORNBACK_LANE_MIN_X);
    Thornback_Update(&thornback, THORNBACK_RECOVERY_TIME);
    CHECK(thornback.state == THORNBACK_STATE_TELL);
    CHECK(thornback.direction == 1);
    Thornback_Update(&thornback, THORNBACK_TELL_TIME);
    CHECK(thornback.state == THORNBACK_STATE_CHARGE);
    Thornback_Update(&thornback, 0.25f);
    CHECK(thornback.position.x > THORNBACK_LANE_MIN_X);
    CHECK(thornback.position.y == THORNBACK_LANE_Y);
}

static void TestDormantAndNonPositiveUpdatesDoNothing(void)
{
    Thornback thornback;
    Thornback_Init(&thornback);
    Thornback_Update(&thornback, 5.0f);
    CHECK(thornback.state == THORNBACK_STATE_DORMANT);

    CHECK(Thornback_Start(&thornback, (Vector2){500.0f, THORNBACK_LANE_Y}));
    Thornback_Update(&thornback, -1.0f);
    CHECK(thornback.state == THORNBACK_STATE_TELL);
    CHECK(thornback.stateTime == 0.0f);
    Thornback_Init(&thornback);
    CHECK(thornback.state == THORNBACK_STATE_DORMANT);
}

int main(void)
{
    TestTrialStartsTowardPlayerAndShowsTell();
    TestStraightChargeStopsAtLaneEdgeAndRecovers();
    TestSweepTellAndSweepLeaveARepositioningLane();
    TestAttackZonesMatchTheirTelegraphedAreas();
    TestRecoveryHitsRequireRangeFacingAndOnlyLandOnce();
    TestRepeatedPhaseOneChargeReversesDirection();
    TestDormantAndNonPositiveUpdatesDoNothing();

    if (failures != 0) {
        fprintf(stderr, "%d Thornback trial check(s) failed.\n", failures);
        return 1;
    }

    puts("All Thornback trial checks passed.");
    return 0;
}
