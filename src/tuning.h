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

/* --- Interaction feel (feel plan section 1.6) --- */
#define INTERACT_RANGE        20.0f
#define INTERACT_PROMPT_RANGE 28.0f
#define INTERACT_PROMPT_FADE  0.15f
#define INTERACT_INPUT_BUFFER 0.12f
#define INTERACT_FACING_DOT   0.819152f /* cos(35 degrees), 70-degree cone */
#define THORNFANG_CUT_RANGE   24.0f
#define THORNFANG_CUT_BUFFER  0.10f
#define THORNFANG_CUT_FEEDBACK_TIME 1.2f

/* --- Camera follow (feel plan section 1.4) --- */
#define CAMERA_FOLLOW_RATE 6.0f
#define CAMERA_LOOK_AHEAD 14.0f

/* --- Thornback Phase 1 prototype --- */
#define THORNBACK_START_X       320.0f
#define THORNBACK_LANE_Y        184.0f
#define THORNBACK_LANE_MIN_X    200.0f
#define THORNBACK_LANE_MAX_X    440.0f
#define THORNBACK_TELL_TIME     1.1f
#define THORNBACK_CHARGE_SPEED  210.0f
#define THORNBACK_RECOVERY_TIME 1.0f
#define THORNBACK_SWEEP_TELL_TIME 0.9f
#define THORNBACK_SWEEP_SPEED   100.0f
#define THORNBACK_SWEEP_HALF_HEIGHT 12.0f
#define THORNBACK_CHARGE_HALF_WIDTH 16.0f
#define THORNBACK_CHARGE_HALF_HEIGHT 6.0f
#define THORNBACK_HEALTH        3
#define THORNBACK_PLAYER_HIT_RANGE 26.0f
#define PLAYER_START_HEALTH     5
#define PLAYER_HIT_INVULNERABILITY 0.5f
#define PLAYER_HIT_KNOCKBACK   8.0f
#define FOOD_SERVING_CAPACITY 3
#define FOOD_HEAL_AMOUNT 2

#endif
