#include "room.h"
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

static const float EPSILON = 0.01f;

static void TestAreaLabels(void)
{
    CHECK(strcmp(Room_AreaLabel(ROOM_AREA_JUNGLE_ENTRANCE), "ENTRANCE") == 0);
    CHECK(strcmp(Room_AreaLabel(ROOM_AREA_ROOT_PATH), "ROOT PATH") == 0);
    CHECK(strcmp(Room_AreaLabel(ROOM_AREA_OVERGROWN_CLEARING), "CLEARING") == 0);
    CHECK(strcmp(Room_AreaLabel(ROOM_AREA_SHRINE_APPROACH), "SHRINE") == 0);
    CHECK(strcmp(Room_AreaLabel(ROOM_AREA_THORNBACK_ARENA), "ARENA") == 0);
    CHECK(strcmp(Room_AreaLabel(ROOM_AREA_ASHFALL_DESCENT), "ASHFALL") == 0);
    CHECK(strcmp(Room_AreaLabel(ROOM_AREA_COUNT), "UNKNOWN") == 0);
}

static void TestPlayerPositionMappingAcrossWorldMap(void)
{
    Vector2 mapPosition = Room_WorldMapPlayerPosition(
        ROOM_AREA_JUNGLE_ENTRANCE, (Vector2){0.0f, 0.0f});
    CHECK(fabsf(mapPosition.x - 210.0f) < EPSILON);
    CHECK(fabsf(mapPosition.y - 25.0f) < EPSILON);

    mapPosition = Room_WorldMapPlayerPosition(
        ROOM_AREA_THORNBACK_ARENA, (Vector2){320.0f, 184.0f});
    CHECK(fabsf(mapPosition.x - 254.0f) < EPSILON);
    CHECK(fabsf(mapPosition.y - 55.0f) < EPSILON);

    mapPosition = Room_WorldMapPlayerPosition(
        ROOM_AREA_ASHFALL_DESCENT, (Vector2){640.0f, 368.0f});
    CHECK(fabsf(mapPosition.x - 226.0f) < EPSILON);
    CHECK(fabsf(mapPosition.y - 58.0f) < EPSILON);

    Vector2 clamped = Room_WorldMapPlayerPosition(
        ROOM_AREA_ASHFALL_DESCENT, (Vector2){800.0f, -40.0f});
    CHECK(fabsf(clamped.x - 226.0f) < EPSILON);
    CHECK(fabsf(clamped.y - 52.0f) < EPSILON);
}

static void TestEveryPlayableAreaHasADistinctWorldMapNode(void)
{
    Vector2 nodes[ROOM_AREA_COUNT];
    for (int area = 0; area < ROOM_AREA_COUNT; area++) {
        nodes[area] = Room_WorldMapPlayerPosition(
            (RoomArea)area, (Vector2){320.0f, 184.0f});
        CHECK(strcmp(Room_AreaLabel((RoomArea)area), "UNKNOWN") != 0);
        for (int previous = 0; previous < area; previous++) {
            CHECK(fabsf(nodes[area].x - nodes[previous].x) > EPSILON ||
                  fabsf(nodes[area].y - nodes[previous].y) > EPSILON);
        }
    }
}

int main(void)
{
    TestAreaLabels();
    TestPlayerPositionMappingAcrossWorldMap();
    TestEveryPlayableAreaHasADistinctWorldMapNode();

    if (failures != 0) {
        fprintf(stderr, "%d minimap check(s) failed.\n", failures);
        return 1;
    }
    puts("Minimap checks passed.");
    return 0;
}
