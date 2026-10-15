#include "inventory.h"

static uint32_t WeaponBit(InventoryWeapon weapon)
{
    if (weapon <= INVENTORY_WEAPON_NONE ||
        weapon >= INVENTORY_WEAPON_COUNT) {
        return 0;
    }
    return UINT32_C(1) << (unsigned int)(weapon - 1);
}

static uint32_t KeyItemBit(InventoryKeyItem item)
{
    if (item < 0 || item >= INVENTORY_KEY_ITEM_COUNT) return 0;
    return UINT32_C(1) << (unsigned int)item;
}

bool Inventory_HasWeapon(const Inventory *inventory, InventoryWeapon weapon)
{
    uint32_t bit = WeaponBit(weapon);
    return inventory != NULL && bit != 0 &&
           (inventory->ownedWeapons & bit) != 0;
}

bool Inventory_AddWeapon(Inventory *inventory, InventoryWeapon weapon)
{
    uint32_t bit = WeaponBit(weapon);
    if (inventory == NULL || bit == 0) return false;
    inventory->ownedWeapons |= bit;
    if (inventory->selectedWeapon == INVENTORY_WEAPON_NONE) {
        inventory->selectedWeapon = weapon;
    }
    return true;
}

bool Inventory_SelectWeapon(Inventory *inventory, InventoryWeapon weapon)
{
    if (inventory == NULL ||
        !Inventory_HasWeapon(inventory, weapon)) {
        return false;
    }
    inventory->selectedWeapon = weapon;
    return true;
}

bool Inventory_HasKeyItem(const Inventory *inventory, InventoryKeyItem item)
{
    uint32_t bit = KeyItemBit(item);
    return inventory != NULL && bit != 0 && (inventory->keyItems & bit) != 0;
}

bool Inventory_AddKeyItem(Inventory *inventory, InventoryKeyItem item)
{
    uint32_t bit = KeyItemBit(item);
    if (inventory == NULL || bit == 0) return false;
    inventory->keyItems |= bit;
    return true;
}

bool Inventory_IsValid(const Inventory *inventory)
{
    if (inventory == NULL) return false;
    const uint32_t weaponMask =
        (UINT32_C(1) << (INVENTORY_WEAPON_COUNT - 1)) - 1;
    const uint32_t keyItemMask =
        (UINT32_C(1) << INVENTORY_KEY_ITEM_COUNT) - 1;
    if ((inventory->ownedWeapons & ~weaponMask) != 0 ||
        (inventory->keyItems & ~keyItemMask) != 0) {
        return false;
    }
    if (inventory->selectedWeapon == INVENTORY_WEAPON_NONE) {
        return inventory->ownedWeapons == 0;
    }
    uint32_t selectedBit = WeaponBit(inventory->selectedWeapon);
    return selectedBit != 0 && (inventory->ownedWeapons & selectedBit) != 0;
}
