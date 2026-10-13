# Palm Island

**One island. Five legendary weapons. One sword worthy of a hero.**

Palm Island is a 2D side-scrolling action-adventure game where you explore a mysterious tropical island, discover five legendary weapons, battle their guardians, solve environmental puzzles, and ultimately prove yourself worthy of Excalibur.

**Technology:** C + Raylib  
**Platform:** Windows PC (primary)  
**Mode:** Single-player offline  
**Status:** Pre-production / Planning → Early prototype

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

**Forgotten Jungle Vertical Slice** (target: 10–15 minutes of complete gameplay)

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
│   ├── sprites/
│   ├── tiles/
│   ├── audio/
│   └── fonts/
├── src/
│   ├── main.c
│   ├── player.c / player.h
│   ├── camera.c / camera.h
│   ├── physics.c / physics.h
│   ├── world.c / world.h
│   └── ... (modules added as needed)
└── docs/
    ├── PALM_ISLAND_GAME_PLAN.md
    ├── FORGOTTEN_JUNGLE_DESIGN.md
    └── DECISIONS.md
```

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

## How to Build (Coming Soon)

Instructions will be added once the Raylib + CMake skeleton is in place.

---

## Author

**Mugdha Sarker**

This project is being developed as a structured C + Raylib game while learning practical game architecture, combat feel, and open-world-style progression in a constrained 2D environment.

---

*“The island does not give power to those who seek to conquer it. It tests those who might one day protect it.”*
