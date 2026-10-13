# Palm Island — Locked Design Decisions

**Status:** Finalized for Vertical Slice / First Prototype  
**Date:** October 2026

These decisions close the open questions from the original Game Plan so development can proceed without ambiguity.

---

## 1. Ichigo’s Personal Motivation

**Decision:**  
Ichigo discovered an old family symbol that matches ancient markings found on Palm Island. He travels to the island seeking to understand what the symbol means and what responsibility it carries.

**Why this works:**
- Personal without requiring a long cutscene or complex backstory.
- Ties directly into the theme of “worthiness” and “protection” rather than conquest or treasure hunting.
- Leaves room for optional lore (family journals, old letters) later.

---

## 2. Driftwood Village

**Decision:**  
Driftwood Village is a small, isolated community that lives *with* the island’s rules. The villagers know the legends of the five weapons and the guardians. They warn outsiders but do not venture deep into the guarded regions themselves.

**Role in the game:**
- Safe hub after the Beach tutorial.
- Source of rumors, warnings, and the first major clue pointing toward the Forgotten Jungle.
- Optional minor NPCs and flavor dialogue.
- Later can offer simple services (rest, minor upgrades, lore).

---

## 3. Region Order / Openness

**Decision:** Semi-linear progression.

- Fixed path: Palmshore Beach → Driftwood Village → Forgotten Jungle.
- After obtaining **Thornfang**, the remaining four major regions (Ember Caverns, Sunken Temple, Skywatch Cliffs, Lost Kingdom) can be completed in any order the player chooses.
- Excalibur Sanctuary unlocks only after all five weapons have been earned.

**Why this works:**
- Gives a strong guided first experience.
- Delivers classic Metroidvania-style freedom after the first major ability.
- Keeps the final sanctuary feeling special and earned.

---

## 4. Excalibur Handling

**Decision:**  
Excalibur is a **final transformation / temporary power** used only during the climax and ending sequence. It is **not** a permanent sixth weapon that the player switches between in the open world.

**Rationale:**
- Avoids inventory/management bloat.
- Makes the final moment feel unique and ceremonial.
- Keeps the five legendary weapons as the core permanent toolkit.

---

## 5. Character Progression / Leveling

**Decision:**  
No traditional XP-based character levels in version 1.

**Growth comes from:**
- Acquiring the five legendary weapons (each changes combat *and* exploration).
- Permanent upgrades (health increases, weapon improvements, etc.).
- Discoveries, shortcuts, and optional challenges.

Optional upgrades help but are never required to finish the main story.

---

## 6. Control Scheme Priority

**Decision:**  
Support **both keyboard and controller** from the beginning.

- Primary development and tuning starts with keyboard.
- Gamepad support is added early (before the vertical slice is considered complete).
- Default bindings will be defined once movement and combat feel are locked.

---

## 7. Forgotten Jungle Scope

**Decision:**  
The first playable milestone is a self-contained **Forgotten Jungle vertical slice** lasting approximately 10–15 minutes.

It must demonstrate:
- Enjoyable movement and basic combat
- Environmental observation and clue discovery
- A guardian fight that teaches the quality of Agility
- Weapon acquisition that meaningfully changes the world (cutting thorn barriers + opening a shortcut)

Full detailed design is in `FORGOTTEN_JUNGLE_DESIGN.md`.

---

## Summary Table

| Topic                  | Final Decision                                      |
|------------------------|-----------------------------------------------------|
| Ichigo’s motivation    | Family symbol / sense of responsibility             |
| Village role           | Isolated community that knows the legends           |
| Region order           | Semi-linear (fixed intro → open after Thornfang)    |
| Excalibur              | Final transformation only (not permanent weapon)    |
| XP / Levels            | None in v1                                          |
| Controls               | Keyboard + Controller from early on                 |
| First milestone        | Forgotten Jungle vertical slice (10–15 min)         |

These decisions are now considered locked for the prototype phase. Any major changes will be recorded here with a date and reason.
