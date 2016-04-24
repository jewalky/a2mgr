#include "zxmgr.h"
#include "zxmgr_gui.h"
#include "Image.h"
#include "utils.h"
#include "config.h"

//static byte* img_MenuNew = 0;
static Image* img_MenuNew = 0;
static Image* img_ButtonsUp[3] = {0, 0, 0};
static Image* img_ButtonsDown[3] = {0, 0, 0};

static int buttonpos[] = {251, 258,
	                      251, 304,
						  251, 351};

static int buttonhover = -1;
static int buttonclick = -1;

void _stdcall DoMenuLoad()
{
	/*
	static char aMenuNew[] = "locale\\ru\\graphics\\mainmenu\\menunew.bmp";

	__asm
	{
		push	0x24
		mov		edx, 0x00401840
		call	edx
		mov		img_MenuNew, eax
		push	offset aMenuNew
		mov		ecx, img_MenuNew
		mov		edx, 0x0042769B
		call	edx
		mov		img_MenuNew, eax
	}*/

	if (!img_MenuNew) img_MenuNew = new Image(Format("data\\locale\\%s\\graphics\\mainmenu\\menunew.png", z_russian?"ru":"en"));
	for (int i = 0; i < 3; i++)
	{
		if (!img_ButtonsUp[i])
			img_ButtonsUp[i] = new Image(Format("data\\graphics\\interface\\inn\\button%doff.bmp", i+1));
		if (!img_ButtonsDown[i])
			img_ButtonsDown[i] = new Image(Format("data\\graphics\\interface\\inn\\button%don.bmp", i+1));
	}
}

void _stdcall DoMenuDraw()
{
	zxmgr::LockBuffer();

	RECT pRect = *(RECT*)(0x00625768);

	int xMin = pRect.right / 2 - 320;
	int yMin = pRect.bottom / 2 - 240;
	img_MenuNew->Display(xMin, yMin);

	// display buttons
	for (int i = 0; i < 3; i++)
	{
		// draw off for now
		int bx = xMin + buttonpos[i*2];
		int by = yMin + buttonpos[i*2+1];

		if (buttonclick == i)
			img_ButtonsDown[i]->Display(bx, by);
		else img_ButtonsUp[i]->Display(bx, by);

		// display text
		unsigned long col = FONT_EXTCOLOR_GOLD;

		int tx = bx + 70;
		int ty = by + 16;

		if (buttonclick == i)
		{
			ty += 1;
			col = FONT_EXTCOLOR_GOLDBRIGHT;
		}
		else if (buttonhover == i)
			col = FONT_EXTCOLOR_GOLDBRIGHT;
		//void DrawText(unsigned long cptr, int x, int y, const char* string, unsigned long align, unsigned long color, unsigned long shadowpos)

		zxmgr::Font::DrawText(FONT4, tx, ty, zxmgr::GetPatchString(238+i), FONT_ALIGN_CENTER, zxmgr::Font::GetColor(col), 1);
	}

	zxmgr::UnlockBuffer();
}

int _stdcall DoMenuClick(int button)
{
	log_format("click\n");
	return 0;
}

void _stdcall DoMenuHover(byte* menu, int x, int y, unsigned int buttons)
{
	RECT pRect = *(RECT*)(0x00625768);

	int xMin = pRect.right / 2 - 320;
	int yMin = pRect.bottom / 2 - 240;

	// for whatever reason, this also gets called on click (null hover)
	buttonhover = -1;
	if (buttons == 0) buttonclick = -1;
	for (int i = 0; i < 3; i++)
	{
		// draw off for now
		int bx = xMin + buttonpos[i*2];
		int by = yMin + buttonpos[i*2+1];
		int bw = 140;
		int bh = 46;
		//log_format("mouse hover %d, %d, %d (%u, %08X)\n", x, y, buttons, buttons, buttons);

		// buttons is 1 or 2
		if (x >= bx && x < bx+bw &&
			y >= by && y < by+bh)
		{
			buttonhover = i;
			if (buttonclick < 0 && buttons == 1)
			{
				buttonclick = i;
				// play menu sound
				__asm
				{
					mov		ecx, menu
					add		ecx, 0xD4
					push	ecx
					mov		edx, 0x0043A750
					call	edx
					add		esp, 4
				}
			}
		}
	}

	// put data to menu (most notably +F0)
	// 0 = 5
	// 1 = 3
	// 2 = 7
	if (buttonclick == buttonhover)
	{
		switch (buttonclick)
		{
		case 0:
			*(uint32_t*)(menu + 0xF0) = 5;
			break;

		case 1:
			*(uint32_t*)(menu + 0xF0) = 3;
			break;

		case 2:
			*(uint32_t*)(menu + 0xF0) = 7;
			break;

		default:
			*(uint32_t*)(menu + 0xF0) = -1;
			break;
		}
	}
	else
	{
		*(uint32_t*)(menu + 0xF0) = -1;
	}
}

void __declspec(naked) MENU_load()
{
	// @ 4AEDF5
	__asm
	{
		call	DoMenuLoad
		/*lea		ecx, [ebp-0x10]
		mov		edx, 0x005AB6FF
		call	edx
		mov		edx, 0x004AEDFD
		jmp		edx*/
		mov		edx, 0x004AF09D
		jmp		edx
	}
}

void __declspec(naked) MENU_draw()
{
	// @ 4AF2C8
	__asm
	{
		call	DoMenuDraw

		mov		edx, 0x004AF43D
		jmp		edx
	}
}

void __declspec(naked) MENU_click()
{
	// @ 4AF93F
	__asm
	{
		cmp		[ebp-0x0C], 7
		ja		loc_4AFA34 // old handling
		mov		edx, 0x004AF949
		jmp		edx

		push	[ebp-0x0C]
		call	DoMenuClick
		mov		[ebp-0x04], eax

		mov		edx, 0x004AFA34
		jmp		edx

loc_4AFA34:
		mov		edx, 0x4AFA34
		jmp		edx
	}
}

void __declspec(naked) MENU_hover1()
{
	// @ 4AF8E6
	__asm
	{
		push	[ebp+0x08]
		push	[ebp+0x10]
		push	[ebp+0x0C]
		push	[ebp-0x04]
		call	DoMenuHover

		mov		edx, 0x004AF918
		jmp		edx
	}
}

void __declspec(naked) MENU_hover2()
{
	// @ 4AF8C3
	__asm
	{
		push	[ebp+0x08]
		push	[ebp+0x10]
		push	[ebp+0x0C]
		push	[ebp-0x04]
		call	DoMenuHover

		mov		edx, 0x004AF8D7
		jmp		edx
	}
}

void __declspec(naked) MENU_hover3()
{
	// @ 4AFA5B
	__asm
	{
		push	[ebp+0x08]
		push	[ebp+0x10]
		push	[ebp+0x0C]
		push	[ebp-0x08]
		call	DoMenuHover

		mov		edx, 0x004AFA6F
		jmp		edx
	}
}