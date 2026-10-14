# Palm Island Assistant Instructions

- Before coding, read `docs/PROJECT_MEMORY.md` sections **Current Status**,
  **Locked Decisions**, and **Next Work Item**. Read only other sections
  needed for the assigned task; avoid loading the archived GDD by default.
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
