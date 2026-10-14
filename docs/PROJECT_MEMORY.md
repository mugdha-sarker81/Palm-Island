# Palm Island — Project Memory and Incremental Build Plan

**Purpose:** This is the canonical, compact-enough handoff for developers and
AI assistants, including models with limited context or usage. It merges the
current README and active planning documents into one working source of truth.
The map-by-map implementation blueprint is maintained in
`docs/MASTER_GAMEPLAY_BLUEPRINT.md`, including the detailed player
exploration/subtask sequence for all 24 proposed zones.

**Last reconciled:** 2026-10-14
**Current milestone:** playable movement-and-atmosphere prototype
**Next milestone:** prototype the selected top-down movement direction in a
separate test room, then make the Forgotten Jungle vertical slice—not the whole
island.

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
4. The exact code/files touched by the assigned task.
5. One relevant design detail section if needed.

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
- The resulting `build\PalmIsland.exe` was launched and remained running.
- Use MSYS2 MinGW64, not the unrelated legacy `C:\MinGW` compiler. If the
  generator differs from the existing CMake cache, remove the generated
  `build` directory and configure it again.
- This workspace was not detected as a Git repository at the time this was
  written. Do not claim a commit/history exists; establish Git separately
  before relying on version-control history.
- No automated gameplay test suite is configured.

### Implemented in the current prototype

- One C source file, `src/main.c`, with a Raylib game loop.
- A 1280×720 window and a code-drawn, 5,200-unit horizontal coast.
- Basic horizontal movement, gravity, platform collision, acceleration,
  coyote time, jump buffering, variable jump height, and collision substeps.
- Camera following, parallax scenery, code-drawn player, HUD, and a marker
  interaction that displays a field-note message.
- Keyboard and gamepad input paths for movement, jump, and marker interaction.
- `R` resets the prototype; Escape closes it.

### Not implemented yet

- The complete game or the 10–15 minute vertical slice.
- The selected top-down eight-direction game; the current executable is still
  the earlier side-scrolling prototype and is not evidence that the new
  direction is implemented.
- Combat, enemies, health, boss/guardian behavior, or Thornfang.
- Driftwood Village, rooms/region transitions, authored maps, or a level editor.
- Production character/environment art, animation sheets, music, or sound.
- Dialogue system, inventory, persistent clue/world state, checkpoints, or
  save/load.
- Character health, food/shop, weapon switching UI, material pickups, fixed
  built tools, and costumes. Their design is specified in the gameplay
  blueprint but no such feature or production sprite is implemented.
- Menu, pause/settings, release packaging, or automated gameplay tests.

The concept images and ZIP archives in `assets/` are references, not assets
loaded by the current game. Consult `docs/ASSET_INDEX.md` and review licenses
before using third-party assets.

## 3. Product direction

The chosen full-game camera is now **top-down 2D pixel art with 3D-like depth
cues** (layered scenery, Y-sorting, occlusion, and explicit elevation links).
This supersedes the earlier side-scrolling full-game direction. The current
side-scrolling executable is a legacy prototype to preserve until a replacement
top-down movement test room is verified.

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
  target for the full game. Preserve the existing side-scrolling build as a
  known-good prototype while developing a separate top-down test room.
- **2026-10-14:** User approved character/HP/weapon-switching/food/shop/tool/
  costume planning and selected a small fixed set of material-built tools,
  not full recipe crafting. Starting balance values remain proposals pending
  playtesting; none of these systems is implemented.

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

### Keep the code small, then split when needed

The current prototype is intentionally one file. Do not create a full custom
engine up front. As responsibilities grow, extract and validate one boundary
at a time:

1. `main.c` — startup, main loop, high-level state.
2. `player.c/.h` — player state, input-facing movement, collision.
3. `world.c/.h` — room/region geometry, triggers, persistent world flags.
4. `interaction.c/.h` — interactables, prompts, clue discovery.
5. `save.c/.h` — versioned persistence/checkpoints when progression exists.
6. Add UI, audio, combat/enemy, and content modules only when the slice needs
   them.

Preserve the current playable build while moving code. Keep input/update/draw
separable enough that physics changes do not require rebuilding unrelated
systems. Do not design data formats or abstractions for unimplemented full-game
content prematurely.

### Movement foundation

The **existing side-scroller** has acceleration, ground/air control, drag,
gravity, coyote time, jump buffering, variable jump height, substep collision
handling, and camera easing. Preserve it as a known-good reference. The
**selected full game** needs a new top-down controller: eight-way normalized
walking, stable wall collision, Y-sorted feet anchor, elevation connectors,
room transitions, camera bounds, keyboard/gamepad parity, and occlusion tests.
Do not carry over coyote-time/jump mechanics as required top-down controls.

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

### Phase 0 — Reliable baseline (currently achieved)

- Configure and build Raylib + Palm Island with MSYS2 MinGW64.
- Launch the executable and verify it remains open.
- Keep generated CMake files under `build/`.

**Gate:** build and launch succeed from documented commands. Recheck after
changes to build files or compiler-facing code.

### Phase 1 — Prototype the selected movement direction (next)

1. Preserve the existing side-scroll build as a baseline; do not destroy the
   only working executable/source while evaluating the new camera.
2. In a bounded test room, implement eight-way normalized input, facing,
   stable solid-wall collision, room camera bounds, and one explicit
   ramp/elevation transition.
3. Add foot-point Y sorting and one foreground occluder to validate the
   3D-like depth cue.
4. Playtest keyboard and a connected gamepad; record reproducible movement or
   collision defects and fix one at a time.

**Gate:** the player can approach obstacles from all eight directions, move
through the test room and elevation connector without speed bias, jitter,
penetration, or softlock. Do not start combat or full content before this gate.

### Phase 2 — Refactor only the first necessary boundary

1. Extract player movement/collision from `main.c` into `player.c/.h`.
2. Preserve behavior exactly; avoid simultaneously redesigning physics.
3. Update CMake and build.
4. Compare play behavior before/after extraction.

**Gate:** same prototype behavior, clean build, no new warnings/errors from the
change. Defer further modules until another responsibility is actually needed.

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

**Create and playtest a separate top-down movement test room** with normalized
eight-direction movement, camera bounds, solid wall collision, and one visible
ramp/elevation transition. Preserve the legacy side-scroller until this is
verified. Do not start combat or build all regions yet.

Checklist for that session:

1. Keep the current build runnable as the known-good legacy prototype.
2. Prototype on a separate branch or safely staged code path; do not delete
   the only working version before the new direction builds.
3. Test cardinal/diagonal speed, wall sliding, corners, narrow passages,
   elevation transition, occlusion, world bounds, and frame-time stability.
4. Test keyboard and a connected controller if available.
5. Record reproducible issues; fix only the top verified issue in a follow-up.

**Completion record:** On 2026-10-14, the Windows target was clean-configured
and built with MSYS2 MinGW64 and launched successfully. Interactive playtesting
of movement is not recorded yet.

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

- `ASSET_INDEX.md` — current concept art and ZIP archive inventory.
- `MASTER_GAMEPLAY_BLUEPRINT.md` — current detailed top-down game proposal:
  campaign zones, player tasks/progression, inventory/weapons/map, obstacles,
  enemy/guardian concepts, exploration subtasks/rewards for every zone,
  character/health/weapon switching/food/shop/fixed tool-building/costume
  proposals, and feature implementation stages.
- `source/ORIGINAL_GAME_DESIGN.txt` and older source documents — historical
  ideas and technical reference; they contain earlier scope and combat
  assumptions that may conflict with the current exploration-first direction.
- `archive/` and `source/backups/` — retained historical/backed-up documents,
  not active plans.
