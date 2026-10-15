# External Reference: Legend of Lua

Reviewed 2026-10-15: [challacade/legend-of-lua](https://github.com/challacade/legend-of-lua), default branch `main` (reviewed revision `351f245653eed9649c7daef34fa1c8b9b7e055f1`).

This is a reference for high-level patterns only. Palm Island remains an
original C11/Raylib game; do not copy its source files, maps, sprites, sounds,
or distinctive presentation into this project. The repository has a root MIT
license for its software, but that alone does not establish the license or
provenance of every bundled asset. Verify asset-specific rights before any
reuse.

## Observed patterns

- The project is a Lua/LÖVE top-down action-adventure with separate modules
  for player, levels, items, enemies, effects, and UI.
- The README links to a browser-playable build, but the game code is a
  single-player project; the reviewed repository does not provide cloud saves,
  accounts, multiplayer, or an online backend.
- Its save model groups player location/current map, progression, health,
  currency/keys, equipment and consumable counts, plus stable IDs for opened
  chests and destroyed breakables.
- Level loading reads object categories such as walls, enemies, loot, chests,
  NPCs, transitions, triggers, trees, and water from map data.
- Chests and breakables connect world interactions to persistent state and
  rewards. This is a useful general pattern for making exploration discoveries
  remain meaningful after leaving and revisiting a room.
- The save implementation serializes executable Lua and evaluates it when
  loading. Do not port that storage format or execution behavior; Palm Island
  should use its planned versioned, validated, non-executable save format.

## Palm Island fit

- The Arena shell visual playtest has passed according to the user's report.
  Follow the current bounded next task in `PROJECT_MEMORY.md`; do not add
  unrelated collection systems or save behavior ahead of that work.
- Later, when persistent progression is in scope, consider stable IDs for
  important one-off world changes (for example a discovered cache, opened
  return route, or collected key item). Keep these small and story/task-driven;
  avoid generic currency grinding.
- When hand-authored room logic becomes difficult to maintain, revisit
  separating room layout/object definitions from rendering and collision.
  Current handcrafted rooms remain the validated approach; do not migrate them
  solely to imitate another project.
- Revisit pickup and reward feedback only in a bounded feel pass and within
  Palm Island's existing Feel Gate and approval rules.

No source code or binary assets from the reference repository are stored in
Palm Island by this note.
