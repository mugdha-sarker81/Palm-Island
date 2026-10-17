#ifndef WORLD_MAP_H
#define WORLD_MAP_H

#include <stdbool.h>
#include "raylib.h"

#define WF_THORNFANG   (1u << 0)
#define WF_LANTERN     (1u << 1)
#define WF_ROOT_KEY    (1u << 2)
#define WF_TIDE_LOW    (1u << 3)
#define WF_STORY_FINAL (1u << 4)

typedef enum ZoneId {
    Z_WRECKSHORE = 0,
    Z_TIDEPOOL_PATH,
    Z_VILLAGE_SQUARE,
    Z_HARBOR_ROW,
    Z_JUNGLE_ENTRANCE,
    Z_ROOT_PATH,
    Z_OVERGROWN_CLEARING,
    Z_SHRINE_APPROACH,
    Z_ASHFALL_DESCENT,
    Z_FORGE_VEINS,
    Z_BURIED_FORGE,
    Z_FLOODED_COURT,
    Z_RESONANCE_HALL,
    Z_TIDE_ENGINE,
    Z_WINDWARD_STEPS,
    Z_MIRROR_WALK,
    Z_SIGNAL_NEST,
    Z_OUTER_ARCHIVE,
    Z_SILENT_COURTYARD,
    Z_MIRROR_GALLERY_SHADOW_CAUSEWAY,
    Z_HOLLOW_HALL,
    Z_OUTER_THRESHOLD,
    Z_MEMORY_GALLERY,
    Z_QUIET_CORE,
    ZONE_COUNT
} ZoneId;

typedef enum RegionId {
    R_PALMSHORE_BEACH = 0,
    R_DRIFTWOOD_VILLAGE,
    R_FORGOTTEN_JUNGLE,
    R_EMBER_CAVERNS,
    R_SUNKEN_TEMPLE,
    R_SKYWATCH_CLIFFS,
    R_LOST_KINGDOM,
    R_EXCALIBUR_SANCTUARY,
    REGION_COUNT
} RegionId;

typedef enum EdgeDir {
    EDGE_NORTH = 0,
    EDGE_EAST,
    EDGE_SOUTH,
    EDGE_WEST,
    EDGE_COUNT
} EdgeDir;

typedef enum GateType {
    GATE_NONE = 0,
    GATE_THORNFANG,
    GATE_EMBER_LAMP,
    GATE_ROOT_KEY,
    GATE_TIDE,
    GATE_STORY_FINAL,
    GATE_COUNT
} GateType;

#define EDGE_NONE 0xFFu
#define NO_EXIT ((Exit){ EDGE_NONE, EDGE_NONE, GATE_NONE })

typedef struct Exit {
    unsigned char target;
    unsigned char entry;
    unsigned char gate;
} Exit;

typedef struct Zone {
    const char *name;
    unsigned char region;
    bool checkpoint;
    Exit exits[EDGE_COUNT];
} Zone;

typedef struct Region {
    const char *name;
    const char *biome;
} Region;

extern const Region g_regions[REGION_COUNT];
extern const Zone g_zones[ZONE_COUNT];

const Zone *world_zone(int idx);
const Region *world_region(int idx);
int world_zone_by_name(const char *name);
int world_region_first_zone(int region);
int world_region_zone_count(int region);
bool world_zone_is_playable(int zone);

unsigned int world_gate_requirement(GateType gate);
const char *world_gate_locked_msg(GateType gate);
bool world_can_travel(int from, EdgeDir direction, unsigned int flags);
int world_travel(int from, EdgeDir direction, unsigned int flags);

bool world_validate(void);
void world_map_draw_minimap(ZoneId current_zone, bool show_trial_marker);
void world_map_draw(ZoneId current_zone);
void world_debug_dump(void);

#endif
