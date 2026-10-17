#ifndef PALM_ROOM_LAYOUT_H
#define PALM_ROOM_LAYOUT_H

#include "room.h"

#define ROOM_LAYOUT_ENTRANCE_RAMP_TILE_X 28
#define ROOM_LAYOUT_ENTRANCE_RAMP_TILE_Y 12

char Room_LayoutTileAt(RoomArea area, int x, int y, bool thornGateOpen);

#endif
