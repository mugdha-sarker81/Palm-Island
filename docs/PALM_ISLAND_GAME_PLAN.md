# Palm Island — Game Plan

**Status:** Planning complete for Vertical Slice  
**Based on:** Original game design text (`source/ORIGINAL_GAME_DESIGN.txt`, GDD v1.0) + locked decisions
**Purpose:** Single source of truth for story, progression, combat, weapons, enemies, and level structure before and during early implementation.

---

## 1. Core Concept

**Palm Island is a side-scrolling action-adventure about exploring a mysterious island, learning how its five legendary weapons work, and proving Ichigo is worthy of the power behind Excalibur.**

The main reward is discovery and new ways to play, not grinding for larger numbers. Exploration, combat, puzzles, story clues, and weapon abilities reinforce one another.

---

## 2. Story Direction

Ichigo arrives on Palm Island after discovering an old family symbol connected to the island. He wants to uncover its meaning and the responsibility it carries, rather than simply collect treasure.

Although the island appears largely abandoned, he finds signs of recent activity and a small isolated community in Driftwood Village.

The island’s five guardians protect weapons created to test five qualities:

- **Thornfang** — Agility
- **Embercleaver** — Strength
- **Stormpiercer** — Awareness
- **Dawnstring** — Precision
- **Voidfang** — Courage

The weapons were not made to help someone conquer the island. They test whether a successor is worthy of protecting it. Excalibur is the final proof of that worthiness.

Story is delivered mainly through short inscriptions, journals, NPC dialogue, guardian encounters, and environmental details. The main story remains understandable without optional lore.

---

## 3. Progression System

**No traditional XP levels in version 1.**

Player growth comes from:
- Acquiring the five legendary weapons
- Permanent upgrades (health, weapon improvements, etc.)
- Discoveries, shortcuts, and optional challenges

### Main Progression Path

1. Palmshore Beach — learn movement and basic combat
2. Driftwood Village — learn the legend and receive the first major clue
3. Forgotten Jungle — defeat Thornback and earn Thornfang
4. Use Thornfang to open new routes and shortcuts
5. Explore the remaining four regions in any order
6. Earn all five weapons
7. Open the Excalibur Sanctuary and complete the final trials
8. Earn Excalibur (as a final transformation) and resolve the story

---

## 4. Combat Direction

**Core fight loop:**  
Read an enemy’s wind-up → dodge or reposition → attack during recovery → use a weapon ability when it creates an advantage.

Early combat stays small and responsive:
- Short, readable basic attack combo
- Dodge with a brief invulnerability window
- Distinct attack wind-ups and recovery windows
- Health + checkpoints as the primary survival systems
- Stamina only if playtesting shows it is needed

Weapons offer different play styles rather than strict power increases.

---

## 5. Weapon Roles

| Weapon        | Quality    | Combat Role                              | Exploration / Puzzle Role                  |
|---------------|------------|------------------------------------------|--------------------------------------------|
| Thornfang     | Agility    | Fast three-hit combo + recovery slash    | Cut thorn barriers, follow root markings   |
| Embercleaver  | Strength   | Slow heavy attacks that break guard      | Smash cracked stone, hold pressure plates  |
| Stormpiercer  | Awareness  | Long thrust + charged strike             | Power crystal nodes, electrical mechanisms |
| Dawnstring    | Precision  | Aimed shots + charged shot               | Distant switches, cut ropes, light puzzles |
| Voidfang      | Courage    | Short shadow dash + counter              | Cross shadow gates, bypass magical barriers|

**Excalibur:** Final transformation used only in the climax and ending. Not a permanent sixth weapon.

---

## 6. Enemy Design Principles

Enemies teach one lesson at a time and have distinct, readable behaviors.

- **Jungle Slime** — slow and forgiving (spacing + basic attack)
- **Island Raider** — melee with clear telegraph (don’t panic attack)
- **Ruin Wraith** — slow projectile (use cover / movement / ranged)
- **Armored minion** — blocks careless attacks (requires heavy or well-timed hits)

Introduce new enemies separately before combining them.

---

## 7. Guardian Encounter Pattern

Each guardian teaches and tests the quality of its weapon:

1. **Observe** — readable attack pattern
2. **Adapt** — dodge, counter, or use environment / correct weapon
3. **Master** — increased pressure that remains fair and readable

A missed attack always creates a recognizable recovery opportunity.

---

## 8. Region Structure

1. Palmshore Beach  
2. Driftwood Village  
3. Forgotten Jungle  
4. Ember Caverns  
5. Sunken Temple  
6. Skywatch Cliffs  
7. Lost Kingdom  
8. Excalibur Sanctuary  

Each major region follows a reusable structure:
Arrival → Exploration → Challenge → Clue → Guardian → Reward & Return

---

## 9. First Design Priority

**Forgotten Jungle vertical slice** (see `FORGOTTEN_JUNGLE_DESIGN.md`)

This is the single most important design and implementation target before expanding the rest of the island.

---

## 10. Technical Notes (High Level)

- Language: C
- Library: Raylib
- Architecture: Modular (player, physics, combat, weapons, world, camera, etc.)
- Levels: Separate maps / areas with persistent world state
- Save system: Required from early on (JSON recommended for prototype)

Detailed module list and architecture live in the original GDD and will be refined as code is written.

---

*This document supersedes earlier open questions. All major design decisions for the prototype phase are now locked in `DECISIONS.md`.*
