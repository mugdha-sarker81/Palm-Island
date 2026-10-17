#include "render.h"
#include <stdio.h>

static int failures;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static void TestBackToFrontAndStableTies(void)
{
    RenderItem items[] = {
        {.feetY = 40.0f, .stableOrder = 4},
        {.feetY = 20.0f, .stableOrder = 3},
        {.feetY = 40.0f, .stableOrder = 1},
        {.feetY = 40.0f, .stableOrder = 2}
    };

    Render_SortItems(items, sizeof(items) / sizeof(items[0]));

    CHECK(items[0].stableOrder == 3);
    CHECK(items[1].stableOrder == 1);
    CHECK(items[2].stableOrder == 2);
    CHECK(items[3].stableOrder == 4);
}

static void TestPlayerSortsBetweenPropAnchors(void)
{
    RenderItem items[] = {
        {.type = RENDER_ITEM_PROP, .feetY = 111.0f, .stableOrder = 1}, /* palm */
        {.type = RENDER_ITEM_PLAYER, .feetY = 88.0f, .stableOrder = 0},
        {.type = RENDER_ITEM_PROP, .feetY = 80.0f, .stableOrder = 2} /* pillar */
    };

    Render_SortItems(items, sizeof(items) / sizeof(items[0]));

    CHECK(items[0].type == RENDER_ITEM_PROP && items[0].stableOrder == 2);
    CHECK(items[1].type == RENDER_ITEM_PLAYER);
    CHECK(items[2].type == RENDER_ITEM_PROP && items[2].stableOrder == 1);
}

static void TestForegroundItemsDrawAfterDepthItems(void)
{
    RenderItem items[] = {
        {.feetY = 10.0f, .stableOrder = 3, .foreground = true},
        {.feetY = 30.0f, .stableOrder = 2},
        {.feetY = 50.0f, .stableOrder = 1, .foreground = true},
        {.feetY = 20.0f, .stableOrder = 4}
    };

    Render_SortItems(items, sizeof(items) / sizeof(items[0]));

    CHECK(items[0].stableOrder == 4);
    CHECK(items[1].stableOrder == 2);
    CHECK(items[2].stableOrder == 3);
    CHECK(items[3].stableOrder == 1);
    CHECK(items[2].foreground);
    CHECK(items[3].foreground);
}

static void TestUpperElevationDrawsAfterGroundElevation(void)
{
    RenderItem items[] = {
        {.feetY = 120.0f, .elevation = ROOM_ELEVATION_GROUND, .stableOrder = 1},
        {.feetY = 24.0f, .elevation = ROOM_ELEVATION_UPPER, .stableOrder = 2},
        {.feetY = 10.0f, .elevation = ROOM_ELEVATION_GROUND, .stableOrder = 3,
         .foreground = true}
    };

    Render_SortItems(items, sizeof(items) / sizeof(items[0]));

    CHECK(items[0].stableOrder == 1);
    CHECK(items[1].stableOrder == 2);
    CHECK(items[2].stableOrder == 3);
}

static void TestThornbackSortsWithPlayerAndProps(void)
{
    RenderItem items[] = {
        {.type = RENDER_ITEM_PROP, .feetY = 190.0f, .stableOrder = 1},
        {.type = RENDER_ITEM_THORNBACK, .feetY = 184.0f, .stableOrder = 11},
        {.type = RENDER_ITEM_PLAYER, .feetY = 180.0f, .stableOrder = 0}
    };

    Render_SortItems(items, sizeof(items) / sizeof(items[0]));

    CHECK(items[0].type == RENDER_ITEM_PLAYER);
    CHECK(items[1].type == RENDER_ITEM_THORNBACK);
    CHECK(items[2].type == RENDER_ITEM_PROP);
}

static void TestAshSkitterSortsWithPlayerAndProps(void)
{
    RenderItem items[] = {
        {.type = RENDER_ITEM_PROP, .feetY = 204.0f, .stableOrder = 1},
        {.type = RENDER_ITEM_ASH_SKITTER, .feetY = 200.0f, .stableOrder = 12},
        {.type = RENDER_ITEM_PLAYER, .feetY = 196.0f, .stableOrder = 0}
    };

    Render_SortItems(items, sizeof(items) / sizeof(items[0]));

    CHECK(items[0].type == RENDER_ITEM_PLAYER);
    CHECK(items[1].type == RENDER_ITEM_ASH_SKITTER);
    CHECK(items[2].type == RENDER_ITEM_PROP);
}

static void TestRootMiteSortsWithPlayerAndProps(void)
{
    RenderItem items[] = {
        {.type = RENDER_ITEM_PROP, .feetY = 204.0f, .stableOrder = 1},
        {.type = RENDER_ITEM_ROOT_MITE, .feetY = 200.0f, .stableOrder = 14},
        {.type = RENDER_ITEM_PLAYER, .feetY = 196.0f, .stableOrder = 0}
    };

    Render_SortItems(items, sizeof(items) / sizeof(items[0]));

    CHECK(items[0].type == RENDER_ITEM_PLAYER);
    CHECK(items[1].type == RENDER_ITEM_ROOT_MITE);
    CHECK(items[2].type == RENDER_ITEM_PROP);
}

int main(void)
{
    TestBackToFrontAndStableTies();
    TestPlayerSortsBetweenPropAnchors();
    TestForegroundItemsDrawAfterDepthItems();
    TestUpperElevationDrawsAfterGroundElevation();
    TestThornbackSortsWithPlayerAndProps();
    TestAshSkitterSortsWithPlayerAndProps();
    TestRootMiteSortsWithPlayerAndProps();

    if (failures != 0) {
        fprintf(stderr, "%d render-depth check(s) failed.\n", failures);
        return 1;
    }

    puts("All render-depth checks passed.");
    return 0;
}
