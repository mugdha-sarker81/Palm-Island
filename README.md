# Palm Island — The Forgotten Coast

Palm Island is a **single-player 2D exploration adventure** built in C11 with
Raylib 5.5. You play as Ichigo, an outsider whose family symbol is connected
to a quiet, mysterious island. The intended game is about noticing, exploring,
and understanding the island—not grinding or constant combat.

> **Honest status:** this is not the complete game or the planned Forgotten
> Jungle vertical slice. The previous side-scrolling prototype was removed.
> The current executable is a top-down prototype with five linked Forgotten
> Jungle slice areas: the Entrance, Root Path, Overgrown Clearing, Shrine
> Approach, and a Thornback Arena shell with a two-pattern encounter
> prototype. A sixth area, Ashfall Descent, begins the Ember Caverns expansion
> with a connected vent-route greybox, one elevation ramp, and a return
> checkpoint. Amber vent warnings precede damaging orange steam bursts; a safe
> upper route avoids them. The user reports the route, hazard fairness, and
> checkpoint passed hands-on playtesting. Ashfall now also has one two-hit
> Ash Skitter, with a bright route tell, dash contact, and a Palmwood Club or
> Thornfang recovery window and a brief color-coded strike cue; this encounter
> awaits its own playtest. Root Path now has an optional two-hit Root Mite
> with a slow approach, a pause-before-lunge tell, and a Palm Fruit drop that
> appears only after the mite is defeated; that encounter also awaits a
> playtest. Ichigo
> starts with the Palmwood Club, while Thornfang remains the Arena reward
> required to cut marked vines. The areas use shared movement, collision, room
> transitions, and save behavior. They have
> placeholder terrain and props, a scrolling camera, a Thornfang-cuttable gate,
> and fade transitions. A few landmarks have small inspect interactions. A
> versioned autosave preserves Jungle checkpoints, health, owned/selected
> weapons, discovered evidence, both Palm Fruit nodes, and the opened gate. A
> compact inventory overlay and two prototype food pickups are implemented;
> broader enemy combat, dialogue, and settings persistence are not.
>
> **Direction:** the game is planned as top-down 2D pixel art with
> eight-direction movement and 3D-like depth cues (layering, occlusion, and
> stairs/ramps between elevation layers). The Feel Gate is reported passed;
> the Jungle Entrance is the first blockout room.

## Start here

- **Current status, locked decisions, and next task:** [`docs/PROJECT_MEMORY.md`](docs/PROJECT_MEMORY.md)
- **Detailed campaign and gameplay blueprint:** [`docs/MASTER_GAMEPLAY_BLUEPRINT.md`](docs/MASTER_GAMEPLAY_BLUEPRINT.md)
- **Movement feel, measurable tests, and signature-mechanic proposals:** [`docs/GAME_FEEL_AND_SIGNATURE_PLAN.md`](docs/GAME_FEEL_AND_SIGNATURE_PLAN.md)
- **Rules for coding assistants:** [`AGENTS.md`](AGENTS.md)
- **Concept-art/source asset notes:** [`docs/ASSET_INDEX.md`](docs/ASSET_INDEX.md)
- **Archived original design:** [`docs/source/ORIGINAL_GAME_DESIGN.txt`](docs/source/ORIGINAL_GAME_DESIGN.txt)

The project memory is canonical for current status, locked decisions, and next
steps. The gameplay blueprint and game-feel plan hold detailed specifications;
proposals in those documents do not become implementation requirements until
approved. Historical documents and snapshots are organized under
[`docs/source/`](docs/source/README.md).

## Build and run on Windows

Requirements: CMake, MSYS2 MinGW64, and internet access for the first build
(CMake downloads Raylib 5.5 automatically).

- **Build and run:** double-click `run.bat`, or run `.\run.bat` from PowerShell.
- **Build only:** double-click `build.bat`, or run `.\build.bat`.

The scripts add the standard MSYS2 MinGW64 tools to PATH and build into the
ignored `build/` folder. If CMake reports that `build/` was configured with a
different generator, rename that generated folder (for example, to
`build-backup`) and run the script again.

Always build through `build.bat` (or put `C:\msys64\mingw64\bin` on `PATH`
first). A raw `cmake --build` in a plain shell fails silently with
`Error 1` and no compiler message, because the MSYS2 compiler cannot load
its own DLLs outside `build.bat`'s environment.

## Current executable

The current executable loads the starter player and tiles into a 320×180 render
target, displays it with integer scaling/letterboxing, and supports eight-way
walking with a fixed-step simulation. The 640×368 Jungle Entrance blockout
loops around a solid root cluster and leads north toward a shrine-door
landmark. It connects by fade transition to the 640×368 Root Path blockout,
which has a closed thorn gate, a branching bypass route, a root bridge prop,
an optional marker, occluding palms, foreground tall grass, and a short ramp.
Root Path also connects to the Overgrown Clearing, whose safe outer loop
rejoins an exposed central route and reaches a hinted side nook with an
inspectable explorer's note. The Clearing connects to a non-combat Shrine
Approach with a courtyard and a Map of Roots mural; the open north doorway
leads into a traversable Thornback Arena shell with a clear perimeter route
and a safe return path. The Arena's north exit leads into Ashfall Descent, the first Ember
Caverns greybox. Its branching upper/lower route leads around periodic, clearly marked
steam vents; the west ramp links ground and upper elevation. Touching active
steam costs a heart, with the existing invulnerability and knockback, and
depleting health resets to Ashfall's entry checkpoint. The southern exit
returns to the Arena. One Ash Skitter patrols between marked safe patches,
telegraphs its route, dashes across it, and can be hit with the Palmwood Club
or Thornfang during recovery; the encounter resets when the room is re-entered.
Inspect the
pulsing cracked mineral seal on the central path to add it to key-item
discoveries, or look for the optional cooled seam on the west lower path.
No Ashfall boss or completed cavern progression has been implemented. The
west marker starts a
Thornback encounter prototype
that alternates a straight charge with a slower, broad sweep. Each attack has
a distinct ground tell and recovery window; the sweep's marked danger band
leaves a safe repositioning strip above and below it. The player can strike
once during each recovery, and must face Thornback and be in range. The
prototype includes five hearts, visible invulnerability/knockback, a full-
health retry cue at the Arena entrance, and a Thornfang reward.
Before Thornfang is earned, a pulsing beacon marks the trial on the Arena's
west-center side; approach from the south and move north, then west. Stand
just south of the marker, face north, and press E/controller A to begin.
The upper-right campaign mini-map always shows all eight regions and their 24
zones; playable blockout zones, planned zones, and your current zone use
different markers. Press M or controller View to open the full readable
campaign overview. Planned zones are map information, not playable rooms.
After earning it, face the marked Root Path vines and press Space/controller X
to cut them and open a direct corridor through the wall. General combat,
quick weapon switching, and save/load for map, tools, and settings are not
implemented. The versioned
autosave at `%APPDATA%/PalmIsland.sav` records room-entry checkpoints across
the Jungle and Ashfall, plus major clue/reward/gate progress, retaining the
previous file as a `.bak`.
The opened gate is the return shortcut toward the Jungle Entrance and the user
reports it works.
Press I/Tab or controller Y to open the compact inventory overlay; it lists
Thornfang and discovered Jungle evidence and pauses gameplay while open.
In the Overgrown Clearing nook or Shrine Approach courtyard, approach a
visible Palm Fruit and press E or controller A to collect it. H/controller B
eats a serving to restore up to two hearts; food is capped at three servings
and cannot be eaten at full health. Both fruit nodes and the food count are
included in the version-4 autosave.
Health is shown with five pixel-heart icons and a numeric count.
At two hearts or fewer, the count also displays a steady "LOW" label.
The top HUD also shows the current Roots Remember objective, derived from
existing saved Jungle progress.
In Root Path, the thorn weave can be inspected and a visible root marker
activated with E or controller A when approached from the front. Activating
the marker traces the bypass around the gate; clue panels can be reviewed or
closed with the same action. The bounded, smoothed camera follows Ichigo and
resets at room transitions. Solid prop bases and walls block the 10×6 feet
collider; prop drawing is sorted by foot/base Y. F3 shows the live input
source/magnitude, frame time, player speed, position, and elevation. The
prototype uses the existing health system and two finite Palm Fruit pickups;
general combat systems and additional Jungle progression are not implemented.
The user reports the Feel Gate, existing Jungle areas, objective tracker, clue
prompt/panel, marker trace, and Clearing note work well. The Shrine Approach
was hands-on tested before its doorway was opened; the user reports that the
completed Arena shell route and return worked well. The user confirmed the
Phase 2 Thornback sweep, full combat/retry loop, and Thornfang gate route work
well. The user also reports the end-to-end Jungle playtest had no softlocks.
The latest timed user pass met the 10–15 minute target with no unclear
mandatory steps or softlocks; exact elapsed time was not reported.
The user also confirmed the F1/Start controls-help overlay pauses play and
switches cleanly with the inventory panel.

## Optional focused tests

The focused movement, camera, render, Thornback trial, versioned save,
inventory/food, and Jungle objective tests are opt-in. From PowerShell in the
repository, configure and run them with MSYS2 MinGW64 available:

```powershell
$env:Path = "C:\msys64\mingw64\bin;C:\msys64\usr\bin;$env:Path"
cmake -S . -B build -G "MinGW Makefiles" -DPALM_BUILD_TESTS=ON
cmake --build build --parallel 4
ctest --test-dir build --output-on-failure
```

| Action | Keyboard | Controller |
|---|---|---|
| Move (8 directions) | WASD or arrow keys | Left stick / D-pad |
| Interact / close panel | E | A (right face button) |
| Thornback recovery strike / cut marked vines | Space | X (left face button) |
| Eat Palm Fruit | H | B (right face button) |
| Inventory | I or Tab | Y (top face button) |
| Controls help (pauses game) | F1 | Start |
| Debug overlay (FPS, speed, position) | F3 | — |
| Borderless windowed | F11 | — |
| Quit | Esc | — |

## Project layout

```text
AGENTS.md                 Assistant behavior and memory entry point
src/main.c                Window, pixel canvas, fixed-step loop, asset lookup
build.bat                 Configure and build for Windows
run.bat                   Build, then launch the game
src/tuning.h              Centralized movement, interaction, camera, and canvas tuning
src/player.[ch]            Eight-way movement, feet collider, health, collision, animation
src/room.[ch]              Jungle blockout layouts, transitions, props, collision, ramps, clue target
src/input.[ch]             Keyboard/gamepad movement, interaction, help, and action input
src/interaction.[ch]       Small inspect-panel open/close state
src/camera.[ch]            Bounded smoothed camera follow, look-ahead, pixel snap
tests/                     Opt-in gameplay, camera, and render behavior tests
src/render.[ch]            Player/prop/Thornback depth sorting and foreground draw pass
src/thornback.[ch]         Thornback charge/sweep encounter and recovery vulnerability
assets/concept_art/       Visual references and art guide
assets/sprites/           Starter player and prop sprites
assets/tilesets/          Starter 16x16 tileset
assets/previews/          Rendered scene previews
assets/source_packs/      Bundled reference archives
tools/generate_assets.py  Starter sprite/tileset generator
tools/make_map.py         World-map reference generator
docs/PROJECT_MEMORY.md    Current canonical design, status, and work plan
docs/MASTER_GAMEPLAY_BLUEPRINT.md
                          Detailed map-by-map gameplay feature plan
docs/GAME_FEEL_AND_SIGNATURE_PLAN.md
                          Movement targets, Feel Gate, and unapproved mechanic proposals
docs/ASSET_INDEX.md       Asset reference and usage notes
docs/source/              Historical source documents
docs/media/m1_m2/         Uploaded M1–M2 reference screenshots
build/                    Generated files; do not edit by hand
```

For the full status, design decisions, incremental roadmap, test gates, and
AI handoff format, read [`docs/PROJECT_MEMORY.md`](docs/PROJECT_MEMORY.md).
