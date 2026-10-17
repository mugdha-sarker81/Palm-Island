# Palm Island Assistant Instructions

- Before coding, read `docs/PROJECT_MEMORY.md` sections **Current Status**,
  **Locked Decisions**, and **Next Work Item**. Read only other sections
  needed for the assigned task; avoid loading the archived GDD by default.
- For campaign, movement, task, inventory, weapon, map, obstacle, or enemy
  planning/implementation, read the matching section of
  `docs/MASTER_GAMEPLAY_BLUEPRINT.md`.
- For movement, camera, combat feel, interaction feedback, effects, audio
  feedback, or "make it cooler/smoother" tasks, read
  `docs/GAME_FEEL_AND_SIGNATURE_PLAN.md`. Treat its numbers as starting
  targets to verify in playtests, keep tunables in `src/tuning.h`, and change
  one tuning value per pass. Signature mechanics are proposals; get approval
  before implementing them and do not start them before the Feel Gate passes.
- For UI, tutorials, onboarding, objectives, accessibility, or
  general player-experience work, read the **Player UX and in-game teaching**
  section in `docs/MASTER_GAMEPLAY_BLUEPRINT.md` and the relevant
  `docs/PROJECT_ROADMAP.md` gate. Design each feature to make the player's
  goal, available action, result, and recovery understandable without
  unsolicited text walls or permanent waypoint arrows.
- Apply the UX rules in every playable increment, not only the final polish
  phase: teach one idea at a time in a safe context, let players try it, give
  immediate and specific feedback, show the current keyboard/controller
  binding, explain blocked actions, and preserve a clear way to retry or leave.
  Avoid relying on color, audio, animation, or reading alone to communicate
  critical information.
- Validate usability with a fresh-player task, not only a build or unit test.
  Record where the player hesitates, misunderstands, repeats an ignored
  action, or cannot explain a failure; fix the cause before adding more
  instructions. Do not claim a UX/playtest gate passed without observed or
  explicitly user-reported evidence.
- Treat proposed HP, damage, food, material counts, and shop prices there as
  untested starting points, not final balance. Keep crafting limited to the
  approved small fixed tool set; do not add generic recipes or material
  grinding.
- Treat `docs/PROJECT_MEMORY.md` as the current source of truth. Archived
  planning documents and old GDD material do not override it.
- Work in small, playable increments. Do not attempt to implement the whole
  game or multiple roadmap phases in one change.
- Inspect the existing code before editing. Preserve working behavior, make
  the smallest complete change, and build the Windows target after code edits.
- Report what changed, the exact validation performed, and any remaining
  blockers. Do not claim a feature works unless it was verified.
- When implementation or a decision changes project state, update the matching
  status/decision in `docs/PROJECT_MEMORY.md`. Keep the handoff concise.
- Do not introduce external packages, large systems, or production assets
  without a clear need and an explicit plan.

See `README.md` for the short project overview and Windows build commands.
