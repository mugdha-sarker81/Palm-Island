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

static void TestPlayerPositionMapping(void)
{
    Vector2 mapPosition = Room_MiniMapPlayerPosition((Vector2){0.0f, 0.0f});
    CHECK(fabsf(mapPosition.x - 234.0f) < EPSILON);
    CHECK(fabsf(mapPosition.y - 20.0f) < EPSILON);

    mapPosition = Room_MiniMapPlayerPosition((Vector2){320.0f, 184.0f});
    CHECK(fabsf(mapPosition.x - 274.0f) < EPSILON);
    CHECK(fabsf(mapPosition.y - 43.0f) < EPSILON);

    mapPosition = Room_MiniMapPlayerPosition((Vector2){640.0f, 368.0f});
    CHECK(fabsf(mapPosition.x - 314.0f) < EPSILON);
    CHECK(fabsf(mapPosition.y - 66.0f) < EPSILON);
}

int main(void)
{
    TestAreaLabels();
    TestPlayerPositionMapping();

    if (failures != 0) {
        fprintf(stderr, "%d minimap check(s) failed.\n", failures);
        return 1;
    }
    puts("Minimap checks passed.");
    return 0;
}
