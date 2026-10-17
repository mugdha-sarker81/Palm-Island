# Documentation Guide

Use these files as the active documentation set:

| Need | Canonical document |
|---|---|
| Overall vision, full project phase plan, prompt-sized tasks, phase gates | [PROJECT_ROADMAP.md](PROJECT_ROADMAP.md) |
| Current implementation, verified playtests, locked decisions, immediate next work | [PROJECT_MEMORY.md](PROJECT_MEMORY.md) |
| Campaign story, region/zone content, tasks, weapons, enemies, guardians | [MASTER_GAMEPLAY_BLUEPRINT.md](MASTER_GAMEPLAY_BLUEPRINT.md) |
| Movement and combat feel targets, Feel Gate, signature proposals | [GAME_FEEL_AND_SIGNATURE_PLAN.md](GAME_FEEL_AND_SIGNATURE_PLAN.md) |
| Existing and reference art, generators, source packs | [ASSET_INDEX.md](ASSET_INDEX.md) |
| External inspiration notes | [REFERENCE_LEGEND_OF_LUA.md](REFERENCE_LEGEND_OF_LUA.md) |
| Supplied historical documents and backups | [source/README.md](source/README.md) |

## Authority and update rules

- Project Memory is the authority for **what is true now** and which decisions
  are locked.
- Project Roadmap is the authority for **what to do next and in what order**.
- The Blueprint and Feel Plan describe intended behavior; a proposal there is
  not automatically approved or implemented.
- Keep older filenames as compatibility redirects. Do not create another
  competing plan or copy implementation status into multiple files.
- Record hands-on results as user-reported unless independently observed.
  Automated tests prove only the behavior they actually exercise.
