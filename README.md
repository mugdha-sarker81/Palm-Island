# Palm Island

**One island. Five legendary weapons. One sword worthy of a hero.**

Palm Island is a 2D side-scrolling action-adventure game where you explore a mysterious tropical island, discover five legendary weapons, battle their guardians, solve environmental puzzles, and ultimately prove yourself worthy of Excalibur.

**Technology:** C + Raylib  
**Platform:** Windows PC (primary)  
**Mode:** Single-player offline  
**Status:** Early prototype — movement and platforming foundation

---

## Core Fantasy

You are **Ichigo**. After discovering an old family symbol that matches ancient markings on Palm Island, you arrive seeking answers. The island appears abandoned, yet its mechanisms still function and five guardians still protect weapons forged to test five qualities:

| Weapon        | Quality    | Region            |
|---------------|------------|-------------------|
| Thornfang     | Agility    | Forgotten Jungle  |
| Embercleaver  | Strength   | Ember Caverns     |
| Stormpiercer  | Awareness  | Sunken Temple     |
| Dawnstring    | Precision  | Skywatch Cliffs   |
| Voidfang      | Courage    | Lost Kingdom      |

Excalibur is not simply a stronger sword — it is the final proof that you are worthy to protect the island.

---

## Design Pillars

1. **Curiosity over quest markers** — The world rewards observation and experimentation.
2. **Every weapon changes gameplay** — Each weapon adds both a combat style and a new way to interact with the world.
3. **Freedom with meaningful progression** — You can explore freely, but certain obstacles require specific abilities.
4. **Combat with purpose** — Enemies teach lessons, create pressure, and support environmental storytelling.
5. **A memorable payoff** — Excalibur must feel earned through mastery and discovery.

---

## Current Milestone

**Current: Movement Prototype.** Responsive movement, jumping, platform collisions, and a side-scrolling camera, using placeholder shapes while controls and physics are tuned.

**Target: Forgotten Jungle Vertical Slice** (10–15 minutes of complete gameplay)

This single region will prove the core loop:
- Movement & basic combat
- Observation & environmental clues
- Guardian fight that teaches the weapon’s quality
- Weapon acquisition that changes the world (cutting thorn barriers + opening shortcuts)

See `docs/FORGOTTEN_JUNGLE_DESIGN.md` for the full breakdown.

---

## Project Structure

```
Palm-Island/
├── README.md
├── CMakeLists.txt
├── .gitignore
├── assets/
│   ├── concept_art/
│   │   ├── forgotten-jungle-concept-sheet.png
│   │   ├── palm-island-concept-poster.png
│   │   └── player-weapon-enemy-concepts.png
│   └── source_packs/
├── src/
│   └── main.c
└── docs/
    ├── DECISIONS.md
    ├── FORGOTTEN_JUNGLE_DESIGN.md
    ├── PALM_ISLAND_GAME_PLAN.md
    ├── ASSET_INDEX.md
    ├── archive/ (superseded planning drafts)
    └── source/ (original designs and reference packages)
```

See [docs/ASSET_INDEX.md](docs/ASSET_INDEX.md) for the concept art and bundled asset packs. Original design documents are kept under `docs/source/`; the earlier game-plan draft is retained under `docs/archive/`.

---

## Locked Design Decisions

See `docs/DECISIONS.md` for the full list of finalized choices (story motivation, region order, Excalibur handling, progression system, controls, etc.).

---

## Development Roadmap (High Level)

1. **Movement Prototype** — Controllable Ichigo with gravity, jump, and side-scrolling camera.
2. **Combat Prototype** — Basic attack + one enemy type that feels good to fight.
3. **Exploration Prototype** — Thorn barriers, clue discovery, and the Thornback fight.
4. **Vertical Slice** — Complete Forgotten Jungle experience (10–15 min).
5. **Full Game** — Expand the same architecture to the remaining regions + Excalibur finale.

---

## Build and Run

Requirements:
- CMake 3.16 or newer
- MSYS2 MinGW64 with GCC, CMake, and Make (`mingw-w64-x86_64-gcc`, `mingw-w64-x86_64-cmake`, `mingw-w64-x86_64-make`)
- Git and an internet connection for the initial Raylib download

From PowerShell in the project directory, after installing MSYS2:

```powershell
$env:Path = "C:\msys64\mingw64\bin;C:\msys64\usr\bin;$env:Path"
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 4
```

Run `build\PalmIsland.exe`. The first configure downloads and extracts pinned Raylib 5.5 into the ignored build directory; subsequent builds reuse it.

### Prototype Controls

- **Move:** A/D or arrow keys
- **Jump:** Space, W, or Up (release early for a shorter jump)
- **Restart:** R
- **Close:** Escape
- **Gamepad:** left stick or D-pad to move, A to jump

The next milestone is a basic attack and one readable enemy encounter.

---

## Author

**Mugdha Sarker**

This project is being developed as a structured C + Raylib game while learning practical game architecture, combat feel, and open-world-style progression in a constrained 2D environment.

---

*“The island does not give power to those who seek to conquer it. It tests those who might one day protect it.”*
