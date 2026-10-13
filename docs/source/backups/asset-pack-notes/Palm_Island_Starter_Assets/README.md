# Palm Island — Starter Asset Pack

This pack is a **prototype starter**, focused on the Forgotten Jungle vertical slice for the C + Raylib game.

## Included

- `concept_art/forgotten_jungle_asset_concept_sheet.png` — original AI-generated visual concept sheet for the jungle environment, platforms, plants, bridge, chest, clue, hero, small enemy, and Thornback-style guardian. Use it as a visual reference or prototype source; it is **not a clean, production-ready sprite atlas**. Individual sprites need to be redrawn/cropped, checked for consistent frame sizes, and exported with transparent backgrounds before use in a real game.
- `docs/ASSET_CHECKLIST.md` — practical asset checklist for the first playable milestone.
- `docs/ASSET_SOURCES.md` — links to third-party assets and their stated licenses.

## Suggested folders for the actual game

```text
assets/
  textures/
    player/
    enemies/
    bosses/
    tilesets/
    props/
    weapons/
    effects/
    ui/
  audio/
    music/
    sfx/
  fonts/
```

## Raylib notes

- Prefer PNG for sprites with transparency.
- Keep tile dimensions consistent (for example, 32×32 or 48×48) within each tileset.
- Use a sprite atlas only after confirming every frame's dimensions and spacing.
- Keep collision shapes in code or a separate map file; don't infer collision from decorative pixels.
- The generated concept sheet is for reference/prototyping, not a ready-to-use atlas.

## Attribution / licensing

The generated concept sheet is original AI-generated artwork provided for this project as a visual reference. Third-party sources listed in `docs/ASSET_SOURCES.md` have separate terms; check the live source and include required attribution before shipping.
