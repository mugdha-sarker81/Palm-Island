# Palm Island — Game Feel and Signature Mechanics Plan

**Status:** Proposed design + tuning targets; nothing here is implemented.
**Purpose:** Make Palm Island *smooth* to play and *memorable* to remember,
while staying inside the quiet, exploration-first pillars and a solo/limited-AI
workflow.
**Read this when:** working on movement, camera, combat feel, interaction
feedback, effects, audio feedback, or any "make it cooler" task.
**Do not read this for:** story, map layout, or save systems.

> All numbers are **starting values at the 320×180 virtual canvas** (16 px
> tiles, 14×20 player sprite). Tune by playing, not by arguing. Keep every
> tunable in one place (`src/tuning.h`) so a weak model can change a number
> without touching logic.

## 0. The one-sentence goal

> Walking around should feel good with nothing else in the game, and every
> discovery should make the player say "oh, *nice*."

Two rules protect this:

1. **Smooth first, cool second.** No signature feature is built until the
   movement room passes the Feel Gate (section 7).
2. **Cool must serve noticing.** A flashy feature that does not help the
   player notice, understand, or enjoy the island is cut.

## 1. Smoothness specification (the foundation)

### 1.1 Update loop and rendering

| Item | Decision / target |
|---|---|
| Simulation | Fixed timestep, **120 Hz** (`1/120 s`), accumulator, max 5 steps per frame; clamp frame time to 0.1 s after a stall |
| Rendering | Display rate with vsync; draw at display rate |
| Canvas | 320×180 `RenderTexture`, point filter, **integer 4×** to 1280×720; letterbox bars for other sizes, never non-integer stretch |
| Positions | Keep `float` world positions; **floor to integer pixels only when drawing** (prevents shimmer) |
| Input | Poll once at the start of each frame; store as "intent" (move vector + button presses with timestamps) |
| Camera | Float target; round to integer **at render time** only |

### 1.2 Walking

| Parameter | Start value | Why |
|---|---:|---|
| Walk speed | **76 px/s** (≈4.75 tiles/s) | Crosses the 320 px screen in ~4 s: lively but observant |
| Time to full speed | **0.07 s** (accel ≈ 1085 px/s²) | Instant-feeling, not teleporty |
| Time to stop | **0.05 s** (decel ≈ 1520 px/s²) | Tiny settle, no ice-skating |
| Direction-change boost | accel × 1.5 when input opposes velocity | Snappy turns |
| Diagonal | Normalize input vector, then scale by speed | No 41% diagonal bonus |
| Analog stick | Radial dead zone 0.20, remap 0.20–1.0 → 0–1, keep magnitude (walk slower with a gentle push) | Controller parity |
| Facing | Last move direction wins; quantize to 4 sprite directions with a **hysteresis of 15°** so diagonals do not flicker | Clean sprites |
| Shallow water | Speed × 0.7 with ripples; deep water blocked | Readable terrain |

### 1.3 Collision (the part that makes it feel "smooth")

- **Feet collider:** 10×6 px, centered on the feet point. The head/body can
  overlap walls and trees above the feet — this is what creates the 3D feeling.
- **Axis-separated resolution** (move X, resolve, move Y, resolve) so the player
  *slides* along walls.
- **Wall-slide target:** walking 45° into a wall keeps ≥ 70% of normal speed
  along the wall.
- **Corner assist:** when blocked but within **4 px** of clearing a corner,
  nudge the player around it. Narrow doorways (16 px) must be enterable without
  pixel-perfect aim.
- **Sub-stepping:** split movement so no step exceeds 4 px (no tunneling).
- Solid tiles, props, and cliff faces share one collision query; draw debug
  boxes with F3.

### 1.4 Camera

| Parameter | Start value |
|---|---|
| Follow | Exponential smoothing, rate **6 /s** |
| Look-ahead | Up to **14 px** toward movement direction, itself smoothed |
| Bounds | Clamp to room; rooms smaller than the screen are centered |
| Snap cases | Hard-snap on room load and respawn (no sliding across the map) |
| Render | Round camera position to integer pixels when drawing |
| Dead zone | None at first; add only if the camera feels nervous |

### 1.5 Transitions

- Zone/door transition: **0.2 s fade out → load → 0.2 s fade in**; input locked
  only during the fade.
- Spawn the player on the *matching* exit marker, facing away from the door.
- Never drop the player inside a wall, hazard, or locked gate (see the
  checkpoint rules in the blueprint).

### 1.6 Interaction feel

- **Interact range:** 20 px from the feet point to the object's interact box,
  and the object must be roughly in front (within ~70° of facing).
- **Prompt:** appears at 28 px (slightly before the press range) with a 0.15 s
  fade; shows the *current* binding.
- **Input buffer:** 0.12 s — pressing E just before the prompt appears still works.
- **Feedback on press:** within 1 frame, give *some* response (sound, glow
  pulse, or sprite pose). Never leave a press feeling ignored.

## 2. Action feel (only when combat/tool use arrives)

Combat stays rare, but every action must be crisp.

| Action | Start values |
|---|---|
| Dodge step | 36 px over **0.20 s**; invulnerable for the first **0.14 s**; cooldown 0.30 s; buffered 0.12 s; cancelable into walking in the last 0.05 s |
| Weapon swing | Anticipation 0.05 s → active 0.07 s → recovery 0.15 s; walking speed ×0.3 during the swing; buffered 0.10 s |
| Hit-stop | 0.04 s freeze on a confirmed hit (2–3 frames), on both attacker and target |
| Screen shake | 1 px, 0.08 s, only for impactful hits and boss slams (never for routine walking) |
| Hurt | 0.5 s invulnerability with blink, small knockback (8 px), no stun-lock |
| Telegraphs | Enemy wind-up ≥ **0.5 s** for the first enemy type, visible flash + sound cue, hit zone shown clearly |

Fairness rule: *every* death should be explainable by the player ("I saw it
and I was late") — never by hidden hitboxes or camera blocking.

## 3. Signature mechanics (what makes Palm Island *cool*)

These are **proposals**. Approve each before building it. They all reinforce
"noticing" instead of adding button-mashing. Keep the first slice to the ones
marked **Slice**.

### 3.1 Attune — "listen to the island" (**Slice**)

Ichigo's family mark is the island's key. **Hold F / left trigger** to
*attune*:

- The screen gently desaturates and darkens at the edges; ambient audio dips and
  a soft hum rises.
- Undiscovered clues within **7 tiles** shimmer with a pulse; closer = faster
  pulse. **No arrow, no minimap marker** — the player still decides where to
  look.
- Walking is slowed to ×0.6 while attuning (it is a choice: speed vs. noticing).
- Free to use, unlimited, no cooldown.

Why it is cool: it turns the core fantasy ("a place waiting for someone to
notice") into a *button*, and it makes empty-feeling spaces feel alive.
Why it is safe: it never gives routes or answers, only hints that something is
here. It satisfies the "no quest markers" pillar.

### 3.2 The Scarf Tell (**Slice**, nearly free)

Ichigo's red scarf is already a character signature. Make it also a quiet UI:

- It trails behind movement and flutters in wind (visual only).
- If the player stands idle for ~5 s near an undiscovered clue (within 7 tiles),
  the scarf slowly drifts toward that general direction.
- Disappears the moment the player moves; works with Attune off.

A subtle, optional help for lost players that never interrupts others.

### 3.3 Living Marks — the five lines (**Slice**)

The old prototype's field note ("Five lines point inward. One has been erased.")
is the game's best hook. Make it systemic:

- Every ancient marker shares one glyph with **five lines**.
- Start of the game: four lines glow faintly, one is dark.
- Each weapon earned **lights one more line on every marker already seen**,
  and the first time the player returns to a marker after a weapon, it plays a
  short pulse.
- Final reveal ties to "the erased line."

Data-driven, cheap (one `lines_lit` counter + one glyph draw routine), and it
gives players a visible "the world changed because of me" feeling.

### 3.4 The Read — reward for observing (**Slice**, for Thornback)

A **perfect dodge** (dodge started within 0.12 s before the hit would land):

- Time slows to ×0.3 for 0.4 s with a soft chime.
- Briefly highlights the guardian's weak point or the safe repositioning lane.
- Gives a free counter window.

It teaches the pillar ("watch, understand, act") and feels amazing, but it is
an *optional* skill; the fight is still beatable without it.

### 3.5 Thorn-cut feel (**Slice**)

Thornfang's first use should be an *event*:

- 0.1 s hit-stop → thorns recoil, then peel apart in 4–6 frames with leaf
  particles and a distinct sound.
- The cut barrier stays open permanently (persisted flag).
- A short camera nudge toward the opened path to show *what* changed.

### 3.6 Vista beats (**Slice**, cheap but gorgeous)

At 1–2 planned spots per zone, the camera gently pans to a landmark (shrine,
tower, waterfall) for ~2 s as the player enters, then returns. Skippable by
moving. No cutscene, no dialogue — a "breath" that shows the world.

### 3.7 Reactive world (**Slice (basic)**, rest later)

- Grass sways and parts when walked through; fireflies scatter and re-gather.
- Water ripples at the feet; sand holds footprints that fade over ~4 s.
- Footstep sounds change by surface (sand, grass, stone, wood, shallow water).
- Birds/insects fall silent when a guardian is near and resume after.

### 3.8 Tide of Palmshore (**Later**)

Palmshore's route changes with a slow tide cycle (visible rising water, new
paths when low). Great flavor, but needs timing and save logic — defer until
after the Jungle slice.

### 3.9 Mood by progress (**Later**)

Region palettes shift subtly as story flags change (e.g., the Jungle becomes
slightly brighter after Thornfang). Implemented as a tint + glow parameter per
region/flag; do it after lighting exists.

### Slice feature budget

| Feature | Slice? | Cost | Pillar served |
|---|---|---|---|
| Attune | Yes | Medium | Curiosity over markers |
| Scarf Tell | Yes | Low | Character + gentle guidance |
| Living Marks | Yes | Low | Story through place |
| The Read | Yes | Medium | Weapons are understanding |
| Thorn-cut feel | Yes | Low | Weapons change the world |
| Vista beats | Yes | Low | Quiet wonder |
| Reactive world (basic) | Yes | Low–Med | Quiet wonder |
| Tide, Mood by progress | No | Med | Later |

**Hard cap:** if a slice milestone slips, cut in this order: Reactive world →
Vista beats → The Read → Scarf Tell. Never cut Attune or Living Marks; they are
the identity.

## 4. Juice and polish menu (cheapest first)

### Tier 0 — Free (build with the movement room)
- Ground shadow ellipse under every character/prop.
- Idle "breathing" (1 px body bob every ~1.5 s).
- Walk dust puffs (2–3 pixels, 0.25 s life) on dirt/sand.
- Squash feel via a 1-frame body offset when stopping.
- Pixel-snapped drawing (`floorf`) everywhere.
- F3 debug overlay: FPS, frame time, collision boxes, feet point, live tuning.

### Tier 1 — Cheap, high impact (build with the Jungle route)
- Warm additive glow at clues and shrines; cool teal ambient grade.
- Vignette and soft light shafts (see `assets/previews/preview_scene.png`).
- 10–20 drifting firefly particles, sparse.
- Foreground canopy occluders that fade to ~40% alpha when the player is under.
- Interaction prompt with a small bounce; clue "ping" sound.
- Fade transitions between zones.

### Tier 2 — Polish (build near the end of the slice)
- Layered ambience per zone; low-passed audio when attuning.
- Hit-stop, 1 px shake, damage flash on the shared hit routine.
- Leaf/petal particles on cuts; ripple rings on water entry.
- Title screen, pause menu, simple settings (volume, fullscreen).

### Tier 3 — Dream (only after the slice succeeds)
- Dynamic light cycle, tide, region mood shifts, weather.
- Photo/sketch entries in the journal for important discoveries.
- Accessibility extras (reduced shake, hold-to-toggle, colorblind clue shapes).

## 5. Audio feel (simple rules)

- Every player action has a sound: step, interact, attune hum, dodge whoosh,
  swing, cut, hit.
- Clue pings get **pitch by distance** (closer = brighter).
- Music is sparse: long ambient layers, stingers for discoveries, no constant loop.
- Use CC0/licensed audio only; record sources in `docs/ASSET_INDEX.md`.

## 6. Jungle slice beat sheet — feel and "cool" per beat

| Beat | Smooth feel focus | Cool moment |
|---|---|---|
| Jungle Entrance | Learn 8-way walk and interact in a safe space | First vista beat: the shrine's glow far away |
| Root Path | Corner assist in narrow root lanes; ramp transition | Attune reveals faint shimmer in the roots; thorn gate first seen |
| Overgrown Clearing | Calm exploration; optional secret | Scarf Tell nudges toward the secret if the player lingers |
| Shrine Approach | Occluding canopy; stair climb | Living Mark glyph with one dark line (the hook) |
| Thornback | Clear telegraphs; dodge timing; checkpoint retry | The Read slow-motion after a perfect dodge |
| Shrine of Agility | No timing pressure; reward moment | Thornfang acquisition: marker lights a line |
| Transformed return | Fast, easy walk home | Thorn-cut with hit-stop; shortcut opens with a camera nudge |

## 7. Feel Gate (replaces "looks right" with measurable checks)

Do not proceed to content, combat, or the Jungle blockout until all pass.
Record results in `PROJECT_MEMORY.md`.

| # | Check | Pass condition |
|---|---|---|
| 1 | Diagonal speed | Within 1% of cardinal speed (log it from the debug overlay) |
| 2 | Wall slide | 45° into a wall retains ≥ 70% speed; no sticking or jitter |
| 3 | Corners | 16 px doorway enterable at all 8 approach angles without pixel-perfect aim |
| 4 | Stopping | Stops in ≤ 0.08 s; no visible slide on tiles |
| 5 | Camera | No shimmer or jitter when walking and stopping; no sliding after respawn |
| 6 | Frame time | Stable at 60 and 144 Hz, and after a deliberate 200 ms stall |
| 7 | Input | Keyboard and controller feel the same; stick has no drift at rest |
| 8 | Depth | Walking behind/in front of the pillar and palm is always correct |
| 9 | Ramp | Elevation change is clear and cannot be exploited to leave the room |
| 10 | "Would I wander?" | A new tester walks around the test room for 2 minutes unprompted |

The last check matters most: if nobody wants to wander the empty test room, fix
feel before adding content.

## 8. Cool-factor review (every milestone)

At the end of each phase answer these in the handoff:

1. What is the **one screenshot** of this milestone I would be proud to share?
2. What is the **one moment** a tester smiled or said "oh nice"?
3. What is the **one thing** that still feels stiff, unclear, or noisy?
4. Did any "cool" addition hurt readability or smoothness? If yes, remove it.

Capture a short GIF or screenshot each milestone and keep it out of the repo
(or in a small `docs/media/` folder) so progress is visible and motivating.

## 9. Tuning workflow for limited-context AI

- All tunables in `src/tuning.h` as `#define`s or one `const` struct.
- F3 shows values; number keys nudge a selected value at runtime; the human
  copies good values into `tuning.h`.
- A prompt like "make movement snappier" becomes a *single-number* change plus a
  playtest note, which a small model can do safely.
- Never combine a tuning pass with a refactor in the same session.
