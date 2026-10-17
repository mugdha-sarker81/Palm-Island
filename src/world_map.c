#include "world_map.h"
#include "tuning.h"

#include <stdio.h>
#include <string.h>

#define EXIT_TO(zone, entry, gate) \
    {(unsigned char)(zone), (unsigned char)(entry), (unsigned char)(gate)}

const Region g_regions[REGION_COUNT] = {
    [R_PALMSHORE_BEACH] = {"Palmshore Beach", "sand, palms, surf"},
    [R_DRIFTWOOD_VILLAGE] = {"Driftwood Village", "lamplight, timber"},
    [R_FORGOTTEN_JUNGLE] = {"Forgotten Jungle", "roots, ferns, canopy"},
    [R_EMBER_CAVERNS] = {"Ember Caverns", "ash, vents, emberlight"},
    [R_SUNKEN_TEMPLE] = {"Sunken Temple", "flooded halls, reflections"},
    [R_SKYWATCH_CLIFFS] = {"Skywatch Cliffs", "wind, bridges, clouds"},
    [R_LOST_KINGDOM] = {"Lost Kingdom", "archives, courtyards, shadows"},
    [R_EXCALIBUR_SANCTUARY] = {"Excalibur Sanctuary", "memory, quiet stone"}
};

const Zone g_zones[ZONE_COUNT] = {
    [Z_WRECKSHORE] = {
        .name = "Wreckshore", .region = R_PALMSHORE_BEACH,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_TIDEPOOL_PATH, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = NO_EXIT,
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_TIDEPOOL_PATH] = {
        .name = "Tidepool Path", .region = R_PALMSHORE_BEACH,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_VILLAGE_SQUARE, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_WRECKSHORE, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_VILLAGE_SQUARE] = {
        .name = "Village Square", .region = R_DRIFTWOOD_VILLAGE,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_JUNGLE_ENTRANCE, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = EXIT_TO(Z_HARBOR_ROW, EDGE_WEST, GATE_NONE),
            [EDGE_SOUTH] = EXIT_TO(Z_TIDEPOOL_PATH, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_HARBOR_ROW] = {
        .name = "Harbor Row", .region = R_DRIFTWOOD_VILLAGE,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_ASHFALL_DESCENT, EDGE_SOUTH,
                                   GATE_THORNFANG),
            [EDGE_EAST] = EXIT_TO(Z_FLOODED_COURT, EDGE_WEST,
                                  GATE_THORNFANG),
            [EDGE_SOUTH] = EXIT_TO(Z_WINDWARD_STEPS, EDGE_WEST,
                                   GATE_THORNFANG),
            [EDGE_WEST] = EXIT_TO(Z_VILLAGE_SQUARE, EDGE_EAST, GATE_NONE)
        }
    },
    [Z_JUNGLE_ENTRANCE] = {
        .name = "Jungle Entrance", .region = R_FORGOTTEN_JUNGLE,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_ROOT_PATH, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_VILLAGE_SQUARE, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_ROOT_PATH] = {
        .name = "Root Path", .region = R_FORGOTTEN_JUNGLE,
        .checkpoint = false,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_OVERGROWN_CLEARING, EDGE_SOUTH,
                                   GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_JUNGLE_ENTRANCE, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_OVERGROWN_CLEARING] = {
        .name = "Overgrown Clearing", .region = R_FORGOTTEN_JUNGLE,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_SHRINE_APPROACH, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_ROOT_PATH, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_SHRINE_APPROACH] = {
        .name = "Shrine Approach and Agility Shrine",
        .region = R_FORGOTTEN_JUNGLE, .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = NO_EXIT,
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_OVERGROWN_CLEARING, EDGE_NORTH,
                                   GATE_NONE),
            [EDGE_WEST] = EXIT_TO(Z_OUTER_ARCHIVE, EDGE_EAST,
                                  GATE_THORNFANG)
        }
    },
    [Z_ASHFALL_DESCENT] = {
        .name = "Ashfall Descent", .region = R_EMBER_CAVERNS,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_FORGE_VEINS, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_HARBOR_ROW, EDGE_NORTH,
                                   GATE_THORNFANG),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_FORGE_VEINS] = {
        .name = "Forge Veins", .region = R_EMBER_CAVERNS,
        .checkpoint = false,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_BURIED_FORGE, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_ASHFALL_DESCENT, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_BURIED_FORGE] = {
        .name = "Buried Forge", .region = R_EMBER_CAVERNS,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = NO_EXIT,
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_FORGE_VEINS, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_FLOODED_COURT] = {
        .name = "Flooded Court", .region = R_SUNKEN_TEMPLE,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_RESONANCE_HALL, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = NO_EXIT,
            [EDGE_WEST] = EXIT_TO(Z_HARBOR_ROW, EDGE_EAST, GATE_THORNFANG)
        }
    },
    [Z_RESONANCE_HALL] = {
        .name = "Resonance Hall", .region = R_SUNKEN_TEMPLE,
        .checkpoint = false,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_TIDE_ENGINE, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_FLOODED_COURT, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_TIDE_ENGINE] = {
        .name = "Tide Engine", .region = R_SUNKEN_TEMPLE,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = NO_EXIT,
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_RESONANCE_HALL, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_WINDWARD_STEPS] = {
        .name = "Windward Steps", .region = R_SKYWATCH_CLIFFS,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_MIRROR_WALK, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = NO_EXIT,
            [EDGE_WEST] = EXIT_TO(Z_HARBOR_ROW, EDGE_SOUTH,
                                  GATE_THORNFANG)
        }
    },
    [Z_MIRROR_WALK] = {
        .name = "Mirror Walk", .region = R_SKYWATCH_CLIFFS,
        .checkpoint = false,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_SIGNAL_NEST, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_WINDWARD_STEPS, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_SIGNAL_NEST] = {
        .name = "Signal Nest", .region = R_SKYWATCH_CLIFFS,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = NO_EXIT,
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_MIRROR_WALK, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_OUTER_ARCHIVE] = {
        .name = "Outer Archive", .region = R_LOST_KINGDOM,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_SILENT_COURTYARD, EDGE_SOUTH,
                                  GATE_NONE),
            [EDGE_EAST] = EXIT_TO(Z_SHRINE_APPROACH, EDGE_WEST,
                                  GATE_THORNFANG),
            [EDGE_SOUTH] = NO_EXIT,
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_SILENT_COURTYARD] = {
        .name = "Silent Courtyard", .region = R_LOST_KINGDOM,
        .checkpoint = false,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_MIRROR_GALLERY_SHADOW_CAUSEWAY,
                                  EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_OUTER_ARCHIVE, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_MIRROR_GALLERY_SHADOW_CAUSEWAY] = {
        .name = "Mirror Gallery and Shadow Causeway",
        .region = R_LOST_KINGDOM, .checkpoint = false,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_HOLLOW_HALL, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_SILENT_COURTYARD, EDGE_NORTH,
                                   GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_HOLLOW_HALL] = {
        .name = "Hollow Hall", .region = R_LOST_KINGDOM,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_OUTER_THRESHOLD, EDGE_SOUTH,
                                   GATE_STORY_FINAL),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_MIRROR_GALLERY_SHADOW_CAUSEWAY,
                                   EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_OUTER_THRESHOLD] = {
        .name = "Outer Threshold", .region = R_EXCALIBUR_SANCTUARY,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = NO_EXIT,
            [EDGE_EAST] = EXIT_TO(Z_MEMORY_GALLERY, EDGE_WEST, GATE_NONE),
            [EDGE_SOUTH] = EXIT_TO(Z_HOLLOW_HALL, EDGE_NORTH,
                                   GATE_STORY_FINAL),
            [EDGE_WEST] = NO_EXIT
        }
    },
    [Z_MEMORY_GALLERY] = {
        .name = "Memory Gallery", .region = R_EXCALIBUR_SANCTUARY,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = EXIT_TO(Z_QUIET_CORE, EDGE_SOUTH, GATE_NONE),
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = NO_EXIT,
            [EDGE_WEST] = EXIT_TO(Z_OUTER_THRESHOLD, EDGE_EAST, GATE_NONE)
        }
    },
    [Z_QUIET_CORE] = {
        .name = "Quiet Core", .region = R_EXCALIBUR_SANCTUARY,
        .checkpoint = true,
        .exits = {
            [EDGE_NORTH] = NO_EXIT,
            [EDGE_EAST] = NO_EXIT,
            [EDGE_SOUTH] = EXIT_TO(Z_MEMORY_GALLERY, EDGE_NORTH, GATE_NONE),
            [EDGE_WEST] = NO_EXIT
        }
    }
};

static const unsigned int REGION_ZONE_COUNTS[REGION_COUNT] = {
    2, 2, 4, 3, 3, 3, 4, 3
};

const Zone *world_zone(int idx)
{
    return idx >= 0 && idx < ZONE_COUNT ? &g_zones[idx] : NULL;
}

const Region *world_region(int idx)
{
    return idx >= 0 && idx < REGION_COUNT ? &g_regions[idx] : NULL;
}

int world_zone_by_name(const char *name)
{
    if (name == NULL) return -1;
    for (int i = 0; i < ZONE_COUNT; i++) {
        if (strcmp(g_zones[i].name, name) == 0) return i;
    }
    return -1;
}

int world_region_first_zone(int region)
{
    if (region < 0 || region >= REGION_COUNT) return -1;
    for (int i = 0; i < ZONE_COUNT; i++) {
        if (g_zones[i].region == (unsigned char)region) return i;
    }
    return -1;
}

int world_region_zone_count(int region)
{
    if (region < 0 || region >= REGION_COUNT) return 0;
    int count = 0;
    for (int i = 0; i < ZONE_COUNT; i++) {
        if (g_zones[i].region == (unsigned char)region) count++;
    }
    return count;
}

bool world_zone_is_playable(int zone)
{
    switch (zone) {
        case Z_JUNGLE_ENTRANCE:
        case Z_ROOT_PATH:
        case Z_OVERGROWN_CLEARING:
        case Z_SHRINE_APPROACH:
        case Z_ASHFALL_DESCENT:
            return true;
        default:
            return false;
    }
}

unsigned int world_gate_requirement(GateType gate)
{
    switch (gate) {
        case GATE_THORNFANG: return WF_THORNFANG;
        case GATE_EMBER_LAMP: return WF_LANTERN;
        case GATE_ROOT_KEY: return WF_ROOT_KEY;
        case GATE_TIDE: return WF_TIDE_LOW;
        case GATE_STORY_FINAL: return WF_STORY_FINAL;
        case GATE_NONE:
        default: return 0u;
    }
}

const char *world_gate_locked_msg(GateType gate)
{
    switch (gate) {
        case GATE_THORNFANG:
            return "Marked vines block the way. Thornfang could cut them.";
        case GATE_EMBER_LAMP:
            return "The dark ahead swallows your light.";
        case GATE_ROOT_KEY:
            return "A root-shaped key is needed to open the sealed door.";
        case GATE_TIDE:
            return "The water is too high to pass.";
        case GATE_STORY_FINAL:
            return "The Sanctuary is not ready to open.";
        case GATE_NONE:
        case GATE_COUNT:
        default:
            return "";
    }
}

bool world_can_travel(int from, EdgeDir direction, unsigned int flags)
{
    if (from < 0 || from >= ZONE_COUNT ||
        (unsigned int)direction >= EDGE_COUNT) {
        return false;
    }

    const Exit *exit = &g_zones[from].exits[direction];
    if (exit->target >= ZONE_COUNT || exit->gate >= GATE_COUNT) return false;
    unsigned int required = world_gate_requirement((GateType)exit->gate);
    return (flags & required) == required;
}

int world_travel(int from, EdgeDir direction, unsigned int flags)
{
    if (!world_can_travel(from, direction, flags)) return -1;
    return g_zones[from].exits[direction].target;
}

bool world_validate(void)
{
    bool valid = true;
    for (int i = 0; i < REGION_COUNT; i++) {
        if (g_regions[i].name == NULL || g_regions[i].biome == NULL ||
            world_region_zone_count(i) != (int)REGION_ZONE_COUNTS[i]) {
            fprintf(stderr, "[world] invalid region definition: %d\n", i);
            valid = false;
        }
    }

    for (int i = 0; i < ZONE_COUNT; i++) {
        const Zone *zone = &g_zones[i];
        if (zone->name == NULL || zone->region >= REGION_COUNT) {
            fprintf(stderr, "[world] invalid zone definition: %d\n", i);
            valid = false;
            continue;
        }
        for (int d = 0; d < EDGE_COUNT; d++) {
            const Exit *exit = &zone->exits[d];
            if (exit->target == EDGE_NONE) {
                if (exit->entry != EDGE_NONE || exit->gate != GATE_NONE) {
                    fprintf(stderr, "[world] invalid empty exit: %s edge %d\n",
                            zone->name, d);
                    valid = false;
                }
                continue;
            }
            if (exit->target >= ZONE_COUNT || exit->entry >= EDGE_COUNT ||
                exit->gate >= GATE_COUNT) {
                fprintf(stderr, "[world] invalid exit: %s edge %d\n",
                        zone->name, d);
                valid = false;
                continue;
            }

            const Exit *back = &g_zones[exit->target].exits[exit->entry];
            if (back->target != (unsigned char)i ||
                back->entry != (unsigned char)d ||
                back->gate != exit->gate) {
                fprintf(stderr,
                        "[world] asymmetric link: %s edge %d -> %s entry %d\n",
                        zone->name, d, g_zones[exit->target].name,
                        exit->entry);
                valid = false;
            }
        }
    }
    if (!valid) return false;

    const unsigned int allFlags = WF_THORNFANG | WF_LANTERN | WF_ROOT_KEY |
                                  WF_TIDE_LOW | WF_STORY_FINAL;
    bool seen[ZONE_COUNT] = {false};
    int queue[ZONE_COUNT];
    int head = 0;
    int tail = 0;
    queue[tail++] = Z_WRECKSHORE;
    seen[Z_WRECKSHORE] = true;
    while (head < tail) {
        int current = queue[head++];
        for (int d = 0; d < EDGE_COUNT; d++) {
            int destination = world_travel(current, (EdgeDir)d, allFlags);
            if (destination < 0 || seen[destination]) continue;
            seen[destination] = true;
            queue[tail++] = destination;
        }
    }
    for (int i = 0; i < ZONE_COUNT; i++) {
        if (!seen[i]) {
            fprintf(stderr, "[world] unreachable zone: %s\n", g_zones[i].name);
            valid = false;
        }
    }
    if (valid) {
        fprintf(stderr, "[world] validated: %d zones, %d regions.\n",
                ZONE_COUNT, REGION_COUNT);
    }
    return valid;
}

void world_map_draw_minimap(ZoneId current_zone, bool show_trial_marker)
{
    static const char *regionLabels[REGION_COUNT] = {
        "PALMSHORE", "DRIFTWOOD", "JUNGLE", "EMBER",
        "SUNKEN", "SKYWATCH", "LOST", "SANCTUARY"
    };
    const Color panel = {11, 26, 34, 235};
    const Color border = {89, 132, 130, 255};
    const Color planned = {129, 148, 148, 255};
    const Color playable = {128, 209, 174, 255};
    const Color current = {105, 225, 238, 255};

    DrawRectangle(203, 6, 115, 72, panel);
    DrawRectangleLines(203, 6, 115, 72, border);
    DrawText("CAMPAIGN MAP", 207, 8, 6, RAYWHITE);
    DrawText("M: FULL", 277, 8, 5, (Color){245, 214, 124, 255});

    for (int region = 0; region < REGION_COUNT; region++) {
        int column = region % 2;
        int row = region / 2;
        int x = 205 + column * 56;
        int y = 17 + row * 15;
        bool regionCurrent = current_zone >= 0 && current_zone < ZONE_COUNT &&
                             g_zones[current_zone].region == region;

        if (regionCurrent) {
            DrawRectangle(x - 1, y - 1, 55, 14, (Color){35, 66, 72, 255});
            DrawRectangleLines(x - 1, y - 1, 55, 14, current);
        }
        DrawText(regionLabels[region], x + 1, y, 5,
                 regionCurrent ? current : (Color){199, 210, 207, 255});

        int zoneIndex = 0;
        for (int zone = 0; zone < ZONE_COUNT; zone++) {
            if (g_zones[zone].region != (unsigned char)region) continue;
            int markerX = x + 3 + zoneIndex * 9;
            int markerY = y + 9;
            if (zone == (int)current_zone) {
                DrawCircle(markerX, markerY, 3.0f, BLACK);
                DrawCircle(markerX, markerY, 2.0f, current);
                if (show_trial_marker && zone == Z_SHRINE_APPROACH) {
                    DrawCircle(markerX + 5, markerY - 2, 1.5f,
                               (Color){255, 214, 112, 255});
                }
            } else {
                DrawCircle(markerX, markerY,
                           world_zone_is_playable(zone) ? 1.6f : 1.0f,
                           world_zone_is_playable(zone) ? playable : planned);
            }
            zoneIndex++;
        }
    }
}

void world_map_draw(ZoneId current_zone)
{
    static const char *zoneLabels[ZONE_COUNT][2] = {
        {"Wreckshore", ""},
        {"Tidepool Path", ""},
        {"Village Square", ""},
        {"Harbor Row", ""},
        {"Jungle Entrance", ""},
        {"Root Path", ""},
        {"Overgrown Clearing", ""},
        {"Shrine Approach", "Agility Shrine"},
        {"Ashfall Descent", ""},
        {"Forge Veins", ""},
        {"Buried Forge", ""},
        {"Flooded Court", ""},
        {"Resonance Hall", ""},
        {"Tide Engine", ""},
        {"Windward Steps", ""},
        {"Mirror Walk", ""},
        {"Signal Nest", ""},
        {"Outer Archive", ""},
        {"Silent Courtyard", ""},
        {"Mirror Gallery", "Shadow Causeway"},
        {"Hollow Hall", ""},
        {"Outer Threshold", ""},
        {"Memory Gallery", ""},
        {"Quiet Core", ""}
    };
    const Color background = {7, 17, 23, 252};
    const Color panel = {15, 34, 40, 255};
    const Color border = {89, 132, 130, 255};
    const Color planned = {129, 148, 148, 255};
    const Color playable = {128, 209, 174, 255};
    const Color current = {105, 225, 238, 255};

    DrawRectangle(0, 0, VIRTUAL_W, VIRTUAL_H, background);
    DrawText("PALM ISLAND - CAMPAIGN MAP", 7, 4, 8, RAYWHITE);
    DrawText("8 REGIONS / 24 ZONES", 7, 14, 6, (Color){245, 214, 124, 255});
    DrawCircle(183, 17, 2.0f, playable);
    DrawText("PLAYABLE", 188, 14, 5, RAYWHITE);
    DrawCircle(237, 17, 2.0f, planned);
    DrawText("PLANNED", 242, 14, 5, RAYWHITE);
    DrawText("M / VIEW CLOSE", 7, 171, 6, (Color){176, 194, 182, 255});

    for (int region = 0; region < REGION_COUNT; region++) {
        int column = region % 2;
        int row = region / 2;
        int x = 5 + column * 157;
        int y = 24 + row * 37;
        DrawRectangle(x, y, 153, 35, panel);
        DrawRectangleLines(x, y, 153, 35, border);
        DrawText(g_regions[region].name, x + 4, y + 2, 6,
                 (Color){245, 214, 124, 255});

        int zone_index = 0;
        for (int zone = 0; zone < ZONE_COUNT; zone++) {
            if (g_zones[zone].region != (unsigned char)region) continue;
            int zone_column = zone_index % 2;
            int zone_row = zone_index / 2;
            int zone_x = x + 4 + zone_column * 74;
            int zone_y = y + 12 + zone_row * 10;
            bool is_current = zone == (int)current_zone;
            bool is_playable = world_zone_is_playable(zone);
            Color label_color = is_current ? current
                                           : is_playable ? playable : planned;
            if (is_current) {
                DrawRectangle(zone_x - 2, zone_y - 1, 72, 10,
                              (Color){37, 76, 81, 255});
                DrawCircle(zone_x, zone_y + 3, 1.5f, current);
                zone_x += 3;
            } else {
                DrawCircle(zone_x, zone_y + 3, 1.0f, label_color);
                zone_x += 3;
            }
            DrawText(zoneLabels[zone][0], zone_x, zone_y, 4, label_color);
            if (zoneLabels[zone][1][0] != '\0') {
                DrawText(zoneLabels[zone][1], zone_x, zone_y + 4, 4,
                         label_color);
            }
            zone_index++;
        }
    }
}

void world_debug_dump(void)
{
    static const char *DIRECTIONS[EDGE_COUNT] = {"N", "E", "S", "W"};
    for (int r = 0; r < REGION_COUNT; r++) {
        printf("== %s (%s) ==\n", g_regions[r].name, g_regions[r].biome);
        for (int i = 0; i < ZONE_COUNT; i++) {
            if (g_zones[i].region != (unsigned char)r) continue;
            printf("  [%2d] %-34s%s  ", i, g_zones[i].name,
                   g_zones[i].checkpoint ? "*" : " ");
            for (int d = 0; d < EDGE_COUNT; d++) {
                const Exit *exit = &g_zones[i].exits[d];
                if (exit->target == EDGE_NONE) continue;
                printf("%s->%s%s ", DIRECTIONS[d],
                       g_zones[exit->target].name,
                       exit->gate == GATE_NONE ? "" : "(locked)");
            }
            printf("\n");
        }
    }
}
