#include "jungle_task.h"
#include <stdio.h>
#include <string.h>

static int failures;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static void CheckObjective(JungleTaskProgress progress, const char *expected)
{
    CHECK(strcmp(JungleTask_CurrentObjective(progress), expected) == 0);
}

int main(void)
{
    JungleTaskProgress progress = {0};
    CheckObjective(progress, "INSPECT THE ROOT MARKINGS");

    progress.rootWeaveSeen = true;
    CheckObjective(progress, "ACTIVATE THE ROOT MARKER");

    progress.rootMarkerActivated = true;
    CheckObjective(progress, "READ THE MAP OF ROOTS");

    progress.rootsMuralSeen = true;
    CheckObjective(progress, "FACE THORNBACK'S TRIAL");

    progress.thornfangAcquired = true;
    CheckObjective(progress, "CUT THE THORN GATE");

    progress.thornGateOpen = true;
    CheckObjective(progress, "RETURN TO THE JUNGLE ENTRANCE");

    progress.returnedToJungleEntrance = true;
    CheckObjective(progress, "THE ROOTS REMEMBER - COMPLETE");

    if (failures != 0) {
        fprintf(stderr, "%d Jungle task check(s) failed.\n", failures);
        return 1;
    }
    puts("All Jungle task checks passed.");
    return 0;
}
