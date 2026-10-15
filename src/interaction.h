#ifndef PALM_INTERACTION_H
#define PALM_INTERACTION_H

#include "room.h"
#include <stdbool.h>

typedef struct InteractionState {
    bool panelOpen;
    bool rootWeaveSeen;
    bool rootMarkerActivated;
    bool clearingNoteSeen;
    bool rootsMuralSeen;
    bool crackedMineralSealSeen;
    bool cooledMineralSeamSeen;
} InteractionState;

void InteractionState_Activate(InteractionState *state,
                               const RoomInteractable *target);
void InteractionState_Close(InteractionState *state);

#endif
