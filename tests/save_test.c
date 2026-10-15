#include "save.h"
#include "tuning.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int failures;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static bool MakeTestPath(char *path, size_t capacity)
{
    time_t now = time(NULL);
    for (int attempt = 0; attempt < 100; attempt++) {
        int length = snprintf(path, capacity,
                             ".palm_island_save_test_%ld_%d.sav",
                             (long)now, attempt);
        if (length < 0 || (size_t)length >= capacity) return false;
        FILE *existing = fopen(path, "rb");
        if (existing == NULL && errno == ENOENT) return true;
        if (existing != NULL) fclose(existing);
    }
    return false;
}

static void RemoveSaveFiles(const char *path)
{
    char sidecar[256];
    remove(path);
    snprintf(sidecar, sizeof(sidecar), "%s.bak", path);
    remove(sidecar);
    snprintf(sidecar, sizeof(sidecar), "%s.tmp", path);
    remove(sidecar);
}

static bool SaveDataMatches(const SaveData *left, const SaveData *right)
{
    return left->checkpointArea == right->checkpointArea &&
           left->health == right->health &&
           left->inventory.ownedWeapons ==
               right->inventory.ownedWeapons &&
           left->inventory.selectedWeapon ==
               right->inventory.selectedWeapon &&
           left->inventory.keyItems == right->inventory.keyItems &&
           left->inventory.foodServings ==
               right->inventory.foodServings &&
           left->thornGateOpen == right->thornGateOpen &&
           left->palmFruitCollected == right->palmFruitCollected;
}

static void TestMissingAndVersionedRoundTrip(const char *path)
{
    SaveData loaded = {0};
    CHECK(Save_Load(path, &loaded) == SAVE_LOAD_MISSING);

    SaveData expected = {
        .checkpointArea = ROOM_AREA_SHRINE_APPROACH,
        .health = PLAYER_START_HEALTH - 2,
        .inventory = {
            .ownedWeapons = UINT32_C(1),
            .selectedWeapon = INVENTORY_WEAPON_THORNFANG,
            .keyItems = (UINT32_C(1) << INVENTORY_KEY_ITEM_COUNT) - 1,
            .foodServings = 2
        },
        .thornGateOpen = true,
        .palmFruitCollected = true
    };
    CHECK(Save_Write(path, &expected) == SAVE_WRITE_OK);
    CHECK(Save_Load(path, &loaded) == SAVE_LOAD_LOADED);
    CHECK(SaveDataMatches(&loaded, &expected));
}

static void TestVersionOneMigration(const char *path)
{
    FILE *file = fopen(path, "w");
    CHECK(file != NULL);
    if (file == NULL) return;
    fputs("PALM_ISLAND_SAVE 1\n"
          "checkpoint 3\n"
          "health 4\n"
          "root_weave_seen 1\n"
          "root_marker_activated 1\n"
          "clearing_note_seen 0\n"
          "roots_mural_seen 1\n"
          "thornfang_acquired 1\n"
          "thorn_gate_open 1\n", file);
    CHECK(fclose(file) == 0);

    SaveData loaded = {0};
    CHECK(Save_Load(path, &loaded) == SAVE_LOAD_LOADED);
    CHECK(loaded.checkpointArea == ROOM_AREA_SHRINE_APPROACH);
    CHECK(loaded.health == 4);
    CHECK(Inventory_HasWeapon(&loaded.inventory,
                              INVENTORY_WEAPON_THORNFANG));
    CHECK(loaded.inventory.selectedWeapon == INVENTORY_WEAPON_THORNFANG);
    CHECK(Inventory_HasKeyItem(&loaded.inventory,
                               INVENTORY_KEY_ITEM_ROOT_WEAVE));
    CHECK(Inventory_HasKeyItem(&loaded.inventory,
                               INVENTORY_KEY_ITEM_ROOT_MARKER_TRACE));
    CHECK(!Inventory_HasKeyItem(&loaded.inventory,
                                INVENTORY_KEY_ITEM_CLEARING_NOTE));
    CHECK(Inventory_HasKeyItem(&loaded.inventory,
                               INVENTORY_KEY_ITEM_ROOTS_MURAL));
    CHECK(loaded.thornGateOpen);
}

static void TestVersionTwoMigration(const char *path)
{
    FILE *file = fopen(path, "w");
    CHECK(file != NULL);
    if (file == NULL) return;
    fputs("PALM_ISLAND_SAVE 2\n"
          "checkpoint 3\n"
          "health 4\n"
          "weapons_owned 1\n"
          "weapon_selected 1\n"
          "key_items 15\n"
          "thorn_gate_open 1\n", file);
    CHECK(fclose(file) == 0);

    SaveData loaded = {0};
    CHECK(Save_Load(path, &loaded) == SAVE_LOAD_LOADED);
    CHECK(loaded.inventory.foodServings == 0);
    CHECK(!loaded.palmFruitCollected);
    CHECK(Inventory_HasWeapon(&loaded.inventory,
                              INVENTORY_WEAPON_THORNFANG));
    CHECK(loaded.thornGateOpen);
}

static void TestInvalidSaveDoesNotModifyOutput(const char *path)
{
    FILE *file = fopen(path, "w");
    CHECK(file != NULL);
    if (file == NULL) return;
    fputs("PALM_ISLAND_SAVE 99\n", file);
    CHECK(fclose(file) == 0);

    SaveData loaded = {.checkpointArea = ROOM_AREA_ROOT_PATH, .health = 3};
    SaveData unchanged = loaded;
    CHECK(Save_Load(path, &loaded) == SAVE_LOAD_INVALID);
    CHECK(SaveDataMatches(&loaded, &unchanged));
}

static void TestBackupRecovery(const char *path)
{
    SaveData first = {
        .checkpointArea = ROOM_AREA_ROOT_PATH,
        .health = 4,
        .inventory = {
            .keyItems = UINT32_C(1) << INVENTORY_KEY_ITEM_ROOT_WEAVE
        }
    };
    SaveData second = {
        .checkpointArea = ROOM_AREA_THORNBACK_ARENA,
        .health = 2,
        .inventory = {
            .ownedWeapons = UINT32_C(1),
            .selectedWeapon = INVENTORY_WEAPON_THORNFANG
        }
    };
    CHECK(Save_Write(path, &first) == SAVE_WRITE_OK);
    CHECK(Save_Write(path, &second) == SAVE_WRITE_OK);

    FILE *file = fopen(path, "w");
    CHECK(file != NULL);
    if (file == NULL) return;
    fputs("corrupt save\n", file);
    CHECK(fclose(file) == 0);

    SaveData loaded = {0};
    CHECK(Save_Load(path, &loaded) == SAVE_LOAD_RECOVERED_BACKUP);
    CHECK(SaveDataMatches(&loaded, &first));
    CHECK(Save_Load(path, &loaded) == SAVE_LOAD_LOADED);
    CHECK(SaveDataMatches(&loaded, &first));

    CHECK(Save_Write(path, &second) == SAVE_WRITE_OK);
    CHECK(remove(path) == 0);
    CHECK(Save_Load(path, &loaded) == SAVE_LOAD_RECOVERED_BACKUP);
    CHECK(SaveDataMatches(&loaded, &first));
}

static void TestInvalidDataAndWriteFailure(const char *path)
{
    SaveData invalid = {
        .checkpointArea = ROOM_AREA_ROOT_PATH,
        .health = 0
    };
    CHECK(Save_Write(path, &invalid) == SAVE_WRITE_INVALID_DATA);

    SaveData valid = {
        .checkpointArea = ROOM_AREA_JUNGLE_ENTRANCE,
        .health = PLAYER_START_HEALTH
    };
    CHECK(Save_Write("missing_directory/palm_island.sav", &valid) ==
          SAVE_WRITE_IO_ERROR);
}

int main(void)
{
    char path[256];
    if (!MakeTestPath(path, sizeof(path))) {
        fputs("Could not find a unique temporary save-test path.\n", stderr);
        return 1;
    }

    TestMissingAndVersionedRoundTrip(path);
    RemoveSaveFiles(path);
    TestVersionOneMigration(path);
    RemoveSaveFiles(path);
    TestVersionTwoMigration(path);
    RemoveSaveFiles(path);
    TestInvalidSaveDoesNotModifyOutput(path);
    RemoveSaveFiles(path);
    TestBackupRecovery(path);
    RemoveSaveFiles(path);
    TestInvalidDataAndWriteFailure(path);
    RemoveSaveFiles(path);

    if (failures != 0) {
        fprintf(stderr, "%d save check(s) failed.\n", failures);
        return 1;
    }
    puts("All save/load checks passed.");
    return 0;
}
