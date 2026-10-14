# Palm Island Asset Index

This page inventories concept references, generated starter art, and bundled
source archives. The starter sprites and tileset are prototypes, not production
art, and are not yet loaded by the game.

## Concept art

| File | Contents | Intended use |
|---|---|---|
| `assets/concept_art/palm-island-concept-poster.png` | Overall Palm Island concept poster | High-level mood and pitch reference |
| `assets/concept_art/player-weapon-enemy-concepts.png` | Player, weapon, and enemy concept board | Character and combat-art reference |
| `assets/concept_art/forgotten-jungle-concept-sheet.png` | Jungle environment and encounter concept sheet | Forgotten Jungle layout and mood reference |
| `assets/concept_art/ART_GUIDE.md` | Palette, pixel-art, and asset-priority guide | Art direction reference |

The two copies previously named `Fantasy Action-Platformer Design Sheet.png` and `a_polished_game_concept_art_sprite_sheet_style_p.png` were byte-identical; one canonical copy is retained. The same was true for `Jungle Platformer Pixel Asset Sheet.png` and `forgotten_jungle_asset_concept_sheet.png`.

## Starter pixel art

| Location | Contents | Intended use |
|---|---|---|
| `assets/tilesets/tileset_16.png` | 16×16 ground, cliff, ramp, water, and thorn tiles | Top-down test-room prototype |
| `assets/sprites/player_sheet_14x20.png` | Four directions with four animation frames each | Player movement prototype |
| `assets/sprites/props/` | Palm, bush, marker, and pillar sprites | Test-room props |
| `assets/previews/preview_scene.png` | 1280×768 lit scene preview (320×192 source) | Visual reference; taller than the selected 320×180 runtime canvas |

These generated assets are not wired into `src/main.c`. See the art guide for
their palette and layout conventions.

## Generator

`tools/generate_assets.py` regenerates the starter sprites, tileset, and
previews into the folders above. Run it from any working directory with Python,
NumPy, and Pillow installed. Its pixel-art source definitions remain editable
in the script; generated files are not production-ready art.

## Source packs

- `assets/source_packs/Palm_Island_Assets.zip` — original concept-poster package.
- `assets/source_packs/Palm_Island_Character_Weapon_Enemy_Concepts.zip` — concept-board package and its usage notes.
- `assets/source_packs/Palm_Island_Starter_Assets.zip` — jungle concept sheet, asset checklist, and suggested third-party asset sources.

These ZIP files are reference/source packages, not files loaded by the prototype. Review their README and license notes before using third-party assets in a release. Do not slice concept artwork directly into animation frames; create consistent, transparent production sprites first.

## Design source documents

Movement tuning targets, feel tests, and proposed signature mechanics are
documented in `docs/GAME_FEEL_AND_SIGNATURE_PLAN.md`. Original and
supplementary design documents are retained in `docs/source/`. The project
memory, gameplay blueprint, and feel plan in `docs/` are the current working
references.
