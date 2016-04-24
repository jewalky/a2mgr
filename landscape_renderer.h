// Ported from ZAllods engine
#pragma once
#include "stdint.h"

struct MapTile
{
	MapTile()
	{
		Exists = false;
		Width = Height = 0;
		Pixels = NULL;
	}

    bool Exists;
    uint32_t Width;
    uint32_t Height;
    uint16_t* Pixels;
};

extern MapTile Tiles[52][14];

void P_LoadTiles();
void P_DisplayTile(uint16_t tile, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t x4, int32_t y4);