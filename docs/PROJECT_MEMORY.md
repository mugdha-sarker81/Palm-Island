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
  repeatable clue panel. Its clue state is included in autosaves.
- The Living Thorn Weave clue and first Root Path marker are separate targets.
  Activating the marker produces a route trace around the gate; its state is
  included in autosaves.
- An optional Explorer's Note in the Overgrown Clearing nook uses the same
  inspect panel with a separate saved seen flag.
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
  Defeat grants Thornfang; zero health retries at
  the Arena entrance with full health and a visible retry cue, then resets the
  encounter. These are provisional encounter values, not final balance.
- Thornfang is a world ability: Space/controller X cuts the
  marked Root Path gate when Ichigo faces it within range. Cutting it removes
  the gate prop and collision, opens a walkable corridor through the root wall,
  and persists the opened state across relaunches. A nearby prompt explains
  the required weapon or cut input.
- A version-2 autosave in `%APPDATA%/PalmIsland.sav` records the current safe
  room-entry checkpoint, health, owned/selected weapons, four discovered
  Jungle evidence entries, and the opened thorn gate. Version-1 saves migrate
  their Thornfang and clue flags into the new inventory state. Autosave runs at
  room transitions and when progression changes; the previous valid file is
  kept as a `.bak` and can restore a damaged/missing primary save. Invalid
  saves without a valid backup are reported and left untouched.
- Inventory state supports permanent weapon ownership/selection and key-item
  discovery. Thornfang is the only implemented weapon; the four key-item
  entries mirror existing inspectable Jungle evidence. A compact inventory
  overlay opens with I/Tab or controller Y, shows weapon/evidence status, and
  pauses simulation while open. Quick weapon switching is deferred until a
  second weapon is implemented.
- Manual save was deferred by user choice; autosaving at checkpoints and major
  progress is the selected prototype behavior.
- The user manually playtested the full Thornback encounter and reports that
  it works well.
- The user manually tested the Thornfang marked-gate route and reports that
  cutting and using the opened corridor worked well. This is user-reported
  visual/gameplay feedback; the gate state is now included in autosaves.
- The user reports the end-to-end current Jungle blockout playtest completed
  without softlocks. The 10–15 minute duration and detailed route observations
  were not reported.
- Placeholder sprites and tiles are prototypes, not production art.

### Verification

- The user confirmed that the original versioned autosave survives a normal
  close and relaunch, restoring the saved checkpoint/progress.
- The user confirmed the version-2 save loads Thornfang and discovered
  evidence, and that migrated inventory progress persists across relaunch.
- The Windows build succeeded with the inventory and version-2 save changes.
- All six focused Windows tests passed: movement/collision, camera,
  render-depth, Thornback trial, versioned save (including version-1 migration),
  and inventory state.
- The inventory overlay build succeeded; visual readability, input parity, and
  pause/resume behavior still need a hands-on playtest.
- The user confirms the Phase 2 sweep pattern works well.
- The user reports the combat, damage, recovery-strike, defeat, reward, and
  retry loop passed hands-on playtesting.
- The Thornfang prompt, cut action, and opened corridor passed a user-reported
  hands-on playtest.
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

- The planned 10–15 minute Forgotten Jungle slice; the end-to-end user pass
  confirmed no softlocks but did not report completion time.
- Inventory UI/quick-switch controls, food, discovered-map/settings
  persistence, general combat, audio, and additional interactions/clues.
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
- **Saving:** autosave at room-entry checkpoints and major progression;
  manual save is deferred during the prototype.
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

Playtest the inventory overlay: confirm I/Tab and controller Y open/close it,
that the displayed weapon and discovered evidence match progress, and that
movement and the Thornback encounter remain paused until the overlay closes.
Then proceed to the first bounded health/food pickup task.

## Upcoming Roadmap

1. Complete the inventory overlay hands-on playtest; defer quick-switch UX
   until another weapon exists.
2. Add food/pickup behavior only with clear world pickups and checkpoint-safe
   progression.
3. Polish the slice's licensed art, audio, menus, prompts, accessibility, and
   measured onboarding; then build and verify a data-driven region template.
4. Expand one weapon region at a time through its own playtest gate, then
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
