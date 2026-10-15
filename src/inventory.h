#ifndef PALM_INVENTORY_H
#define PALM_INVENTORY_H

#include <stdbool.h>
#include <stdint.h>

typedef enum InventoryWeapon {
    INVENTORY_WEAPON_NONE,
    INVENTORY_WEAPON_THORNFANG,
    INVENTORY_WEAPON_PALMWOOD_CLUB,
    INVENTORY_WEAPON_COUNT
} InventoryWeapon;

typedef enum InventoryKeyItem {
    INVENTORY_KEY_ITEM_ROOT_WEAVE,
    INVENTORY_KEY_ITEM_ROOT_MARKER_TRACE,
    INVENTORY_KEY_ITEM_CLEARING_NOTE,
    INVENTORY_KEY_ITEM_ROOTS_MURAL,
    INVENTORY_KEY_ITEM_CRACKED_MINERAL_SEAL,
    INVENTORY_KEY_ITEM_COOLED_MINERAL_SEAM,
    INVENTORY_KEY_ITEM_COUNT
} InventoryKeyItem;

typedef struct Inventory {
    uint32_t ownedWeapons;
    InventoryWeapon selectedWeapon;
    uint32_t keyItems;
    int foodServings;
} Inventory;

bool Inventory_HasWeapon(const Inventory *inventory, InventoryWeapon weapon);
bool Inventory_AddWeapon(Inventory *inventory, InventoryWeapon weapon);
bool Inventory_SelectWeapon(Inventory *inventory, InventoryWeapon weapon);
bool Inventory_HasKeyItem(const Inventory *inventory, InventoryKeyItem item);
bool Inventory_AddKeyItem(Inventory *inventory, InventoryKeyItem item);
bool Inventory_AddFood(Inventory *inventory);
bool Inventory_ConsumeFood(Inventory *inventory, int *health,
                           int maximumHealth);
bool Inventory_IsValid(const Inventory *inventory);

#endif
