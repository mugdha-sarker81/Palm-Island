#include "interaction.h"

void InteractionState_Activate(InteractionState *state,
                               const RoomInteractable *target)
{
    if (state->panelOpen) {
        state->panelOpen = false;
        return;
    }
    state->panelOpen = true;
    if (target->kind == ROOM_INTERACTABLE_ROOT_WEAVE) {
        state->rootWeaveSeen = true;
    }
    if (target->kind == ROOM_INTERACTABLE_ROOT_MARKER) {
        state->rootMarkerActivated = true;
    }
    if (target->kind == ROOM_INTERACTABLE_CLEARING_NOTE) {
        state->clearingNoteSeen = true;
    }
    if (target->kind == ROOM_INTERACTABLE_ROOTS_MURAL) {
        state->rootsMuralSeen = true;
    }
}

void InteractionState_Close(InteractionState *state)
{
    state->panelOpen = false;
}
