# Palm Island — The Forgotten Coast

Palm Island is a **single-player 2D exploration adventure** built in C11 with
Raylib 5.5. You play as Ichigo, an outsider whose family symbol is connected
to a quiet, mysterious island. The intended game is about noticing, exploring,
and understanding the island—not grinding or constant combat.

> **Honest status:** this is not the complete game or the planned Forgotten
> Jungle vertical slice. The previous side-scrolling prototype was removed;
> the current executable is a minimal Raylib window scaffold. Top-down
> movement, authored levels, enemies, combat, dialogue, audio, and saving are
> not implemented.
>
> **Direction:** the game is planned as top-down 2D pixel art with
> eight-direction movement and 3D-like depth cues (layering, occlusion, and
> stairs/ramps between elevation layers). A movement test room is the next
> implementation milestone.

## Start here

- **AI/developer project memory and build plan:** [`docs/PROJECT_MEMORY.md`](docs/PROJECT_MEMORY.md)
- **Detailed campaign and gameplay blueprint:** [`docs/MASTER_GAMEPLAY_BLUEPRINT.md`](docs/MASTER_GAMEPLAY_BLUEPRINT.md)
- **Rules for coding assistants:** [`AGENTS.md`](AGENTS.md)
- **Concept-art/source asset notes:** [`docs/ASSET_INDEX.md`](docs/ASSET_INDEX.md)
- **Archived original design:** [`docs/source/ORIGINAL_GAME_DESIGN.txt`](docs/source/ORIGINAL_GAME_DESIGN.txt)

The project memory is the single current source of truth for the vision,
decisions, progress, and next steps. The older planning files in `docs/` are
kept as short redirects so they do not compete with it.

## Build and run on Windows

Requirements: CMake 3.16+, MSYS2 MinGW64 toolchain, and internet access for
the first Raylib download. Open PowerShell in this project folder:

```powershell
$env:Path = "C:\msys64\mingw64\bin;C:\msys64\usr\bin;$env:Path"
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 4
.\build\PalmIsland.exe
```

If CMake reports that the build directory was configured with another
generator, remove only the generated `build` directory and configure again:

```powershell
Remove-Item -Recurse -Force build
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 4
.\build\PalmIsland.exe
```

The `build` directory is generated and ignored by Git. The first configure
downloads Raylib 5.5 into `build/_deps`; subsequent builds reuse it.

## Current executable

The previous side-scrolling prototype has been removed. The current executable
is a minimal Raylib window scaffold; the top-down movement room is not
implemented yet. Close the window or press Esc to exit.

## Project layout

```text
AGENTS.md                 Assistant behavior and memory entry point
src/main.c                Minimal runnable application scaffold
src/player.[ch]            Planned player movement and collision module
src/room.[ch]              Planned test-room geometry and elevation connectors
src/input.[ch]             Planned keyboard/controller input mapping
src/camera.[ch]            Planned room-bounded camera
src/render.[ch]            Planned pixel-art scene rendering and depth ordering
assets/concept_art/       Visual references and art guide
assets/sprites/           Starter player and prop sprites
assets/tilesets/          Starter 16x16 tileset
assets/previews/          Rendered scene previews
assets/source_packs/      Bundled reference archives
tools/generate_assets.py  Starter art generator
docs/PROJECT_MEMORY.md    Current canonical design, status, and work plan
docs/MASTER_GAMEPLAY_BLUEPRINT.md
                          Detailed map-by-map gameplay feature plan
docs/ASSET_INDEX.md       Asset reference and usage notes
docs/source/              Historical source documents
build/                    Generated files; do not edit by hand
```

For the full status, design decisions, incremental roadmap, test gates, and
AI handoff format, read [`docs/PROJECT_MEMORY.md`](docs/PROJECT_MEMORY.md).
