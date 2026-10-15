#ifndef PALM_SAVE_H
#define PALM_SAVE_H

#include "inventory.h"
#include "room.h"
#include <stdbool.h>

typedef struct SaveData {
    RoomArea checkpointArea;
    int health;
    Inventory inventory;
    bool thornGateOpen;
    uint32_t collectedFoodNodes;
} SaveData;

typedef enum SaveLoadResult {
    SAVE_LOAD_MISSING,
    SAVE_LOAD_LOADED,
    SAVE_LOAD_RECOVERED_BACKUP,
    SAVE_LOAD_INVALID,
    SAVE_LOAD_IO_ERROR
} SaveLoadResult;

typedef enum SaveWriteResult {
    SAVE_WRITE_OK,
    SAVE_WRITE_INVALID_DATA,
    SAVE_WRITE_IO_ERROR
} SaveWriteResult;

SaveLoadResult Save_Load(const char *path, SaveData *data);
SaveWriteResult Save_Write(const char *path, const SaveData *data);

#endif
