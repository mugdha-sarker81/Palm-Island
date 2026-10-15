# Palm Island — Project Memory

**Purpose:** A short, current handoff for developers and coding assistants.
Detailed game design belongs in the gameplay blueprint; movement targets and
feel proposals belong in the feel plan. This file records verified project
status, locked decisions, and the next bounded task.

**Last reconciled:** 2026-10-15

## Current Status

### Project

- Windows, offline, single-player game written in C11 with CMake and Raylib
  5.5.
- The current prototype is a top-down, eight-direction exploration game.
- The current blockout contains five linked 40×23 (640×368 px) areas:
  Jungle Entrance, Root Path, Overgrown Clearing, Shrine Approach, and a
  Thornback Arena shell with a two-pattern combat encounter prototype.

### Implemented

- Fixed-step movement, keyboard/gamepad input, F3 telemetry, bounded camera,
  prop depth sorting, feet collision, and elevation-aware ramps.
- Jungle Entrance loop, root landmark, and shrine-door vista.
- Root Path thorn wall/gate with a tested upper bypass, root-bridge prop,
  marker, palms, foreground grass, and ramp.
- Overgrown Clearing with a central exposed route, reconnecting safe outer
  loop, marked optional nook, rock thickets, palms, and foreground grass.
- Paired fade transitions, destination spawns, and camera resets connect
  Entrance/Root Path and Root Path/Overgrown Clearing.
- Root Path root markings have a minimal inspect interaction: E/controller A,
  in-front distance targeting, a fading prompt, buffered input, and a
  repeatable clue panel. The clue is session-only; there is no save/progression
  integration.
- The Living Thorn Weave clue and first Root Path marker are separate targets.
  Activating the marker produces a session-only route trace around the gate;
  this has no save/progression integration.
- An optional Explorer's Note in the Overgrown Clearing nook uses the same
  inspect panel with a separate session-only seen flag.
- A non-combat Shrine Approach connects to Overgrown Clearing and the
  Thornback Arena shell. It has a walkable courtyard loop, an open north
  doorway, and a reachable Map of Roots mural clue.
- The Thornback Arena shell has a passable perimeter route, an open central
  floor, placeholder rock/marker props, and a paired return to Shrine Approach.
- The user visually playtested the Arena route, props, doorway, and return and
  reported that they work well.
- The west Arena marker starts a bounded Thornback encounter prototype. It
  alternates a straight charge with a slow broad sweep across the center lane;
  each has a distinct ground tell and recovery. The sweep's marked danger band
  leaves a safe repositioning strip above and below the attack. The user
  confirmed this Phase 2 pattern works well.
- The player has five prototype hearts, with visible blink invulnerability and
  knockback on a hit. Each attack can hit only once; the charge danger zone
  follows Thornback's body while the sweep uses the telegraphed band.
- Thornback can be struck once per recovery by pressing Space/controller X
  while facing it and within range. Three recovery hits defeat the guardian.
  Defeat sets a session-only Thornfang-acquired flag; zero health retries at
  the Arena entrance with full health and a visible retry cue, then resets the
  encounter. These are provisional encounter values, not final balance.
- Thornback remains a procedural placeholder. The acquired flag does not yet
  enable a weapon inventory/action or open the thorn gate/return shortcut.
- The user manually playtested Phase 1 and reports that starting the trial,
  reading/avoiding the charge lane, recovery, and the Arena return all work
  well.
- Placeholder sprites and tiles are prototypes, not production art.

### Verification

- Windows build: `.\build.bat` succeeded with the two-pattern encounter,
  damage, and recovery-strike prototype.
- Focused tests: `ctest --test-dir build --output-on-failure` passed all 4
  tests: `movement_collision`, `camera_follow`, `render_depth`, and
  `thornback_trial`. Tests cover pattern alternation, timing, attack zones,
  one hit per enemy attack/recovery, facing/range gates, defeat, and player
  damage invulnerability/knockback.
- The user confirms the Phase 2 sweep pattern works well.
- The combat, damage, recovery-strike, defeat, reward, and retry loop has
  passed automated tests but still needs a user hands-on playtest.
- The executable launched and remained responsive with the expected
  `Palm Island` window title.
- The user manually playtested both linked rooms and reported that the
  transitions, routes, camera, props, ramp, and collision all work perfectly.
- The user approved the Overgrown Clearing as working well and proceeded to
  the next task; detailed visual findings were not supplied.
- The user reports that the original clue prompt and panel work as expected.
  The clue target was subsequently moved to the thorn weave and split from the
  marker interaction; this revised placement has not yet had a hands-on check.
- The user responded positively to the revised Root Path interaction work and
  asked to continue; no detailed visual observations were supplied.
- The user confirmed the marker trace and Clearing note look good; this is a
  user-reported visual pass.
- Automated tests cover the clue and marker target IDs, ranges, facing,
  reachable approach points, unsupported areas, and independent repeatable
  open/close states for the Root Path targets and Clearing note. Shrine
  Approach tests cover the courtyard, newly open north route, paired room
  transitions, and mural reachability/state; Arena tests cover perimeter
  access, bounds, and return-spawn safety.
- The user reports all ten Feel Gate checks passed. Manual test details are
  user-reported; do not infer or invent measurements.
- The user confirmed that the Shrine Approach mural, routes, sealed threshold,
  and return worked. This was a hands-on pass before the threshold was opened
  into the arena.
- The user manually playtested the Thornback Arena shell and reports that
  everything works well, including the route through the arena and return to
  Shrine Approach. Visual details are user-reported, not independently verified.
- The user manually playtested the Thornback Phase 1 prototype and reports that
  it works well. Visual details are user-reported, not independently verified.
- Treat visual and hands-on passes as user-reported; automated tests do not
  verify the game’s appearance or feel.

### Not implemented

- The 10–15 minute Forgotten Jungle slice; the current Thornback encounter is
  still a prototype and its combat/retry loop needs hands-on testing.
- Persistent checkpoints/progression/save-load, general combat and health
  systems, Thornfang's vine-cutting action, open gate/shortcut, audio, and
  additional interactions/clues.
- Production art, finished UI, and release packaging.

## Locked Decisions

- **Genre:** top-down 2D pixel-art exploration with Y-sorting, occlusion, and
  explicit elevation links; no required platformer jump.
- **Target/technology:** Windows PC, offline single-player; C11, CMake,
  Raylib 5.5.
- **Presentation/simulation:** 320×180 virtual canvas, 16×16 tiles, point
  filtering, fixed 120 Hz simulation with bounded catch-up.
- **Controls:** keyboard and controller; maintain parity and tune with the
  keyboard first.
- **First deliverable:** one 10–15 minute Forgotten Jungle slice. Validate
  handcrafted spaces before expanding the island.
- **Progression:** Thornfang is the first planned traversal ability;
  Excalibur is a temporary final transformation, not a sixth open-world
  weapon.
- **Scope:** no generic crafting/grinding, XP levels, large enemy roster, or
  broad content expansion during the first slice. Signature mechanics remain
  proposals and require approval before implementation.
- **Feel:** movement values in `src/tuning.h` are tunable starting points.
  Change one value per tuning pass and verify it in play.

The detailed campaign, weapon concepts, region designs, player tasks, and
implementation-stage proposal live in
[`MASTER_GAMEPLAY_BLUEPRINT.md`](MASTER_GAMEPLAY_BLUEPRINT.md). The movement
feel targets and signature-mechanic proposals live in
[`GAME_FEEL_AND_SIGNATURE_PLAN.md`](GAME_FEEL_AND_SIGNATURE_PLAN.md).
Historical decision notes are in [`DECISIONS.md`](DECISIONS.md).

## Next Work Item

Manually playtest the encounter prototype: confirm charge and sweep tells are
readable, hit zones match their warnings, Space/controller X strikes only
Thornback's recovery when facing it in range, and hits/defeat/retry feedback
are clear. Values are tunable starting points; change one per tuning pass.

## Upcoming Roadmap

1. Complete the Thornback encounter prototype's user playtest gate.
2. Implement Thornfang's actual vine-cutting action and the marked thorn-gate
   route/shortcut back toward the Jungle Entrance.
3. Playtest the full 10–15 minute Forgotten Jungle slice for route clarity,
   task completion, return paths, controller use, and softlocks.
4. After the slice gate passes, add the fixed inventory/health/checkpoint and
   versioned save foundation described in Stage D.
5. Polish the slice's licensed art, audio, menus, prompts, accessibility, and
   measured onboarding; then build and verify a data-driven region template.
6. Expand one weapon region at a time through its own playtest gate, then
   implement the Sanctuary/finale and clean Windows packaging.

Keep Thornback signature mechanics such as "The Read" out of scope unless
separately approved. Do not begin a later stage before the preceding
playtest gate passes.

## Feel Gate Record

The user reported all ten checks passed on 2026-10-14. The pass is
user-reported; exact results for manual checks were not supplied.

| # | Check | Status and evidence |
|---|---|---|
| 1 | Diagonal speed | Pass; automated 76.000 vs 76.000 px/s, 0.000% difference |
| 2 | Wall slide | Pass; automated 53.740 px/s tangential (70.7%); user reports feel check passed |
| 3 | Corners | Pass; user reports all-angle 16 px doorway check passed |
| 4 | Stopping | Pass; automated 0.050 s and 1.583 px drift |
| 5 | Camera | Pass; automated behavior checks and user-reported visual check |
| 6 | Frame time | Pass; user reports check passed; exact 60/144 Hz and stall measurements unavailable |
| 7 | Input | Pass; user reports keyboard/controller parity and no drift at rest |
| 8 | Depth | Pass; automated ordering checks and user-reported visual check |
| 9 | Ramp | Pass; automated transition/collision checks and user-reported visual check |
| 10 | Two-minute wander | Pass; user reports unprompted test passed |

## Working Rules and References

- Before coding, read this status, **Locked Decisions**, and **Next Work Item**.
- Read only the relevant section of the blueprint or feel plan for the task.
- Take one bounded task at a time. Inspect current code, preserve working
  behavior, build after code edits, run focused tests, and report unperformed
  manual checks explicitly.
- Update this file when verified status, a locked decision, or the next task
  changes. Do not treat proposals or user-reported results as independently
  verified.
- [README.md](../README.md): overview and Windows build/run commands.
- [ASSET_INDEX.md](ASSET_INDEX.md): art, preview, generator, and source-pack
  inventory.
- [REFERENCE_LEGEND_OF_LUA.md](REFERENCE_LEGEND_OF_LUA.md): high-level external
  reference notes; no third-party code or assets are included.
- [source/README.md](source/README.md): historical documents and backups.
- [AGENTS.md](../AGENTS.md): assistant workflow.
