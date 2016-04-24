#include "lib/ScanrangeCalc.hpp"
#include <cstring>

bool scanrange_initialized = false;
CScanrangeCalc Scanrange;
char* scanrange_map = NULL;

bool a2mgr_CheckValid(int16_t x, int16_t y)
{
	if(!scanrange_map) return false;
	return (x >= 7 && y >= 7 &&
		x < *(uint32_t*)(scanrange_map + 0x84) - 7 &&
		y < *(uint32_t*)(scanrange_map + 0x88) - 7);
}

uint8_t a2mgr_GetHeight(int16_t x, int16_t y)
{
	if(!a2mgr_CheckValid(x, y)) return 0;
	char* mapthis = *(char**)(scanrange_map + 0x80);
	uint32_t mapwidth = *(uint32_t*)(mapthis + 4);
	char* heightmap = *(char**)(mapthis + 0x10);
	return heightmap[y*mapwidth+x];
}

int _stdcall CalcVisMapCLIENT(char* unit)
{
	char* pthis;
	__asm mov pthis, ecx;
	scanrange_map = pthis;

	if(!scanrange_initialized)
	{
		Scanrange.InitializeTables();
		scanrange_initialized = true;
	}

	uint32_t* pt = (uint32_t*)(pthis + 0x17E4);

	uint32_t coordX = *(uint32_t*)(unit + 0x38);
	uint32_t coordY = *(uint32_t*)(unit + 0x3C);

	uint16_t scanRange = *(uint16_t*)(unit + 0x10A);
	Scanrange.CalculateVision(coordX, coordY, scanRange, &a2mgr_GetHeight, &a2mgr_CheckValid);
	memcpy(pt, Scanrange.pTablesVision, 0x1A44);
}