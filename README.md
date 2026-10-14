# Palm Island — The Forgotten Coast

Palm Island is a **single-player 2D exploration adventure** built in C11 with
Raylib 5.5. You play as Ichigo, an outsider whose family symbol is connected
to a quiet, mysterious island. The intended game is about noticing, exploring,
and understanding the island—not grinding or constant combat.

> **Honest status:** this is an early playable prototype, not the complete
> game or the planned Forgotten Jungle vertical slice. The current executable
> has a long code-drawn coast, basic platform movement, parallax scenery, one
> ancient marker interaction, and a field-note message. It does not yet have
> enemies, combat, Thornfang, authored levels, dialogue, audio, or saving.
>
> **Direction change:** the planned full game is now top-down 2D pixel art
> with eight-direction movement and 3D-like depth cues (layering, occlusion,
> and stairs/ramps between elevation layers). The current side-scrolling build
> is a legacy movement prototype; it has not been converted yet.

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

## Current prototype controls

| Action | Keyboard | Controller |
|---|---|---|
| Move | A/D or arrow keys | Left stick / D-pad |
| Jump | Space, W, or Up | Bottom face button |
| Examine marker | E | Right face button |
| Restart prototype | R | — |
| Quit | Esc | — |

## Project layout

```text
AGENTS.md                 Assistant behavior and memory entry point
src/main.c                Current prototype implementation
assets/concept_art/       Visual references
assets/source_packs/      Bundled reference archives
docs/PROJECT_MEMORY.md    Current canonical design, status, and work plan
docs/MASTER_GAMEPLAY_BLUEPRINT.md
                          Detailed map-by-map gameplay feature plan
docs/ASSET_INDEX.md       Asset reference and usage notes
docs/source/              Historical source documents
build/                    Generated files; do not edit by hand
```

For the full status, design decisions, incremental roadmap, test gates, and
AI handoff format, read [`docs/PROJECT_MEMORY.md`](docs/PROJECT_MEMORY.md).
