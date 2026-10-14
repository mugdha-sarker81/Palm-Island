# Palm Island — Master Gameplay Blueprint

**Status:** Proposed full-game plan; not implemented
**Perspective:** Top-down 2D pixel-art world with a 3D-like sense of depth
**Campaign shape:** 8 regions, 24 authored map zones
**First build target:** a short, vertical slice of the Forgotten Jungle
**Technology target:** C11 + Raylib 5.5, Windows PC, offline single-player

This document turns the existing story, redesign, weapon, and jungle-slice
plans into a concrete map-by-map and system-by-system blueprint. Treat counts,
encounter details, and zone names below as the production proposal. Build only
the next small milestone; do not attempt the 24-zone game in one pass.

## 1. The game the player is meant to play

Ichigo arrives at Palm Island because a symbol inherited from his family
matches marks on the island. He explores a lived-in but isolated community,
follows evidence that contradicts the island's heroic legend, and learns that
five guardians and their weapons maintain or contain something. The reward for
exploration is understanding and new ways through the island—not XP farming.

### Core loop

1. **Notice:** see a landmark, unusual object, path, sound, symbol, or NPC clue.
2. **Approach:** navigate a readable top-down space using paths, bridges,
   stairs, ramps, caves, shallow water, or a learned weapon ability.
3. **Interpret:** inspect a clue, compare it to journal/map knowledge, or
   observe a machine/enemy pattern.
4. **Experiment:** operate a mechanism, choose a route, avoid a hazard, or
   use the relevant weapon.
5. **Discover:** reveal a clue, shortcut, safe route, optional story, or
   guardian weakness.
6. **Change the world:** open a route, restore a mechanism, gain a weapon, or
   update village knowledge. Record important progress.

The game should remain understandable without completing optional lore.
Combat exists to add tension and teach observation, not to occupy every room.

## 2. Scope and map count

The proposed complete main campaign has **8 regions containing 24 authored
zones**. “Zone” means a small room/map chunk with clear entrances and exits,
not a separate huge level or a full-screen stage. Regions share a world map and
reconnect through doors, paths, caves, bridges, and shortcuts.

| # | Region | Zones | Main reward |
|---|---|---:|---|
| 1 | Palmshore Beach | 2 | Reach Driftwood; family-symbol clue |
| 2 | Driftwood Village | 2 | Hub, rumors, jungle route |
| 3 | Forgotten Jungle | 4 | Thornfang |
| 4 | Ember Caverns | 3 | Embercleaver |
| 5 | Sunken Temple | 3 | Stormpiercer |
| 6 | Skywatch Cliffs | 3 | Dawnstring |
| 7 | Lost Kingdom | 4 | Voidfang |
| 8 | Excalibur Sanctuary | 3 | Final revelation, temporary Excalibur |
|  | **Total** | **24** | Five permanent legendary weapons |

After Thornfang, regions 4–7 may be attempted in any order. Show readable
gates for missing abilities and ensure multiple routes remain available; do
not allow the player to enter an area they cannot leave. The Sanctuary remains
locked until all five weapons and mandatory story flags are present.

**Scope warning:** 24 zones describe the eventual campaign, not the first
release milestone. Implement and playtest one region slice before building
other regions. A complete 24-zone campaign is a long-term solo project.

## 3. Camera, pixel presentation, and movement

### Camera / visual target

- Use a **top-down 2D camera**. A 3D-like feeling comes from layered art,
  shadows, height cues, and foreground occlusion—not a 3D engine.
- Render to a **320×180 virtual canvas** to match the starter art kit's 16×16
  tiles and 14×20 player sprite. Use point filtering and integer scaling
  (4× at 1280×720), with letterboxing at other window sizes. Keep tile, sprite,
  and UI pixel grids consistent.
- Start with 16×16 logical ground tiles; larger trees, buildings, cliffs, and
  props occupy multiple tiles. Decide exact sprite/tile size after a visual
  movement prototype, before mass asset production.
- Render order: distant backdrop → ground → ground decoration → shadows →
  entities ordered by foot/ground Y → foreground canopy/roof occluders → world
  effects → HUD and menus.
- Define each character's “feet point” (bottom-center) as its world anchor.
  Sort characters and props by that anchor so Ichigo walks behind/in front of
  trees and walls plausibly.
- Use discrete elevation layers for bridges, ledges, balconies, and cave
  walkways. A ramp/stair transition changes elevation only at authored
  connectors. Do not let overlapping 2D graphics imply traversable paths where
  there are none.
- Use landmarks, silhouettes, lighting, and paths to guide navigation. Avoid
  permanent quest arrows on the playfield.

### Movement

- Eight-direction free walking: up, down, left, right, and diagonals.
- Normalize diagonal input so it is not faster than cardinal movement.
- Keyboard: WASD or arrow keys; gamepad: left stick/D-pad.
- Keep acceleration/turn response responsive; use a small gamepad dead zone.
- Face/sprite direction follows the last meaningful movement or aim direction.
- Use walk, idle, interact, push/pull, hurt, dodge/roll (only if playtesting
  supports it), and weapon-use states. Do not copy platformer jump mechanics
  into the top-down controller by default.
- Add collision against solid tiles, objects, walls, and blocked map edges.
  Resolve movement predictably against horizontal/vertical walls; prevent
  tunneling if high speed or frame stalls are possible.
- Stairs are walkable ramps/connector tiles that move the player between
  elevation layers. Bridges and upper walkways use separate collision/sort
  planes. Doors and passages visibly indicate the correct entrance point.
- Crossing shallow water may slow movement or be blocked, depending on the
  region. Deep water, lava, void, and cliffs are blocked/hazardous and must be
  visually distinct.
- No free jump is required in the core top-down game. A vault/roll can be a
  later optional action, but it must not be needed to read basic navigation.
- Camera follows with gentle smoothing, room bounds, and no jitter. Transition
  between zones through door/path triggers, with a short fade and spawn
  position; preserve player facing where it helps orientation.

### Input/action proposal

| Action | Keyboard default | Controller |
|---|---|---|
| Move | WASD / arrows | Left stick / D-pad |
| Interact / talk / inspect | E | South face button |
| Attune (proposed; hold to sense nearby clues) | F | Left trigger |
| Primary weapon action | J or left mouse | West face button / right trigger |
| Dodge / quick step | Space or Shift | East face button |
| Open inventory | I / Tab | Y / North face button |
| Open map | M | View/Back |
| Journal / tasks | Q | Menu/Start |
| Pause | Esc | Menu/Start |

These are proposed bindings. Attune is a proposed signature mechanic from
`docs/GAME_FEEL_AND_SIGNATURE_PLAN.md`, not approved for implementation yet.
Test keyboard and controller before locking bindings; support remapping later
if feasible. Context-sensitive interaction should show the current binding.

## 4. Player tasks, sub-tasks, and progression

### Task categories and states

- **Main story task:** required to complete the campaign.
- **Region task:** required for that region's guardian/reward, sometimes
  optional to the overall story if that region is optional after the jungle.
- **Side task:** optional character/lore/reward, never blocks the main ending.
- **Discovery:** optional environmental finding, map note, or lore entry.
- **Tutorial task:** teaches an action and is marked complete once performed.

Each task has a stable ID, title, short description, parent task ID (optional),
prerequisite flags, objective list, completion rule, reward IDs, and journal
text. Example IDs: `main.find_family_mark`, `jungle.read_root_map`,
`jungle.restore_waterwheel`. Store objective completion separately so
multi-step tasks can resume. Mark tasks as hidden, available, active,
completed, or failed only when failure is meaningful. Avoid fake checklist
objectives that simply restate every walk to the next room.

### Main quest sequence

1. **The Mark from Home**
   - Inspect the symbol in Ichigo's possession.
   - Reach Palmshore using the family trail/old chart.
   - Find an identical mark on the wreck or shore stone.
   - Subtask: identify the safe path inland; reward: island-map outline.
2. **A Village That Remembers**
   - Reach Driftwood Village.
   - Speak to the keeper and one optional resident.
   - Compare the village legend with an old carved fragment.
   - Subtask: find the overgrown jungle entrance; reward: route added to map.
3. **The Roots Remember**
   - Follow the root markings in the Forgotten Jungle.
   - Read the Map of Roots clue.
   - Observe and pass Thornback's trial.
   - Earn Thornfang; cut an earlier thorn gate; return via shortcut.
4. **Four Voices of the Island**
   - Visit Ember Caverns, Sunken Temple, Skywatch Cliffs, and Lost Kingdom
     in any order.
   - In each: find its clue, understand its mechanism/trial, meet its guardian,
     earn its weapon, and activate the return route.
   - Subtask: return to the village or central landmark with evidence that
     challenges the original legend.
5. **The Legend Was Edited**
   - Use all five weapons on five corresponding island mechanisms (exact
     pattern learned from clues; avoid a random symbol hunt).
   - Reveal the Excalibur Sanctuary route.
   - Subtask: choose whether to share the discovery with the village before
     entering the finale (dialogue changes, not a required moral quiz).
6. **What the Island Kept**
   - Enter Sanctuary; complete three trials that test understanding of
     traversal, clues, and restraint.
   - Receive Excalibur temporarily for the ending sequence.
   - Resolve Ichigo's immediate family-symbol question; leave the island's
     origin partly mysterious.

### Player progression model

- No XP levels and no enemy-drop grinding.
- Major permanent progression: access flags for the five weapons, revealed
  routes, completed guardians, solved mechanisms, learned story facts.
- Optional upgrades: a small number of health increases, extra consumable
  capacity, or convenience improvements; none required for main completion.
- Skill comes from understanding enemy tells, safe paths, and interactions.
- Region challenge should be solvable with the region weapon and observation;
  do not make players grind damage.
- Death/failure returns to the last safe checkpoint, preserving major
  progression. Decide exact consumable penalty only after playtesting; default
  proposal: no permanent weapon/clue loss and no punishing resource loss.

### Checkpoint and fail rules

- Place a checkpoint at the region entrance, before a demanding guardian, and
  after a major reward where appropriate. Do not save in hazards or while an
  enemy is mid-attack.
- On respawn restore safe health/state and place the player at the checkpoint;
  keep collected weapons, clues, opened shortcuts, and solved permanent
  mechanisms.
- Guard against loops where a restored player spawns inside a wall, hazard, or
  locked gate.
- Each guardian arena has a clear exit and a reliable retry route.

## 5. Inventory, weapons, map, and journal

### Inventory

Keep the inventory small and legible:

1. **Weapons:** permanent collection of five weapons; one selected active
   weapon, with quick switch once multiple are acquired. Weapons are not
   consumed.
2. **Key items:** family symbol, clue fragments, map tokens, and story objects.
   Key items are not sellable/droppable.
3. **Consumables:** initially one healing item type; capacity starts small.
   Add more types only if they support a real tactical choice.
4. **Lore:** journal entries unlocked by inspecting clues or speaking with
   characters; entries preserve the actual found text and may add a short
   interpretation, clearly distinguished from fact.
5. **Optional upgrades:** small permanent upgrade count/slots; no gear rarity,
   loot affixes, freeform crafting tree, or large equipment grid in v1.

UI: separate tabs for Weapons, Items, and Journal. Display weapon description,
combat action, world interaction, and whether its ability is currently useful.
Pause movement/combat while full-screen inventory is open.

### Player character: Ichigo

**Role:** an observant outsider following a family symbol, not a chosen
conqueror. Age and detailed biography remain undecided; keep those out of
mechanical systems until narrative writing needs them.

**Personality in play:** curious, cautious but capable, empathetic toward
villagers and guardians. The player demonstrates this through inspect,
dialogue, and route choices, not a morality meter.

**Readable pixel-art silhouette:**

- Tousled dark hair, compact travel coat, and a red scarf that makes the
  character recognizable against foliage and stone.
- Small explorer's satchel, practical boots, simple wrist/hand wraps; one
  warm accent color. Keep details readable at the chosen internal resolution.
- A consistent feet-point anchor at the bottom-center of the sprite. Sort by
  this point so trees, roofs, and props correctly occlude the character.
- Eight directional facing: up, down, left, right, and four diagonals. If
  final art cannot support eight unique facings, use a tested four-direction
  base with clean diagonal movement rather than blurry sprite rotation.
- Core animation states: idle/breathe, walk, interact/inspect, push/pull,
  hurt, use item, each weapon action, and tool use. Add dodge/roll only if it
  survives movement/combat playtests.
- Keep a clear player outline in dark rooms, canopy shadows, and boss
  telegraphs. Scarf and idle details add life but do not obscure weapon tells.

### Health, damage, and recovery

**Starting tuning proposal (test, not final balance):**

| Rule | Initial proposal |
|---|---|
| Starting/max health | 5 hearts; optional permanent upgrades can raise maximum to 7 |
| Ordinary readable hit | 1 heart |
| Heavy/guardian hit | 1 heart; use 2 only for clearly telegraphed exceptional attacks |
| Non-lethal hazard touch | 1/2 heart with visible knockback/invulnerability feedback |
| Recovery after damage | brief, visible invulnerability; prevent repeated damage every frame |
| Healing food | one serving restores 2 hearts, capped at current maximum |
| Death/respawn | return to checkpoint with full health; keep weapons, clues, tools, opened routes |

Health appears as five clear pixel hearts in the HUD; upgraded hearts use a
consistent visual treatment. Show low-health feedback gently and accessibly;
do not flash the screen harshly. The initial numbers are intentionally small
and readable, not a promise of final difficulty.

Damage must be avoidable through reading tells, moving out of marked lanes,
and using cover or a learned response. No unavoidable contact damage from
decorative sprites. Boss difficulty comes from understanding the pattern, not
inflated health. Add difficulty/accessibility options only after default
encounters are playable and observed.

### Clothing and costume progression

**Default costume:** “Coast Wanderer” — dark tousled hair, blue-teal short
travel coat with warm trim, red scarf, brown satchel/boots, and practical
wraps. The design should look weathered and handmade, not like heavy armor.

- Silhouette remains stable when costumes change; scarf and player outline
  stay recognizable.
- Cosmetic unlocks are optional: e.g. village sash, rain cloak, explorer's
  scarf variant, ceremonial coat. Earn through discovery, NPC gratitude, or
  a shop; never require real-money purchases or stat grinding.
- Costumes do not change hitboxes, speed, health, weapon power, or access.
- The inventory has a simple appearance slot/preview only if it is inexpensive
  to implement; costume collection is not a mandatory v1 feature.
- Avoid mixing unrelated asset-pack art. All costumes need the same pixel
  scale, palette logic, facing directions, and animation coverage.

### Weapon switching and quick access

- Each earned legendary weapon stays permanently in the weapon inventory.
- One weapon is active at a time; the active weapon determines primary attack
  and can be used on its matching marked world interactions.
- Proposed controls: mouse wheel or number keys `1`–`5` to select owned
  weapons; controller shoulder buttons cycle owned weapons. Opening the
  inventory allows direct selection. Verify conflict-free bindings before
  locking them.
- Switching is immediate outside an attack animation and safely queued or
  rejected during a committed attack; never lose an input silently.
- Show active weapon icon/name and matching nearby gate feedback. A quick
  cycle skips unowned weapons.
- No weapon durability, ammunition, loss on death, or consumable charge for
  core exploration abilities.
- Weapon switching must be tested in safe rooms and combat; the inventory
  pauses gameplay, while quick switching does not.

### Pickups, food, shop, and fixed tool-building

**Pickup rules**

- Pickups are visibly represented in the world and use contextual interaction
  or automatic pickup only when deliberate and readable. Show a brief name,
  amount, and inventory-full message.
- Main-story items and weapons cannot be missed, sold, or discarded.
- Use finite authored gather points and optional caches. Do not respawn
  valuable crafting materials for enemy grinding.
- On death/respawn, collected progression materials, tools, weapons, and
  opened caches persist. Health can be restored at the checkpoint as above.

**Food and shop**

- Forage one or two region-themed food items (e.g. palm fruit, tide kelp,
  cavern root) from visible, finite world nodes. Eating restores health; food
  is also sellable/exchangeable only if the economy remains simple.
- Buy a small number of prepared meals at Driftwood Village or a safe region
  vendor. Starting proposal: one food heals 2 hearts; a more expensive meal
  restores all health once. Exact prices/capacity are tuned after playtests.
- Use shells as the single optional currency, found in caches and through
  one-off optional tasks. Do not drop currency from enemies or require buying
  food to finish the main route.
- Shop stock prioritizes food, a small capacity/health upgrade, map hints, and
  costumes. Never sell a mandatory key item, weapon, or required tool.
- Each vendor shows price, effect, and whether the player already owns the
  item. Confirm purchase and prevent spending if inventory/capacity is full.

**Small fixed tool set (approved direction; not freeform crafting)**

Tools are story/exploration equipment, not extra weapons or a recipe sandbox.
Each tool has a named fixed build quest, known materials, one clear use, and
no durability. The game teaches the use before asking players to spend rare
materials. Materials come from obvious authored nodes and caches, not random
drop tables.

| Tool | Build moment / materials (proposal) | Use | Gate feedback |
|---|---|---|---|
| Palm-Line | Village boatwright: 3 tough vines + 1 driftwood spool + 1 shell clasp | Cross only marked short anchor gaps or pull a reachable lever | Anchor points use a distinct rope-knot icon |
| Tideglass Lantern | Temple clue task: 1 intact lens + 2 glow crystals + 1 prepared oil | Reveal faint marks and optional safe paths in dark interiors | Faded marks shimmer when lantern is nearby/active |
| Repair Kit | Cavern workshop: 2 scrap plates + 1 resin + 1 shaped wood | Repair explicitly marked small mechanisms/shortcuts | Broken mechanisms have a consistent wrench/repair silhouette |

The initial Jungle slice should use **no required craft gate**. If a tool is
needed to complete a route, provide its materials on that route, explain the
purpose before construction, and ensure the tool can never be sold/lost. Treat
the table as a future campaign proposal; only implement a tool after the
relevant region prototype succeeds.

**Resource and recipe limits**

- Track only 3 common materials initially: driftwood, tough vine, and shell
  clasp. Add scrap plate, lens, glow crystal, and resin only in regions that
  use them.
- Materials have a small visible stack cap (proposal: 10 per common material).
  Required quest materials are reserved when found; optional excess is left
  visibly in the world or stored at the village chest, never silently deleted.
- No random recipes, ingredient rarity tiers, crafting stations everywhere,
  durability repairs, or repeatable material farming.
- Construction happens at a named workbench/NPC with an illustrated
  confirmation: needed → owned → remaining → build. Preserve materials and
  tool ownership in save data.

### Weapon design cards

All five weapons remain permanent and switchable. Each weapon has one primary
combat action and one clear world interaction; do not create a large skill
tree.

| Weapon | Region / virtue | Combat use | World use | Guardian |
|---|---|---|---|---|
| Thornfang | Jungle / agility | Fast short-range cuts; brief follow-up after a well-timed evade | Cut marked thorn vines; reveal root paths | Thornback |
| Embercleaver | Caverns / strength | Slow committed strike that interrupts armor | Break marked cracked stone; redirect/activate forge channels | Magma Golem |
| Stormpiercer | Temple / awareness | Reach/thrust or pulse with deliberate timing | Conduct a pulse through matching crystal nodes | Tide Warden |
| Dawnstring | Cliffs / precision | Aimed projectile with limited, readable firing cadence | Hit distant switches; align sunlight/mirrors | Sunhawk |
| Voidfang | Kingdom / courage | Short shadow counter/dash with a clear cooldown | Cross short marked shadow gaps; reveal hidden traces | Hollow Knight |

Bosses should teach the corresponding action. Exploration gates must have
matching visual language, readable from first encounter; never require guessing
which random weapon opens a gate.

### World map

- A diegetic world map is acquired early and fills in as Ichigo observes
  landmarks and visits exits.
- Show region names, visited routes, known exits, checkpoints, and discovered
  landmarks. Show locked ability gates with a clear icon/name only after
  Ichigo has seen them.
- Do not reveal undiscovered rooms, optional secrets, or exact quest answers.
- Player marker appears only in explored/current spaces; support a simple
  current-region map and an island overview.
- Optional pins: player-created markers (secret, locked gate, revisit) with a
  small fixed palette, if implementation effort allows.
- Journal/tasks list active main task, optional region tasks, discovered
  clues, known gate requirements, and completed tasks. Avoid auto-selecting a
  waypoint arrow.
- When a new region is visited, add an outline and entry point. When a shortcut
  opens, visibly update the map and give a brief discovery notification.

## 6. Obstacles, puzzles, and encounter grammar

### Reusable obstacle types

- **Solid geometry:** walls, cliffs, fences, closed doors; visible and
  collision-aligned.
- **Elevation connectors:** stairs, ramps, ladders, lifts; explicit entry and
  exit points between sorted/collision layers.
- **Ability gate:** thorn, cracked stone, crystal circuit, distant switch,
  shadow gap; visibly indicates weapon family and has an alternate escape if
  entered from the far side.
- **Hazard:** lava, falling rocks, deep water, current, gusts, collapsing floor,
  shadow zones. Telegraph safe timing and recovery; avoid instant unavoidable
  damage.
- **Mechanism puzzle:** small number of steps, clear input feedback, reset path,
  and visual confirmation. Clues teach rules, not obscure button codes.
- **Social obstacle:** NPC trust/knowledge or a story choice. Optional tasks
  may reveal a bypass; the main route must not silently depend on hidden
  dialogue.

### Region puzzle rules

- Use the region's visual language and its new weapon or observation ability.
- Teach the mechanic safely, then test it in one slightly more complex setup.
- Provide feedback when input is wrong, including a reset/retry method.
- Do not combine multiple new mechanics in one mandatory puzzle.
- Avoid timed switches unless timing is the intended lesson and windows are
  generous/accessibility settings can help.

### Enemy principles

- Enemy presence is sparse; safe exploration areas contain no respawning
  ambushes.
- Every attack has a readable wind-up, hit region, recovery, and cancel/escape
  rule. Effects and audio must communicate threat without hiding the player.
- Introduce one enemy behavior at a time before combinations.
- Normal enemies cannot permanently block required exits or one-time rewards.
- Guardians are unique puzzles/encounters; weakness is discoverable by
  observation/clue, not a hidden damage multiplier.

## 7. Complete region and zone plan

Each zone below lists its purpose, route/task beats, obstacles, encounter
content, and progression result. Exact geometry and coordinates come later,
after movement and tile scale are tested. “No boss” is intentional: quiet
spaces are needed for pacing.

### Region 1 — Palmshore Beach (2 zones)

**Theme:** open sky, surf, wreckage, first impossible landmark.
**Purpose:** teach movement and interaction with no combat pressure.

#### 1.1 Wreckshore

- **Entry/exit:** start at the beached vessel; one inland trail exits north to
  Tidepool Path.
- **Player task:** orient, walk around wreck debris, inspect family symbol,
  find a safe path off the beach.
- **Subtasks:** move to a visible landmark; interact with the old ship mark;
  notice matching mark on a standing stone; cross a low bridge/step path.
- **Obstacles:** solid wreckage; shallow shoreline slows but does not trap;
  explicit ramp/bridge teaches elevation transition.
- **Enemies/boss:** none.
- **Reward/progress:** first journal entry, map outline, `main.find_family_mark`
  active/completed.

#### 1.2 Tidepool Path

- **Entry/exit:** Wreckshore west/east edge; north gate/path to Village.
- **Player task:** follow the visible bell/lantern cue; inspect optional tide
  pools and reach village outskirts.
- **Subtasks:** find a path around deep water; activate a simple lantern marker;
  discover the village from an overlook.
- **Obstacles:** deep-water boundary, safe stepping stones, one short ramp;
  tide is visual only initially (no real-time timer).
- **Enemies/boss:** none.
- **Reward/progress:** world-map coastline and village entrance discovered.

### Region 2 — Driftwood Village (2 zones)

**Theme:** warm lamps, patched timber, ordinary life under unusual rules.
**Purpose:** safe hub, context, services, returns between regions.

#### 2.1 Village Square

- **Entry/exit:** beach approach at south; trail to Jungle Approach north;
  east lane to Harbor Row.
- **Player task:** meet the keeper and learn why villagers avoid the jungle.
- **Subtasks:** speak to keeper; inspect a family-symbol carving; choose one
  lead from available rumor dialogue; find map desk.
- **Obstacles:** NPCs/props are solid where visually appropriate; doors show
  interaction prompt; no combat.
- **Enemies/boss:** none.
- **Reward/progress:** active main task `main.find_jungle_route`; map landmark
  and known jungle gate displayed.

#### 2.2 Harbor Row

- **Entry/exit:** connects Village Square to beach return path and a locked
  central-island route.
- **Player task:** gather optional villager context and learn map/journal UI.
- **Subtasks:** inspect one noticeboard; speak with boatwright (optional);
  activate village checkpoint; preview locked gate symbols.
- **Obstacles:** small stairs/boardwalk elevation; locked route is visible but
  not presented as a required dead end.
- **Enemies/boss:** none.
- **Reward/progress:** village checkpoint; optional rumor `side.harbor_legend`;
  consumable vendor/upgrade services may be deferred until after slice tests.

### Region 3 — Forgotten Jungle (4 zones)

**Theme:** layered canopy, enormous roots, filtered light, old stone.
**Purpose:** first full gameplay slice and first weapon.
**Guardian/weapon:** Thornback / Thornfang.

#### 3.1 Jungle Entrance

- **Entry/exit:** from Village; exits to Root Path.
- **Player task:** learn eight-direction walking and observe the distant shrine.
- **Subtasks:** follow a safe trail; interact with a non-required marker;
  reach a checkpoint/clearing.
- **Obstacles:** gentle narrow passage, trees with foot-point occlusion, one
  short ramp; no required combat.
- **Enemies/boss:** optional slow Root Mite only after safe navigation teaching.
- **Reward/progress:** region map outline, first jungle checkpoint.

#### 3.2 Root Path

- **Entry/exit:** Entrance to Overgrown Clearing; visible return connection.
- **Player task:** follow root patterns and understand marked thorn gates.
- **Subtasks:** inspect a root symbol; approach an uncuttable vine wall; locate
  the safe path around it; optionally find a hidden ledge/nook.
- **Obstacles:** one obvious Thornfang gate; branching paths reconnect, no
  one-way trap; ramps/bridge maintain elevation cues.
- **Enemies/boss:** one Root Mite or none, based on slice pacing.
- **Reward/progress:** journal clue `jungle.root_markings_seen`.

#### 3.3 Overgrown Clearing

- **Entry/exit:** Root Path to Shrine Approach; side nook loops back.
- **Player task:** observe hostile behavior or bypass safely; find one optional
  secret.
- **Subtasks:** identify telegraph, use cover/space or simple weapon action;
  notice environmental hint toward the Map of Roots.
- **Obstacles:** soft hazard zone with visible safe edge; secret is hinted by
  displaced leaves/light, not pixel-hunt.
- **Enemies/boss:** one Briarling; optional only if combat remains out of scope
  for first slice.
- **Reward/progress:** optional lore/cache; short recovery checkpoint.

#### 3.4 Shrine Approach and Agility Shrine

- **Entry/exit:** Clearing to Thornback Arena, then shrine and return shortcut
  to Entrance.
- **Player task:** read Map of Roots, observe and pass Thornback's trial, claim
  Thornfang, reopen the earlier path.
- **Subtasks:** inspect mandatory mural; enter arena; dodge/read charge and
  recovery; interact with shrine; test weapon on marked vine; cut Root Path
  gate; open return shortcut.
- **Obstacles:** arena boundary is clear; boss patterns have safe lanes; vine
  gate blocks before reward and breaks after.
- **Enemies/boss:** Thornback, guardian of adaptation. Phase 1 teaches a
  straight charge into recovery; phase 2 adds a slow sweep after another clear
  tell. Avoid a long health bar and hidden damage rules.
- **Reward/progress:** Thornfang, guardian flag, shortcut flag, mandatory clue,
  village return route.

### Region 4 — Ember Caverns (3 zones)

**Theme:** charcoal stone, mineral seams, heat shimmer, low orange light.
**Weapon:** Embercleaver / strength.
**Main question:** what was deliberately buried?

#### 4.1 Ashfall Descent

- **Entry/exit:** central island after Thornfang; descends to Forge Veins.
- **Player task:** map a safe route through the hot entrance.
- **Subtasks:** identify periodic vent glow; cross between vents; inspect
  cracked mineral seal.
- **Obstacles:** telegraphed steam vents; falling rocks have shadow indicators;
  stairs descend between explicit elevation layers.
- **Enemies/boss:** one Ash Skitter introduced alone.
- **Reward/progress:** cavern map and checkpoint.

#### 4.2 Forge Veins

- **Entry/exit:** Ashfall Descent to Buried Forge; side loop shortcut.
- **Player task:** understand heat channel direction and find the tablet
  fragment.
- **Subtasks:** rotate one channel; safely redirect heat to a dormant lift;
  collect clue `cavern.broken_tablet`.
- **Obstacles:** hot channel crossing, breakable-looking but unbreakable wall
  until weapon; pit boundaries; resettable valve puzzle.
- **Enemies/boss:** one Forge Warden only after channel lesson is clear.
- **Reward/progress:** opens lift/shortcut; strength clue.

#### 4.3 Buried Forge

- **Entry/exit:** Forge Veins to boss arena, shrine, then return shortcut.
- **Player task:** face Magma Golem and earn Embercleaver.
- **Subtasks:** observe ground slam; move out of marked impact ring; strike
  cooled/unguarded core during recovery; redirect forge channel after fight.
- **Obstacles:** short safe pillars; hot floor phases with ample warning.
- **Enemies/boss:** Magma Golem. Phase 1 slam/recovery; phase 2 sends a slow
  wave that can be avoided behind a pillar. No random projectile spam.
- **Reward/progress:** Embercleaver breaks marked cracked stone and holds
  weight switches; opens route toward central island.

### Region 5 — Sunken Temple (3 zones)

**Theme:** flooded halls, reflections, blue-green light, remembered voices.
**Weapon:** Stormpiercer / awareness.
**Main question:** why does the temple remember voices?

#### 5.1 Flooded Court

- **Entry/exit:** central island to Resonance Hall.
- **Player task:** learn shallow-water movement and read reflected symbols.
- **Subtasks:** follow floating markers; inspect an echo inscription; locate
  high-ground path.
- **Obstacles:** shallow water slows; deep pool is blocked until puzzle state;
  current direction clearly shown.
- **Enemies/boss:** one Echo Eel introduced in safe water.
- **Reward/progress:** map and temple checkpoint.

#### 5.2 Resonance Hall

- **Entry/exit:** Flooded Court to Tide Engine.
- **Player task:** connect three crystal nodes in a clue-indicated order.
- **Subtasks:** inspect note; pulse nodes one at a time; observe reflected
  feedback; open sluice and lower water.
- **Obstacles:** resettable pulse puzzle; currents move the player only in
  obvious marked channels; no hidden timing.
- **Enemies/boss:** none during first puzzle; optional Echo Eel side room.
- **Reward/progress:** safe access to Tide Engine; clue recorded.

#### 5.3 Tide Engine

- **Entry/exit:** Resonance Hall to guardian room, reward alcove, return route.
- **Player task:** face Tide Warden and earn Stormpiercer.
- **Subtasks:** observe pulse/charge sequence; avoid the marked water lane;
  interrupt exposed core with a conducted pulse; activate temple mechanism.
- **Obstacles:** rising water is phase-based with safe platforms; arena cannot
  drown the player without an escape.
- **Enemies/boss:** Tide Warden. Alternates slow sweeping wave and crystal
  pulse; each leaves recovery.
- **Reward/progress:** Stormpiercer pulses matching nodes; permanently records
  the opened water route.

### Region 6 — Skywatch Cliffs (3 zones)

**Theme:** strong wind, long sightlines, suspended bridges, cloud shadows.
**Weapon:** Dawnstring / precision.
**Main question:** who built routes between islands?

#### 6.1 Windward Steps

- **Entry/exit:** central island to Mirror Walk.
- **Player task:** navigate in visible gust cycles and spot the signal tower.
- **Subtasks:** wait for safe gust; follow sheltered wall; inspect old bridge
  mechanism.
- **Obstacles:** wind pushes gently with clear environmental warning; broken
  cliff edges are collision barriers, never invisible instant falls.
- **Enemies/boss:** one Cliff Skimmer with a readable swoop.
- **Reward/progress:** map and checkpoint; establishes wind timing.

#### 6.2 Mirror Walk

- **Entry/exit:** Windward Steps to Signal Nest.
- **Player task:** align two mirrors to direct light to a distant switch.
- **Subtasks:** inspect sun-shadow clue; rotate nearby mirror; use Dawnstring
  only after it is earned later (pre-weapon version uses local lever/line of
  sight); find a safe bridge.
- **Obstacles:** narrow bridge with handrails/clear safe route; optical puzzle
  with feedback.
- **Enemies/boss:** optional one Cliff Skimmer after puzzle.
- **Reward/progress:** unlocks signal route and the later shortcut gate.

#### 6.3 Signal Nest

- **Entry/exit:** Mirror Walk to Sunhawk arena and shrine, then return route.
- **Player task:** defeat the Sunhawk and earn Dawnstring.
- **Subtasks:** track its shadow; move from dive lane; hit an exposed target
  during recovery; shoot distant switch after reward.
- **Obstacles:** gust lanes visibly displace projectiles; arena has cover and a
  safe recovery area.
- **Enemies/boss:** Sunhawk. Changes altitude, marks dive lane, then recovers;
  phase 2 adds a slow fan that leaves open lanes.
- **Reward/progress:** Dawnstring hits distant switches/cuts ropes/aligns
  special mirrors; restores a bridge shortcut.

### Region 7 — Lost Kingdom (4 zones)

**Theme:** abandoned civic life, empty courtyards, erased records, hidden
traces.
**Weapon:** Voidfang / courage.
**Main question:** what was removed from the official story?

#### 7.1 Outer Archive

- **Entry/exit:** central island via multi-weapon gate to Silent Courtyard.
- **Player task:** read ordinary records that conflict with the village legend.
- **Subtasks:** inspect archive fragments; compare one royal record with a
  guardian clue; locate a shadow gate.
- **Obstacles:** locked archive door opens by a visible two-weapon mechanism;
  allow return path; no combat required.
- **Enemies/boss:** one Archive Shade introduced with a slow, avoidable
  approach.
- **Reward/progress:** story flag `kingdom.records_found`; map checkpoint.

#### 7.2 Silent Courtyard

- **Entry/exit:** Outer Archive to Mirror Gallery and Shadow Causeway.
- **Player task:** navigate empty public spaces and reveal one hidden trace.
- **Subtasks:** observe missing statue bases; use environmental footprints;
  solve one small line-of-sight puzzle.
- **Obstacles:** shadow patches are warning zones; safe lit tiles are visible;
  one shortcut gate requires Voidfang after acquisition.
- **Enemies/boss:** two enemy behaviors are never introduced together.
- **Reward/progress:** clue suggests guardians were maintainers, not thieves.

#### 7.3 Mirror Gallery and Shadow Causeway

- **Entry/exit:** courtyard branches connect gallery and causeway; rejoin at
  Hollow Hall.
- **Player task:** learn shadow navigation and expose the safe path.
- **Subtasks:** align mirror/mark; activate reveal; cross short shadow gaps;
  optionally recover a lost explorer note.
- **Obstacles:** only designated gaps are traversable; no blind jumps; fall
  prevention/respawn is reliable.
- **Enemies/boss:** one Shade Knight sentinel patrol with clear facing cone.
- **Reward/progress:** opens Hollow Hall and an optional archive cache.

#### 7.4 Hollow Hall

- **Entry/exit:** causeway to Hollow Knight, shrine, then central island
  shortcut.
- **Player task:** face the mirrored fighter and earn Voidfang.
- **Subtasks:** read copycat pattern; bait a committed attack; use a different
  response instead of repeating attacks; activate the record chamber.
- **Obstacles:** arena mirrors can mislead silhouette but never hide hitboxes;
  clear player outline always preserved.
- **Enemies/boss:** Hollow Knight mirrors last action with a delay; phase 2
  tests varied movement and patience. Never copy damage output unfairly.
- **Reward/progress:** Voidfang reveals/crosses marked shadow gaps; major
  shortcut and story flag.

### Region 8 — Excalibur Sanctuary (3 zones)

**Theme:** quiet scale, minimal visual noise, ceremonial light.
**Reward:** temporary Excalibur; finale is not a sixth open-world weapon.

#### 8.1 Outer Threshold

- **Entry/exit:** appears on world map after all five weapons and key story
  clues; leads to Memory Gallery.
- **Player task:** use weapon symbols to open the threshold.
- **Subtasks:** observe five marks; activate with the matching weapons; read
  the final contradiction in the public legend.
- **Obstacles:** five clear sockets; incorrect selection provides feedback;
  leaving is always possible.
- **Enemies/boss:** none.
- **Reward/progress:** sanctuary checkpoint; final quest active.

#### 8.2 Memory Gallery

- **Entry/exit:** threshold to Quiet Core.
- **Player task:** complete three short trials reusing existing language.
- **Subtasks:** traverse a safe route; interpret a clue; choose restraint over
  an apparent combat solution.
- **Obstacles:** one puzzle per trial, no newly introduced control mechanic;
  generous reset.
- **Enemies/boss:** no standard enemies; trial images are non-damaging echoes.
- **Reward/progress:** final chamber opens; family-symbol story is answered.

#### 8.3 Quiet Core

- **Entry/exit:** final story space; returns to epilogue/village on completion.
- **Player task:** confront the island's contained truth and choose how to
  respond; wield Excalibur only during the authored climax.
- **Subtasks:** activate the five-weapon convergence; survive a short,
  readable final sequence; make a dialogue/interpretation choice.
- **Obstacles:** cinematic traversal remains playable; no irreversible
  fail-state; autosave before final choice.
- **Enemies/boss:** optional final guardian echo, designed as a short test of
  observation rather than a health sponge.
- **Reward/progress:** ending and epilogue variation; Excalibur is not retained
  as a selectable permanent weapon.

## 8. Zone-by-zone exploration and subtask plan

The map list above defines what each zone contains; this section defines how
the player can spend time there. Each zone should provide a clear main route
plus optional reasons to look around. Optional play should deepen the place,
not make players sweep every corner to finish the main story.

### Exploration rules for every zone

- Put the main route in sight or make it legible through landmarks, light,
  paths, sound cues, architecture, or NPC hints. Exploration means choosing
  where to look, not pixel-hunting for an invisible exit.
- Give each zone two or three optional beats at most: for example one
  environmental observation, one route/secret, and one character/lore task.
  They can be discovered in any order and can be left unfinished.
- Reward optional play with a useful map note, shortcut, small upgrade,
  character response, world-state change, or story context. Do not pad time
  with repeatable enemy farming, empty corridors, or arbitrary fetch counts.
- Every optional branch reconnects to the main route or a known safe point.
  Show a landmark or map hint before a player commits to a long detour.
- Support multiple paces: a focused player can follow the main route; a curious
  player can stop, inspect, revisit, and finish small side tasks. Do not gate
  the ending behind optional completion.
- Optional tasks use stable IDs and explicit prerequisite/reward flags. Journal
  text says what the player learned and never marks an unverified assumption as
  fact.
- Record observations and playtest where testers naturally pause, get lost, or
  skip everything. Adjust signposting or payoff instead of forcing a longer
  playtime target.

Durations are provisional **first-visit design targets**, not promises or
minimums. They include reading/interacting and one or two optional discoveries;
they are not time gates. Tune them from playtests.

| Zone | Focused route | Curious first visit |
|---|---:|---:|
| Wreckshore | 4–6 min | 8–12 min |
| Tidepool Path | 4–6 min | 8–12 min |
| Village Square | 5–8 min | 12–20 min |
| Harbor Row | 4–7 min | 10–18 min |
| Jungle Entrance | 4–6 min | 8–12 min |
| Root Path | 5–8 min | 10–16 min |
| Overgrown Clearing | 5–8 min | 10–18 min |
| Shrine Approach / Agility Shrine | 8–12 min | 12–20 min |
| Ashfall Descent | 5–8 min | 10–16 min |
| Forge Veins | 7–10 min | 14–22 min |
| Buried Forge | 8–12 min | 12–20 min |
| Flooded Court | 5–8 min | 10–16 min |
| Resonance Hall | 7–10 min | 14–22 min |
| Tide Engine | 8–12 min | 12–20 min |
| Windward Steps | 5–8 min | 10–16 min |
| Mirror Walk | 7–10 min | 14–22 min |
| Signal Nest | 8–12 min | 12–20 min |
| Outer Archive | 6–9 min | 14–24 min |
| Silent Courtyard | 6–9 min | 14–22 min |
| Mirror Gallery / Shadow Causeway | 7–10 min | 14–24 min |
| Hollow Hall | 8–12 min | 12–20 min |
| Outer Threshold | 4–7 min | 8–12 min |
| Memory Gallery | 8–12 min | 12–20 min |
| Quiet Core | 10–15 min | 15–25 min |

### Region 1 — Palmshore Beach

#### 1.1 Wreckshore — exploration sequence

1. **Orient:** let the player walk the beach freely; distant shipwreck and
   inland trail establish north/south. Teach movement through open safe space,
   not pop-up instruction walls.
2. **Mandatory task:** approach the wreck, inspect Ichigo's family symbol,
   then compare it to the carved standing stone.
3. **Optional branch A:** follow a line of unusual shells to a tidepool
   viewpoint; the journal records a repeating island mark.
4. **Optional branch B:** search the sheltered side of the wreck for a
   readable sailor's note and a small healing item; show the opened container
   on the map.
5. **Return/payoff:** the inland path is visible from the viewpoint; a short
   observation animation or journal update confirms the first matching mark.
   No enemy encounter and no item collection quota.

#### 1.2 Tidepool Path — exploration sequence

1. **Mandatory task:** follow the bell or lantern sightline to the village
   approach; step across safe shallows to learn the water boundary.
2. **Optional branch A:** inspect three rock pools with distinct natural
   details; the third reveals a small map annotation, not three generic
   collectibles.
3. **Optional branch B:** find an old weather marker and rotate it to point
   inland, producing a visible lantern/route signal for the return trip.
4. **Navigation beat:** reach the overlook and identify the village roofline;
   let the player choose the boardwalk or higher ramp route.
5. **Return/payoff:** the path back to Wreckshore remains open and readable;
   newly discovered tidepool symbols stay marked on the map.

### Region 2 — Driftwood Village

#### 2.1 Village Square — exploration sequence

1. **Mandatory task:** enter the square and speak to the keeper; this gives a
   rumor but does not automatically mark every objective.
2. **Subtask:** inspect a family-symbol carving and compare it with the one
   from the beach; update the journal with the specific match and remaining
   uncertainty.
3. **Optional branch A:** speak to two residents whose accounts disagree about
   the jungle guardians; record both as claims, not confirmed history.
4. **Optional branch B:** inspect the unlit communal shrine and find its
   maintenance note; flag it for a later return after a weapon is earned.
5. **Return/payoff:** keeper points out the jungle trail using a landmark;
   the player can leave immediately or continue to Harbor Row. Avoid requiring
   every conversation before the exit unlocks.

#### 2.2 Harbor Row — exploration sequence

1. **Mandatory task:** activate the village checkpoint and learn how visited
   map routes appear.
2. **Optional branch A:** help the boatwright locate a missing measuring
   weight among nearby props; reward: shortcut rumor or small capacity
   upgrade, not a combat stat requirement.
3. **Optional branch B:** examine the noticeboard, then follow one clue to a
   mooring post with a scratched old route name; add that route to the map as
   “unknown/closed.”
4. **Revisit task:** after restoring one island route, return to the
   boatwright to see an actual boat/boardwalk change and hear an updated line.
5. **Return/payoff:** village services and map stay available; no errands
   expire, and no one must grind currency to leave.

### Region 3 — Forgotten Jungle

#### 3.1 Jungle Entrance — exploration sequence

1. **Mandatory task:** walk a broad safe loop around a root cluster to learn
   eight-way movement, collision, facing, and contextual interaction.
2. **Subtask:** observe a landmark through the canopy; the route to Root Path
   is hinted by leaf movement/light, not an arrow.
3. **Optional branch A:** follow harmless fireflies to a stone with a small
   symbol; add a comparison note in the journal.
4. **Optional branch B:** inspect a shallow footprint trail and find a safe
   rest spot/checkpoint without combat.
5. **Return/payoff:** a side loop rejoins the central trail. First-time
   guidance is skippable on replay; do not lock the path behind a tutorial.

#### 3.2 Root Path — exploration sequence

1. **Mandatory task:** follow the root pattern to a thorn wall and inspect its
   living weave; record the gate's visual language for a later revisit.
2. **Subtask:** take the visible side route around the wall and activate the
   first root marker; feedback shows how markers affect the path.
3. **Optional branch A:** trace a faint root line to a lookout that reveals
   the Shrine landmark from a different angle.
4. **Optional branch B:** find a sheltered alcove with an explorer's mark;
   compare it to the village legend for a small contradictory detail.
5. **Return/payoff:** the return route is short and safe. After Thornfang,
   cutting the gate opens the planned shortcut; the map updates and the player
   sees the remembered gate location.

#### 3.3 Overgrown Clearing — exploration sequence

1. **Mandatory task:** enter from cover and watch a Briarling's routine before
   choosing to engage or circle around it.
2. **Subtask:** cross the clearing using either a safe outer path or a shorter
   exposed center route; both rejoin before the shrine.
3. **Optional branch A:** notice disturbed plants and use the clue to uncover
   a small cache/lore item; avoid invisible floor switches.
4. **Optional branch B:** inspect an abandoned camp with a readable note and
   one world detail that suggests earlier visitors left voluntarily or were
   redirected.
5. **Return/payoff:** an optional checkpoint gives a safer retry before
   Thornback. No required fight in the first slice if playtesting shows combat
   crowds out exploration.

#### 3.4 Shrine Approach and Agility Shrine — exploration sequence

1. **Mandatory task:** read the Map of Roots mural; the mural connects
   agility/adaptation to how roots bend rather than break.
2. **Subtask:** walk the arena perimeter first; notice Thornback's route,
   safe cover, and two distinct telegraph marks before activating the trial.
3. **Guardian:** observe charge → evade → approach during recovery; later
   observe sweep → reposition. Each failure teaches a visible cue and returns
   to a nearby checkpoint.
4. **Reward beat:** claim Thornfang, then use it on a marked practice vine in
   the shrine room; show the changed route immediately.
5. **Optional branch:** revisit Root Path's gate to uncover a small return
   nook with a note that changes the meaning of a village claim.
6. **Return/payoff:** open the shortcut to Entrance, then optionally return
   to the village for new dialogue. Required slice can finish at the shortcut;
   completion is not delayed by optional cleanup.

### Region 4 — Ember Caverns

#### 4.1 Ashfall Descent — exploration sequence

1. **Mandatory task:** observe one full steam-vent cycle from a safe tile,
   then cross the marked route.
2. **Subtask:** choose the upper sheltered stair or lower mineral path; both
   arrive at Forge Veins but reveal different views.
3. **Optional branch A:** inspect a cooled mineral seam and record its
   distinct crack pattern for later comparison.
4. **Optional branch B:** follow safe ash footprints to a rest alcove and
   optional history fragment.
5. **Return/payoff:** checkpoint and map annotation make vent timing clear on
   subsequent passes; falling rocks never spawn without a visible shadow cue.

#### 4.2 Forge Veins — exploration sequence

1. **Mandatory task:** inspect the tablet fragment before using the valve;
   it depicts heat flowing toward, not away from, the sealed channel.
2. **Subtask:** rotate one valve, watch the channel illuminate, then redirect
   heat to power the lift. The puzzle is resettable without leaving the room.
3. **Optional branch A:** trace an inactive pipe to a side chamber and find a
   worker's note about the forge being shut down deliberately.
4. **Optional branch B:** return with Embercleaver to crack the marked seal
   and open a shortcut/cache; show the seal on the map when first seen.
5. **Encounter beat:** introduce the Forge Warden after the puzzle lesson;
   player can use the safe outer path to retreat.
6. **Return/payoff:** powered lift shortcuts the vent route; both states
   persist after leaving.

#### 4.3 Buried Forge — exploration sequence

1. **Mandatory task:** inspect the arena's cooled pillars and ground scoring
   before waking the Magma Golem.
2. **Guardian:** evade the marked slam, use the recovery opening, then use a
   pillar against the slow wave. Second phase combines known patterns without
   adding an unreadable surprise.
3. **Reward beat:** claim Embercleaver; test it on a nearby cracked plate and
   heavy pressure switch before exiting.
4. **Optional branch A:** break a clearly marked side seal to reveal a
   maintenance log about what the forge was designed to contain.
5. **Optional branch B:** reopen the Forge Veins lift and retrieve one
   previously visible item; no new enemy wave on the backtrack.
6. **Return/payoff:** restore a central route and receive updated village
   dialogue/map mark.

### Region 5 — Sunken Temple

#### 5.1 Flooded Court — exploration sequence

1. **Mandatory task:** follow floating debris to learn the current direction;
   compare the reflection on the water with the actual wall symbol.
2. **Subtask:** choose a shallow-water path or dry upper walkway; each teaches
   a different safe route to Resonance Hall.
3. **Optional branch A:** inspect three reflected fragments in one contiguous
   pool to reveal one complete mural; fragments are visible together, not
   scattered random pickups.
4. **Optional branch B:** observe an Echo Eel's water-lane sweep from a
   protected bank; confrontation may be bypassed.
5. **Return/payoff:** opening the sluice later lowers water and exposes an
   alternate path in this same zone.

#### 5.2 Resonance Hall — exploration sequence

1. **Mandatory task:** inspect the inscription that shows the node order;
   activate the nearest node as a safe tutorial.
2. **Subtask:** pulse all three nodes and read the light/sound response after
   each; wrong order resets with explicit feedback, not punishment.
3. **Optional branch A:** find a dry alcove behind a side current and inspect
   a voice-recording crystal with optional lore.
4. **Optional branch B:** return after lowering water to reach a previously
   visible submerged chest/map note.
5. **Return/payoff:** the sluice visibly changes water depth in both this zone
   and Flooded Court; the player can use the new route immediately.

#### 5.3 Tide Engine — exploration sequence

1. **Mandatory task:** inspect safe platforms and water-lane markers before
   activating the Tide Warden.
2. **Guardian:** avoid sweep, observe pulse, conduct energy through the
   correct node during recovery. Keep a dry, safe repositioning lane.
3. **Reward beat:** claim Stormpiercer; pulse one nearby node to demonstrate
   the ability outside combat.
4. **Optional branch A:** use the changed water level to open the engine's
   lower maintenance corridor and read a short caretakers' record.
5. **Optional branch B:** open a shortcut back to the Flooded Court and
   revisit the side alcove without the guardian resetting.
6. **Return/payoff:** the water route becomes permanent and the world map
   records its new connection.

### Region 6 — Skywatch Cliffs

#### 6.1 Windward Steps — exploration sequence

1. **Mandatory task:** read wind direction from flags and drifting leaves;
   cross during a safe gust window or take a longer sheltered path.
2. **Subtask:** inspect the old bridge mechanism and find why a plank section
   is missing.
3. **Optional branch A:** climb a broad stair to an overlook that previews the
   signal tower and another island landmark.
4. **Optional branch B:** follow a Cliff Skimmer's shadow without fighting it
   to discover its nest and a small lore detail.
5. **Return/payoff:** activate the checkpoint before the exposed route; later
   bridge repair opens a quick path back here.

#### 6.2 Mirror Walk — exploration sequence

1. **Mandatory task:** inspect the sun-shadow clue, rotate the first mirror,
   and follow the reflected line to a visible target.
2. **Subtask:** choose between a protected bridge or longer ground path to
   reach the second mirror.
3. **Optional branch A:** redirect light to a side marker that reveals a
   historical route name on the map.
4. **Optional branch B:** use Dawnstring after acquisition to hit a distant
   switch that opens a small return cache; this must not block first entry.
5. **Return/payoff:** the aligned signal remains active; the route changes
   visibly and reconnects to Windward Steps.

#### 6.3 Signal Nest — exploration sequence

1. **Mandatory task:** observe the Sunhawk circling and follow its shadow to
   understand the dive lane before starting the fight.
2. **Guardian:** leave the lane during the dive, use cover against the fan,
   then strike the exposed target during recovery.
3. **Reward beat:** claim Dawnstring and immediately hit a large, forgiving
   switch to open the first bridge.
4. **Optional branch A:** shoot a second, clearly visible switch to reveal a
   wind map/optional lookout rather than a hidden tiny target.
5. **Optional branch B:** revisit Mirror Walk and direct the signal to the
   village; unlock new dialogue and map context.
6. **Return/payoff:** bridge and signal route persist; backtracking contains
   no repeated boss/enemy gauntlet.

### Region 7 — Lost Kingdom

#### 7.1 Outer Archive — exploration sequence

1. **Mandatory task:** compare an ordinary civic record to the village
   legend; update the journal to mark contradiction, not truth.
2. **Subtask:** open the archive wing using a visible two-weapon mechanism
   that provides a clue if one required ability is missing.
3. **Optional branch A:** catalog two adjacent records as a single context
   discovery; reading them in either order is valid.
4. **Optional branch B:** follow a faint dust-free rectangle to a moved
   artifact and infer who removed it.
5. **Return/payoff:** checkpoint before Silent Courtyard; new record changes
   one village resident's later line.

#### 7.2 Silent Courtyard — exploration sequence

1. **Mandatory task:** notice the missing statue bases and footprints that
   avoid a central shadow patch.
2. **Subtask:** use safe lit ground to cross, then line up two visible traces
   to reveal a short memory image.
3. **Optional branch A:** inspect ordinary household objects left in the
   empty courtyard; three details make one story beat, not three fetch tasks.
4. **Optional branch B:** use Voidfang after acquisition to open the marked
   short-cut gate and reach a small cache.
5. **Return/payoff:** the courtyard remains a neutral breathing space; the
   revealed traces are saved and referenced in the journal.

#### 7.3 Mirror Gallery / Shadow Causeway — exploration sequence

1. **Mandatory task:** rotate a mirror to reveal a safe, marked shadow path.
2. **Subtask:** cross one short designated gap, then apply the same rule to a
   slightly longer path with a visible fallback/respawn point.
3. **Optional branch A:** choose the archive-side branch for a note or the
   causeway-side branch for a shortcut; both rejoin before Hollow Hall.
4. **Optional branch B:** use Voidfang to reveal a hidden trace and small
   memory scene; do not require it for the guardian.
5. **Encounter beat:** pass one Shade Knight patrol using stealth/route
   timing or a readable fight; safe retreat is possible.
6. **Return/payoff:** no blind platform jumps; map marks safe route after
   player has crossed it.

#### 7.4 Hollow Hall — exploration sequence

1. **Mandatory task:** inspect the mirrored hall, then observe the Hollow
   Knight copy one of Ichigo's actions after a delay.
2. **Guardian:** test varied movement/action rather than repeated attacks;
   each mirrored pattern has a clear tell and cooldown.
3. **Reward beat:** claim Voidfang and reveal a nearby shadow trace as an
   immediate low-risk practice.
4. **Optional branch A:** open the central-island shortcut before leaving;
   test it from both directions.
5. **Optional branch B:** return to earlier marked shadow gates for optional
   lore/map discoveries. The journal can keep a short “known gates” list.
6. **Return/payoff:** review the conflicting evidence at the village; give
   NPCs updated responses based on records/clues found, not optional
   completion percentage.

### Region 8 — Excalibur Sanctuary

#### 8.1 Outer Threshold — exploration sequence

1. **Mandatory task:** inspect five weapon marks and compare each to a
   discovery already recorded in the journal.
2. **Subtask:** activate matching sockets in any order; wrong matches explain
   the mismatch and do not reset correct sockets.
3. **Optional branch A:** inspect the wall spaces between the sockets to find
   evidence that the official story was edited.
4. **Optional branch B:** leave and return to any weapon region before
   committing to Memory Gallery; preserve all threshold state.
5. **Return/payoff:** threshold records each activated weapon and opens only
   when the required collection is complete.

#### 8.2 Memory Gallery — exploration sequence

1. **Mandatory task:** complete three short trials using familiar mechanics:
   read a clue, navigate an elevation route, and use a weapon to change a
   route.
2. **Subtask:** after each trial, inspect a memory image and choose whether
   Ichigo records it as observation or interpretation.
3. **Optional branch A:** revisit a side room to compare the images in
   chronological order; unlock an optional journal synthesis.
4. **Optional branch B:** use a no-combat path through the trial spaces;
   do not hide lore behind a perfect score.
5. **Return/payoff:** each trial has an immediate checkpoint/reset. No long
   gauntlet repeats after a mistake.

#### 8.3 Quiet Core — exploration sequence

1. **Mandatory task:** walk the quiet core and inspect the family symbol's
   final matching mark before activating the convergence.
2. **Subtask:** use the five weapons to restore/contain the system; the
   interface reflects prior discoveries and explains a missed mandatory
   clue clearly.
3. **Final encounter:** optional guardian echo is a short, readable
   observation test; it cannot erase weapons or trap the ending.
4. **Choice/payoff:** decide how much Ichigo tells the village. The choice
   alters the epilogue/dialogue, not access to completion or core content.
5. **After ending:** allow return to the village/world for optional completion
   and conversation, while clearly marking the main story finished.

### Cross-zone exploration threads

These are optional, persistent threads that encourage players to revisit
meaningful places without requiring full-map cleanup:

1. **Marks and contradictions:** compare symbols found at Wreckshore,
   Village Square, Root Path, and the Sanctuary. Each comparison adds a
   journal connection; missing an optional symbol never blocks the ending.
2. **Village responses:** new dialogue becomes available after major weapon
   rewards and selected story records. NPCs reference what Ichigo actually
   learned; no daily timers or dialogue grind.
3. **Shortcuts:** every ability region opens one visible return connection.
   Revisit gives a quicker route plus a changed environmental/NPC detail.
4. **Map annotations:** discovered gates and landmarks remain useful as
   revisit reminders. Do not flood the map with unvisited collectible icons.
5. **Optional records:** use a small set of authored, contextual discoveries
   with distinct stories; no large interchangeable page-count collection.
6. **Personal notes:** optional player pins mark a gate, clue, or vista to
   revisit. Pins are convenience, not required objectives.

### What “enough exploration” means

- The player can complete a zone after the main task and still feel the place
  is understandable.
- A curious player usually finds at least one optional beat without following
  a guide; additional secrets reward observation, not exhaustive tile checks.
- The zone contains a visible “I can return later” marker when an ability is
  missing, plus a map reminder after that gate has been seen.
- Revisiting changes something concrete: route, shortcut, NPC knowledge,
  environmental state, or newly accessible clue.
- A zone's duration comes from meaningful choices and discovery. If playtests
  show players wandering in circles or repeating combat, improve signposting
  and payoffs rather than adding more chores.

## 9. Enemy roster and AI requirements

Keep initial standard roster small; reuse behavior with distinct tells rather
than adding many similar enemies.

| Enemy | First use | Behavior lesson | Required fairness cue |
|---|---|---|---|
| Root Mite | Jungle | Slow approach / spacing | Pause before lunge |
| Briarling | Jungle optional | Short charge, recovery | Body lowers and ground line |
| Ash Skitter | Cavern | Moves between heat-safe patches | Bright body/route warning |
| Forge Warden | Cavern | Guarded front, slow turn | Exposed back after heavy swing |
| Echo Eel | Temple | Water lane sweep | Ripples mark direction |
| Cliff Skimmer | Cliffs | Dive from shadow | Shadow marks landing path |
| Archive Shade | Kingdom | Slow approach through shadow | Lit safe area and attack tell |
| Shade Knight | Kingdom | Patrol/committed strike | Facing cone and long recovery |

Shared AI states: idle/patrol → notice → telegraph → attack → recovery → hurt/
defeated. Use deterministic or seeded patterns for fairness. Enemies should
not chase through doors into safe rooms. Define hitboxes separately from art;
show invulnerability clearly and prevent repeated damage every frame.

## 10. Boss and guardian standard

Each guardian: unique arena, two or three readable behaviors, at least one
safe learning cycle, recovery after a miss, checkpoint nearby, one clear
weapon reward, and a post-fight world change. No multi-phase boss should
introduce a mechanic without a safe demonstration.

| Guardian | Virtue | Teaches | World change |
|---|---|---|---|
| Thornback | Adaptation/agility | Read charge, evade, use recovery | Vine barriers become cuttable |
| Magma Golem | Strength/change | Respect wind-up, use cover and heavy opening | Forge/stone route restored |
| Tide Warden | Awareness/memory | Read pulse/water lanes, connect nodes | Water levels/routes updated |
| Sunhawk | Precision/perspective | Track shadow, position, aim at recovery | Bridge and signal network restored |
| Hollow Knight | Courage/acceptance | Observe imitation, vary response, avoid reflex loop | Shadow traces/gates revealed |
| Final echo (optional) | Responsibility | Combine, do not spam, protect rather than conquer | Finale resolves; no permanent weapon |

The heart counts and damage/healing values above are provisional starting
values, not locked balance. Establish final damage and healing through a small
encounter prototype and playtests.

## 11. UI, audio, accessibility, and release needs

### Required player-facing UI

- Title/new game/continue/quit; pause/resume/settings/return to title.
- Context action prompt; small health and active-weapon display only when
  relevant; no cluttered permanent quest arrow.
- Inventory, map, journal/tasks, save/checkpoint feedback.
- Clear discovered/locked gate notification; confirmation before destructive
  new game/overwrite.
- Key rebinding or documented defaults; keyboard and gamepad labels update to
  current device.

### Audio/art requirements

- Ambient identity for every region; small number of reusable footstep and
  interaction sounds; distinct guardian telegraphs/recovery.
- Audio should be mixable/mutable in settings; no required information is
  audio-only.
- Pixel-art palette and sprite scale consistent; animation state readable at
  internal resolution; player never disappears under canopy.
- Concept art is reference only. Check licenses and build coherent production
  sprites/tiles; do not use concept-sheet crops as final animations.

### Accessibility and usability baseline

- Adjustable master/music/effects volume and full-screen/windowed mode.
- Controller support; remappable controls if feasible.
- Optional reduced camera motion/flashing; readable text size; don't rely on
  color alone for hazards/weapon gates.
- Pause single-player play; safe checkpoints and forgiving retry loops.
- Provide a no-rapid-input setting or generous hold/toggle behavior for any
  repeated interaction where practical.

### Packaging

- Windows build runs from a clean folder with required runtime DLLs/assets or
  a static-link decision verified for the chosen toolchain.
- No development absolute paths; save in Local AppData; include license and
  asset attributions; test fresh install and upgrade/save behavior.

## 12. Data and architecture to grow into

Do not create every subsystem now. When corresponding gameplay is reached:

- `game`: state, update/draw lifecycle, pause/transition.
- `input`: actions abstracted from keyboard/gamepad bindings.
- `player`: movement, facing, collision body, interaction range, animation
  state.
- `world/map`: tile layers, collision, elevation layer, exits, spawn points,
  object IDs, region flags.
- `interaction`: talk/inspect/use, contextual prompt, stable IDs.
- `quest/journal`: task definitions and objective states.
- `inventory/weapon`: ownership, selection, world abilities and UI.
- `enemy/boss`: data/state-driven behavior and damage rules.
- `save`: versioned, validated state; atomic write and backup.
- `render`: pixel scaling, Y-sort, occlusion, camera, effects.
- `ui/audio`: menus, map, journal, mix settings and cleanup.

Prefer text/data-defined object records for stable IDs and small hand-authored
maps. Keep map rendering separate from collision. Prototype with code-built
test rooms first; choose a map editor/format only after room needs are known.
Never store absolute developer paths in committed level/save data.

## 13. Implementation plan — one verified task at a time

### Stage A — Confirm the new direction

1. Replace the minimal Raylib scaffold with a bounded top-down test room.
2. Define virtual resolution, tile size, and 8-direction movement on a plain
   test floor.
3. Implement normalized keyboard/gamepad movement, foot-point collision,
   facing, wall sliding, and room camera.
4. Add one ramp/elevation connector and Y-sort/foreground occlusion test.

**Gate:** run around a block, approach from all eight directions, transition
between elevations, and verify no diagonal speed boost, jitter, or wall
penetration. Review art scale only after movement feels right.

### Stage B — One-room interaction and world shell

1. Add one map/room, spawn, exit, solid collision, and stable object ID.
2. Add inspect/talk prompt, one clue, and journal record.
3. Add one checkpoint and safe respawn.
4. Add local map visited state; test room transition in both directions.

**Gate:** reliable start → inspect → transition → return → checkpoint flow.

### Stage C — Jungle greybox vertical slice

1. Implement the 4 Jungle zones in placeholder art.
2. Add the thorn gate and explicit state flags.
3. Add Map of Roots task/subtasks and one optional secret.
4. Add Thornback with two fair patterns.
5. Add Thornfang, cut gate, activate shortcut, return to entrance.
6. Playtest timing, route readability, health/failure, controller.

**Gate:** 10–15 minute slice test, no softlocks, progression persists per
session, and every mandatory step can be understood without a waypoint arrow.

### Stage D — Save/inventory foundation

1. Add weapon ownership/selection and key-item inventory.
2. Add health/food behavior and explicit, readable world pickups; verify
   checkpoint recovery does not remove major progression.
3. Add the first fixed tool only when its region/task is implemented; include
   its material flags and a clear workbench interaction, not a generic recipe
   engine.
4. Add versioned save for area/checkpoint, health, quest/objective flags,
   discovered map, weapons, tools/materials, opened gates, and settings.
5. Test new game, manual/checkpoint save, relaunch, invalid/missing save,
   backup recovery, and old save version.

**Gate:** save/load never loses major progression silently or crashes.

### Stage E — Production slice polish

1. Replace only the slice's core path assets with a coherent licensed style.
2. Add region ambience, interaction/telegraph/reward sounds, and settings.
3. Add menus, controller prompts, accessibility basics, and route feedback.
4. Measure player completion time and observe confusion; adjust one issue at a
   time.

**Gate:** a new tester can finish the slice and explain clue, Thornfang use,
and shortcut.

### Stage F — Reusable region template

1. Extract room data, exits, collision/elevation layers, object IDs, task
   definitions, and stable world state.
2. Build one tiny test region using data rather than new hardcoded logic.
3. Verify map transitions, persistence, debug tools, and load/unload cleanup.

**Gate:** adding a room does not require modifying player physics or duplicating
progression logic.

### Stage G — Expand one weapon region at a time

For each region: greybox its 3–4 zones → teach its obstacle → add clue/task →
add optional find → add one common enemy only if needed → build guardian →
award weapon → open return path → save/load test → playtest → polish. Do not
start the next region until the current one has passed its gate.

### Stage H — Finale and release

1. Implement weapon convergence and Sanctuary.
2. Playtest all order permutations for regions 4–7.
3. Test missed clues, optional tasks, saves at every checkpoint, and return
   routes.
4. Test clean packaging on a Windows machine/folder without development
   dependencies.
5. Fix critical progression bugs before adding optional features.

## 14. What is deliberately not specified/locked yet

Do not invent precision where playtesting should decide:

- Exact tiles per map, total campaign hours, pixels-per-meter, sprite frame
  counts, or final art pipeline.
- Final health/damage numbers, dodge cooldown, consumable stack capacity,
  tool-material counts, and shop prices. The values above are starting
  proposals to test, not final balance.
- Exact NPC names/dialogue and final ending wording.
- Optional quest count and shop economy.
- Whether every region's final opening includes standard enemies.

Use prototypes and playtests to resolve these. Update this blueprint and the
decision log once chosen.

## 15. Progress tracker

| Item | Status |
|---|---|
| Existing Windows build and launch | Verified |
| Existing side-scroll coast prototype | Removed on 2026-10-14 at user direction |
| Current executable scaffold | Minimal Raylib window; top-down game not implemented |
| Top-down camera/game direction | Selected for future game; not implemented |
| 8-region / 24-zone campaign outline | Planned in this document |
| Top-down eight-direction controller | Not started |
| Jungle slice in top-down world | Not started |
| Inventory/map/quest/save systems | Planned; not implemented |
| Health/food/shop/tool-building system | Proposed; not implemented |
| Ichigo costume/animation set | Appearance direction proposed; no production sprites |
| Enemy and guardian systems | Planned; not implemented |

**Next bounded task:** replace the runnable scaffold with a top-down movement
test room using normalized eight-direction movement, camera bounds, solid wall
collision, and one visible ramp/elevation transition. Add code modules only as
their responsibilities are implemented; the old side-scroller has been
removed.
