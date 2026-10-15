#include "inventory.h"
#include "tuning.h"
#include <stdio.h>

static int failures;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        failures++; \
    } \
} while (0)

int main(void)
{
    Inventory inventory = {0};
    CHECK(Inventory_IsValid(&inventory));
    CHECK(!Inventory_HasWeapon(&inventory, INVENTORY_WEAPON_THORNFANG));
    CHECK(!Inventory_SelectWeapon(&inventory, INVENTORY_WEAPON_THORNFANG));

    CHECK(Inventory_AddWeapon(&inventory, INVENTORY_WEAPON_THORNFANG));
    CHECK(Inventory_HasWeapon(&inventory, INVENTORY_WEAPON_THORNFANG));
    CHECK(inventory.selectedWeapon == INVENTORY_WEAPON_THORNFANG);
    CHECK(Inventory_IsValid(&inventory));

    CHECK(Inventory_AddKeyItem(&inventory, INVENTORY_KEY_ITEM_ROOT_WEAVE));
    CHECK(Inventory_HasKeyItem(&inventory, INVENTORY_KEY_ITEM_ROOT_WEAVE));
    CHECK(Inventory_IsValid(&inventory));

    CHECK(Inventory_AddFood(&inventory));
    CHECK(Inventory_AddFood(&inventory));
    CHECK(Inventory_AddFood(&inventory));
    CHECK(!Inventory_AddFood(&inventory));
    CHECK(inventory.foodServings == FOOD_SERVING_CAPACITY);
    int health = PLAYER_START_HEALTH - 1;
    CHECK(Inventory_ConsumeFood(&inventory, &health, PLAYER_START_HEALTH));
    CHECK(health == PLAYER_START_HEALTH);
    CHECK(inventory.foodServings == FOOD_SERVING_CAPACITY - 1);
    CHECK(!Inventory_ConsumeFood(&inventory, &health, PLAYER_START_HEALTH));
    CHECK(inventory.foodServings == FOOD_SERVING_CAPACITY - 1);
    health = PLAYER_START_HEALTH - 3;
    CHECK(Inventory_ConsumeFood(&inventory, &health, PLAYER_START_HEALTH));
    CHECK(health == PLAYER_START_HEALTH - 1);

    CHECK(!Inventory_AddWeapon(&inventory, INVENTORY_WEAPON_COUNT));
    CHECK(!Inventory_AddKeyItem(&inventory, INVENTORY_KEY_ITEM_COUNT));
    inventory.selectedWeapon = INVENTORY_WEAPON_NONE;
    CHECK(!Inventory_IsValid(&inventory));

    if (failures != 0) {
        fprintf(stderr, "%d inventory check(s) failed.\n", failures);
        return 1;
    }
    puts("All inventory checks passed.");
    return 0;
}
