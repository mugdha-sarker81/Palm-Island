#include "jungle_task.h"

const char *JungleTask_CurrentObjective(JungleTaskProgress progress)
{
    if (!progress.rootWeaveSeen) return "INSPECT THE ROOT MARKINGS";
    if (!progress.rootMarkerActivated) return "ACTIVATE THE ROOT MARKER";
    if (!progress.rootsMuralSeen) return "READ THE MAP OF ROOTS";
    if (!progress.thornfangAcquired) return "FACE THORNBACK'S TRIAL";
    if (!progress.thornGateOpen) return "CUT THE THORN GATE";
    if (!progress.returnedToJungleEntrance) {
        return "RETURN TO THE JUNGLE ENTRANCE";
    }
    return "THE ROOTS REMEMBER - COMPLETE";
}
