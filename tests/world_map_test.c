#include "world_map.h"

#include <stdio.h>
#include <string.h>

static int failures;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static void TestBlueprintRegionAndZoneCounts(void)
{
    static const int expectedCounts[REGION_COUNT] = {2, 2, 4, 3, 3, 3, 4, 3};
    static const int expectedFirstZones[REGION_COUNT] = {0, 2, 4, 8, 11, 14,
                                                         17, 21};

    CHECK(REGION_COUNT == 8);
    CHECK(ZONE_COUNT == 24);
    for (int region = 0; region < REGION_COUNT; region++) {
        CHECK(world_region(region) != NULL);
        CHECK(world_region_zone_count(region) == expectedCounts[region]);
        CHECK(world_region_first_zone(region) == expectedFirstZones[region]);
    }
    CHECK(world_region(-1) == NULL);
    CHECK(world_region(REGION_COUNT) == NULL);
    CHECK(world_region_first_zone(-1) == -1);
    CHECK(world_region_zone_count(REGION_COUNT) == 0);
}

static void TestZoneLookups(void)
{
    CHECK(world_zone(Z_WRECKSHORE) != NULL);
    CHECK(strcmp(world_zone(Z_WRECKSHORE)->name, "Wreckshore") == 0);
    CHECK(strcmp(world_zone(Z_QUIET_CORE)->name, "Quiet Core") == 0);
    CHECK(world_zone(-1) == NULL);
    CHECK(world_zone(ZONE_COUNT) == NULL);
    CHECK(world_zone_by_name("Jungle Entrance") == Z_JUNGLE_ENTRANCE);
    CHECK(world_zone_by_name("unknown") == -1);
    CHECK(world_zone_by_name(NULL) == -1);
}

static void TestPlayableZoneMarkers(void)
{
    CHECK(world_zone_is_playable(Z_JUNGLE_ENTRANCE));
    CHECK(world_zone_is_playable(Z_ROOT_PATH));
    CHECK(world_zone_is_playable(Z_OVERGROWN_CLEARING));
    CHECK(world_zone_is_playable(Z_SHRINE_APPROACH));
    CHECK(world_zone_is_playable(Z_ASHFALL_DESCENT));
    CHECK(!world_zone_is_playable(Z_WRECKSHORE));
    CHECK(!world_zone_is_playable(Z_FORGE_VEINS));
    CHECK(!world_zone_is_playable(Z_QUIET_CORE));
    CHECK(!world_zone_is_playable(-1));
    CHECK(!world_zone_is_playable(ZONE_COUNT));
}

static void TestTravelAndGateRequirements(void)
{
    CHECK(world_travel(Z_WRECKSHORE, EDGE_NORTH, 0) == Z_TIDEPOOL_PATH);
    CHECK(world_travel(Z_TIDEPOOL_PATH, EDGE_SOUTH, 0) == Z_WRECKSHORE);
    CHECK(world_travel(Z_HARBOR_ROW, EDGE_NORTH, 0) == -1);
    CHECK(world_travel(Z_HARBOR_ROW, EDGE_NORTH, WF_THORNFANG) ==
          Z_ASHFALL_DESCENT);
    CHECK(world_travel(Z_ASHFALL_DESCENT, EDGE_SOUTH, WF_THORNFANG) ==
          Z_HARBOR_ROW);
    CHECK(world_travel(Z_HOLLOW_HALL, EDGE_NORTH, WF_THORNFANG) == -1);
    CHECK(world_travel(Z_HOLLOW_HALL, EDGE_NORTH, WF_STORY_FINAL) ==
          Z_OUTER_THRESHOLD);
    CHECK(world_gate_requirement(GATE_TIDE) == WF_TIDE_LOW);
    CHECK(world_gate_requirement(GATE_NONE) == 0);
    CHECK(world_gate_locked_msg(GATE_THORNFANG)[0] != '\0');
    CHECK(world_gate_locked_msg(GATE_NONE)[0] == '\0');
}

static void TestInvalidTravelInputs(void)
{
    CHECK(!world_can_travel(-1, EDGE_NORTH, 0));
    CHECK(!world_can_travel(Z_WRECKSHORE, (EdgeDir)-1, 0));
    CHECK(!world_can_travel(Z_WRECKSHORE, EDGE_COUNT, 0));
    CHECK(world_travel(Z_WRECKSHORE, EDGE_SOUTH, 0) == -1);
}

static void TestMapValidation(void)
{
    CHECK(world_validate());
    for (int zone = 0; zone < ZONE_COUNT; zone++) {
        for (int direction = 0; direction < EDGE_COUNT; direction++) {
            const Exit *exit = &g_zones[zone].exits[direction];
            if (exit->target == EDGE_NONE) continue;
            CHECK(exit->target < ZONE_COUNT);
            CHECK(exit->entry < EDGE_COUNT);
            const Exit *back = &g_zones[exit->target].exits[exit->entry];
            CHECK(back->target == zone);
            CHECK(back->entry == direction);
            CHECK(back->gate == exit->gate);
        }
    }
}

int main(void)
{
    TestBlueprintRegionAndZoneCounts();
    TestZoneLookups();
    TestPlayableZoneMarkers();
    TestTravelAndGateRequirements();
    TestInvalidTravelInputs();
    TestMapValidation();

    if (failures != 0) {
        fprintf(stderr, "%d world-map check(s) failed.\n", failures);
        return 1;
    }
    puts("Campaign world-map checks passed.");
    return 0;
}
