# Palm Island Asset Index 

This page inventories concept references, generated starter art, and bundled
source archives. The starter sprites and tileset are prototypes, not production
art. The current prototype loads the player sheet, tileset, and the closed-gate,
closed-shrine, palm, rock, marker, tall-grass, and root-bridge props.

## Concept art

| File | Contents | Intended use |
|---|---|---|
| `assets/concept_art/palm-island-concept-poster.png` | Overall Palm Island concept poster | High-level mood and pitch reference |
| `assets/concept_art/player-weapon-enemy-concepts.png` | Player, weapon, and enemy concept board | Character and combat-art reference |
| `assets/concept_art/forgotten-jungle-concept-sheet.png` | Jungle environment and encounter concept sheet | Forgotten Jungle layout and mood reference |
| `assets/concept_art/ART_GUIDE.md` | Palette, pixel-art, and asset-priority guide | Art direction reference |

The two copies previously named `Fantasy Action-Platformer Design Sheet.png` and `a_polished_game_concept_art_sprite_sheet_style_p.png` were byte-identical; one canonical copy is retained. The same was true for `Jungle Platformer Pixel Asset Sheet.png` and `forgotten_jungle_asset_concept_sheet.png`.

## Superseded map prototype

An older supplied world-map ZIP and its temporary extraction proposed a
different set of region names, gate flags, save fields, and fast-travel
behavior. Its reusable architectural points—data-defined campaign topology,
paired exits, and graph validation—are already represented by the current
runtime map and tests. Its region list and suggested save/fast-travel changes
are superseded or unapproved; use the
[Project Roadmap](PROJECT_ROADMAP.md) and
[Master Gameplay Blueprint](MASTER_GAMEPLAY_BLUEPRINT.md) as the campaign
authority. The redundant ZIP and extracted staging copy were removed.

The campaign map PNGs below are retained as visual concepts only. They do not
define current runtime topology or override the approved region/zone names.

## Starter pixel art

| Location | Contents | Intended use |
|---|---|---|
| `assets/tilesets/tileset_16.png` | 16×16 ground, cliff, ramp, water, and thorn tiles | Top-down test-room prototype |
| `assets/tilesets/tile_wood_plank.png` | Single wood-plank tile | Jungle/village blockout placeholder |
| `assets/sprites/player_sheet_14x20.png` | Four directions with four animation frames each | Player movement prototype |
| `assets/sprites/props/` | Palm, bush, marker, pillar, rock, root bridge, shrine door states, tall grass, thorn gate states, and village hut sprites | Test-room and upcoming blockout props |
| `assets/previews/preview_scene.png` | 1280×768 lit scene preview (320×192 source) | Visual reference; taller than the selected 320×180 runtime canvas |
| `assets/previews/phase3_preview_4x.png` | 4× pixel-art preview showing proposed blockout props | Phase 3 visual reference; not a runtime scene |
| `assets/previews/palm_island_map_1280x720.png` | 4× labeled overview map of Palm Island | Older world-layout concept; not a runtime scene or authoritative topology |
| `assets/previews/palm_island_mission_map_1780x720.png` | Island map with a numbered narrative route, weapons, and route legend | Older mission-flow concept; not implemented gameplay or authoritative region order |

The open gate/shrine variants, village hut, bush, and pillar are currently
reference assets, not props used by the three-area prototype. Runtime palms and
other solid prop bases participate in collision; props are drawn by foot/base
Y with foreground grass in the foreground pass. The marker is available in the
Root Path blockout. See the art guide for palette and layout conventions.

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

The [Project Roadmap](PROJECT_ROADMAP.md) describes when to plan and replace
prototype assets. Movement tuning targets, feel tests, and proposed signature
mechanics are documented in [Game Feel and Signature Plan](GAME_FEEL_AND_SIGNATURE_PLAN.md).
Original and supplementary design documents are retained in
[source/](source/README.md). Use [Project Memory](PROJECT_MEMORY.md) for
current implementation status and the [Master Gameplay Blueprint](MASTER_GAMEPLAY_BLUEPRINT.md)
for the intended regions and content.
