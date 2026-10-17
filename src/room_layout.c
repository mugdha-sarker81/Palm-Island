#include "room_layout.h"

static bool EntrancePath(int x, int y)
{
    bool northTrail = x >= 18 && x <= 21 && y >= 3 && y <= 10;
    bool southTrail = x >= 18 && x <= 21 && y >= 15;
    bool loopFloor = y >= 15 && y <= 16 && x >= 14 && x <= 25;
    bool westTrail = x >= 10 && x <= 14 && y >= 9 && y <= 15;
    bool eastTrail = x >= 25 && x <= 30 && y >= 9 && y <= 15;
    bool northLoop = y >= 8 && y <= 9 && x >= 14 && x <= 25;
    return northTrail || southTrail || loopFloor || westTrail || eastTrail ||
           northLoop;
}

static char EntranceTileAt(int x, int y)
{
    if (x >= 18 && x <= 21 && y <= 2) return 'S';
    if (x >= 15 && x <= 24 && y >= 11 && y <= 14) return 'N';
    if (x >= ROOM_LAYOUT_ENTRANCE_RAMP_TILE_X &&
        x < ROOM_LAYOUT_ENTRANCE_RAMP_TILE_X + 2 &&
        y == ROOM_LAYOUT_ENTRANCE_RAMP_TILE_Y - 1) return 'S';
    if (x >= ROOM_LAYOUT_ENTRANCE_RAMP_TILE_X &&
        x < ROOM_LAYOUT_ENTRANCE_RAMP_TILE_X + 2 &&
        y == ROOM_LAYOUT_ENTRANCE_RAMP_TILE_Y) return 'R';
    if (EntrancePath(x, y)) return 'D';
    return ((x * 7 + y * 13) % 5 == 0) ? 'G' : 'J';
}

static bool RootPathFloor(int x, int y)
{
    bool southTrail = x >= 18 && x <= 21 && y >= 11;
    bool southJunction = y >= 10 && y <= 12 && x >= 12 && x <= 27;
    bool sideWest = x >= 12 && x <= 14 && y >= 4 && y <= 11;
    bool sideEast = x >= 25 && x <= 27 && y >= 4 && y <= 11;
    bool upperJunction = y >= 3 && y <= 4 && x >= 12 && x <= 27;
    bool northTrail = x >= 18 && x <= 21 && y <= 3;
    bool rampWest = x >= 8 && x <= 9 && y >= 12 && y <= 13;
    return southTrail || southJunction || sideWest || sideEast ||
           upperJunction || northTrail || rampWest;
}

static char RootPathTileAt(int x, int y, bool thornGateOpen)
{
    if (x >= 17 && x <= 22 && y >= 6 && y <= 7) {
        return thornGateOpen ? 'D' : 'T';
    }
    if (thornGateOpen && x >= 17 && x <= 22 && y >= 5 && y <= 10) {
        return 'D';
    }
    if (x >= 15 && x <= 24 && y >= 14 && y <= 17) return 'N';
    if (x >= 8 && x <= 9 && y == 10) return 'S';
    if (x >= 8 && x <= 9 && y == 11) return 'R';
    if (RootPathFloor(x, y)) return 'D';
    if (x >= 15 && x <= 24 && y >= 5 && y <= 10) return 'N';
    return ((x * 11 + y * 5) % 4 == 0) ? 'G' : 'J';
}

static bool ClearingPath(int x, int y)
{
    bool southTrail = x >= 18 && x <= 21 && y >= 18;
    bool lowerJunction = y >= 17 && y <= 20 && x >= 8 && x <= 31;
    bool westTrail = x >= 8 && x <= 10 && y >= 5 && y <= 17;
    bool eastTrail = x >= 29 && x <= 31 && y >= 5 && y <= 17;
    bool upperJunction = y >= 4 && y <= 5 && x >= 8 && x <= 31;
    bool northTrail = x >= 18 && x <= 21 && y <= 4;
    bool exposedTrail = x >= 18 && x <= 21 && y >= 5 && y <= 17;
    bool nookTrail = x >= 4 && x <= 8 && y >= 12 && y <= 14;
    return southTrail || lowerJunction || westTrail || eastTrail ||
           upperJunction || northTrail || exposedTrail || nookTrail;
}

static char ClearingTileAt(int x, int y)
{
    if ((x >= 13 && x <= 16 && y >= 8 && y <= 14) ||
        (x >= 23 && x <= 26 && y >= 8 && y <= 14)) return 'N';
    if (ClearingPath(x, y)) {
        bool outerPath = (x >= 8 && x <= 10) || (x >= 29 && x <= 31) ||
                         (y >= 17 && y <= 20 && x >= 8 && x <= 31);
        return outerPath ? 'G' : 'D';
    }
    if (x >= 2 && x <= 3 && y >= 11 && y <= 15) return 'N';
    return ((x * 7 + y * 13) % 5 == 0) ? 'G' : 'J';
}

static bool ShrineApproachPath(int x, int y)
{
    bool southTrail = x >= 18 && x <= 21 && y >= 18;
    bool lowerPlaza = y >= 16 && y <= 19 && x >= 10 && x <= 29;
    bool westWalk = x >= 10 && x <= 12 && y >= 7 && y <= 16;
    bool eastWalk = x >= 27 && x <= 29 && y >= 7 && y <= 16;
    bool upperPlaza = y >= 6 && y <= 9 && x >= 10 && x <= 29;
    bool shrineWalk = x >= 18 && x <= 21 && y >= 3 && y <= 6;
    bool muralCourt = x >= 16 && x <= 23 && y >= 10 && y <= 15;
    return southTrail || lowerPlaza || westWalk || eastWalk ||
           upperPlaza || shrineWalk || muralCourt;
}

static char ShrineApproachTileAt(int x, int y)
{
    if (y <= 2) return x >= 18 && x <= 21 ? 'D' : 'S';
    if ((x >= 14 && x <= 16 && y >= 11 && y <= 14) ||
        (x >= 23 && x <= 25 && y >= 11 && y <= 14)) return 'N';
    if (ShrineApproachPath(x, y)) {
        bool courtyard = y >= 6 && y <= 19;
        return courtyard ? 'D' : 'G';
    }
    return ((x * 5 + y * 11) % 4 == 0) ? 'G' : 'J';
}

static bool ThornbackArenaPath(int x, int y)
{
    bool westPerimeter = x >= 4 && x <= 8 && y >= 3 && y <= 19;
    bool eastPerimeter = x >= 31 && x <= 35 && y >= 3 && y <= 19;
    bool southPerimeter = y >= 16 && y <= 19 && x >= 4 && x <= 35;
    bool northPerimeter = y >= 3 && y <= 6 && x >= 4 && x <= 35;
    bool centerBowl = x >= 12 && x <= 27 && y >= 7 && y <= 15;
    bool entrance = x >= 18 && x <= 21 && y >= 19;
    return westPerimeter || eastPerimeter || southPerimeter ||
           northPerimeter || centerBowl || entrance;
}

static char ThornbackArenaTileAt(int x, int y)
{
    if (ThornbackArenaPath(x, y)) {
        return (x >= 12 && x <= 27 && y >= 7 && y <= 15) ? 'D' : 'G';
    }
    return ((x * 11 + y * 7) % 4 == 0) ? 'J' : 'G';
}

static char AshfallDescentTileAt(int x, int y)
{
    bool centralTrail = x >= 18 && x <= 21;
    bool junction = (y >= 17 && y <= 18) || (y >= 4 && y <= 5);
    bool eastTrail = x >= 28 && x <= 31 && y >= 5 && y <= 17;
    bool westLowerTrail = x >= 8 && x <= 11 && y >= 12 && y <= 17;
    bool westUpperTrail = x >= 8 && x <= 11 && y >= 5 && y <= 10;
    bool westRampLanding = x >= 8 && x <= 9 && y == 11;
    bool westRamp = x >= 8 && x <= 9 && y == 12;

    if (x <= 1 || x >= ROOM_COLS - 2) return 'S';
    if (y <= 2 || y >= 21) return centralTrail ? 'D' : 'S';
    if (westRampLanding) return 'S';
    if (x >= 10 && x <= 11 && y == 11) return 'S';
    if (westRamp) return 'R';
    if (centralTrail || junction || eastTrail || westLowerTrail ||
        westUpperTrail) return 'D';
    return ((x * 11 + y * 7) % 4 == 0) ? 'G' : 'J';
}

char Room_LayoutTileAt(RoomArea area, int x, int y, bool thornGateOpen)
{
    switch (area) {
        case ROOM_AREA_JUNGLE_ENTRANCE:
            return EntranceTileAt(x, y);
        case ROOM_AREA_ROOT_PATH:
            return RootPathTileAt(x, y, thornGateOpen);
        case ROOM_AREA_OVERGROWN_CLEARING:
            return ClearingTileAt(x, y);
        case ROOM_AREA_SHRINE_APPROACH:
            return ShrineApproachTileAt(x, y);
        case ROOM_AREA_THORNBACK_ARENA:
            return ThornbackArenaTileAt(x, y);
        case ROOM_AREA_ASHFALL_DESCENT:
            return AshfallDescentTileAt(x, y);
        case ROOM_AREA_COUNT:
        default:
            return EntranceTileAt(x, y);
    }
}
