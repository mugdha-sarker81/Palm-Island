# Palm Island — Art Guide (simple but gorgeous)

## The rule that makes it look expensive
Few assets + strong lighting. Pixel art looks "gorgeous" because of
COLOR DISCIPLINE and LIGHT, not because of detail.

## Locked numbers
| Thing | Value |
|---|---|
| Internal resolution | 320x180, scaled 4x to your 1280x720 window (nearest-neighbor) |
| Tile size | 16x16 (screen = 20 x 11.25 tiles) |
| Player | 14x20 px, 4 directions x 4 frames (idle, walk A, idle, walk B) |
| Trees / props | 1-3 tiles tall, origin = feet (for Y-sorting) |
| Palette | ~35 colors, defined once in generate_assets.py (dict P) |

## Palette roles (from your docs)
- Shadows: ink 0b1a22, deep 14303a, teal 1d4a52  (never pure black)
- Foliage: jade ramp 1f5a48 -> 2f7a5a -> 46995f -> 72bb77 -> a8d68a
- Stone: 3b4c4c -> 55685f -> 7a8d80 -> a3b19b -> c5cfb8
- Light / clues ONLY: pale f4d58d, glow ffe9a8, amber e8b867
- Player accent: red scarf c9505a (the only saturated red in the world)
RULE: gold glow is reserved for things the player can interact with.

## Five rules for gorgeous
1. Each material = 3-5 colors, light from top-left, always.
2. Put the player in the most contrasting spot (red scarf on green).
3. Light = additive warm glow at clues + cool teal ambient everywhere else.
4. Vignette + god rays + 15 fireflies = 80% of the "wow".
5. Never fill a screen evenly: leave dark calm areas around one bright one.

## Asset priority (do in this order)
1. Ground tiles: grass, jungle floor, dirt path, sand, water(2f), stone floor  [DONE]
2. Cliff face + ramp/stairs + thorn barrier                                   [DONE]
3. Player 4-dir walk                                                         [DONE starter]
4. Props: palm, bush, ancient marker, pillar                                 [DONE]
5. NEXT: shrine door, root bridge, thorn gate open/closed frames, Thornback boss (32x32),
   rock, tall grass overlay, dock/wood planks, village hut (48x48)
6. LAST: UI frame, icons, music

## Subtask / quest UI (keep it tiny)
- Field Notes journal: one parchment panel (amber on ink), max 5 lines per clue.
- Subtask marker: NO quest arrows. A soft gold sparkle on the object when the
  player is within 5 tiles and has not inspected it.
- Interaction prompt: small "E" key chip above the object, 8px bob.
- Task states as 3 icons only: ? unseen clue, diamond found, filled done.

## Raylib setup (320x180 pixel-perfect)
```c
RenderTexture2D rt = LoadRenderTexture(320, 180);
SetTextureFilter(rt.texture, TEXTURE_FILTER_POINT);
Texture2D tiles  = LoadTexture("assets/tilesets/tileset_16.png");
Texture2D player = LoadTexture("assets/sprites/player_sheet_14x20.png");
SetTextureFilter(tiles, TEXTURE_FILTER_POINT);
SetTextureFilter(player, TEXTURE_FILTER_POINT);

// each frame
BeginTextureMode(rt);
  ClearBackground(BLACK);
  // draw tiles, then props + player sorted by feet Y
  Rectangle src = { frame*14, dir*20, 14, 20 };   // dir: 0 down,1 left,2 right,3 up
  DrawTextureRec(player, src, (Vector2){ floorf(px-7), floorf(py-20) }, WHITE);
EndTextureMode();
BeginDrawing();
  // note: render textures are flipped vertically, so use negative height
  DrawTexturePro(rt.texture, (Rectangle){0,0,320,-180}, (Rectangle){0,0,1280,720},
                 (Vector2){0,0}, 0, WHITE);
EndDrawing();
```
Always `floorf()` draw positions to avoid shimmering pixels.

## Lighting in-game (cheap)
Draw a 320x180 darkness layer, then add warm radial gradients with
BLEND_ADDITIVE at the marker, torches and player lantern. See
assets/previews/preview_scene.png.

## Free pro-quality sources (check each license!)
- Kenney.nl (CC0) - safest
- OpenGameArt.org (CC0/CC-BY filter)
- itch.io free top-down packs (read the license on each page)
- Aseprite or LibreSprite to draw your own; Tiled to build maps.

## Maps
Use Tiled (.tmx) with 16x16 tiles; 3 layers: ground, objects, collision.
Keep each zone under ~60x40 tiles. Load later; for the test room a hard-coded
char grid (like in generate_assets.py LAY) is fine.
