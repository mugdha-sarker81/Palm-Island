#!/usr/bin/env python3
"""Palm Island world map generator.

Renders a 320x180 pixel-art overview map (the game's virtual canvas size)
using the palette direction from docs/PROJECT_MEMORY.md: deep teal + ink,
jade/moss foliage, pale amber, aged stone, sea mist, restrained gold.

Usage: python tools/make_map.py OUT_DIR
"""
import sys, os, math
import numpy as np
from PIL import Image
from scipy import ndimage as ndi

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

W, H = 320, 180
SEED = 81


def hx(s):
    return tuple(int(s[i:i + 2], 16) for i in (1, 3, 5))


# ------------------------------------------------------------------ palette
C = {k: hx(v) for k, v in dict(
    ink="#0b1f26", deep="#0f3042", sea="#17495c", shallow="#2a7886",
    mist="#7fc4c0", foam="#d3efe6",
    sand="#e6cd8c", sand2="#cfae6e", sand3="#b58f55",
    amber="#f0b860", gold="#f5d67c", trail2="#a9743a",
    grass="#58a05a", grass2="#468d52", moss="#78b650",
    jade="#24604a", jade2="#1b4d3c", jade3="#113a2f",
    stone="#8b9096", stone2="#666b73", stone3="#b9bcbd",
    char="#3a3b42", char2="#2a2b31", char3="#4a4a52",
    ember="#e2622a", lava="#f7a03c",
    vio="#6f6685", vio2="#4c4763", vio3="#9c94b0",
    teal="#2f7f78", teal2="#215f5c", teal3="#58b3a4",
    cliff="#a9a99a", cliff2="#85867f", cliff3="#cfcdb9",
    wood="#8a5a3a", wood2="#5e3b26", roof="#c4533a", white="#f4ecd2",
    w_thorn="#5fd06a", w_ember="#f08a3a", w_storm="#4aa8f0",
    w_dawn="#f7e26a", w_void="#a46cf0",
).items()}
WEAPON = [C["w_thorn"], C["w_ember"], C["w_storm"], C["w_dawn"], C["w_void"]]

# ------------------------------------------------------------------ helpers
img = np.zeros((H, W, 3), np.uint8)


def put(x, y, c):
    x = int(x); y = int(y)
    if 0 <= x < W and 0 <= y < H:
        img[y, x] = c


def get(x, y):
    return tuple(int(v) for v in img[int(y), int(x)])


def noise(scale, seed):
    r = np.random.default_rng(seed)
    g = r.random((int(H / scale) + 4, int(W / scale) + 4))
    z = ndi.zoom(g, scale, order=3)[:H, :W]
    return (z - z.min()) / (z.max() - z.min())


def mix(c1, c2, t):
    return tuple(int(round(a * (1 - t) + b * t)) for a, b in zip(c1, c2))


def line_pts(x0, y0, x1, y1):
    pts = []
    dx, dy = abs(x1 - x0), abs(y1 - y0)
    sx, sy = (1 if x0 < x1 else -1), (1 if y0 < y1 else -1)
    err = dx - dy
    while True:
        pts.append((x0, y0))
        if x0 == x1 and y0 == y1:
            break
        e2 = 2 * err
        if e2 > -dy:
            err -= dy; x0 += sx
        if e2 < dx:
            err += dx; y0 += sy
    return pts


def poly_pts(wps):
    out = []
    for a, b in zip(wps, wps[1:]):
        seg = line_pts(a[0], a[1], b[0], b[1])
        out += seg if not out else seg[1:]
    return out


def smooth_wps(wps, n=6):
    """Catmull-Rom through waypoints for organic trails."""
    if len(wps) < 3:
        return wps
    p = [wps[0]] + list(wps) + [wps[-1]]
    out = []
    for i in range(1, len(p) - 2):
        p0, p1, p2, p3 = p[i - 1], p[i], p[i + 1], p[i + 2]
        for k in range(n):
            t = k / n
            t2, t3 = t * t, t * t * t
            x = 0.5 * ((2 * p1[0]) + (-p0[0] + p2[0]) * t +
                       (2 * p0[0] - 5 * p1[0] + 4 * p2[0] - p3[0]) * t2 +
                       (-p0[0] + 3 * p1[0] - 3 * p2[0] + p3[0]) * t3)
            y = 0.5 * ((2 * p1[1]) + (-p0[1] + p2[1]) * t +
                       (2 * p0[1] - 5 * p1[1] + 4 * p2[1] - p3[1]) * t2 +
                       (-p0[1] + 3 * p1[1] - 3 * p2[1] + p3[1]) * t3)
            out.append((int(round(x)), int(round(y))))
    out.append(tuple(wps[-1]))
    return out


def sprite(x, y, pat, cmap, anchor="cb"):
    """Draw ascii sprite. anchor c=center b=bottom / cc=center."""
    h = len(pat); w = max(len(r) for r in pat)
    ox = x - w // 2
    oy = y - (h - 1) if anchor == "cb" else y - h // 2
    for j, row in enumerate(pat):
        for i, ch in enumerate(row):
            if ch != "." and ch in cmap:
                put(ox + i, oy + j, cmap[ch])


# 3x5 pixel font ------------------------------------------------------------
F = {
    "A": [".#.", "#.#", "###", "#.#", "#.#"], "B": ["##.", "#.#", "##.", "#.#", "##."],
    "C": [".##", "#..", "#..", "#..", ".##"], "D": ["##.", "#.#", "#.#", "#.#", "##."],
    "E": ["###", "#..", "##.", "#..", "###"], "F": ["###", "#..", "##.", "#..", "#.."],
    "G": [".##", "#..", "#.#", "#.#", ".##"], "H": ["#.#", "#.#", "###", "#.#", "#.#"],
    "I": ["###", ".#.", ".#.", ".#.", "###"], "J": ["..#", "..#", "..#", "#.#", ".#."],
    "K": ["#.#", "#.#", "##.", "#.#", "#.#"], "L": ["#..", "#..", "#..", "#..", "###"],
    "M": ["#.#", "###", "###", "#.#", "#.#"], "N": ["##.", "#.#", "#.#", "#.#", "#.#"],
    "O": ["###", "#.#", "#.#", "#.#", "###"], "P": ["##.", "#.#", "##.", "#..", "#.."],
    "Q": ["###", "#.#", "#.#", "###", "..#"], "R": ["##.", "#.#", "##.", "#.#", "#.#"],
    "S": [".##", "#..", ".#.", "..#", "##."], "T": ["###", ".#.", ".#.", ".#.", ".#."],
    "U": ["#.#", "#.#", "#.#", "#.#", "###"], "V": ["#.#", "#.#", "#.#", "#.#", ".#."],
    "W": ["#.#", "#.#", "###", "###", "#.#"], "X": ["#.#", "#.#", ".#.", "#.#", "#.#"],
    "Y": ["#.#", "#.#", ".#.", ".#.", ".#."], "Z": ["###", "..#", ".#.", "#..", "###"],
    " ": ["..", "..", "..", "..", ".."], "-": ["...", "...", "###", "...", "..."],
    ".": [".", ".", ".", ".", "#"], "'": ["#", "#", ".", ".", "."],
    "/": ["..#", "..#", ".#.", "#..", "#.."],
}


def text_w(s):
    return sum(len(F[ch][0]) + 1 for ch in s) - 1


def text(x, y, s, fg, outline=None, center=False):
    if center:
        x = x - text_w(s) // 2
    cx = x
    glyphs = []
    for ch in s:
        g = F[ch]
        glyphs.append((cx, g))
        cx += len(g[0]) + 1
    if outline is not None:
        for gx, g in glyphs:
            for j, row in enumerate(g):
                for i, p in enumerate(row):
                    if p == "#":
                        for dx in (-1, 0, 1):
                            for dy in (-1, 0, 1):
                                put(gx + i + dx, y + j + dy, outline)
    for gx, g in glyphs:
        for j, row in enumerate(g):
            for i, p in enumerate(row):
                if p == "#":
                    put(gx + i, y + j, fg)


# ------------------------------------------------------------------ world
yy, xx = np.mgrid[0:H, 0:W].astype(np.float32)

# region anchors: id -> (x, y, sigma)
REG = {
    0: (158, 152, 21),   # Palmshore Beach
    1: (166, 126, 19),   # Driftwood Village
    2: (102, 102, 27),   # Forgotten Jungle
    3: (82, 56, 25),     # Ember Caverns
    4: (240, 98, 26),    # Sunken Temple
    5: (232, 44, 23),    # Skywatch Cliffs
    6: (150, 30, 24),    # Lost Kingdom
    7: (160, 78, 22),    # Central island / Sanctuary
}
BLOBS = [(158, 152, 24, 1.0), (166, 126, 22, 1.0), (102, 102, 30, 1.0),
         (82, 56, 27, 1.0), (240, 98, 29, 1.0), (232, 44, 26, 1.0),
         (150, 30, 26, 1.0), (160, 80, 30, 1.0), (192, 132, 12, 0.7),
         (122, 80, 18, 0.7), (200, 70, 18, 0.7), (150, 56, 16, 0.7)]

fbm = 0.5 * noise(44, SEED) + 0.3 * noise(20, SEED + 1) + 0.2 * noise(9, SEED + 2)
field = np.zeros((H, W), np.float32)
for bx, by, s, w in BLOBS:
    field += w * np.exp(-((xx - bx) ** 2 + (yy - by) ** 2) / (2 * s * s))
field = field + (fbm - 0.5) * 0.55
land = field > 0.60

# carve a bay east of the village so Harbor Row has a real waterfront
bay = ((xx - 214) / 13.0) ** 2 + ((yy - 148) / 15.0) ** 2 < 1
land &= ~bay

land = ndi.binary_fill_holes(land)
lab, n = ndi.label(land)
sizes = ndi.sum(land, lab, range(1, n + 1))
land = lab == (1 + int(np.argmax(sizes)))
# keep a few tiny islets as decoration, drop the rest
land = ndi.binary_opening(land, structure=np.ones((2, 2)))

edt_in = ndi.distance_transform_edt(land)           # distance to ocean (land px)
edt_out, near_idx = ndi.distance_transform_edt(~land, return_indices=True)

# region assignment (jittered voronoi on normalised distance)
jit = noise(14, SEED + 5)
best = np.full((H, W), 1e9, np.float32)
second = np.full((H, W), 1e9, np.float32)
region = np.zeros((H, W), np.int8)
region2 = np.zeros((H, W), np.int8)
for rid, (rx, ry, rs) in REG.items():
    d = np.sqrt((xx - rx) ** 2 + (yy - ry) ** 2) / rs + (jit - 0.5) * 0.45
    m = d < best
    # demote old best to second
    second[m] = best[m]; region2[m] = region[m]
    best[m] = d[m]
    region[m] = rid
    m2 = (~m) & (d < second)
    second[m2] = d[m2]; region2[m2] = rid
# harbor row belongs to the village
harb = ((xx - 190) ** 2 + (yy - 133) ** 2) < 14 ** 2
region[harb & land & (region != 7)] = 1

# elevation (for hillshade)
hgt = 0.6 * noise(16, SEED + 7) + 0.4 * noise(7, SEED + 8)
elev_bias = {0: 0.0, 1: 0.02, 2: 0.08, 3: 0.35, 4: -0.05, 5: 0.45, 6: 0.12, 7: 0.22}
for rid, b in elev_bias.items():
    hgt = hgt + b * (region == rid)
hgt = ndi.gaussian_filter(hgt, 1.2)
gx_, gy_ = np.gradient(hgt)
shade = -(gx_ + gy_) * 9.0   # light from NW

bayer = np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]]) / 16.0
BAY = np.tile(bayer, (H // 4 + 1, W // 4 + 1))[:H, :W]
fine = noise(4, SEED + 11)

# per-region ground palettes: (A, B, accent)
GROUND = {
    0: (C["sand"], C["sand2"], C["amber"]),
    1: (C["grass"], C["moss"], C["sand2"]),
    2: (C["jade"], C["jade2"], C["grass2"]),
    3: (C["char"], C["char2"], C["char3"]),
    4: (C["teal"], C["teal2"], C["teal3"]),
    5: (C["cliff"], C["cliff2"], C["cliff3"]),
    6: (C["vio"], C["vio2"], C["vio3"]),
    7: (C["cliff3"], C["stone3"], C["gold"]),
}

# ---- ocean
dist = edt_out
for y in range(H):
    for x in range(W):
        if land[y, x]:
            continue
        d = dist[y, x]
        t = fine[y, x]
        b = BAY[y, x]
        if d <= 1.5:
            col = C["foam"] if (t + b * 0.3) > 0.55 else C["mist"]
        elif d <= 3.2:
            col = C["mist"] if b < 0.35 else C["shallow"]
        elif d <= 6:
            col = C["shallow"] if (d - 3.2) / 2.8 < b else C["sea"]
        elif d <= 12:
            col = C["sea"] if (d - 6) / 6.0 < b * 0.9 + 0.1 else C["deep"]
        else:
            col = C["deep"] if b < 0.8 else C["sea"]
        img[y, x] = col

# ocean wave ticks
rng = np.random.default_rng(SEED + 20)
for _ in range(260):
    x, y = int(rng.integers(2, W - 6)), int(rng.integers(2, H - 2))
    if dist[y, x] > 9 and not land[y, x]:
        for i in range(3):
            put(x + i, y, C["sea"] if i != 1 else C["shallow"])

# ---- land ground
for y in range(H):
    for x in range(W):
        if not land[y, x]:
            continue
        r = int(region[y, x])
        gap = second[y, x] - best[y, x]
        if gap < 0.22 and BAY[y, x] < (0.22 - gap) / 0.22 * 0.55:
            r = int(region2[y, x])
        A, B, acc = GROUND[r]
        t = fine[y, x] * 0.7 + hgt[y, x] * 0.3
        col = B if t + (BAY[y, x] - 0.5) * 0.5 > 0.52 else A
        if r == 0 and edt_in[y, x] > 14:
            col = C["grass2"] if BAY[y, x] < (edt_in[y, x] - 14) / 6 else col
        if r == 1 and fine[y, x] > 0.8 and BAY[y, x] > 0.5:
            col = acc
        # hillshade quantised -> flat pixel steps
        s = shade[y, x]
        if s > 0.10:
            col = mix(col, (255, 245, 220), 0.16)
        elif s < -0.10:
            col = mix(col, C["ink"], 0.22)
        img[y, x] = col

# shoreline / cliff rim
for y in range(H):
    for x in range(W):
        if not land[y, x]:
            continue
        r = int(region[y, x]); e = edt_in[y, x]
        if r in (3, 5, 6):          # rocky coasts: dark cliff face
            if e <= 1.5:
                img[y, x] = C["stone2"] if r != 6 else C["vio2"]
            elif e <= 2.5 and BAY[y, x] > 0.5:
                img[y, x] = C["stone3"] if r == 5 else img[y, x]
        else:                        # sandy rim
            if e <= 2.2:
                img[y, x] = C["sand"] if (BAY[y, x] + 0.2) > 0.25 else C["sand2"]
            elif e <= 3.4 and BAY[y, x] < 0.45:
                img[y, x] = C["sand2"]

# ---- Sunken Temple lagoon (water carved into the land)
lag_c = (250, 106)
lag = (((xx - lag_c[0]) / 16.0) ** 2 + ((yy - lag_c[1]) / 11.0) ** 2) < 1
lag &= land
lag_in = ndi.distance_transform_edt(lag)
for y in range(H):
    for x in range(W):
        if lag[y, x]:
            d = lag_in[y, x]
            col = C["shallow"] if d > 2.2 else C["mist"]
            if d > 5:
                col = C["sea"] if BAY[y, x] < 0.6 else C["shallow"]
            img[y, x] = col
        elif land[y, x]:
            # ring of wet stone around lagoon
            dd = math.hypot((x - lag_c[0]) / 16.0, (y - lag_c[1]) / 11.0)
            if 1.0 <= dd < 1.16 and region[y, x] == 4:
                img[y, x] = C["teal3"] if BAY[y, x] > 0.4 else C["teal2"]
protect_water = lag.copy()

# ---- Ember Caverns: lava cracks + cooled stone
rng = np.random.default_rng(SEED + 31)
for _ in range(14):
    for _try in range(30):
        x, y = int(rng.integers(52, 112)), int(rng.integers(28, 86))
        if land[y, x] and region[y, x] == 3 and edt_in[y, x] > 4:
            break
    else:
        continue
    for step in range(int(rng.integers(6, 14))):
        if land[y, x] and region[y, x] == 3:
            put(x, y, C["ember"] if step % 4 else C["lava"])
        x += int(rng.integers(-1, 2)); y += int(rng.integers(0, 2)) * int(rng.choice([-1, 1]))
        x = int(np.clip(x, 0, W - 1)); y = int(np.clip(y, 0, H - 1))

# ---- stone clutter on rocky regions
for _ in range(220):
    x, y = int(rng.integers(0, W)), int(rng.integers(0, H))
    if land[y, x] and region[y, x] in (3, 5, 6, 7) and edt_in[y, x] > 3:
        base = {3: C["char3"], 5: C["cliff3"], 6: C["vio3"], 7: C["stone2"]}[int(region[y, x])]
        put(x, y, base); put(x + 1, y, base); put(x, y + 1, mix(base, C["ink"], 0.4))

# ------------------------------------------------------------------ routes
ZONES = {
    # region 1 Palmshore
    "wreck": (156, 164), "tidepool": (158, 149),
    # region 2 Driftwood
    "square": (162, 129), "harbor": (190, 134),
    # region 3 Jungle
    "j_entr": (131, 115), "j_root": (117, 107), "j_clear": (105, 97), "j_shrine": (93, 88),
    # region 4 Ember
    "e_ash": (112, 71), "e_forge": (93, 61), "e_buried": (76, 52),
    # region 5 Temple
    "t_court": (214, 95), "t_hall": (232, 84), "t_engine": (254, 88),
    # region 6 Cliffs
    "c_steps": (205, 66), "c_mirror": (221, 54), "c_nest": (240, 40),
    # region 7 Kingdom
    "k_archive": (153, 58), "k_court": (146, 46), "k_gallery": (158, 36), "k_hollow": (146, 24),
}
HUB = (160, 101)
SANCT = (160, 76)

TRAILS = []   # (waypoints, style)  style: solid|dashed
TRAILS.append(([ZONES["wreck"], (157, 157), ZONES["tidepool"], (160, 138), ZONES["square"]], "solid"))
TRAILS.append(([ZONES["square"], (176, 130), ZONES["harbor"]], "solid"))
TRAILS.append(([ZONES["square"], (148, 124), (138, 119), ZONES["j_entr"], ZONES["j_root"],
                ZONES["j_clear"], ZONES["j_shrine"]], "solid"))
# locked central-island route
TRAILS.append(([ZONES["harbor"], (186, 120), (176, 110), HUB], "dashed"))
# hub -> four weapon regions
TRAILS.append(([HUB, (146, 94), (132, 83), (120, 75), ZONES["e_ash"], ZONES["e_forge"], ZONES["e_buried"]], "solid"))
TRAILS.append(([HUB, (186, 100), (200, 99), ZONES["t_court"], ZONES["t_hall"], ZONES["t_engine"]], "solid"))
TRAILS.append(([(188, 100), (194, 86), (200, 74), ZONES["c_steps"], ZONES["c_mirror"], ZONES["c_nest"]], "solid"))
TRAILS.append(([(146, 94), (141, 80), (146, 66), ZONES["k_archive"], ZONES["k_court"],
                ZONES["k_gallery"], ZONES["k_hollow"]], "dashed"))
# sanctuary approach from the hub
TRAILS.append(([HUB, (160, 92), (160, 86)], "solid"))
# jungle shortcut (opens after Thornfang) – loops back to the entrance
SHORTCUT = [ZONES["j_shrine"], (90, 100), (96, 112), (112, 119), ZONES["j_entr"]]

trail_mask = np.zeros((H, W), bool)
trail_px = []
for wps, style in TRAILS:
    pts = poly_pts(smooth_wps(wps, 7))
    trail_px.append((pts, style))
    for (x, y) in pts:
        if 0 <= x < W and 0 <= y < H:
            trail_mask[y, x] = True
sc_pts = poly_pts(smooth_wps(SHORTCUT, 7))
for (x, y) in sc_pts:
    if 0 <= x < W and 0 <= y < H:
        trail_mask[y, x] = True
keepout = ndi.binary_dilation(trail_mask, iterations=3)

# ------------------------------------------------------------------ trees / foliage
def poisson(mask, min_d, tries, rng, extra_ok=None):
    pts = []
    cand = np.argwhere(mask)
    if len(cand) == 0:
        return pts
    for _ in range(tries):
        y, x = cand[int(rng.integers(0, len(cand)))]
        if extra_ok and not extra_ok(x, y):
            continue
        if all((x - px) ** 2 + (y - py) ** 2 >= min_d ** 2 for px, py in pts):
            pts.append((int(x), int(y)))
    return pts


rng = np.random.default_rng(SEED + 40)
LM_KEEP = np.zeros((H, W), bool)       # landmark keep-out
for k, (zx, zy) in ZONES.items():
    LM_KEEP |= ((xx - zx) ** 2 + (yy - zy) ** 2) < 7 ** 2
LM_KEEP |= ((xx - SANCT[0]) ** 2 + (yy - SANCT[1]) ** 2) < 15 ** 2
LM_KEEP |= ((xx - HUB[0]) ** 2 + (yy - HUB[1]) ** 2) < 7 ** 2
LM_KEEP |= ((xx - 148) ** 2 + (yy - 140) ** 2) < 20 ** 2 & False if False else False

ok_ground = land & ~keepout & ~LM_KEEP & ~protect_water & (edt_in > 3)

tree_jade = {"d": C["jade2"], "l": C["grass2"], "k": C["jade3"], "L": C["moss"]}
TREE_J = [".LLl.", "lddkd", "dddkk", ".dkk.", "..k.."]
TREE_G = [".ll.", "lddk", "ddkk", ".kk."]
tree_g = {"d": C["grass2"], "l": C["moss"], "k": C["jade"]}
PALM = ["gg.g.gg", ".ggggg.", "..gbg..", "...b...", "...b..."]
palm_c = {"g": C["moss"], "b": C["wood"]}

jungle_pts = poisson(ok_ground & (region == 2), 4.4, 4000, rng)
village_pts = poisson(ok_ground & (region == 1), 9.0, 800, rng)
center_pts = poisson(ok_ground & (region == 7) & (np.sqrt((xx - 160) ** 2 + (yy - 78) ** 2) > 17), 10.0, 600, rng)
temple_pts = poisson(ok_ground & (region == 4), 12.0, 600, rng)
beach_palm = poisson(land & (region == 0) & ~keepout & ~LM_KEEP & (edt_in > 2) & (edt_in < 10), 9.0, 900, rng)
village_palm = poisson(land & (region == 1) & ~keepout & ~LM_KEEP & (edt_in > 2) & (edt_in < 6), 10.0, 600, rng)

draw_list = []
for p in jungle_pts:
    draw_list.append((p[1], "tj", p))
for p in village_pts + center_pts + temple_pts:
    draw_list.append((p[1], "tg", p))
for p in beach_palm + village_palm:
    draw_list.append((p[1], "palm", p))
draw_list.sort()
for _, kind, (x, y) in draw_list:
    if kind == "tj":
        sprite(x, y, TREE_J, tree_jade)
    elif kind == "tg":
        sprite(x, y, TREE_G, tree_g)
    else:
        sprite(x, y, PALM, palm_c)

# ------------------------------------------------------------------ trails
def draw_trail(pts, style):
    for i, (x, y) in enumerate(pts):
        if not (0 <= x < W and 0 <= y < H):
            continue
        if protect_water[y, x]:
            continue
        if style == "dashed" and (i // 3) % 2 == 1:
            continue
        put(x, y + 1, C["trail2"])
    for i, (x, y) in enumerate(pts):
        if not (0 <= x < W and 0 <= y < H):
            continue
        if protect_water[y, x]:
            continue
        if style == "dashed" and (i // 3) % 2 == 1:
            continue
        put(x, y, C["amber"] if style == "solid" else C["gold"])


for pts, style in trail_px:
    draw_trail(pts, style)

# jungle shortcut: thin jade-bright dotted line
for i, (x, y) in enumerate(sc_pts):
    if i % 3 != 2:
        put(x, y, C["w_thorn"])

# ------------------------------------------------------------------ landmarks
def disc(cx, cy, r, col, only=None):
    for j in range(-r, r + 1):
        for i in range(-r, r + 1):
            if i * i + j * j <= r * r + 1:
                put(cx + i, cy + j, col)


# -- Wreckshore: beached ship + standing stone
SHIP = ["..bb.......", ".bBBBbbb...", "bBBBBBBBbb.", ".bbbbbbbb.."]
sprite(150, 167, SHIP, {"b": C["wood"], "B": C["wood2"]})
put(149, 160, C["wood2"]); put(149, 161, C["wood2"]); put(149, 162, C["wood2"]); put(150, 161, C["white"])
sprite(164, 166, ["..s", ".ss", ".sS", ".sS"], {"s": C["stone3"], "S": C["stone2"]})
put(164, 162, C["gold"])   # family mark glint

# -- tide pools
for tx, ty in ((148, 150), (169, 152), (172, 160)):
    disc(tx, ty, 1, C["shallow"]); put(tx, ty, C["mist"])

# -- Driftwood Village houses + keeper's lantern
HOUSE = [".rrrrr.", "rrrrrrr", "wwwwwww", "wwdwwww", "wwdwwww"]
HOUSE2 = ["..rrr..", ".rrrrr.", "rrrrrrr", "wwwwwww", "wdwwwww"]
hc = {"r": C["roof"], "w": C["wood"], "d": C["wood2"]}
for hx_, hy_, pat in ((150, 126, HOUSE), (172, 122, HOUSE2), (176, 132, HOUSE), (150, 136, HOUSE2), (165, 118, HOUSE)):
    sprite(hx_, hy_, pat, hc)
# well / square
disc(162, 129, 2, C["stone2"]); put(162, 129, C["shallow"])
put(163, 125, C["gold"]); put(163, 126, C["wood2"])      # lantern post

# -- Harbor Row: pier toward nearest ocean pixel + two boats
hx0, hy0 = ZONES["harbor"]
ny, nx = near_idx[0][hy0, hx0], near_idx[1][hy0, hx0]
vx, vy = nx - hx0, ny - hy0
L = max(1, math.hypot(vx, vy))
ex, ey = int(round(nx + vx / L * 6)), int(round(ny + vy / L * 6))
for (x, y) in line_pts(hx0, hy0, ex, ey):
    put(x, y, C["wood"]); put(x + 1, y, C["wood2"]) if abs(vy) > abs(vx) else put(x, y + 1, C["wood2"])
for bx_, by_ in ((ex + 3, ey - 1), (ex - 4, ey + 3)):
    sprite(bx_, by_, [".w.", "bbb"], {"w": C["white"], "b": C["wood"]})

# -- Forgotten Jungle: Root Path thorn gate + shrine
tg = ZONES["j_root"]
for dx_, dy_ in ((-3, -3), (-2, -2), (-1, -1), (0, 0), (1, 1), (2, 2), (3, 3), (3, -3), (2, -2), (1, -1), (-1, 1), (-2, 2), (-3, 3)):
    put(tg[0] + dx_, tg[1] + dy_, C["w_thorn"] if (dx_ + dy_) % 2 == 0 else C["jade3"])
for dx_, dy_ in ((-3, -4), (3, -4), (-4, 0), (4, 0), (0, -4), (0, 4)):
    put(tg[0] + dx_, tg[1] + dy_, C["jade3"])
sx_, sy_ = ZONES["j_shrine"]
SHRINE = [".sss.", "sSssS", "ssGss", "sS.Ss", "ss.ss"]
sprite(sx_, sy_ + 3, SHRINE, {"s": C["stone"], "S": C["stone2"], "G": C["w_thorn"]})
for a in range(0, 360, 60):
    put(sx_ + int(round(5 * math.cos(math.radians(a)))), sy_ + int(round(5 * math.sin(math.radians(a)))) , C["w_thorn"])

# -- Ember Caverns: volcano + cavern mouth
vx_, vy_ = 70, 44
for j in range(0, 12):
    half = int(2 + j * 0.9)
    for i in range(-half, half + 1):
        col = C["char2"] if i > 0 else C["char3"]
        if j < 2:
            col = C["lava"] if abs(i) <= 1 else C["ember"]
        if j >= 2 and abs(i) == half:
            col = C["ink"]
        put(vx_ + i, vy_ + j, col)
for k, (dx_, dy_) in enumerate(((0, -1), (-1, -2), (1, -3), (0, -4))):
    put(vx_ + dx_, vy_ + dy_, mix(C["char3"], C["stone3"], 0.35 + 0.15 * k))   # smoke
ex_, ey_ = ZONES["e_forge"]
sprite(ex_ + 4, ey_ + 4, [".ccc.", "cdddc", "cdddc"], {"c": C["stone2"], "d": C["ink"]})
bf = ZONES["e_buried"]
disc(bf[0], bf[1], 3, C["ember"]); disc(bf[0], bf[1], 1, C["lava"])

# -- Sunken Temple: columns, arch, steps
def column(x, y, h=5):
    for j in range(h):
        put(x, y - j, C["teal3"]); put(x + 1, y - j, C["teal2"])
    put(x - 1, y - h, C["teal3"]); put(x, y - h, C["white"]); put(x + 1, y - h, C["teal3"]); put(x + 2, y - h, C["teal2"])


for cx_, cy_ in ((232, 98), (240, 92), (260, 94), (268, 102), (250, 123), (238, 120)):
    column(cx_, cy_, 4)
for j in range(3):                                   # sluice gate
    put(250, 97 + j, C["stone2"]); put(251, 97 + j, C["stone"])
put(251, 96, C["w_storm"]); put(250, 96, C["w_storm"])
# crystal nodes (Stormpiercer)
for nx_, ny_ in ((232, 110), (250, 128), (266, 112)):
    sprite(nx_, ny_, [".b.", "bBb", ".b."], {"b": C["w_storm"], "B": C["white"]}, "cc")

# -- Skywatch Cliffs: signal tower + rope bridge + wind marks
tx_, ty_ = ZONES["c_nest"]
for j in range(9):
    put(tx_, ty_ + 3 - j, C["stone2"]); put(tx_ + 1, ty_ + 3 - j, C["stone3"])
put(tx_ - 1, ty_ - 6, C["stone2"]); put(tx_ + 2, ty_ - 6, C["stone2"])
put(tx_, ty_ - 7, C["w_dawn"]); put(tx_ + 1, ty_ - 7, C["white"])
for ang in (-0.8, 0, 0.8):                          # beacon rays
    put(tx_ + int(round(3 * math.cos(ang - 1.57 + 0.0))) + 1, ty_ - 7 + int(round(3 * math.sin(ang - 1.57))), C["gold"])
br0, br1 = (212, 61), (228, 50)
for i, (x, y) in enumerate(line_pts(*br0, *br1)):
    put(x, y + (1 if i % 2 else 0), C["wood"])
    put(x, y + 2, C["wood2"]) if i % 3 == 0 else None
for gx2, gy2 in ((196, 62), (214, 40), (246, 58), (252, 36)):  # wind streaks
    for i in range(5):
        put(gx2 + i, gy2 + (1 if i in (2, 3) else 0), C["white"])
mir = ZONES["c_mirror"]
sprite(mir[0] + 5, mir[1] + 4, [".w.", "wWw", ".w."], {"w": C["w_dawn"], "W": C["white"]}, "cc")

# -- Lost Kingdom: broken walls, towers, statue bases
def ruin_wall(x, y, w, h, col=None, dark=None):
    col = col or C["vio3"]; dark = dark or C["vio2"]
    for j in range(h):
        for i in range(w):
            if (i * 7 + j * 3) % 5 == 0 and j == 0:
                continue
            put(x + i, y + j, col if j == 0 else dark)


ruin_wall(132, 40, 9, 3); ruin_wall(132, 31, 3, 9); ruin_wall(166, 44, 8, 3)
ruin_wall(168, 28, 3, 16); ruin_wall(136, 20, 10, 2); ruin_wall(150, 62, 3, 6)
for tx2, ty2 in ((130, 52), (172, 56), (140, 14), (164, 20)):
    for j in range(6):
        put(tx2, ty2 - j, C["vio3"]); put(tx2 + 1, ty2 - j, C["vio2"]); put(tx2 + 2, ty2 - j, C["vio2"])
    put(tx2, ty2 - 6, C["vio3"]); put(tx2 + 2, ty2 - 6, C["vio3"])
for sx3, sy3 in ((150, 38), (156, 50), (140, 30), (162, 40)):   # empty statue bases
    put(sx3, sy3, C["vio3"]); put(sx3 + 1, sy3, C["vio3"]); put(sx3, sy3 + 1, C["vio2"]); put(sx3 + 1, sy3 + 1, C["vio2"])
# shadow causeway: dark stripes
for i in range(8):
    put(158 + i, 36 + i // 2, C["ink"]) if (i % 2 == 0) else None
sprite(ZONES["k_hollow"][0], ZONES["k_hollow"][1] + 2, ["..v..", ".vVv.", "vVVVv", "vVvVv"], {"v": C["w_void"], "V": C["vio2"]})

# -- Central island: hub signpost and the Excalibur Sanctuary plateau
sx0, sy0 = SANCT
for r_, col in ((11, C["stone2"]), (10, C["cliff3"]), (8, C["gold"]), (7, C["cliff3"]), (4, C["white"])):
    disc(sx0, sy0, r_, col)
for a in range(0, 360, 12):                           # ceremonial wall dots
    put(sx0 + int(round(12 * math.cos(math.radians(a)))), sy0 + int(round(12 * math.sin(math.radians(a)))), C["ink"])
disc(sx0, sy0, 2, C["gold"]); put(sx0, sy0, C["white"])
for k, col in enumerate(WEAPON):                       # five weapon sockets
    a = math.radians(-90 + 72 * k)
    px_, py_ = sx0 + int(round(9 * math.cos(a))), sy0 + int(round(9 * math.sin(a)))
    disc(px_, py_, 1, C["ink"]); put(px_, py_, col)
# hub crossroads
disc(HUB[0], HUB[1], 3, C["stone2"]); disc(HUB[0], HUB[1], 2, C["stone3"]); put(HUB[0], HUB[1], C["gold"])


def gate(x, y, col):
    for dx_ in (-2, -1, 0, 1, 2):
        put(x + dx_, y - 3, C["ink"])
    for j in range(-2, 3):
        put(x - 2, y + j, C["ink"]); put(x + 2, y + j, C["ink"])
    put(x - 1, y - 2, col); put(x, y - 2, col); put(x + 1, y - 2, col)
    put(x - 1, y - 1, col); put(x + 1, y - 1, col)
    put(x - 1, y, col); put(x + 1, y, col)


def padlock(x, y):
    put(x - 1, y - 3, C["ink"]); put(x, y - 3, C["ink"]); put(x + 1, y - 3, C["ink"])
    put(x - 1, y - 2, C["ink"]); put(x + 1, y - 2, C["ink"])
    for j in range(0, 3):
        for i in range(-2, 3):
            put(x + i, y + j - 1, C["gold"] if (i, j) != (0, 0) else C["ink"])


# ability / multi-weapon gates on the routes
gate(125, 77, C["w_ember"])            # Embercleaver (ashfall approach)
gate(203, 99, C["w_storm"])            # Stormpiercer (temple approach)
gate(197, 78, C["w_dawn"])             # Dawnstring (cliffs approach)
gate(143, 74, C["w_void"])             # Voidfang (kingdom approach)
padlock(180, 114)                      # locked Harbor Row -> central island

# jungle gate icon above the thorn wall
gate(tg[0] + 1, tg[1] - 6, C["w_thorn"])

# ------------------------------------------------------------------ zone nodes
for k, (zx, zy) in ZONES.items():
    for j in (-1, 0, 1):
        for i in (-1, 0, 1):
            put(zx + i, zy + j, C["ink"])
    put(zx, zy, C["gold"])

# decorative: sail boats and a couple of islets
sprite(40, 120, [".w.", "ww.", "bbb"], {"w": C["white"], "b": C["wood"]})
sprite(285, 70, [".w.", ".ww", "bbb"], {"w": C["white"], "b": C["wood"]})
for ix, iy in ((38, 28), (292, 128), (210, 170)):
    if not land[iy, ix]:
        disc(ix, iy, 2, C["sand"]); put(ix, iy, C["sand2"]); put(ix + 1, iy - 3, C["moss"]); put(ix + 1, iy - 2, C["wood"])
        for dx_ in (-3, -2, 2, 3):
            put(ix + dx_, iy + 3, C["foam"])

# ------------------------------------------------------------------ save: no labels
base = img.copy()
Image.fromarray(base).save(os.path.join(OUT, "palm_island_map_nolabels_320x180.png"))

# ------------------------------------------------------------------ labels / UI
INK, WHITE, GOLD = C["ink"], C["white"], C["gold"]


def label(x, y, s, col=WHITE):
    text(x, y, s, col, INK, center=True)


label(158, 171, "PALMSHORE")
label(168, 140, "DRIFTWOOD")
label(104, 124, "FORGOTTEN")
label(104, 130, "JUNGLE")
label(62, 70, "EMBER")
label(62, 76, "CAVERNS")
label(247, 126, "SUNKEN TEMPLE")
label(242, 24, "SKYWATCH")
label(242, 30, "CLIFFS")
label(152, 8, "LOST KINGDOM")
label(160, 60, "SANCTUARY") if False else None
label(SANCT[0], SANCT[1] + 15, "SANCTUARY", GOLD)
label(HUB[0] + 18, HUB[1] + 4, "CENTRAL", WHITE)
label(HUB[0] + 18, HUB[1] + 10, "ISLAND", WHITE)

# Title
text(6, 5, "PALM ISLAND", GOLD, INK)
text(6, 12, "THE FORGOTTEN COAST", WHITE, INK)

# Legend panel (bottom-left)
lx, ly, lw, lh = 4, 140, 64, 37
for j in range(lh):
    for i in range(lw):
        edge = i in (0, lw - 1) or j in (0, lh - 1)
        put(lx + i, ly + j, C["gold"] if edge else C["ink"])
for j in range(1, lh - 1):
    for i in range(1, lw - 1):
        if (i + j) % 2 == 0 or True:
            put(lx + i, ly + j, mix(C["ink"], C["deep"], 0.6))
yy0 = ly + 5
# zone
for j in (-1, 0, 1):
    for i in (-1, 0, 1):
        put(lx + 6 + i, yy0 + j, C["ink"] if (i or j) else C["gold"])
        if (i or j):
            put(lx + 6 + i, yy0 + j, C["white"])
put(lx + 6, yy0, C["gold"])
text(lx + 12, yy0 - 2, "ZONE", WHITE)
yy0 += 8
for i in range(10):
    put(lx + 3 + i, yy0, C["amber"]); put(lx + 3 + i, yy0 + 1, C["trail2"])
text(lx + 16, yy0 - 2, "TRAIL", WHITE)
yy0 += 8
for i in range(10):
    if (i // 3) % 2 == 0:
        put(lx + 3 + i, yy0, C["gold"]); put(lx + 3 + i, yy0 + 1, C["trail2"])
text(lx + 16, yy0 - 2, "LOCKED", WHITE)
yy0 += 8
for k, col in enumerate(WEAPON):
    disc(lx + 5 + k * 4, yy0 + 1, 1, col)
text(lx + 27, yy0 - 2, "GATES", WHITE) if False else None
text(lx + 28, yy0 - 2, "GATE", WHITE)

# Compass (bottom-right)
cx0, cy0 = 296, 160
for j in range(-9, 10):
    put(cx0, cy0 + j, C["ink"])
for i in range(-9, 10):
    put(cx0 + i, cy0, C["ink"])
for j in range(-8, 9):
    put(cx0, cy0 + j, C["white"] if j < 0 else C["stone2"])
for i in range(-8, 9):
    put(cx0 + i, cy0, C["white"] if i % 2 == 0 else C["stone2"])
disc(cx0, cy0, 2, C["gold"]); put(cx0, cy0, C["ink"])
text(cx0, cy0 - 17, "N", WHITE, INK, center=True)

# Frame
for i in range(W):
    put(i, 0, C["ink"]); put(i, H - 1, C["ink"])
for j in range(H):
    put(0, j, C["ink"]); put(W - 1, j, C["ink"])

out = Image.fromarray(img)
out.save(os.path.join(OUT, "palm_island_map_320x180.png"))
out.resize((W * 4, H * 4), Image.NEAREST).save(os.path.join(OUT, "palm_island_map_1280x720.png"))
Image.fromarray(base).resize((W * 4, H * 4), Image.NEAREST).save(os.path.join(OUT, "palm_island_map_nolabels_1280x720.png"))
print("ok", land.sum(), "land px")
