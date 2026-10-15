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
- The current blockout contains five linked 40×23 (640×368 px) Jungle slice
  areas plus the first Ember Caverns greybox, Ashfall Descent. Ashfall is
  reached from the Arena and returns using the shared room data, movement,
  collision, transitions, and save validation.

### Implemented

- Fixed-step movement, keyboard/gamepad input, F3 telemetry, bounded camera,
  prop depth sorting, feet collision, and elevation-aware ramps.
- Jungle Entrance loop, root landmark, and shrine-door vista.
- Root Path thorn wall/gate with a tested upper bypass, root-bridge prop,
  marker, palms, foreground grass, and ramp.
- Overgrown Clearing with a central exposed route, reconnecting safe outer
  loop, marked optional nook, rock thickets, palms, and foreground grass.
- Paired fade transitions and camera resets connect the five Jungle slice
  areas and Ashfall Descent. The twelve directed exits and their destination
  spawns are authored in the room definitions.
- A compact room-definition table associates each of the five Jungle areas
  with its safe entry spawn, authored prop array, inspectable targets, ramp
  metadata, optional gate prop used by collision, outgoing transitions, and
  area-specific food-node identity, Palm Fruit position, and tile generator.
- Ashfall Descent reuses the former room-template pilot slot and adds a
  branching vent-route greybox, one explicit elevation ramp, a safe return to
  the Arena, and room-entry checkpoint saving. Amber telegraphs precede active
  damaging steam; health depletion returns Ichigo to the Ashfall spawn.
- One Ash Skitter now patrols between marked patches, shows a bright route tell,
  dashes with once-per-pass contact damage, and accepts one Thornfang hit per
  recovery window until defeated. Its state resets on room re-entry or death;
  it adds no save flags or drops.
- Ashfall is still accessible before Thornfang; its HUD now tells unequipped
  players to return to the Arena trial rather than leaving the weapon
  requirement implicit.
- The Arena trial now has a pulsing world beacon and a west-center HUD locator
  while Thornfang is missing and the trial is dormant, addressing the reported
  difficulty finding the marker. This locator still needs a hands-on check.
- A current-room minimap now appears in the upper-right HUD, showing the
  current area name, generated paths/terrain, and a player-position dot.
  While the Thornback trial is dormant and Thornfang is missing, the map also
  marks the trial. It does not reveal other rooms; visual playtest is pending.
- The user reports that the Ashfall route, vent warning/fairness, damage/reset,
  and checkpoint after leaving/relaunching all passed hands-on testing on
  2026-10-15. This is a user-reported playtest result.
- The Ash Skitter implementation and its automated state, hit, damage-contact,
  reset, and depth-order checks are complete; its readability and combat feel
  still need a hands-on playtest.
- Each existing area's authored tile-layout generator is selected through an
  indexed table; Root Path alone receives the gate-open tile-state flag.
- Root Path root markings have a minimal inspect interaction: E/controller A,
  in-front distance targeting, a fading prompt, buffered input, and a
  repeatable clue panel. Its clue state is included in autosaves.
- The Living Thorn Weave clue and first Root Path marker are separate targets.
  Activating the marker produces a route trace around the gate; its state is
  included in autosaves.
- The "Roots Remember" objective HUD advances through the root markings,
  marker, Map of Roots, Thornback, thorn gate, and return to the Jungle
  Entrance using existing saved flags, without a generic quest framework.
- A modal controls-help panel is available with F1/controller Start, lists
  keyboard/gamepad bindings, pauses the simulation, and switches exclusively
  with the inventory overlay.
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
- A version-4 autosave in `%APPDATA%/PalmIsland.sav` records the current safe
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
- Finite Palm Fruit pickups are placed in the Overgrown Clearing nook and
  Shrine Approach courtyard. E/A collects them into a three-serving food
  capacity; H/controller B consumes one serving to restore up to two hearts,
  capped at current max health. Full health does not consume food. Pickup
  nodes and food count autosave in version 4; version-1/version-2 saves
  migrate with no food and both nodes available, while version-3 saves retain
  the Clearing node state. Capacity/healing are prototype tunables.
- Health is displayed as five pixel hearts with an adjacent numeric count for
  readability; empty hearts use an outlined, muted treatment. At two hearts or
  fewer, an explicit amber "LOW" label supplements the heart colors without
  flashing.
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
- The user confirmed the inventory overlay displays saved state, opens/closes
  with its keyboard/controller inputs, and pauses/resumes gameplay.
- The Windows build succeeded with version-4 food persistence.
- All eight focused Windows tests passed: movement/collision, camera,
  render-depth, Thornback trial, Ash Skitter, versioned save (including version-1 through
  version-3 migration), inventory/food state, and Jungle
  objective progression. Tests cover food capacity, healing/clamping, no
  consumption at full health, and both collectible nodes' reachability and
  collected-state hiding.
- Movement tests cover all eight directed room links, inclusive exit
  thresholds, invalid-area rejection, and collision-free paired spawns.
- Movement tests check that every area's definition supplies props and a
  collision-free spawn.
- Existing ramp/elevation and open-gate collision tests cover the ramp and
  gate metadata now stored in the per-area definitions.
- Interaction tests continue to cover target IDs, facing/range limits,
  collected-food filtering, and independent progression flags through the
  per-area authored interactable definitions.
- Movement tests verify the Clearing and Shrine Approach food-node identities
  and Palm Fruit positions, while confirming other areas have no food node.
- Save validation uses `ROOM_AREA_COUNT` as its exclusive area bound, avoiding
  a separate final-area constant when room IDs are extended.
- The Stage F room-template gate was verified with a paired Arena connection,
  authored layout/props/spawn, safe movement path, and save-checkpoint
  round-trip; that pilot slot is now the Ashfall Descent greybox.
- Ashfall's shared-movement route, ramp elevation change, paired return
  transition, vent telegraph/active/recovery windows, and save-checkpoint
  round-trip are covered by automated checks; the user reports passing a
  hands-on navigation/hazard/checkpoint playtest on 2026-10-15. Ash Skitter
  state, combat, contact, and depth ordering are automated; that encounter
  still needs hands-on validation.
- Inventory overlay hands-on check is user-confirmed.
- The user reports that Palm Fruit collection/eating, full-health no-consume,
  and first-pickup persistence across relaunch and defeat/respawn all work.
- The user confirmed the pixel-heart HUD is readable and correctly updates at
  full/reduced health, including damage and healing.
- The user confirmed the steady amber "LOW" label is readable at 2/5 and 1/5
  health, does not overlap the fruit/objective HUD, and clears after healing
  above two hearts.
- The user confirmed the Shrine Approach Palm Fruit is visible/reachable,
  collects correctly, persists across relaunch, and remains available when
  the food pack is full.
- The user confirmed the Roots Remember objective advances through its
  markings, marker, mural, Thornback, gate, return, and completion steps without
  obscuring the rest of the HUD or combat cues.
- The user reports the end-to-end Jungle slice pass met the 10–15 minute
  target, with no unclear required steps or softlocks; exact elapsed minutes
  were not provided.
- A new tester completed the Jungle slice, explained the Map of Roots clue,
  Thornfang, and shortcut without coaching or a waypoint, and reported no route
  blockers or softlocks. The clues felt complex but understandable; elapsed
  time was not reported.
- The user confirmed the controls-help panel opens/closes from keyboard and
  controller, pauses play, switches cleanly with inventory, and has readable
  bindings.
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

- The current Jungle slice still uses placeholder art and audio. The existing
  user reports a 10–15 minute pass; a new tester completed it and understood
  the required clue/ability/shortcut progression with no reported blockers.
- Quick weapon-switch controls, additional food varieties, discovered-map/
  settings persistence, general combat, audio, and additional interactions/clues.
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

Stage F's room-template gate is complete. The Ashfall Descent navigation gate
passed in a user-reported hands-on playtest on 2026-10-15: route, vent
warnings/fairness, damage/reset feedback, and checkpoint persistence after
relaunch were confirmed. The Arena trial has a pulsing marker, HUD locator, and
a current-room minimap with the player's position and a trial icon while the
trial is active; hands-on verification is pending. The first Ash Skitter is
implemented as a two-hit Thornfang encounter with a bright route tell,
dash/contact, and recovery; automated checks cover alert-range boundary,
once-per-dash contact, and reinitialization. Next, verify the Arena map and
locator in play, then hands-on test Ash Skitter's warning readability,
evasion, elevation safety, attack range/facing, recovery timing, and
defeat/reset behavior. Do not add the Forge Warden or a guardian.

## Upcoming Roadmap

1. Stage E usability gate passed; the tester found the clues complex but
   understandable. Defer licensed art/audio until the plan is clear.
2. With both food nodes, health HUD, and objective tracker user-tested, keep
   food to this single healing item type until measured playtests call for a
   tactical alternative.
3. Stage F room-template gate passed with a non-campaign pilot area using
   shared movement, collision, transitions, and save validation.
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
