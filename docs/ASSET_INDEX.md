# Palm Island Asset Index

This page inventories concept references, generated starter art, and bundled
source archives. The starter sprites and tileset are prototypes, not production
art; the current movement room loads the player sheet and tileset.

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
| `assets/tilesets/tile_wood_plank.png` | Single wood-plank tile | Jungle/village blockout placeholder |
| `assets/sprites/player_sheet_14x20.png` | Four directions with four animation frames each | Player movement prototype |
| `assets/sprites/props/` | Palm, bush, marker, pillar, rock, root bridge, shrine door states, tall grass, thorn gate states, and village hut sprites | Test-room and upcoming blockout props |
| `assets/previews/preview_scene.png` | 1280×768 lit scene preview (320×192 source) | Visual reference; taller than the selected 320×180 runtime canvas |
| `assets/previews/phase3_preview_4x.png` | 4× pixel-art preview showing proposed blockout props | Phase 3 visual reference; not a runtime scene |
| `assets/previews/palm_island_map_1280x720.png` | 4× labeled overview map of Palm Island | World-layout reference; not a runtime scene |
| `assets/previews/palm_island_mission_map_1780x720.png` | Island map with a numbered narrative route, weapons, and route legend | Mission-flow concept/reference; not implemented gameplay |

The player sheet, tileset, palm, pillar, and bush are loaded in the
movement-room prototype. The palm and pillar use foot/base Y sorting; the bush
is a foreground occluder. Palm-trunk and pillar-base collision is enabled;
upper canopy areas remain walk-through. The marker remains unused. See the art
guide for palette and layout conventions.

## Generator

`tools/generate_assets.py` regenerates the starter sprites, tileset, and
previews into the folders above. Run it from any working directory with Python,
NumPy, and Pillow installed. Its pixel-art source definitions remain editable
in the script; generated files are not production-ready art.

`tools/make_map.py` generates labeled and unlabeled 320×180 world-map PNGs and
4× nearest-neighbor versions. Run it from the repository root with an output
directory, for example `python tools/make_map.py assets/previews`. It requires
NumPy, Pillow, and SciPy; the generated maps are visual references, not runtime
game assets.

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
