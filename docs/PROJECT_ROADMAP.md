# Palm Island — Project Roadmap

**Purpose:** One ordered, prompt-sized execution plan from the current
prototype to a complete Windows single-player campaign.

## Start here

- **Current truth:** [Project Memory](PROJECT_MEMORY.md) owns verified status,
  locked decisions, playtest evidence, and the immediate next task.
- **What to build:** [Master Gameplay Blueprint](MASTER_GAMEPLAY_BLUEPRINT.md)
  owns campaign, zone, task, obstacle, enemy, and guardian design.
- **How it should feel:** [Game Feel and Signature Plan](GAME_FEEL_AND_SIGNATURE_PLAN.md)
  owns movement targets, feel gates, and unapproved signature proposals.
- **Assets:** [Asset Index](ASSET_INDEX.md) lists art, references, and
  generators. Starter art is placeholder work, not production content.
- **Historical material:** [source/](source/README.md) preserves prior plans
  and supplied documents. It is not active direction.

If documents disagree, use Project Memory for current status and decisions,
this roadmap for work order, the Blueprint for approved design detail, and the
Feel Plan for feel tasks. A proposal in a design document is not approval to
implement it.

## Vision

Palm Island is a quiet, top-down, eight-direction 2D exploration adventure
about noticing, interpreting, and understanding a mysterious island. The
player explores readable spaces, follows environmental clues, learns a small
set of distinct weapon abilities, and changes the world through discoveries.
Combat is occasional and teachable; exploration and comprehension are the
main rewards.

The intended campaign has **8 regions and 24 authored zones**, five permanent
legendary weapons, and a final Sanctuary. The player can approach the four
middle weapon regions in flexible order after the Forgotten Jungle. The
Sanctuary stays locked until the required weapons and story progress are
complete. This is the destination, not the current playable scope.

### Non-negotiable project constraints

- Windows PC, offline, single-player; C11, CMake, Raylib 5.5.
- Top-down pixel art, 320×180 virtual canvas, 16×16 tiles, keyboard and
  controller parity, fixed-step simulation, readable depth/occlusion and
  explicit elevation links.
- Build one playable increment at a time. Keep each coding prompt to one
  bounded subphase and its direct tests.
- Preserve working play behavior. Test both the exact behavior changed and
  the Windows target.
- No generic crafting/grinding, XP levels, broad enemy roster, or large
  frameworks without an approved gameplay need.
- Treat proposed balance values as starting points. Keep tuning centralized
  and change one feel value per tuning pass.
- Do not add production art/audio until the asset plan and usage/licensing are
  clear. Never use concept-sheet crops as finished runtime art.

## Campaign overview

| Region | Zones | Main purpose / reward | Planned status |
|---|---|---|---|
| 1. Palmshore Beach | Wreckshore; Tidepool Path | Arrival, family-symbol clue, path to village | Not playable |
| 2. Driftwood Village | Village Square; Harbor Row | Safe hub, keeper/rumors, Jungle route | Not playable |
| 3. Forgotten Jungle | Jungle Entrance; Root Path; Overgrown Clearing; Shrine Approach / Agility Shrine | First full slice; Thornfang | Blockout and Thornback prototype exist |
| 4. Ember Caverns | Ashfall Descent; Forge Veins; Buried Forge | Heat-flow clues; Embercleaver | Ashfall prototype exists |
| 5. Sunken Temple | Flooded Court; Resonance Hall; Tide Engine | Water/resonance puzzles; Stormpiercer | Not playable |
| 6. Skywatch Cliffs | Windward Steps; Mirror Walk; Signal Nest | Wind/light puzzles; Dawnstring | Not playable |
| 7. Lost Kingdom | Outer Archive; Silent Courtyard; Mirror Gallery / Shadow Causeway; Hollow Hall | Conflicting history; Voidfang | Not playable |
| 8. Excalibur Sanctuary | Outer Threshold; Memory Gallery; Quiet Core | Final revelation and temporary Excalibur | Not playable |

The runtime campaign topology and full map exist as metadata; map visibility
does not mean a zone is playable. Read the matching region section in the
Blueprint before implementing its content.

## How to execute a subphase

Every subphase below is designed to fit one focused coding prompt:

1. Read Project Memory's **Current Status**, **Locked Decisions**, and
   **Next Work Item**, then read only the relevant Blueprint/Feel sections.
2. Inspect existing implementation and tests; name the exact files and
   behavior in scope.
3. Implement only the named subphase. Do not start the next subphase or add
   speculative content.
4. Add or update focused automated tests. Run them, build with `.\build.bat`,
   and do any specified hands-on check before calling the gate passed.
5. Update Project Memory with what is verified, what remains unverified, and
   the next subphase. Keep user-reported playtest results labeled as such.

For every playable subphase, include a small usability check: can a player
identify the goal, the available action/control, the result, and a safe next
step if they fail? Observe rather than coach; record confusion points and fix
the earliest cause before adding more on-screen instructions. The Blueprint's
**Player UX and in-game teaching** section contains the detailed rules.

Suggested prompt ending:

> Implement only subphase **[ID]** from `docs/PROJECT_ROADMAP.md`. Preserve
> existing behavior, follow the matching Blueprint section, run focused tests
> and `.\build.bat`, update `docs/PROJECT_MEMORY.md`, and stop at this
> subphase's boundary. Report any required hands-on checks without claiming
> they passed.

Do not advance through a phase just because its code compiles. Its stated gate
must pass first.

## Phase 0 — Validate the existing prototype

**Goal:** finish the current manual playtest gates before expanding the
campaign. **Current status:** automated build/tests are recorded as passed;
the hands-on checks below remain open in Project Memory.

- [ ] **0.1 — Verify campaign map and trial locator.** Open both map views in
  play; check labels, playable/planned/current distinction, player marker,
  locator visibility, and that no UI obscures play.
- [ ] **0.2 — Playtest Palmwood Club and Ash Skitter.** Check the warning is
  legible, dash can be evaded, elevation is safe, range/facing/recovery feel
  fair, and defeat/reset behaves clearly.
- [ ] **0.3 — Playtest Root Mite and reward.** Check nest/sleep readability,
  pause-before-lunge cue, lunge fairness, elevation safety, death reset/HUD,
  and collection from the walkable row north of the nest while facing south.
- [ ] **0.4 — Reconcile results.** Fix only confirmed blockers; record
  observations and retest. Mark the gate passed only when all three checks
  are reported and unresolved blockers are listed.

**Gate:** map, locator, Ash Skitter, and Root Mite checks are either passed or
the user explicitly accepts an unresolved issue and it is recorded with an
owner and follow-up. A discovered blocker may require its own bounded fix and
retest subphase. Do not begin Phase 1 until the gate is explicitly resolved.

## Phase 1 — Complete Ember Caverns

**Goal:** continue from the existing Ashfall Descent prototype and build one
zone increment at a time. Reference Blueprint sections 4.2–4.3 and Region 4.
Do not add the Forge Warden or Magma Golem until the subphase gates allow it.

### 1A — Forge Veins

- [ ] **1.1 — Forge Veins route blockout.** Add only the greybox zone, safe
  entry/exit spawns, return route, collision, and checkpoint. Confirm
  reachability and no one-way trap.
- [ ] **1.2 — Teach heat channels.** Implement one readable, resettable channel
  interaction and its visible feedback. Do not create a generic puzzle engine.
- [ ] **1.3 — Open the dormant lift.** Connect the solved channel to one world
  change and a safe path forward; cover unsolved/solved/reset behavior.
- [ ] **1.4 — Tablet clue and persistence.** Add the broken-tablet discovery
  with a stable ID, review behavior, and versioned-save round trip if it
  changes persistent progress.
- [ ] **1.5 — Side loop and shortcut.** Add the authored optional loop and
  shortcut only after the main route is playable; test both directions.
- [ ] **1.6 — Forge Veins playtest gate.** Test puzzle comprehension, reset
  recovery, checkpoint, return route, accessibility of feedback, and save /
  reload. Fix blockers before adding a new enemy.

### 1B — Forge Warden and Buried Forge

- [ ] **1.7 — Forge Warden prototype.** Only after 1.6 passes, build the
  single standard enemy lesson described by the Blueprint. Test its fairness
  and ensure it cannot follow the player into safe rooms.
- [ ] **1.8 — Buried Forge route and arena greybox.** Add the final cavern
  zone, paired transitions, nearby checkpoint, safe arena boundaries, and
  return route. No boss yet.
- [ ] **1.9 — Magma Golem encounter.** Implement the slam tell/recovery first;
  add the slow wave and pillar-safe response as a separate bounded increment.
  Use explicit hitboxes and avoid untelegraphed damage.
- [ ] **1.10 — Embercleaver reward and world change.** Grant the weapon after
  victory, implement its approved cracked-stone/weight-switch uses, and
  persist reward and opened path.
- [ ] **1.11 — Caverns integration gate.** Test all three zones, routes,
  checkpoints, death/retry, weapon interaction, save migration/round trip,
  and hands-on encounter fairness.

**Gate:** the cavern route is completable, recoverable, understandable, and
persists major progression. Do not build another weapon region before the
hands-on gate.

## Phase 2 — Build the campaign opening

**Goal:** make the playable campaign start before the current Jungle slice.
Reference Blueprint Regions 1–2. Keep village interaction small and authored;
do not build a generic dialogue/economy system.

### 2A — Palmshore Beach

- [ ] **2.1 — Wreckshore blockout.** Add room shell, wreck collision, landmark
  path, spawn/exit, and safe bridge/ramp.
- [ ] **2.2 — Family-symbol discovery.** Add the inspectable marks and one
  journal/task state; confirm the required clue is not gated by optional lore.
- [ ] **2.3 — Tidepool Path.** Add the second beach zone, readable shallow /
  deep-water boundary, optional tidepool clue, and safe route toward the
  village. No real-time tide timer.
- [ ] **2.4 — Beach route/save gate.** Test both directions, spawns, map
  discovery, checkpoint behavior, and clue persistence.

### 2B — Driftwood Village

- [ ] **2.5 — Village Square blockout.** Add hub routes to beach and Jungle,
  solid props/NPC positions, checkpoint, and readable visible locked gates.
- [ ] **2.6 — Keeper and one required lead.** Add the smallest authored
  conversation/interaction that explains the Jungle route. Avoid broad
  dialogue tools and optional branching until needed.
- [ ] **2.7 — Harbor Row.** Add its beach return, noticeboard, optional
  boatwright context, and preview of the locked central route. Defer shops and
  upgrades.
- [ ] **2.8 — Opening campaign gate.** Play from new game through village into
  Jungle and back. Confirm instructions, map, task state, checkpoint, and
  save/load are understandable.

**Gate:** a fresh player can start at the shore, understand why to visit the
Jungle, and return to the hub without a softlock.

## Phase 3 — Build the Sunken Temple

**Goal:** deliver Region 5 and Stormpiercer after the opening and Caverns
gates. Reference Blueprint Region 5 and its exploration sequence.

- [ ] **3.1 — Flooded Court blockout and water rules.** Teach shallow slowdown,
  deep-water boundaries, high ground, route, entry/exit, and checkpoint.
- [ ] **3.2 — Echo Eel teaching encounter.** Add the single readable water-lane
  behavior only after the route is safe; test warning and retreat space.
- [ ] **3.3 — Resonance Hall puzzle.** Implement the clue-guided three-node
  sequence with feedback and reset; no hidden timing requirement.
- [ ] **3.4 — Sluice/world change.** Persist the opened route and changed water
  state. Test before/after reload and safe access.
- [ ] **3.5 — Tide Engine arena and guardian.** Build the Tide Warden's
  pulse/wave pattern, marked water lane, recovery openings, and nearby retry.
  Split the encounter into separate prompts if its pattern work is too large.
- [ ] **3.6 — Stormpiercer and region gate.** Grant the weapon, implement the
  approved matching-node ability, persist progression, and hands-on test the
  complete region.

**Gate:** puzzle order is understandable, water cannot trap the player, the
guardian is fair, reward works in and out of the temple, and save/reload is
safe.

## Phase 4 — Build Skywatch Cliffs

**Goal:** deliver Region 6 and Dawnstring. Reference Blueprint Region 6.

- [ ] **4.1 — Windward Steps.** Greybox sheltered route, explicit cliff bounds,
  checkpoint, and a visible gust cycle. Validate safe waiting/movement.
- [ ] **4.2 — Cliff Skimmer.** Add one shadow-marked dive lesson with safe
  space and test it independently.
- [ ] **4.3 — Mirror Walk puzzle.** Implement two-mirror alignment with
  immediate visible feedback and a safe bridge route. Do not require an
  unearned weapon.
- [ ] **4.4 — Signal Nest arena.** Greybox Sunhawk arena with recovery-safe
  space and checkpoint.
- [ ] **4.5 — Sunhawk and Dawnstring.** Add dive tell/recovery, then the
  secondary slow fan in a separate prompt; grant and persist Dawnstring.
- [ ] **4.6 — Bridge/signal world change and gate.** Implement the approved
  post-reward use and shortcut; test route, controller, save/load, and
  hands-on fairness.

**Gate:** all three zones and return routes work, the ranged ability has clear
feedback and cannot softlock progress, and encounter behavior is playtested.

## Phase 5 — Build the Lost Kingdom

**Goal:** deliver Region 7 and Voidfang while keeping story clues optional
unless explicitly identified as required. Reference Blueprint Region 7.

- [ ] **5.1 — Outer Archive.** Greybox the multi-weapon entrance, return route,
  checkpoint, and first archive clue. Show why a gate is locked.
- [ ] **5.2 — Archive Shade.** Add a slow, avoidable shadow approach after
  safe route teaching; test lit safe areas and retreat.
- [ ] **5.3 — Silent Courtyard.** Add missing-statue/footprint clues and one
  small line-of-sight puzzle; keep a clear route.
- [ ] **5.4 — Mirror Gallery / Shadow Causeway.** Build the connected branch,
  visible safe gaps, reveal interaction, and return-safe traversal. No blind
  jumps.
- [ ] **5.5 — Shade Knight.** Add the sentinel's facing cone and committed
  strike as a separately tested enemy lesson.
- [ ] **5.6 — Hollow Hall arena and guardian.** Add the Hollow Knight only
  after the zone route and checkpoint pass; prototype and playtest its
  observation/counter rules before extending the phase pattern.
- [ ] **5.7 — Voidfang and story resolution.** Grant/persist the weapon and
  its approved shadow-gate ability; make the guardian-truth revelation
  understandable without optional records.
- [ ] **5.8 — Kingdom region gate.** Test branch reconnection, weapon gates,
  encounter resets, all required story states, and save/load.

**Gate:** optional exploration enriches but does not obscure the main truth;
every gate has a safe return and every required route is recoverable.

## Phase 6 — Build Excalibur Sanctuary

**Goal:** deliver the three-zone finale only after the five permanent weapon
routes and required story flags are working. Reference Blueprint Region 8.

- [ ] **6.1 — Sanctuary access gate.** Require the approved weapons/story
  flags; show clear missing requirements and leave all prior regions
  accessible.
- [ ] **6.2 — Outer Threshold.** Greybox the final entry route, safe return,
  checkpoint, and convergence of known abilities.
- [ ] **6.3 — Memory Gallery.** Build the playable truth/revelation path using
  established visual language; do not add a new core mechanic solely for
  novelty.
- [ ] **6.4 — Quiet Core and Excalibur.** Implement the approved temporary
  final transformation and ending sequence. Excalibur is not a sixth
  permanent open-world weapon.
- [ ] **6.5 — Finale playtest.** Test incomplete-weapon rejection, complete
  campaign entry, ending, restart/continue behavior, and save integrity.

**Gate:** finale remains inaccessible too early, becomes accessible with the
correct state, and resolves the central story clearly.

## Phase 7 — Campaign-wide integration and completion

**Goal:** prove the authored campaign works as one game, not as isolated
regions.

- [ ] **7.1 — Region graph and shortcuts.** Verify all 24 zones, paired exits,
  unlock conditions, return routes, and map states against the Blueprint.
- [ ] **7.2 — Progression order permutations.** Test the permitted orderings
  of Ember Caverns, Sunken Temple, Skywatch Cliffs, and Lost Kingdom after
  Thornfang. Block impossible entries and guarantee escape from every
  entered zone.
- [ ] **7.3 — Save compatibility.** Test fresh save, each checkpoint, each
  weapon/gate/world change, malformed/missing saves, backup recovery, and
  supported old-version migration. Report failures explicitly.
- [ ] **7.4 — Full campaign route tests.** Test required progression,
  optional clue omission, defeat/retry, revisits, and finale unlock.
- [ ] **7.5 — Usability pass.** Give a new tester the game without coaching;
  record route confusion, softlocks, clue comprehension, and completion
  time. Fix blockers before adding optional content.

**Gate:** no progression softlocks or silent progress loss; the game can be
completed through the supported route permutations.

## Phase 8 — Production polish and Windows release

**Goal:** replace prototype presentation only when content and scope are
stable, then ship a clean, attributed Windows build.

- [ ] **8.1 — Production asset plan.** Decide visual style, asset list,
  creation/source, licenses, and attribution before replacing placeholders.
- [ ] **8.2 — Region art pass.** Produce one region at a time; preserve
  collision, silhouettes, occlusion, and hazard readability. Verify assets
  are original or correctly licensed.
- [ ] **8.3 — Audio pass.** Add concise ambience and interaction/telegraph/
  reward cues; ensure no required information is audio-only.
- [ ] **8.4 — Settings and accessibility.** Implement only approved settings
  (volume, display, readable text, reduced flashing/motion, and control
  options as scoped); verify keyboard/gamepad parity and pause behavior.
- [ ] **8.5 — Menus and usability cleanup.** Add title/new/continue/quit and
  pause/settings flows, with confirmation for destructive new-game actions.
- [ ] **8.6 — Clean Windows package.** Build and launch from a clean folder
  without development paths; include required assets/runtime files/licenses;
  test a fresh install and upgrade save.
- [ ] **8.7 — Release candidate gate.** Run the complete automated suite,
  Windows build, clean-folder smoke test, save migration test, controller
  check, and final new-player playtest. Publish only verified claims.

**Gate:** clean install and launch succeed, legal attributions ship, saves
survive upgrade, controls/settings work, and no critical progression defect
remains.

## Completion definition

The project is complete when all planned campaign regions/zones are playable,
all five permanent weapons and the temporary finale transformation work,
required progression can be completed and saved, the full campaign passes
playtesting without softlocks, and the Windows release passes the Phase 8
gate. A map marker, concept image, prototype, or automated unit test alone
does not count as a playable or hands-on-verified feature.
