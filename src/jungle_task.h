#ifndef PALM_JUNGLE_TASK_H
#define PALM_JUNGLE_TASK_H

#include <stdbool.h>

typedef struct JungleTaskProgress {
    bool rootWeaveSeen;
    bool rootMarkerActivated;
    bool rootsMuralSeen;
    bool thornfangAcquired;
    bool thornGateOpen;
    bool returnedToJungleEntrance;
} JungleTaskProgress;

const char *JungleTask_CurrentObjective(JungleTaskProgress progress);

#endif
