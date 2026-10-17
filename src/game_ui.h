#ifndef PALM_GAME_UI_H
#define PALM_GAME_UI_H

#include "interaction.h"
#include "inventory.h"
#include "room.h"

void GameUI_DrawInteraction(const RoomInteractable *target, float promptFade,
                            const InteractionState *state);
void GameUI_DrawInventory(const Inventory *inventory);
void GameUI_DrawHelp(void);
void GameUI_DrawHealth(int health);
void GameUI_DrawRootMarkerRoute(void);
void GameUI_DrawArenaTrialLocator(float worldTime);
void GameUI_DrawAshfallSeal(float worldTime, bool discovered);
void GameUI_DrawCooledMineralSeam(float worldTime, bool discovered);

#endif
