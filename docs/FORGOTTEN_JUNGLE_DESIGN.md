# Forgotten Jungle — Vertical Slice Design

**Target Length:** 10–15 minutes  
**Purpose:** Prove the core loop of Palm Island (movement → observation → combat → weapon that changes the world)  
**Status:** Ready for implementation planning

---

## High-Level Flow

```
Beach/Village exit
        ↓
1. Jungle Entrance          (safe teaching space)
        ↓
2. Root Path                (foreshadow thorn barriers)
        ↓
3. Overgrown Clearing       (combat + optional secret)
        ↓
4. Shrine Approach          (clue discovery)
        ↓
5. Thornback Arena          (guardian fight)
        ↓
6. Shrine of Agility        (obtain Thornfang)
        ↓
7. Return Path (transformed)(cut barriers + shortcut)
```

---

## Detailed Area Breakdown

### 1. Jungle Entrance
**Mood:** Lush, inviting, slightly overgrown but not threatening.

**What the player sees:**
- Soft light filtering through canopy
- Simple platforms and ledges
- A few low thorn bushes (decorative, not blocking)
- One or two Jungle Slimes that are easy to read

**Player goals:**
- Practice movement and jumping
- Learn basic attack
- Feel safe enough to experiment

**Teaching:**
- Horizontal movement + jump timing
- Basic three-hit combo feel (or simple attack)
- Enemy telegraph (slow slime hop)

**Exit:** Path continues deeper into denser foliage.

---

### 2. Root Path
**Mood:** More vertical and tangled. Nature is starting to resist.

**What the player sees:**
- Large root systems forming natural platforms
- **Thorn barriers** blocking side paths and a higher route (clearly visible, clearly uncuttable yet)
- Environmental markings / root patterns that hint at a deeper path

**Player goals:**
- Navigate upward and forward using platforms
- Notice the thorn barriers and feel the limitation

**Teaching / Foreshadowing:**
- “There are paths I cannot take yet.”
- Visual language of thorns = future ability gate

**Optional:** A small ledge that requires a precise jump (rewards a minor pickup or just a nice view).

---

### 3. Overgrown Clearing
**Mood:** Open but tense. Combat space.

**What the player sees:**
- Wider arena-like clearing
- Mix of Jungle Slimes + one Island Raider (melee enemy with a clear wind-up)
- One **optional secret** (hidden chest or lore fragment) reachable with good platforming or a small risk

**Player goals:**
- Survive a small combat encounter
- Decide whether to take the risk for the secret

**Teaching:**
- Spacing and reading attack wind-ups
- Risk vs reward
- That combat can be more than just button mashing

**Exit:** Path leads toward ancient stonework (Shrine Approach).

---

### 4. Shrine Approach
**Mood:** Mysterious, ancient, quiet.

**What the player sees:**
- Stone structures half-swallowed by roots
- The **Map of Roots** clue (inscription, mural, or journal page)
- A heavy thorn barrier or vine wall blocking the final approach to the shrine / arena

**Player goals:**
- Discover the clue
- Understand that something important lies beyond
- Feel the final barrier before the guardian

**Clue content (short):**
Something like:  
*“The roots remember the agile one. Only those who move as the wind through the thorns may claim what was left behind.”*

**Exit:** Force the player into the Thornback Arena.

---

### 5. Thornback Arena
**Mood:** Tense, focused, fair but demanding.

**Guardian:** Thornback — a creature covered in vines and bark.

**Fight structure (classic three-phase teaching pattern):**

1. **Observe**  
   - Clear charge attack with visible wind-up  
   - Leaves a recovery window if it misses

2. **Adapt**  
   - Player learns to dodge / reposition  
   - Rewarded for attacking during recovery

3. **Master**  
   - Adds a second charge direction or a new readable attack  
   - Still telegraphs clearly  
   - Never becomes unfair or unreadable

**Design goals:**
- The fight itself teaches the quality of **Agility**
- Winning feels like the player got better, not that the boss got weaker
- After victory, the path to the shrine opens

---

### 6. Shrine of Agility
**Mood:** Sacred, quiet, rewarding.

**What happens:**
- Player obtains **Thornfang**
- Short moment of power fantasy (new attack animations, sound, visual flourish)
- Immediate opportunity to test the new ability on nearby thorns

**Thornfang immediate abilities:**
- Fast three-hit combo
- Recovery slash after a successful dodge
- **Exploration:** Cuts thorn barriers / vine walls

---

### 7. Return Path (Transformed)
**Mood:** Empowering and satisfying.

**What changes:**
- The same thorn barriers the player saw earlier can now be cut
- A major shortcut opens that leads quickly back toward the Jungle Entrance / Village direction
- At least one previously inaccessible side path is revealed (small reward or lore)

**Emotional payoff:**
- “The world just changed because of me.”
- Strong desire to go back and explore what was locked before

---

## Enemy Roster for the Slice

| Enemy          | Role                              | Teaching Focus                  |
|----------------|-----------------------------------|---------------------------------|
| Jungle Slime   | Slow, forgiving basic enemy       | Spacing, basic attack timing    |
| Island Raider  | Melee with clear wind-up          | Reading telegraphs, not panic attacking |
| Thornback      | Guardian boss                     | Agility, observation, recovery windows |

---

## Collectibles & Secrets (Minimal)

- 1× Map of Roots (mandatory clue)
- 1× Optional secret chest / lore fragment in Overgrown Clearing
- 1× Small reward on the newly opened path after Thornfang

Keep the number low so the slice stays focused.

---

## Success Criteria for the Vertical Slice

The slice is considered successful when:

1. Movement feels responsive and fun.
2. Basic combat is readable and satisfying.
3. The player notices the thorn barriers early and feels the limitation.
4. The Thornback fight teaches more than it frustrates.
5. Obtaining Thornfang produces a clear “I am more powerful now” moment.
6. Cutting the earlier barriers and opening the shortcut feels rewarding.
7. Total playtime sits roughly in the 10–15 minute range for a first-time player.

---

## Next Steps After This Document

1. Block out the geometry of the seven areas (even with placeholder rectangles).
2. Implement movement + camera.
3. Add the two basic enemies.
4. Build the Thornback fight.
5. Implement Thornfang + the ability to cut barriers.
6. Wire the shortcut and test the full loop.

This document is the design source of truth for the first playable milestone.
