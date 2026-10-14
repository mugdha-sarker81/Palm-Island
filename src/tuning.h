#ifndef PALM_TUNING_H
#define PALM_TUNING_H

/* Every feel/movement number lives here. See docs/GAME_FEEL_AND_SIGNATURE_PLAN.md.
 * Starting values at the 320x180 virtual canvas; tune by playing.
 * Change ONE number per tuning pass. */

/* --- Canvas and loop (locked decisions) --- */
#define VIRTUAL_W        320
#define VIRTUAL_H        180
#define TILE_SIZE        16
#define SIM_HZ           120
#define SIM_DT           (1.0f / (float)SIM_HZ)
#define MAX_SIM_STEPS    5      /* drop extra time after a stall instead of spiralling */
#define MAX_FRAME_TIME   0.1f   /* seconds; clamp a long stall */

/* --- Walking (feel plan section 1.2) --- */
#define WALK_SPEED        76.0f  /* px/s, about 4.75 tiles/s */
#define ACCEL_TIME        0.07f  /* seconds to reach full speed */
#define DECEL_TIME        0.05f  /* seconds to stop */
#define TURN_ACCEL_MULT   1.5f   /* extra accel when input opposes velocity */
#define STICK_DEADZONE    0.20f  /* radial dead zone for analog sticks */
#define FACING_KEEP_DEG   60.0f  /* keep current facing within this angle (45 + 15 hysteresis) */

/* --- Derived (do not edit) --- */
#define WALK_ACCEL        (WALK_SPEED / ACCEL_TIME)
#define WALK_DECEL        (WALK_SPEED / DECEL_TIME)

/* --- Animation --- */
#define WALK_FRAME_TIME   0.14f  /* seconds per walk frame at full speed */

/* --- Player sprite (assets/sprites/player_sheet_14x20.png) --- */
#define PLAYER_SPRITE_W   14
#define PLAYER_SPRITE_H   20

/* --- Feet collision (feel plan section 1.3) --- */
#define PLAYER_COLLIDER_W 10.0f
#define PLAYER_COLLIDER_H 6.0f
#define COLLISION_MAX_STEP 4.0f
#define CORNER_ASSIST_MAX 4

#endif
