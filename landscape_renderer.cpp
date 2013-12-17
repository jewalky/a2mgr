#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdint.h>
#include "a2mgr.h"
#include "lib/utils.hpp"
#include "landscape_renderer.h"
#include <windows.h>

MapTile Tiles[52][14];

void P_LoadTiles()
{
	static bool SDLInit = false;
	if(!SDLInit)
	{
		if(SDL_Init(SDL_INIT_VIDEO) != 0)
			return;
		SDLInit = true;
	}

    log_format("P_LoadTiles: Loading tiles...\n");

    for(uint32_t i = 0; i < 52; i++)
    {
        uint8_t tile_d = ((i & 0xF0) >> 4) + 1;
        uint8_t tile_c = i & 0x0F;
        std::string file = Format("graphics/terrain/tile%u-%02u.bmp", tile_d, tile_c);
        //Image* img = new Image(file);
		SDL_Surface* srf = SDL_LoadBMP(file.c_str());
		if(!srf) log_format("  Error loading file \"%s\"!\n", file.c_str());

		SDL_Surface* transformed = NULL;
		uint32_t count_tiles = srf ? (srf->h / 32) : 0;
		if(srf && (srf->w != 32))
		{
			log_format("  Error image dimensions, file \"%s\"!\n", file.c_str());
			SDL_FreeSurface(srf);
			srf = NULL;
			count_tiles = 0;
		}

		//if(srf->h % 32 != 0) count_tiles--;

		transformed = srf ? SDL_CreateRGBSurface(0, srf->w, srf->h, 16, 0xF800, 0x07E0, 0x001F, 0) : NULL;
		if(srf && !transformed)
		{
			log_format("  Error converting image, file \"%s\"!\n", file.c_str());
			SDL_FreeSurface(srf);
			srf = NULL;
			count_tiles = 0;
		}

		if(srf && transformed && (SDL_BlitSurface(srf, NULL, transformed, NULL) == -1))
		{
			log_format("  Error converting image, file \"%s\" (phase 2)!\n", file.c_str());
			SDL_FreeSurface(transformed);
			SDL_FreeSurface(srf);
			transformed = NULL;
			srf = NULL;
			count_tiles = 0;
		}

		if(srf) SDL_FreeSurface(srf);
		srf = NULL;

		uint16_t* pixels = transformed ? (uint16_t*)transformed->pixels : NULL;

        for(uint32_t j = 0; j < 14; j++)
        {
            if(j >= count_tiles)
            {
                Tiles[i][j].Exists = false;
                Tiles[i][j].Pixels = NULL;
                Tiles[i][j].Width = 0;
                Tiles[i][j].Height = 0;
            }
            else
            {
                uint16_t* pixels2 = new uint16_t[32 * 32];
                for(uint16_t x = 0; x < 32; x++)
                {
                    for(uint16_t y = j * 32; y < j * 32 + 32; y++)
                    {
                        uint32_t x2 = x;
                        uint32_t y2 = y % 32;
                        pixels2[x2 * 32 + y2] = pixels[y * 32 + x];
                    }
                }
                Tiles[i][j].Width = 32;
                Tiles[i][j].Height = 32;
                Tiles[i][j].Pixels = pixels2;
                Tiles[i][j].Exists = true;
            }
        }

		SDL_FreeSurface(transformed);
		transformed = NULL;
    }
}

void P_UnloadTiles()
{
    for(uint32_t i = 0; i < 52; i++)
    {
        for(uint32_t j = 0; j < 14; j++)
        {
			if(Tiles[i][j].Width == 32 &&
				Tiles[i][j].Height == 32 &&
				Tiles[i][j].Pixels &&
				Tiles[i][j].Exists)
			{
				delete[] Tiles[i][j].Pixels;
				Tiles[i][j].Pixels = NULL;
				Tiles[i][j].Exists = false;
				Tiles[i][j].Width = 0;
				Tiles[i][j].Height = 0;
			}
        }
    }
}

void P_DisplayTile(uint16_t tile, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t x4, int32_t y4)
{
    if(x1 != x3) return;
    if(x2 != x4) return;

	static bool RenderBoundsReady = false;
	static SDL_Rect RenderBounds;

	if(!RenderBoundsReady)
	{
		RECT* rcSrc = (RECT*)(0x00625768);
		RenderBounds.x = rcSrc->left;
		RenderBounds.y = rcSrc->top;
		RenderBounds.w = rcSrc->right-rcSrc->left;
		RenderBounds.h = rcSrc->bottom-rcSrc->top;

		RenderBoundsReady = true;
	}

    uint8_t tile_dc = (tile & 0x0FF0) >> 4;
    uint8_t tile_i = (tile & 0x000F);

    if(!Tiles[tile_dc][tile_i].Exists ||
       !Tiles[tile_dc][tile_i].Pixels) return;

    int32_t top_y[32];
    top_y[0] = y1;
    top_y[31] = y2;

    int32_t bottom_y[32];
    bottom_y[0] = y3;
    bottom_y[31] = y4;

    int32_t minY = y2;
    if(y1 < y2) minY = y1;

    int32_t maxY = y3;
    if(y4 > y3) maxY = y4;

    if(minY > maxY) return;

    if(maxY < RenderBounds.y) return;
    if(minY >= RenderBounds.y + RenderBounds.h) return;

    for(uint32_t i = 1; i < 31; i++)
    {
        float cnf = (float)i / 32.0;
        top_y[i] = top_y[0] + (top_y[31] - top_y[0]) * cnf;
        bottom_y[i] = bottom_y[0] + (bottom_y[31] - bottom_y[0]) * cnf;
    }

    uint16_t* pixels = Tiles[tile_dc][tile_i].Pixels;
	uint32_t row_width = *(uint32_t*)(0x00625708);
    uint8_t* screen = *(uint8_t**)(0x0062571C);//(uint16_t*)RenderTarget->pixels;

    for(uint32_t x = 0; x < 32; x++)
    {
        if(bottom_y[x] <= top_y[x]) continue;
        int32_t screenX = x1 + x;
        uint32_t h = bottom_y[x] - top_y[x];
        float cfh = 32.0 / (float)h;
        int32_t screenY = top_y[x];
        float inY = 0.0;
        if(screenX < RenderBounds.x) continue;
        if(screenX >= RenderBounds.x + RenderBounds.w) continue;
        for(uint32_t y = 0; y < h; y++)
        {
            inY += cfh;
            if(inY > 31.0) inY = 31.0;
            screenY++;

            if(screenY < RenderBounds.y) continue;
            if(screenY >= RenderBounds.y + RenderBounds.h) continue;

            //screen[screenY * RenderBounds.w + screenX] = *(pixels + (uint32_t)inY);
			*(uint16_t*)(screen + screenY * row_width + screenX * 2) = *(pixels + (uint32_t)inY);
        }
        pixels += 32;
    }
}

void _stdcall P_LoadTilesNative(uint32_t flags)
{
	if(!flags)
		P_UnloadTiles();
	else P_LoadTiles();
}

void __declspec(naked) TILES_loadTiles()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		push	0xFFFFFFFF
		push	0x005C86FD
		mov		eax, fs:[0]
		push	eax
		mov		fs:[0], esp
		sub		esp, 0x298
		push	[ebp+0x08]
		call	P_LoadTilesNative
		mov		edx, 0x0047B8B7
		jmp		edx
	}
}

void _stdcall P_DisplayTileNative(uint16_t tile, int32_t x1, int32_t x2, int32_t h1, int32_t h2, int32_t h3, int32_t h4, uint32_t unk1, uint32_t unk2, uint32_t unk3, uint32_t unk4)
{
	log_format("tile = %04X; x1 = %d, x2 = %d, h1 = %d, h2 = %d, h3 = %d, h4 = %d;\n", tile, x1, x2, h1, h2, h3, h4);
	log_format("             unk1 = %u, unk2 = %u, unk3 = %u, unk4 = %u;\n", unk1, unk2, unk3, unk4);
	//P_DisplayTile(tile, x1, h1, x2, h2, x1, h3, x2, h4);
}

void __declspec(naked) TILES_displayTile1()
{
	__asm
	{
		mov		eax, [ebp-0x2C]
		add		eax, [ebp-0x38]
		movzx	ecx, byte ptr [eax+1]
		push	ecx
		movzx	ecx, byte ptr [eax]
		push	ecx
		mov		eax, [ebp-0x2C]
		add		eax, [ebp-0x48]
		movzx	ecx, byte ptr [eax+1]
		push	ecx
		movzx	ecx, byte ptr [eax]
		push	ecx
		push	[ebp-0x6C]
		push	[ebp-0x64]
		push	[ebp-0x58]
		push	[ebp-0x50]
		mov		ecx, [ebp-0x24]
		add		ecx, 1
		shl		ecx, 5
		push	ecx
		mov		ecx, [ebp-0x24]
		shl		ecx, 5
		push	ecx
		movzx	edx, word ptr [ebp-0x60]
		push	edx
		call	P_DisplayTileNative
		mov		edx, 0x0040529D
		jmp		edx
	}
}

void __declspec(naked) TILES_debug()
{
	__asm
	{
		retn	
	}
}