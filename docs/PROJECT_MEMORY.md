# Palm Island — Project Memory and Incremental Build Plan

**Purpose:** This is the canonical, compact-enough handoff for developers and
AI assistants, including models with limited context or usage. It merges the
current README and active planning documents into one working source of truth.
The map-by-map implementation blueprint is maintained in
`docs/MASTER_GAMEPLAY_BLUEPRINT.md`, including the detailed player
exploration/subtask sequence for all 24 proposed zones.

**Last reconciled:** 2026-10-14
**Current milestone:** M1–M6 movement systems support two linked 640×368
Forgotten Jungle blockout areas: Jungle Entrance and Root Path. The user
reports all ten Feel Gate checks pass. Automated baseline measurements include
diagonal speed (0.000% difference), wall-slide speed (53.740 px/s; 70.7% of
walk speed), and stopping (0.050 s, 1.583 px).
**Next milestone:** verify the player-facing route and transitions between
Jungle Entrance and Root Path, then continue the Forgotten Jungle blockout.
**Next work item:** launch with `run.bat` and playtest both rooms, including
the closed thorn gate's bypass, both fade transitions, the Root Path ramp and
bridge, and prop depth. Record route/readability issues before making another
area. Do not add combat, Thornfang, or signature mechanics.

> Update this document when implementation status, a locked decision, or the
> next task changes. Keep the current status factual. Do not mark design ideas
> as implemented.

## 1. Fast handoff

### In one paragraph

Palm Island is a mysterious, quiet 2D exploration adventure made in C11 and
Raylib 5.5 for Windows PC. Ichigo follows a family symbol to an island whose
guardians and five legendary weapons are tied to a deeper, partly erased
history. The game should reward observation and discovery. Build and validate
one playable Forgotten Jungle slice before making the full island.

### Read only these first

1. `AGENTS.md` — assistant workflow.
2. This file's **Current Status**, **Locked Decisions**, and **Next Work Item**.
3. `docs/MASTER_GAMEPLAY_BLUEPRINT.md` only when working on game features,
   campaign maps, player movement, progression, or zone exploration flow.
4. `docs/GAME_FEEL_AND_SIGNATURE_PLAN.md` only for movement, camera, combat
   feel, interaction feedback, effects/audio feedback, or "make it
   cooler/smoother" tasks.
5. The exact code/files touched by the assigned task.
6. One relevant design detail section if needed.

Do not reread every plan or the archived source GDD for an ordinary coding
task. The other active design documents have been consolidated into this file;
the original GDD remains historical reference material.

### Task boundaries for limited-context models

- Take **one work item** from the roadmap, not an entire milestone.
- First identify the behavior that must stay working.
- Make the smallest complete change; avoid unrelated cleanup and speculative
  systems.
- Build after code changes, then launch/playtest when behavior is visual or
  interactive.
- End with a brief handoff: changed files, behavior delivered, commands run,
  result, known issue, and next work item.
- Update **Current Status** and **Next Work Item** when the verified state
  changes. Record a design choice in the decision log before relying on it.
- If a task conflicts with a locked decision, stop and ask for a deliberate
  decision rather than quietly changing the design.

## 2. Current status (verified facts)

### Project/build

- Platform target: Windows PC; single-player and offline.
- Language/build: C11 + CMake; Raylib 5.5 is downloaded into the ignored
  `build/_deps` folder during first configure.
- A clean configure/build succeeded on 2026-10-14 using the MSYS2 MinGW64
  toolchain (`C:\msys64\mingw64\bin`) and CMake's `MinGW Makefiles` generator.
  The M1–M2 executable launched and remained open with the expected `Palm
  Island` window title. The M3 Windows build and focused collision CTest also
  passed; Raylib emitted an upstream CMake minimum-version deprecation warning.
- Use MSYS2 MinGW64, not the unrelated legacy `C:\MinGW` compiler. If the
  generator differs from the existing CMake cache, rename the generated
  `build` directory (for example, `build-backup`) and configure again.
- On Windows, use `build.bat` to configure/build or `run.bat` to build and
  launch; the scripts set the standard MSYS2 MinGW64 PATH automatically.
- This workspace is a Git clone of `mugdha-sarker81/Palm-Island` on `main`,
  tracking `origin/main`.
- No interactive gameplay automation is configured; opt-in CTest covers the
  M3 collision logic.

### Current code state

- The previous side-scrolling coast implementation has been removed.
- M1–M2 provide a 320×180 point-filtered render target with integer
  letterboxing; normalized eight-way keyboard/gamepad movement intent;
  acceleration/deceleration, facing hysteresis, walk animation, a fixed-step
  simulation, and an F3 debug overlay. F11 toggles borderless windowed mode;
  Esc exits.
- The current 40×23 (640×368 px) maps are the Jungle Entrance and Root Path
  blockouts. The Entrance has a safe loop around a root cluster and a northern
  shrine-door landmark. Root Path has a branching upper bypass around a solid
  thorn wall, a closed gate, a root bridge prop, a side marker, palms, tall
  grass, and a short optional ramp. Both areas use placeholder terrain and
  props.
- A paired north/south room connection switches between the two areas with
  fade-out/fade-in and destination spawn/camera reset. The route, closed-gate
  collision, paired transition conditions, and ramp behavior are covered by
  focused tests; the new transitions and Root Path still need hands-on visual
  playtesting.
- M3 provides a 10×6 feet collider, solid collision against the root cluster,
  landmark, tree trunks, and room bounds, axis-separated movement with
  substeps no larger than 4 px, and a 4 px corner assist.
- M4 adds a bounded camera module with 6/s exponential follow, smoothed
  velocity-based look-ahead up to 14 px, room-size-aware centering, and
  render-time integer target rounding. The camera now scrolls through the
  expanded 640×368 room and clamps at its edges. Focused camera tests cover
  small-room centering, follow smoothing, look-ahead easing, room bounds,
  hard initialization snap, and render rounding.
- M5's depth sorter orders the player and props by foot/base Y with a stable
  tie-break, with foreground props drawn last. Palm-trunk colliders are shared
  with room collision; canopies remain walk-through for depth overlap. The
  entrance uses these rules for palms and foreground tall grass.
- M6's two-tile ramp/elevation connector is reused as a short optional ramp on
  the entrance's eastern loop. Crossing its midpoint changes elevation; only
  the aligned upper landing is traversable on that layer. Automated movement
  tests cover ascent, descent, and side blocking.
- Uploaded M1–M2 screenshots are retained in `docs/media/m1_m2/` as reference
  captures; they were not captured during the Windows verification.
- The user reports that the full Feel Gate, including interactive movement,
  camera feel, prop occlusion, ramp safety, controller/input checks, and the
  two-minute wander test, passed on 2026-10-14. The pass is user-reported;
  detailed measurements and a recording were not supplied.
- Latest Windows verification: `.\build.bat` succeeded and
  `ctest --test-dir build --output-on-failure` passed all three tests
  (`movement_collision`, `camera_follow`, and `render_depth`). The movement
  suite verifies the Root Path bypass reaches the north portal without
  colliding and that the gate remains solid.

### Not implemented yet

- The complete game or the 10–15 minute vertical slice.
- The rest of the Forgotten Jungle slice: Overgrown Clearing, Shrine
  Approach/Arena, interactions, clue state, checkpoints, and authored exits
  beyond the current Jungle Entrance–Root Path connection.
- Combat, enemies, health, boss/guardian behavior, or Thornfang.
- Driftwood Village, rooms/region transitions, authored maps, or a level editor.
- Production character/environment art, animation sheets, music, or sound.
  Starter pixel-art sprites and a tileset exist under `assets/` and are loaded
  by the movement-room prototype, but remain visual prototypes.
- Dialogue system, inventory, persistent clue/world state, checkpoints, or
  save/load.
- Character health, food/shop, weapon switching UI, material pickups, fixed
  built tools, and costumes. Their design is specified in the gameplay
  blueprint but no such feature or production sprite is implemented.
- Menu, pause/settings, release packaging, or automated gameplay tests.

The concept images and ZIP archives in `assets/` remain references. The game
loads the starter player and tile assets; consult `docs/ASSET_INDEX.md` and
review licenses before using third-party assets.

## 3. Product direction

The chosen full-game camera is **top-down 2D pixel art with 3D-like depth cues**
(layered scenery, Y-sorting, occlusion, and explicit elevation links). The
earlier side-scrolling prototype has been removed; the current executable is
only a runnable scaffold while the top-down movement room is built.

### Player promise

“I found a place that was waiting for someone to notice.”

### Pillars

1. **Curiosity over quest markers:** landmarks, clues, and environmental
   changes lead the player.
2. **Quiet wonder over constant danger:** mood, light, sound, scale, and
   negative space matter.
3. **Weapons are understanding, not loot tiers:** each changes traversal and
   how the player interprets the island; combat is secondary.
4. **Movement is reliable and expressive:** controls should feel good before
   adding more abilities or content.
5. **Handcrafted spaces:** prove one region's quality before expanding.
6. **Story through place:** optional lore enriches but is not required to
   understand the main thread.

### Scope guards for the first slice

- Do not build all regions as empty maps first.
- Do not add XP/levels, crafting, procedural terrain, a large enemy roster,
  complex inventory, or sprawling side quests to the slice.
- Keep combat rare, short, readable, and relevant to the region.
- A feature is not done because it compiles: verify its player-facing behavior.
- Use simple placeholders during blockout, but do not mistake them for final
  production art or slice polish.

## 4. Story and world

Ichigo comes to Palm Island after finding an old family symbol that matches
marks on the island. The symbol raises a question of responsibility, not
treasure. Driftwood Village is a small isolated community that lives by the
island's rules and knows parts of the weapon legends. It provides a safe
introduction, warnings, and rumors.

The island's public legend says five guardians protect five weapons for a
worthy successor. Evidence should gradually complicate that story: the
guardians may be caretakers or mechanisms, and the weapons may have been used
to contain or maintain something. Excalibur is a final, temporary
transformation/revelation, not a permanent sixth weapon. Resolve Ichigo's
immediate motivation while leaving a larger mystery open.

### Narrative delivery

- Prefer environmental storytelling, short inscriptions, clues, and concise
  NPC dialogue to long cutscenes.
- Every mandatory clue should teach, reframe, or open something; optional lore
  must not obscure the main plot if skipped.
- Use negative space and contradictions intentionally. Don't explain every
  mystery as soon as it appears.

### World progression

1. Palmshore Beach — teach movement and introduce an impossible landmark.
2. Driftwood Village — safe hub, rumors, first major clue.
3. Forgotten Jungle — first guardian/weapon and vertical slice.
4. After Thornfang, allow the player to explore Ember Caverns, Sunken Temple,
   Skywatch Cliffs, and Lost Kingdom in a flexible order.
5. Excalibur Sanctuary — opens only after the five weapons and leads to the
   final trials and ending.

This is semi-linear: the opening is guided, then ability gates create
meaningful return paths and player choice.

## 5. Locked design decisions

These decisions combine the active design documents. Change them only after
recording the reason and date in the decision log below.

| Topic | Current decision |
|---|---|
| Genre / camera | Top-down 2D pixel-art exploration adventure; 3D-like depth cues, not a 3D engine |
| Target | Windows PC; single-player, offline |
| Technology | C11, CMake, Raylib 5.5 |
| Lead | Ichigo; family symbol connects him to the island |
| Tone | Mysterious, beautiful, quiet, exploratory |
| Combat | Minimal and readable; guardians test observation and timing |
| Progression | No XP levels in v1; abilities, discoveries, shortcuts, optional upgrades |
| Region order | Beach → Village → Jungle; remaining four weapon regions flexible after Thornfang |
| Excalibur | Temporary final transformation/reward, not an open-world weapon |
| Controls | Keyboard and controller both supported; keyboard first for tuning |
| First deliverable | 10–15 minute Forgotten Jungle vertical slice |
| Campaign outline | 8 regions / 24 connected authored zones; long-term proposal, not current implementation scope |
| Core movement | Eight-direction walking; stairs/ramps connect explicit elevation layers; no required platformer jump |
| Tool crafting | Small fixed, quest-built tool set using authored materials; no freeform recipe/grinding system |
| Pixel canvas | 320×180 virtual canvas; 16×16 tiles; integer 4× at 1280×720 with point filtering and letterboxing at other sizes |
| Simulation target | Fixed 120 Hz update with an accumulator; render at display rate with bounded catch-up |

### Weapons (working design)

| Weapon | Quality / region | Exploration expression | Narrative question |
|---|---|---|---|
| Thornfang | Agility / Forgotten Jungle | Cut thorn barriers; reveal root paths | What has the island been protecting? |
| Embercleaver | Strength / Ember Caverns | Crack specific seals; redirect heat | What was deliberately buried? |
| Stormpiercer | Awareness / Sunken Temple | Pulse ancient nodes | Why does the temple remember voices? |
| Dawnstring | Precision / Skywatch Cliffs | Strike distant mechanisms; align light | Who built routes between islands? |
| Voidfang | Courage / Lost Kingdom | Cross short shadow gaps; reveal hidden traces | What was erased from the official story? |

Weapon abilities are design targets, not implemented features. The active
redesign favors traversal and interpretation over damage upgrades. Keep the
weapons distinct without making any one a strict power upgrade.

### Decision log

- **2026-10:** Make exploration, atmosphere, discovery, and movement the
  priority; validate a single region before full-island production.
- **2026-10:** Use the Forgotten Jungle as the first 10–15 minute vertical
  slice.
- **2026-10:** Keep the five weapons as progression keys and Excalibur as a
  final temporary transformation.
- **2026-10-14:** Consolidate active planning into this project memory.
  Older active documents become redirects; the original GDD remains archived.
- **2026-10-14:** User selected a top-down 2D pixel-art world with eight-way
  movement and 3D-like depth cues. This supersedes the earlier side-scrolling
  target for the full game.
- **2026-10-14:** User directed removal of the previous side-scrolling game
  implementation. Replace it with a minimal runnable scaffold and plan the
  top-down movement-room source modules; do not build a full game architecture
  before those responsibilities are implemented.
- **2026-10-14:** Added `docs/GAME_FEEL_AND_SIGNATURE_PLAN.md` as the detailed
  feel and tuning reference. Use its movement/camera values as test-room
  starting targets, not final balance. Adopt a 320×180 canvas to match the
  starter art and target a fixed 120 Hz simulation with display-rate rendering.
  Signature mechanics remain proposals and require approval before
  implementation.
- **2026-10-14:** Completed the controller dead-zone and analog-magnitude pass
  in the movement input layer; debug output now surfaces the live input
  magnitude and whether the source is keyboard or gamepad to aid the Feel Gate.
- **2026-10-14:** User approved character/HP/weapon-switching/food/shop/tool/
  costume planning and selected a small fixed set of material-built tools,
  not full recipe crafting. Starting balance values remain proposals pending
  playtesting; none of these systems is implemented.
- **2026-10-14:** Implement M6 using the existing two-tile shrine stair as an
  explicit ground-to-upper connector. Elevation changes at its midpoint; the
  aligned two-tile upper landing is walkable only on the upper layer. Keep the
  solution specific to this room and do not introduce free jumping or a
  general elevation framework.
- **2026-10-14:** Expand the movement test room to 40×23 tiles around the
  existing 20×11 shrine area so the bounded camera can scroll and players can
  explore beyond the initial viewport. Preserve the existing shrine layout,
  stair opening, prop positions relative to that layout, and collision rules.

## 6. First playable target: Forgotten Jungle

### Player experience / route

Target first-playthrough length: approximately 10–15 minutes. This slice is
now designed for top-down eight-direction walking, explicit ramps/bridges,
room transitions, and Y-sorted/occluding scenery—not side-scrolling jumps.

1. **Jungle Entrance:** safe place to learn eight-way movement, interaction,
   collision, and the clear landmark.
2. **Root Path:** branching top-down navigation with ramps/bridges; show thorn
   barriers that cannot yet be crossed.
3. **Overgrown Clearing:** one small, readable challenge and one optional
   observation/navigation secret. Combat only if it supports the intended
   quiet pacing.
4. **Shrine Approach:** ancient stone and a mandatory clue that suggests the
   quality of agility and points toward the guardian.
5. **Thornback encounter:** one fair guardian trial. Telegraph attacks, make
   observation/repositioning useful, and provide a readable recovery window.
   The encounter must not become a long damage sponge.
6. **Shrine of Agility:** earn Thornfang; clearly demonstrate the new ability.
7. **Transformed return path:** cut a barrier seen earlier, open a shortcut,
   and reveal a small optional reward/lore beat.

### Slice acceptance criteria

- Eight-way movement remains responsive; diagonals are normalized, collision
  is stable at normal/high frame times, and elevation transitions are clear.
- New players understand walking/interacting and discover the main clue
  without a quest arrow.
- The earlier thorn gate is visually legible before Thornfang, and becomes
  traversable afterward.
- The guardian's attacks are readable; success feels earned rather than
  random or based on grinding.
- Thornfang changes both the player's options and the reachable world.
- The return shortcut visibly connects back toward the entrance/village side.
- A checkpoint/save allows a player to stop and resume without losing major
  progress.
- First-time completion is roughly 10–15 minutes, confirmed by playtest.
- A complete run can be started, completed, and repeated without a
  game-breaking bug.

## 7. Technical approach

### Planned source-file responsibilities

The previous all-in-one side-scroller was removed. Keep the replacement
incremental: the files below are the intended boundaries for the movement
test room, not a requirement to create empty modules in advance.

| File | Responsibility |
|---|---|
| `src/main.c` | Raylib startup/shutdown and top-level frame loop; delegate gameplay work |
| `src/tuning.h` | Centralized movement/camera/feel constants; tune one value per playtest pass |
| `src/input.h/.c` | Poll keyboard/gamepad once per frame and produce movement/action intent |
| `src/player.h/.c` | Player state, facing, eight-way movement, and solid collision |
| `src/room.h/.c` | Test-room layout, blocked geometry, world bounds, and explicit ramp/elevation connector |
| `src/camera.h/.c` | Follow the player while clamping the view to room bounds |
| `src/render.h/.c` | Pixel-scaled scene drawing, ground/entity/foreground ordering, feet-point Y sorting, and one occluder |
| `CMakeLists.txt` | Add each implementation file to the executable as it is introduced |

Start with the smallest buildable set and add a module when its responsibility
is implemented. Do not create interaction, save, combat, audio, or generic
engine modules for this movement-room milestone. Keep shared types local until
more than one module genuinely needs them.

### Movement foundation

The top-down controller is not implemented. Build eight-way normalized
walking, stable wall collision, a feet-point Y-sort anchor, one explicit
elevation connector, bounded camera movement, keyboard/gamepad parity, and a
foreground occlusion test. Do not carry over platformer jump mechanics as
required top-down controls.

### World and persistence targets

- Build a small set of rooms/areas connected by explicit exits before an
  enormous continuous world.
- Keep persistent world state separate from drawn objects; use stable IDs for
  clues, gates, chests, and checkpoints.
- Save version, current area/checkpoint, and major progression. Validate data
  on load; a failed save load must not crash or silently claim progress was
  restored.
- Use a user data directory (Windows Local AppData) when persistence is
  implemented, not a fragile current-working-directory path.
- Choose a JSON library or format only when a save/load task is scheduled; do
  not add a dependency just for the planning document.

## 8. Ordered roadmap

Use the order below. Each numbered task is a separate bounded AI work session
unless it genuinely fits with its direct prerequisite. Stop at the gate and
verify before continuing.

### Phase 0 — Reliable baseline

- Configure and build Raylib + Palm Island with MSYS2 MinGW64.
- Launch the executable and verify it remains open.
- Keep generated CMake files under `build/`.

**Gate:** M1–M2 Windows MinGW64 configure/build and launch passed on
2026-10-14. Configure reported an upstream Raylib CMake deprecation warning;
the game sources compiled without warnings.

### Phase 1 — Prototype the selected movement direction (in progress)

1. Keep the top-down movement room buildable as it grows one task at a time.
2. In a bounded test room, implement eight-way normalized input, facing,
   stable solid-wall collision, room camera bounds, and one explicit
   ramp/elevation transition.
3. Add foot-point Y sorting and one foreground occluder to validate the
   3D-like depth cue.
4. Use the starting values in `docs/GAME_FEEL_AND_SIGNATURE_PLAN.md`; keep
   tunables in `src/tuning.h` and make one tuning change per playtest pass.
5. Extend the F3 debug overlay with frame time, feet collider, and active
   tuning; playtest keyboard and controller.

**Gate (Feel Gate):** pass all ten measurable checks in
`docs/GAME_FEEL_AND_SIGNATURE_PLAN.md` section 7, including diagonal speed
within 1%, wall sliding, 16 px doorway entry, stopping within 0.08 s, stable
camera/frame timing, keyboard/controller parity, correct depth sorting, a safe
ramp, and an unprompted two-minute wander test. Record results here before
starting the Jungle blockout, combat, or signature mechanics.

### Phase 1 micro-tasks

Do one bounded task per session; build and verify before continuing:

| # | Task | Visible result |
|---|---|---|
| M1 ✅ | Add 320×180 render target, point filtering, integer scale/letterbox, and a tile-room blockout | Pixel-scaled room; Windows build/launch verified |
| M2 ✅ | Load the player sheet and implement normalized eight-way movement using `tuning.h` | Movement integrated; interactive speed test pending |
| M3 ✅ | Add a 10×6 feet collider, axis-separated collision, and 4 px corner assist | Automated obstacle, bounds, wall-slide, corner-assist, and stair-approach checks pass |
| M4 ✅ | Add bounded camera follow, look-ahead, and render-time pixel snapping | Camera behavior tests pass; interactive feel pending |
| M5 ✅ | Add foot-Y sorting for palm/pillar props and one foreground occluder | Sort and foreground-order tests pass; visual playtest pending |
| M6 | Add the explicit ramp/elevation connector | Visible, safe elevation change |
| M7 | Add controller dead-zone and analog-magnitude behavior | Controller matches keyboard movement |
| M8 | Add F3 debug data and run the Feel Gate | Measured pass/fail is recorded |

Dimensions, speeds, and collision/camera numbers in the feel plan are starting
hypotheses; tune them by playtesting rather than treating them as final balance.

### Phase 2 — Add only necessary boundaries

Add or refine the planned source modules as the movement-room responsibilities
are implemented. Do not extract placeholder behavior or introduce unrelated
game systems.

**Gate:** each new module builds cleanly and keeps its behavior within its
documented responsibility.

### Phase 3 — Block out the top-down Jungle route

1. Build a compact, intentionally traversed set of connected top-down zones.
2. Block out Jungle Entrance, Root Path, Overgrown Clearing, Shrine/Arena,
   and return shortcut with placeholders.
3. Place a visible thorn gate early and make the route geometry support
   backtracking.
4. Add clear stairs/ramps where elevation changes; tune Y-sort/occlusion,
   camera boundaries, room transitions, and reliable checkpoint respawn.

**Gate:** route is navigable end-to-end with no ambiguous elevation, blocked
exits, or softlocks. Do not add the full island map.

### Phase 4 — Interaction and clue prototype

1. Create a small interactable representation with stable IDs.
2. Add prompt/display behavior for a clue and the Map of Roots story beat.
3. Track clue state in one game session; make the interaction repeat behavior
   intentional.
4. Add one optional secret that the environment hints at.

**Gate:** player can identify what to inspect, receive clear feedback, and
understand how the clue relates to the route.

### Phase 5 — Thornfang ability and world change

1. Add Thornfang as an explicit progression flag.
2. Implement a thorn gate that blocks the player before the ability.
3. Demonstrate acquisition and an immediate first use.
4. Revisit and cut the earlier gate; open a shortcut and reveal one optional
   reward.

**Gate:** the same gate changes from impassable to passable because of
progression, and the shortcut is traversable both ways as designed.

### Phase 6 — Minimal encounter

1. Only after movement and the route work, add the smallest enemy interaction
   that supports pacing.
2. Add Thornback as an observation-first guardian, with clear telegraphs and
   recovery.
3. Connect victory to shrine access and Thornfang acquisition.
4. Tune hitboxes, damage, failure, checkpoint return, and encounter length.

**Gate:** multiple playtests find the fight readable and fair; no progression
can be granted incorrectly or permanently blocked.

### Phase 7 — Persistence and vertical-slice completion

1. Save checkpoint, clue/gate state, guardian completion, and Thornfang.
2. Test new game, save, quit, relaunch, and resume. Test absent and invalid
   save files safely.
3. Add pause/settings and minimal audio/art needed for clarity and atmosphere.
4. Playtest for the 10–15 minute target; revise confusing or slow beats.

**Gate:** satisfy every slice acceptance criterion in section 6. Call this a
vertical slice only after an end-to-end player test, not on feature count.

### Phase 8 — Expand the game

Only after the slice succeeds: decide production scope, build shared region
and persistence tools incrementally, then make the remaining four regions.
Each new region must deliver a distinct traversal expression and story reveal.
Do not use the old six-month estimates as commitments.

## 9. Visual / audio direction

- Palette: deep teal and ink shade, layered moss/jade foliage, warm pale amber,
  aged stone, sea mist, restrained gold/relic glow.
- Composition: foreground, playable midground, and distant background with
  layered parallax. In top-down scenes, use foot-point Y sorting, canopy/roof
  occlusion, and explicit elevation transitions. Show a landmark before the
  player reaches it. Protect player silhouette/readability.
- Ichigo: tousled dark hair, short travel coat, red scarf responding to wind
  and motion, compact travel gear. Prioritize idle, eight-direction walk,
  interact/inspect, push/pull, hurt, and weapon-use states before elaborate
  combat animation. Jump/fall/land remain legacy prototype animations unless a
  later design requires a top-down vault.
- Keep contrast/glow for meaningful clues and interactions; avoid filling
  every screen with visual noise.
- Use a consistent licensed art style. Concept sheets are references; do not
  slice concept artwork into production animation frames.
- Add music and effects after route readability and interaction feedback are
  established; audio supports pacing, it does not substitute for it.

## 10. Test and definition-of-done policy

For every code task:

1. Build using the README Windows commands (or a relevant narrower check plus
   build).
2. For visual/input/physics work, launch the game and exercise the changed
   behavior.
3. Recheck the behavior that was already working and could be affected.
4. Report any validation not performed.

Before calling a phase complete, check:

- startup/shutdown and no crash;
- collision and respawn around the changed area;
- progression/gates with and without the required ability;
- keyboard and gamepad if input changed;
- state persistence and invalid/missing save if save code changed;
- no accidental dependency on absolute machine paths.

The current project has no automated gameplay test suite. Do not claim
automated coverage exists. Add small testable helpers/tests when the code
architecture makes them worthwhile.

## 11. AI memory protocol and reusable prompts

### End-of-session checkpoint

Before stopping after a meaningful task, update:

- **Current Status:** only newly verified implementation facts.
- **Next Work Item:** one concrete task, bounded enough for a fresh/limited
  context model.
- **Decision log:** any approved design/technical choice and reason.
- **Build result:** command and pass/fail; include runtime result for interactive
  changes.
- **Known issues:** reproducible blockers only.

Do not add speculative tasks to the verified-status section. Keep completed
roadmap items marked as complete or remove stale wording.

### New-session prompt

> Read `AGENTS.md`, then read `docs/PROJECT_MEMORY.md` sections Current Status,
> Locked Decisions, and Next Work Item. Work only on the Next Work Item. Inspect
> relevant code before editing, preserve existing behavior, build and run the
> smallest relevant validation, and update project memory with verified
> results. Do not implement later phases.

### Bounded-task prompt

> Task: [one behavior or defect]. Relevant files: [paths]. Preserve: [existing
> behavior]. Acceptance checks: [observable conditions]. Do not change:
> [scope guard]. First inspect current implementation; then make a surgical
> change, build, and verify. Report changed files, commands/results, and any
> remaining issue.

### Handoff format

```text
Completed:
Changed files:
Validation (exact commands and outcome):
Runtime/playtest result:
Known issue:
Next work item:
Memory updated: yes/no
```

## 12. Current next work item

**Playtest the two linked blockout areas** in the Windows build. Confirm the
Entrance loop and shrine landmark are legible; the Root Path side route clearly
bypasses the closed thorn gate; the two fades and spawn/camera resets are
seamless; and the palms, tall grass, bridge, and ramp read and collide
correctly. Automated build/tests pass, but this visual playtest remains
outstanding. Do not add combat, the guardian, Thornfang, or signature mechanics.

### Feel Gate status (2026-10-14)

| # | Check | Status | Evidence / remaining verification |
|---|---|---|---|
| 1 | Diagonal speed | Pass | Automated: 76.000 vs 76.000 px/s; 0.000% difference |
| 2 | Wall slide | Pass | Automated: 53.740 px/s tangential (70.7% of 76 px/s); user reports full feel check passed |
| 3 | Corners | Pass | User reports the doorway/corner check passed, including the all-angle approach requirement |
| 4 | Stopping | Pass | Automated: 0.050 s and 1.583 px drift; user reports full feel check passed |
| 5 | Camera | Pass | Unit checks cover bounds, smoothing, initialization, and render rounding; user reports visual check passed |
| 6 | Frame time | Pass | User reports the frame-time check passed; exact 60/144 Hz and stall measurements were not supplied |
| 7 | Input | Pass | User reports keyboard/controller parity and stick-at-rest check passed |
| 8 | Depth | Pass | Unit checks cover stable ordering; user reports visual palm/pillar occlusion check passed |
| 9 | Ramp | Pass | Automated transition/collision checks plus user-reported visual and safety check |
| 10 | Two-minute wander | Pass | User reports the unprompted wander test passed |

Checklist for the Feel Gate playtest:

1. Run the game with keyboard and controller where available; verify the camera
   scrolls in all directions and stays inside room bounds.
2. Check all ten measurable Feel Gate requirements in
   `docs/GAME_FEEL_AND_SIGNATURE_PLAN.md` section 7.
3. Record which checks pass, fail, or need adjustment before starting the
   Forgotten Jungle blockout.

**Completion record:** On 2026-10-14, the user reported that all ten Feel Gate
checks passed, including the hands-on and human tests. This closes the gate
for planning purposes; detailed results for the manual checks were not
provided. The earlier automated baseline measurements for diagonal speed,
wall slide, and stopping are recorded above.

On 2026-10-14, M1–M2 were clean-configured and built
with MSYS2 MinGW64. The executable launched and remained open with the expected
window title. Raylib configure emitted an upstream CMake deprecation warning;
game sources built without warnings. Uploaded screenshots are reference
captures, not screenshots from this Windows verification. Interactive
movement, controller input, and visual asset loading were not independently
checked in that earlier assistant verification.

## 13. Consolidation and reference map

These active planning documents were merged into this file on 2026-10-14 and
reduced to redirects to prevent multiple sources of truth:

- `REDESIGN_PLAN.md` — current tone, visual direction, architecture, milestone
  strategy.
- `PALM_ISLAND_GAME_PLAN.md` — story, world progression, weapons, combat, and
  broad design.
- `DECISIONS.md` — locked design decisions.
- `FORGOTTEN_JUNGLE_DESIGN.md` — detailed first-slice route and acceptance
  criteria.

Other references:

- `ASSET_INDEX.md` — concept art, starter sprites/tiles, generator, and source
  archive inventory.
- `MASTER_GAMEPLAY_BLUEPRINT.md` — current detailed top-down game proposal:
  campaign zones, player tasks/progression, inventory/weapons/map, obstacles,
  enemy/guardian concepts, exploration subtasks/rewards for every zone,
  character/health/weapon switching/food/shop/fixed tool-building/costume
  proposals, and feature implementation stages.
- `GAME_FEEL_AND_SIGNATURE_PLAN.md` — movement/camera starting targets,
  measurable Feel Gate, polish tiers, and signature-mechanic proposals. The
  proposals require approval; the tuning values require playtesting.
- `source/backups/m1_m2-upload/` — original uploaded M1–M2 patch and archive;
  `media/m1_m2/` — uploaded screenshot references.
- `source/ORIGINAL_GAME_DESIGN.txt` and older source documents — historical
  ideas and technical reference; they contain earlier scope and combat
  assumptions that may conflict with the current exploration-first direction.
- `archive/` and `source/backups/` — retained historical/backed-up documents,
  not active plans.
