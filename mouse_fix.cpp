#include <windows.h>
#include "zxmgr.h"
#include "lib/stdint.h"
#include <map>
#include <vector>
#include "utils.h"

struct CursorInfo
{
	std::vector<HICON> Frames;
};

std::map<byte*, CursorInfo*> a2ToWinCursor;

inline static void SpriteAddIXIY(int& ix, int& iy, int w, int add)
{
	int x = ix;
	int y = iy;
	for (int i = 0; i < add; i++)
	{
		x++;
		if (x >= w)
		{
			y++;
			x = x - (int)w;
		}
	}

	ix = x;
	iy = y;
}

std::vector<HBITMAP> _stdcall A16ToBitmap(byte* a16)
{
	std::vector<HBITMAP> bitmaps;
	uint32_t sprite_count = *(uint32_t*)(a16 + 4);
	uint32_t* palette = *(uint32_t**)(a16 + 0x20);
	
	for (uint32_t i = 0; i < sprite_count; i++)
	{
		byte* frame_ptr = *(byte**)(*(uint32_t*)(a16 + 12) + 4 * i);
		int32_t frame_w = *(uint32_t*)(frame_ptr);
		int32_t frame_h = *(uint32_t*)(frame_ptr+4);
		int32_t frame_size = *(uint32_t*)(frame_ptr+8);
		uint16_t* frame_data = (uint16_t*)(frame_ptr+0x0C);
		// lets render the cursor now
		uint32_t* pixels = new uint32_t[frame_w * frame_h];
		// color = 0xBBGGRRAA
		// render 16a to texture
		// ported from Unity
		int ix = 0, iy = 0;
		int32_t ids = frame_size;
		memset(pixels, 0, frame_w * frame_h * 4);
		while (ids > 0)
		{
			uint16_t ipx = *frame_data++;
			ipx &= 0xC0FF;
			ids -= 2;

			if (ipx & 0xC000)
			{
				if ((ipx & 0xC000) == 0x4000)
				{
					ipx &= 0xFF;
					SpriteAddIXIY(ix, iy, frame_w, ipx * frame_w);
				}
				else
				{
					ipx &= 0xFF;
					SpriteAddIXIY(ix, iy, frame_w, ipx);
				}
			}
			else
			{
				ipx &= 0xFF;
				for (int j = 0; j < ipx; j++)
				{
					uint16_t ss = *frame_data++;
					uint32_t alpha = (((ss & 0xFF00) >> 9) & 0x0F) + (((ss & 0xFF00) >> 5) & 0xF0);
					uint16_t idx = ((ss & 0xFF00) >> 1) + ((ss & 0x00FF) >> 1);
					idx &= 0xFF;
					alpha &= 0xFF;
					uint32_t color32 = palette[idx] & 0xFFFFFF;
					pixels[iy * frame_w + ix] = color32 | (alpha << 24);
					SpriteAddIXIY(ix, iy, frame_w, 1);
				}
				ids -= ipx * 2;
			}
		}
		HBITMAP bmp = CreateBitmap(frame_w, frame_h, 1, 32, pixels);
		delete[] pixels;
		bitmaps.push_back(bmp);
	}
	
	return bitmaps;
}

void _stdcall ReplaceCursor(byte* source)
{

	// for now support only .16a cursor, one frame
	byte* image_ptr = *(byte**)(source + 4);
	int32_t offset_x = *(int32_t*)(source + 0x18);
	int32_t offset_y = *(int32_t*)(source + 0x1C);
	uint32_t delay = *(uint32_t*)(source + 0x2C);

	// same logic as ROM2. animate mouse
	DWORD ct = timeGetTime();
	if (ct - *(DWORD*)(source + 0x30) > * (DWORD*)(source + 0x2C))
	{
		*(DWORD*)(source + 0x30) = ct;
		if (++ * (DWORD*)(source + 0x24) >= *(DWORD*)(source + 0x28))
			*(DWORD*)(source + 0x24) = 0;
	}


	//log_format("cursor ptr = %p\n", image_ptr);

	if (!image_ptr)
	{
		SetCursor(0);
		return;
	}

	uint32_t type_id = *(uint32_t*)(image_ptr);

	//log_format("cursor type_id = %08x\n", type_id);

	CursorInfo* cinf = a2ToWinCursor[image_ptr];
	if (!cinf)
	{
		std::vector<HBITMAP> bitmaps;
		if (type_id == 0x005D5488) // vtable of 16a
			bitmaps = A16ToBitmap(image_ptr);
		if (!bitmaps.size())
		{
			SetCursor(0);
			return;
		}
		cinf = new CursorInfo();
		//log_format("loading cursor with %d images\n", bitmaps.size());
		for (int i = 0; i < bitmaps.size(); i++)
		{
			ICONINFO inf;
			BITMAP bm;
			GetObject(bitmaps[i], sizeof(BITMAP), &bm);
			inf.hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, nullptr);
			inf.hbmColor = bitmaps[i];
			inf.fIcon = false;
			inf.xHotspot = offset_x;
			inf.yHotspot = offset_y;
			HICON ico = CreateIconIndirect(&inf);
			//log_format("bitmap = %p, ico = %p\n", bitmaps[i], ico);
			cinf->Frames.push_back(ico);
		}
		a2ToWinCursor[image_ptr] = cinf;
	}

	//log_format("set cursor to %p\n", cinf);
	SetCursor(cinf ? cinf->Frames[*(DWORD*)(source+0x24)] : 0);

}

void __declspec(naked) CURSOR_preventHide()
{
	// injected instead of WinAPI SetCursor in a lot of places
	__asm
	{
		push	ebp
		mov		ebp, esp
		mov		esp, ebp
		pop		ebp
		retn	0x0004
	}
}

void __declspec(naked) CURSOR_display()
{
	// 1. sub_428BD1
	// 2. sub_428B92

	__asm
	{
		push	ebp
		mov		ebp, esp

		// *this = cursor to display
		push	ecx
		call	ReplaceCursor

		mov		esp, ebp
		pop		ebp
		retn
	}
}