# Palm Island — The Forgotten Coast

Palm Island is a **single-player 2D exploration adventure** built in C11 with
Raylib 5.5. You play as Ichigo, an outsider whose family symbol is connected
to a quiet, mysterious island. The intended game is about noticing, exploring,
and understanding the island—not grinding or constant combat.

> **Honest status:** this is not the complete game or the planned Forgotten
> Jungle vertical slice. The previous side-scrolling prototype was removed.
> The current executable is a small top-down movement room with an integer-
> scaled tile map and eight-way player movement. Collision, camera, props,
> authored levels, enemies, combat, dialogue, audio, and saving are not
> implemented.
>
> **Direction:** the game is planned as top-down 2D pixel art with
> eight-direction movement and 3D-like depth cues (layering, occlusion, and
> stairs/ramps between elevation layers). The movement room is underway;
> obstacle collision and camera bounds are next.

## Start here

- **AI/developer project memory and build plan:** [`docs/PROJECT_MEMORY.md`](docs/PROJECT_MEMORY.md)
- **Detailed campaign and gameplay blueprint:** [`docs/MASTER_GAMEPLAY_BLUEPRINT.md`](docs/MASTER_GAMEPLAY_BLUEPRINT.md)
- **Movement feel, measurable tests, and signature-mechanic proposals:** [`docs/GAME_FEEL_AND_SIGNATURE_PLAN.md`](docs/GAME_FEEL_AND_SIGNATURE_PLAN.md)
- **Rules for coding assistants:** [`AGENTS.md`](AGENTS.md)
- **Concept-art/source asset notes:** [`docs/ASSET_INDEX.md`](docs/ASSET_INDEX.md)
- **Archived original design:** [`docs/source/ORIGINAL_GAME_DESIGN.txt`](docs/source/ORIGINAL_GAME_DESIGN.txt)

The project memory is canonical for current status, decisions, and next steps.
The gameplay blueprint and game-feel plan provide detailed supporting
specifications; proposals in those documents do not become implementation
requirements until approved.

## Build and run on Windows

Requirements: CMake, MSYS2 MinGW64, and internet access for the first build
(CMake downloads Raylib 5.5 automatically).

- **Build and run:** double-click `run.bat`, or run `.\run.bat` from PowerShell.
- **Build only:** double-click `build.bat`, or run `.\build.bat`.

The scripts add the standard MSYS2 MinGW64 tools to PATH and build into the
ignored `build/` folder. If CMake reports that `build/` was configured with a
different generator, rename that generated folder (for example, to
`build-backup`) and run the script again.

## Current executable

The current executable loads the starter tile and player art into a 320×180
render target, displays it with integer scaling/letterboxing, and supports
eight-way walking with a fixed-step simulation. A 10×6 feet collider blocks
cliffs, water, shrine stone, and room edges; the original 32 px stair opening
remains unchanged and can be entered without pixel-perfect horizontal
alignment. Palm trunks and the pillar base block the feet collider; their
canopies remain walk-through so Ichigo can pass visually behind them. Palm and
pillar props are sorted with the player by foot/base Y, and a bush draws as a
foreground occluder. A bounded, smoothed camera with movement look-ahead is
integrated; this 320×176 room is smaller than the 320×180 view, so it stays
centered. F3 shows the collider while debug mode is active. These behaviors are
unit-tested but visual feel still needs in-game playtesting.

## Optional movement tests

The focused movement, camera, and draw-order tests are opt-in. From PowerShell
in the repository, configure and run them with MSYS2 MinGW64 available:

```powershell
$env:Path = "C:\msys64\mingw64\bin;C:\msys64\usr\bin;$env:Path"
cmake -S . -B build -G "MinGW Makefiles" -DPALM_BUILD_TESTS=ON
cmake --build build --parallel 4
ctest --test-dir build --output-on-failure
```

| Action | Keyboard | Controller |
|---|---|---|
| Move (8 directions) | WASD or arrow keys | Left stick / D-pad |
| Debug overlay (FPS, speed, position) | F3 | — |
| Borderless windowed | F11 | — |
| Quit | Esc | — |

## Project layout

```text
AGENTS.md                 Assistant behavior and memory entry point
src/main.c                Window, pixel canvas, fixed-step loop, asset lookup
build.bat                 Configure and build for Windows
run.bat                   Build, then launch the game
src/tuning.h              Centralized movement, collision, camera, and canvas tuning
src/player.[ch]            Eight-way movement, feet collider, collision, animation
src/room.[ch]              Tile-room drawing and solid terrain queries (ramp: M6)
src/input.[ch]             Keyboard and gamepad movement input
src/camera.[ch]            Bounded smoothed camera follow, look-ahead, pixel snap
tests/                     Opt-in movement collision and camera behavior tests
src/render.[ch]            Player/prop depth sorting and foreground draw pass
assets/concept_art/       Visual references and art guide
assets/sprites/           Starter player and prop sprites
assets/tilesets/          Starter 16x16 tileset
assets/previews/          Rendered scene previews
assets/source_packs/      Bundled reference archives
tools/generate_assets.py  Starter art generator
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
