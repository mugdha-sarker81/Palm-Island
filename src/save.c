#include "save.h"
#include "tuning.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

static bool SaveData_IsValid(const SaveData *data)
{
    return data != NULL &&
           data->checkpointArea >= ROOM_AREA_JUNGLE_ENTRANCE &&
           data->checkpointArea <= ROOM_AREA_THORNBACK_ARENA &&
           data->health >= 1 && data->health <= PLAYER_START_HEALTH &&
           Inventory_IsValid(&data->inventory) &&
           (data->collectedFoodNodes & ~ROOM_FOOD_NODE_ALL) == 0 &&
           (!data->thornGateOpen ||
            Inventory_HasWeapon(&data->inventory,
                                INVENTORY_WEAPON_THORNFANG));
}

static bool ReadBoolean(FILE *file, const char *expectedKey, bool *value)
{
    char key[32];
    int parsed;
    if (fscanf(file, "%31s %d", key, &parsed) != 2 ||
        strcmp(key, expectedKey) != 0 || (parsed != 0 && parsed != 1)) {
        return false;
    }
    *value = parsed != 0;
    return true;
}

static SaveLoadResult ReadSaveFile(const char *path, SaveData *data)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return errno == ENOENT ? SAVE_LOAD_MISSING : SAVE_LOAD_IO_ERROR;
    }

    char magic[32];
    int version;
    SaveData parsed = {0};
    bool valid = fscanf(file, "%31s %d", magic, &version) == 2 &&
                 strcmp(magic, "PALM_ISLAND_SAVE") == 0 &&
                 (version >= 1 && version <= 4);
    char checkpointKey[32];
    int checkpoint = 0;
    if (valid) {
        valid = fscanf(file, "%31s %d", checkpointKey, &checkpoint) == 2 &&
                strcmp(checkpointKey, "checkpoint") == 0;
        parsed.checkpointArea = (RoomArea)checkpoint;
    }
    if (valid) {
        char healthKey[32];
        valid = fscanf(file, "%31s %d", healthKey, &parsed.health) == 2 &&
                strcmp(healthKey, "health") == 0;
    }
    if (valid && version == 1) {
        bool rootWeaveSeen = false;
        bool rootMarkerActivated = false;
        bool clearingNoteSeen = false;
        bool rootsMuralSeen = false;
        bool thornfangAcquired = false;
        valid = ReadBoolean(file, "root_weave_seen", &rootWeaveSeen) &&
                ReadBoolean(file, "root_marker_activated",
                            &rootMarkerActivated) &&
                ReadBoolean(file, "clearing_note_seen",
                            &clearingNoteSeen) &&
                ReadBoolean(file, "roots_mural_seen", &rootsMuralSeen) &&
                ReadBoolean(file, "thornfang_acquired",
                            &thornfangAcquired) &&
                ReadBoolean(file, "thorn_gate_open",
                            &parsed.thornGateOpen);
        if (valid) {
            if (rootWeaveSeen) {
                Inventory_AddKeyItem(&parsed.inventory,
                                     INVENTORY_KEY_ITEM_ROOT_WEAVE);
            }
            if (rootMarkerActivated) {
                Inventory_AddKeyItem(
                    &parsed.inventory, INVENTORY_KEY_ITEM_ROOT_MARKER_TRACE);
            }
            if (clearingNoteSeen) {
                Inventory_AddKeyItem(
                    &parsed.inventory, INVENTORY_KEY_ITEM_CLEARING_NOTE);
            }
            if (rootsMuralSeen) {
                Inventory_AddKeyItem(&parsed.inventory,
                                     INVENTORY_KEY_ITEM_ROOTS_MURAL);
            }
            if (thornfangAcquired) {
                Inventory_AddWeapon(&parsed.inventory,
                                    INVENTORY_WEAPON_THORNFANG);
            }
        }
    } else if (valid) {
        int ownedWeapons = 0;
        int selectedWeapon = 0;
        int keyItems = 0;
        valid = fscanf(file, " %31s %d", checkpointKey, &ownedWeapons) == 2 &&
                strcmp(checkpointKey, "weapons_owned") == 0 &&
                fscanf(file, " %31s %d", checkpointKey, &selectedWeapon) == 2 &&
                strcmp(checkpointKey, "weapon_selected") == 0 &&
                fscanf(file, " %31s %d", checkpointKey, &keyItems) == 2 &&
                strcmp(checkpointKey, "key_items") == 0 &&
                ownedWeapons >= 0 && selectedWeapon >= 0 && keyItems >= 0;
        if (valid) {
            parsed.inventory.ownedWeapons = (uint32_t)ownedWeapons;
            parsed.inventory.selectedWeapon =
                (InventoryWeapon)selectedWeapon;
            parsed.inventory.keyItems = (uint32_t)keyItems;
            if (version >= 3) {
                valid = fscanf(file, " %31s %d", checkpointKey,
                               &parsed.inventory.foodServings) == 2 &&
                        strcmp(checkpointKey, "food_servings") == 0;
                if (valid) {
                    if (version == 3) {
                        bool palmFruitCollected = false;
                        valid = ReadBoolean(file, "palm_fruit_collected",
                                            &palmFruitCollected);
                        if (valid && palmFruitCollected) {
                            parsed.collectedFoodNodes =
                                ROOM_FOOD_NODE_CLEARING;
                        }
                    } else {
                        unsigned int collectedFoodNodes = 0;
                        valid = fscanf(file, " %31s %u", checkpointKey,
                                       &collectedFoodNodes) == 2 &&
                                strcmp(checkpointKey,
                                       "food_nodes_collected") == 0;
                        if (valid) {
                            parsed.collectedFoodNodes =
                                (uint32_t)collectedFoodNodes;
                        }
                    }
                }
            }
        }
        if (valid) {
            valid = ReadBoolean(file, "thorn_gate_open",
                                &parsed.thornGateOpen);
        }
    }
    if (valid) {
        char trailing;
        valid = fscanf(file, " %c", &trailing) != 1;
    }
    bool readError = ferror(file) != 0;
    if (fclose(file) != 0 || readError) {
        return SAVE_LOAD_IO_ERROR;
    }
    if (!valid || !SaveData_IsValid(&parsed)) return SAVE_LOAD_INVALID;
    *data = parsed;
    return SAVE_LOAD_LOADED;
}

static bool CopySaveFile(const char *sourcePath, const char *destinationPath)
{
    FILE *source = fopen(sourcePath, "rb");
    if (source == NULL) return false;
    FILE *destination = fopen(destinationPath, "wb");
    if (destination == NULL) {
        fclose(source);
        return false;
    }

    bool copied = true;
    char buffer[4096];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        if (fwrite(buffer, 1, bytesRead, destination) != bytesRead) {
            copied = false;
            break;
        }
    }
    if (ferror(source)) copied = false;
    if (fclose(source) != 0) copied = false;
    if (fclose(destination) != 0) copied = false;
    return copied;
}

static bool ReplaceSaveFile(const char *temporaryPath, const char *path)
{
#ifdef _WIN32
    if (remove(path) != 0 && errno != ENOENT) return false;
#endif
    return rename(temporaryPath, path) == 0;
}

static bool MakeSidecarPath(char *output, size_t capacity, const char *path,
                            const char *suffix)
{
    int length = snprintf(output, capacity, "%s%s", path, suffix);
    return length >= 0 && (size_t)length < capacity;
}

static bool PromoteBackup(const char *path, const char *backupPath)
{
    char temporaryPath[1024];
    if (!MakeSidecarPath(temporaryPath, sizeof(temporaryPath), path, ".tmp")) {
        return false;
    }
    if (!CopySaveFile(backupPath, temporaryPath)) {
        remove(temporaryPath);
        return false;
    }
    if (!ReplaceSaveFile(temporaryPath, path)) {
        remove(temporaryPath);
        return false;
    }
    return true;
}

SaveLoadResult Save_Load(const char *path, SaveData *data)
{
    if (path == NULL || path[0] == '\0' || data == NULL) {
        return SAVE_LOAD_INVALID;
    }

    SaveData parsed;
    SaveLoadResult primary = ReadSaveFile(path, &parsed);
    if (primary == SAVE_LOAD_LOADED) {
        *data = parsed;
        return SAVE_LOAD_LOADED;
    }
    char backupPath[1024];
    if (!MakeSidecarPath(backupPath, sizeof(backupPath), path, ".bak")) {
        return SAVE_LOAD_IO_ERROR;
    }
    SaveData backup;
    SaveLoadResult backupResult = ReadSaveFile(backupPath, &backup);
    if (backupResult == SAVE_LOAD_LOADED) {
        if (!PromoteBackup(path, backupPath)) return SAVE_LOAD_IO_ERROR;
        *data = backup;
        return SAVE_LOAD_RECOVERED_BACKUP;
    }
    if (primary == SAVE_LOAD_MISSING &&
        backupResult == SAVE_LOAD_MISSING) {
        return SAVE_LOAD_MISSING;
    }
    if (primary == SAVE_LOAD_IO_ERROR || backupResult == SAVE_LOAD_IO_ERROR) {
        return SAVE_LOAD_IO_ERROR;
    }
    return SAVE_LOAD_INVALID;
}

SaveWriteResult Save_Write(const char *path, const SaveData *data)
{
    if (path == NULL || path[0] == '\0' || !SaveData_IsValid(data)) {
        return SAVE_WRITE_INVALID_DATA;
    }

    char temporaryPath[1024];
    char backupPath[1024];
    if (!MakeSidecarPath(temporaryPath, sizeof(temporaryPath), path, ".tmp") ||
        !MakeSidecarPath(backupPath, sizeof(backupPath), path, ".bak")) {
        return SAVE_WRITE_IO_ERROR;
    }

    FILE *file = fopen(temporaryPath, "w");
    if (file == NULL) return SAVE_WRITE_IO_ERROR;
    int written = fprintf(
        file,
        "PALM_ISLAND_SAVE 4\n"
        "checkpoint %d\n"
        "health %d\n"
        "weapons_owned %u\n"
        "weapon_selected %d\n"
        "key_items %u\n"
        "food_servings %d\n"
        "food_nodes_collected %u\n"
        "thorn_gate_open %d\n",
        (int)data->checkpointArea, data->health,
        (unsigned int)data->inventory.ownedWeapons,
        (int)data->inventory.selectedWeapon,
        (unsigned int)data->inventory.keyItems,
        data->inventory.foodServings,
        (unsigned int)data->collectedFoodNodes,
        data->thornGateOpen);
    bool writeSucceeded = written >= 0 && fflush(file) == 0;
    if (fclose(file) != 0) writeSucceeded = false;
    if (!writeSucceeded) {
        remove(temporaryPath);
        return SAVE_WRITE_IO_ERROR;
    }

    FILE *existing = fopen(path, "rb");
    if (existing != NULL) {
        bool closed = fclose(existing) == 0;
        if (!closed || !CopySaveFile(path, backupPath)) {
            remove(temporaryPath);
            return SAVE_WRITE_IO_ERROR;
        }
    } else if (errno != ENOENT) {
        remove(temporaryPath);
        return SAVE_WRITE_IO_ERROR;
    }

    if (!ReplaceSaveFile(temporaryPath, path)) {
        remove(temporaryPath);
        return SAVE_WRITE_IO_ERROR;
    }
    return SAVE_WRITE_OK;
}
