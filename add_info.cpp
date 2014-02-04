#include "zxmgr.h"
#include "config.h"
#include "utils.h"

#include <ctime>

using namespace zxmgr;
using namespace std;

#include "Image.h"
bool c_Visible = false;

time_t start_time = time(NULL);

bool _stdcall ConsoleProc(unsigned char ch, int unk)
{
	// log_format("ch = %02X (%c); unk = %08X (%d, %u)\n", ch, ch, unk, unk, unk);

	if(ch == 0xC0 && unk == 0xB8 &&
		(GetAsyncKeyState(VK_SHIFT) & 0x8000)) // Shift+~
	{
		c_Visible = !c_Visible;
		return true;
	}
	
	if(ch == 0x10 && unk == 0x08 && (GetAsyncKeyState(VK_RSHIFT) & 0x8000)) // Right Shift
	{
		start_time = time(NULL);
	}

	char ch2 = MapVirtualKey(ch, 2);

	//log_format("outChar = %02X (%c)\n", (unsigned)ch2, ch2);

	return false;
}

Image* cImageBG = NULL;
Image* cImageLeft = NULL;
Image* cImageMid = NULL;
Image* cImageRight = NULL;

void ConsoleImpl()
{
	if(!c_Visible) return;

	if(!cImageBG) cImageBG = new Image("patch/console/console-bg.png");
	if(!cImageLeft) cImageLeft = new Image("patch/console/console-left.png");
	if(!cImageMid) cImageMid = new Image("patch/console/console-middle.png");
	if(!cImageRight) cImageRight = new Image("patch/console/console-right.png");

	RECT pRect = *(RECT*)(0x00625768);
	int32_t pWidth2 = pRect.right-(pRect.left+128);
	if(pWidth2 < 0) pWidth2 = 0;
	uint32_t num = pWidth2 / 64;
	uint32_t lastSize = pWidth2 % 64;
	if(lastSize != 0) num++;
	//if(c_Visible)
	{
		cImageLeft->Display(pRect.left, 320);
		for(uint32_t i = 0; i < num; i++)
		{
			uint32_t width = 64;
			if(i == num-1) width = lastSize;
			cImageMid->DisplayEx(64+i*64, 320, 0, 0, width, 64, false);
		}
		cImageRight->Display(pRect.right-64, 320);

		uint32_t pWidth = pRect.right-pRect.left;
		num = pWidth / 64;
		lastSize = pWidth % 64;
		if(lastSize != 0) num++;

		uint32_t numY = 320 / 64;
		uint32_t lastSizeY = 320 % 64;
		if(lastSizeY != 0) numY++;

		for(uint32_t y = 0; y < numY; y++)
		{
			for(uint32_t x = 0; x < num; x++)
			{
				uint32_t width = 64;
				uint32_t height = 64;
				if(y == numY-1) height = 64;//lastSizeY;
				if(x == num-1) width = lastSize;
				cImageBG->DisplayEx(64*x, 64*y, 0, 0, width, height, false);
			}
		}
	}
}

void _stdcall FpsImpl(unsigned long cptr, RECT* rcSrc2)
{
	//ConsoleImpl();

	int top = rcSrc2->top + 16;
	if(r_fps)
	{
		unsigned long pt1 = *(unsigned long*)(cptr + 0xD0);
		unsigned long pt2 = *(unsigned long*)(cptr + 0xD4);
		string fps_string = Format("%3.1f", pt1, pt2);

		Font::DrawText(FONT1, rcSrc2->right - 136, rcSrc2->top + 16, "fps:", FONT_ALIGN_LEFT, FONT_COLOR_GRAY, 1);
		Font::DrawText(FONT1, rcSrc2->right - 86, rcSrc2->top + 16, fps_string.c_str(), FONT_ALIGN_LEFT, FONT_COLOR_WHITE, 1);
		top += 16;
	}
	if(r_time)
	{
		time_t t = time(NULL);
		struct tm *tm = localtime(&t);
		string time_string = Format("%02u:%02u:%02u", tm->tm_hour, tm->tm_min, tm->tm_sec);

		Font::DrawText(FONT1, rcSrc2->right - 136, top, "time:", FONT_ALIGN_LEFT, FONT_COLOR_GRAY, 1);
		Font::DrawText(FONT1, rcSrc2->right - 86, top, time_string.c_str(), FONT_ALIGN_LEFT, FONT_COLOR_WHITE, 1);
		top += 16;
	}
	if(show_stopwatch)
	{
		time_t time_diff = (time_t)difftime(time(NULL), start_time);
		struct tm *td = gmtime(&time_diff);

		string time_string = Format("%02u:%02u:%02u", td->tm_hour, td->tm_min, td->tm_sec);

		Font::DrawText(FONT1, rcSrc2->right - 86, top, time_string.c_str(), FONT_ALIGN_LEFT, FONT_COLOR_WHITE, 1);
	}
}

void __declspec(naked) INFO_fps()
{
	__asm
	{ // 40AAF5
		lea		eax, [ebp-0x48]
		push	eax
		push	[ebp-0x02F4]
		call	FpsImpl
		mov		edx, 0x0040ABDA
		jmp		edx
	}
}