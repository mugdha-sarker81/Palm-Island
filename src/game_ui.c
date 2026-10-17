#include "game_ui.h"
#include "tuning.h"
#include <math.h>

static bool InteractionWasSeen(const InteractionState *state,
                               const RoomInteractable *target)
{
    switch (target->kind) {
        case ROOM_INTERACTABLE_ROOT_WEAVE: return state->rootWeaveSeen;
        case ROOM_INTERACTABLE_ROOT_MARKER: return state->rootMarkerActivated;
        case ROOM_INTERACTABLE_CLEARING_NOTE: return state->clearingNoteSeen;
        case ROOM_INTERACTABLE_ROOTS_MURAL: return state->rootsMuralSeen;
        case ROOM_INTERACTABLE_CRACKED_MINERAL_SEAL:
            return state->crackedMineralSealSeen;
        case ROOM_INTERACTABLE_COOLED_MINERAL_SEAM:
            return state->cooledMineralSeamSeen;
        case ROOM_INTERACTABLE_THORNBACK_TRIAL: return false;
        default: return false;
    }
}

void GameUI_DrawInteraction(const RoomInteractable *target, float promptFade,
                            const InteractionState *state)
{
    const Color panelColor = {11, 26, 34, 245};
    const Color edgeColor = {127, 196, 192, 255};

    if (state->panelOpen && target != NULL) {
        DrawRectangle(8, 124, 304, 48, Fade(panelColor, 0.96f));
        DrawRectangleLines(8, 124, 304, 48, edgeColor);
        DrawText(target->title, 14, 129, 10, (Color){245, 214, 124, 255});
        const char *text = target->kind == ROOM_INTERACTABLE_ROOT_MARKER &&
                           state->rootMarkerActivated &&
                           target->activatedText != NULL
                               ? target->activatedText : target->text;
        DrawText(text, 14, 142, 8, RAYWHITE);
        DrawText("E / A Close", 254, 161, 8,
                 (Color){127, 196, 192, 255});
    } else if (target != NULL && promptFade > 0.0f) {
        DrawRectangle(8, 154, 304, 18, Fade(panelColor, 0.94f * promptFade));
        DrawRectangleLines(8, 154, 304, 18,
                           Fade(edgeColor, promptFade));
        bool alreadyUsed = InteractionWasSeen(state, target);
        const char *action;
        if (target->kind == ROOM_INTERACTABLE_THORNBACK_TRIAL) {
            action = "Begin";
        } else if (target->kind == ROOM_INTERACTABLE_PALM_FRUIT) {
            action = "Collect";
        } else if (target->kind == ROOM_INTERACTABLE_ROOT_MARKER) {
            action = alreadyUsed ? "Review" : "Activate";
        } else {
            action = alreadyUsed ? "Review" : "Read";
        }
        DrawText(TextFormat("E / A %s %s", action, target->title),
                 14, 158, 8, Fade(RAYWHITE, promptFade));
    }
}

void GameUI_DrawInventory(const Inventory *inventory)
{
    const Color panelColor = {11, 26, 34, 250};
    const Color edgeColor = {127, 196, 192, 255};
    const Color headingColor = {245, 214, 124, 255};
    const Color mutedColor = {164, 184, 181, 255};
    const char *keyItemNames[INVENTORY_KEY_ITEM_COUNT] = {
        "Living Thorn Weave",
        "Root Marker Trace",
        "Explorer's Note",
        "Map of Roots",
        "Cracked Mineral Seal",
        "Cooled Mineral Seam"
    };

    DrawRectangle(12, 12, 296, 156, panelColor);
    DrawRectangleLines(12, 12, 296, 156, edgeColor);
    DrawText("INVENTORY", 20, 18, 12, headingColor);
    DrawText("WEAPONS", 20, 38, 8, headingColor);
    if (Inventory_HasWeapon(inventory,
                            INVENTORY_WEAPON_PALMWOOD_CLUB)) {
        DrawText(inventory->selectedWeapon ==
                         INVENTORY_WEAPON_PALMWOOD_CLUB
                     ? "> Palmwood Club  ACTIVE"
                     : "  Palmwood Club  OWNED",
                 24, 50, 8, RAYWHITE);
        DrawText("A simple strike for Ashfall foes", 32, 61, 8,
                 mutedColor);
    } else {
        DrawText("No starter weapon", 24, 50, 8, mutedColor);
    }
    if (Inventory_HasWeapon(inventory, INVENTORY_WEAPON_THORNFANG)) {
        DrawText(inventory->selectedWeapon == INVENTORY_WEAPON_THORNFANG
                     ? "> Thornfang  ACTIVE"
                     : "  Thornfang  OWNED",
                 24, 76, 8, RAYWHITE);
        DrawText("Cuts marked thorn barriers", 32, 87, 8, mutedColor);
    }
    DrawText(TextFormat("Palm fruit %d/%d  H / B to eat",
                        inventory->foodServings, FOOD_SERVING_CAPACITY),
             24, 105, 8, inventory->foodServings > 0 ? RAYWHITE : mutedColor);

    DrawText("KEY ITEMS / DISCOVERIES", 164, 38, 8, headingColor);
    for (int i = 0; i < INVENTORY_KEY_ITEM_COUNT; i++) {
        bool found = Inventory_HasKeyItem(inventory, (InventoryKeyItem)i);
        DrawText(TextFormat("%s %s", found ? "[x]" : "[ ]",
                            keyItemNames[i]),
                 168, 51 + i * 13, 8, found ? RAYWHITE : mutedColor);
    }
    DrawLine(20, 136, 300, 136, edgeColor);
    DrawText("I / TAB / Y  CLOSE", 20, 145, 8, RAYWHITE);
    DrawText("World paused", 226, 145, 8, mutedColor);
}

void GameUI_DrawHelp(void)
{
    const Color panelColor = {11, 26, 34, 250};
    const Color edgeColor = {127, 196, 192, 255};
    const Color headingColor = {245, 214, 124, 255};
    const Color mutedColor = {164, 184, 181, 255};
    const char *actions[] = {
        "Move",
        "Interact / close",
        "Strike / cut",
        "Eat Palm Fruit",
        "Inventory",
        "Help",
        "Debug overlay",
        "Campaign map",
        "Window mode",
        "Quit"
    };
    const char *keys[] = {
        "WASD / arrows",
        "E",
        "Space",
        "H",
        "I / Tab",
        "F1",
        "F3",
        "M",
        "F11",
        "Esc"
    };
    const char *controller[] = {
        "Left stick / D-pad",
        "A",
        "X",
        "B",
        "Y",
        "Start",
        "--",
        "View",
        "--",
        "--"
    };

    DrawRectangle(20, 16, 280, 148, Fade(panelColor, 0.98f));
    DrawRectangleLines(20, 16, 280, 148, edgeColor);
    DrawText("PALM ISLAND - CONTROLS", 32, 23, 10, headingColor);
    DrawText("ACTION", 32, 38, 8, mutedColor);
    DrawText("KEYBOARD", 132, 38, 8, mutedColor);
    DrawText("CONTROLLER", 216, 38, 8, mutedColor);
    for (int i = 0; i < (int)(sizeof(actions) / sizeof(actions[0])); i++) {
        int y = 50 + i * 9;
        DrawText(actions[i], 32, y, 8, RAYWHITE);
        DrawText(keys[i], 132, y, 8, RAYWHITE);
        DrawText(controller[i], 216, y, 8, RAYWHITE);
    }
    DrawText("WORLD PAUSED - F1 / START CLOSES", 32, 150, 8, headingColor);
}

void GameUI_DrawHealth(int health)
{
    static const unsigned char heartRows[] = {
        0x66, 0xFF, 0xFF, 0x7E, 0x3C, 0x18
    };
    const Color fullColor = {226, 91, 83, 255};
    const Color outlineColor = {183, 202, 194, 255};
    const Color emptyColor = {35, 53, 59, 255};

    for (int heart = 0; heart < PLAYER_START_HEALTH; heart++) {
        int x = 8 + heart * 18;
        for (int row = 0; row < (int)(sizeof(heartRows) /
                                      sizeof(heartRows[0])); row++) {
            for (int column = 0; column < 8; column++) {
                if ((heartRows[row] & (1u << (7 - column))) == 0) continue;
                bool hasAbove = row > 0 &&
                    (heartRows[row - 1] & (1u << (7 - column))) != 0;
                bool hasBelow = row + 1 < (int)(sizeof(heartRows) /
                                                sizeof(heartRows[0])) &&
                    (heartRows[row + 1] & (1u << (7 - column))) != 0;
                bool hasLeft = column > 0 &&
                    (heartRows[row] & (1u << (8 - column))) != 0;
                bool hasRight = column < 7 &&
                    (heartRows[row] & (1u << (6 - column))) != 0;
                bool edge = !hasAbove || !hasBelow || !hasLeft || !hasRight;
                Color color = heart < health
                                  ? fullColor
                                  : (edge ? outlineColor : emptyColor);
                DrawRectangle(x + column * 2, 8 + row * 2, 2, 2, color);
            }
        }
    }
    bool lowHealth = health <= PLAYER_LOW_HEALTH_THRESHOLD;
    DrawText(lowHealth
                 ? TextFormat("%d/%d LOW", health, PLAYER_START_HEALTH)
                 : TextFormat("%d/%d", health, PLAYER_START_HEALTH),
             102, 9, 8,
             lowHealth ? (Color){255, 192, 112, 255} : RAYWHITE);
}

void GameUI_DrawRootMarkerRoute(void)
{
    const Color routeColor = {100, 219, 143, 220};
    DrawLineEx((Vector2){216.0f, 144.0f}, (Vector2){216.0f, 56.0f},
               2.0f, routeColor);
    DrawLineEx((Vector2){216.0f, 56.0f}, (Vector2){320.0f, 56.0f},
               2.0f, routeColor);
    DrawLineEx((Vector2){320.0f, 56.0f}, (Vector2){320.0f, 24.0f},
               2.0f, routeColor);
}

void GameUI_DrawArenaTrialLocator(float worldTime)
{
    float pulse = 0.65f + 0.35f * sinf(worldTime * 4.0f);
    Color locatorColor = Fade((Color){255, 214, 112, 255}, pulse);
    DrawCircleLines(176, 198, 13.0f, locatorColor);
    DrawCircleLines(176, 198, 18.0f, Fade(locatorColor, 0.65f));
    DrawLineEx((Vector2){176.0f, 153.0f}, (Vector2){176.0f, 164.0f},
               2.0f, locatorColor);
    DrawTriangle((Vector2){171.0f, 162.0f}, (Vector2){181.0f, 162.0f},
                 (Vector2){176.0f, 168.0f}, locatorColor);
    DrawText("TRIAL", 158, 143, 8, locatorColor);
}

void GameUI_DrawAshfallSeal(float worldTime, bool discovered)
{
    float pulse = discovered ? 0.45f : 0.65f + 0.35f * sinf(worldTime * 3.0f);
    Color sealColor = Fade((Color){255, 181, 83, 255}, pulse);
    DrawCircleLines(320, 152, 10.0f, Fade(sealColor, 0.55f));
    DrawLineEx((Vector2){316.0f, 145.0f}, (Vector2){319.0f, 151.0f},
               2.0f, sealColor);
    DrawLineEx((Vector2){319.0f, 151.0f}, (Vector2){316.0f, 157.0f},
               2.0f, sealColor);
    DrawLineEx((Vector2){320.0f, 151.0f}, (Vector2){324.0f, 155.0f},
               1.0f, sealColor);
}

void GameUI_DrawCooledMineralSeam(float worldTime, bool discovered)
{
    float pulse = discovered ? 0.42f : 0.55f + 0.25f * sinf(worldTime * 2.5f);
    Color seamColor = Fade((Color){126, 205, 211, 255}, pulse);
    DrawCircleLines(152, 232, 8.0f, Fade(seamColor, 0.6f));
    DrawLineEx((Vector2){148.0f, 231.0f}, (Vector2){152.0f, 235.0f},
               2.0f, seamColor);
    DrawLineEx((Vector2){152.0f, 235.0f}, (Vector2){156.0f, 231.0f},
               2.0f, seamColor);
    DrawLineEx((Vector2){152.0f, 235.0f}, (Vector2){152.0f, 238.0f},
               1.0f, seamColor);
}
