#pragma once

#include <windows.h>

#undef GetWindowLong
#undef DrawText

#define FONT1 0x00629C88
#define FONT2 0x0062A6C8
#define FONT3 0x0062AFF8
#define FONT4 0x0062AF90

#define FONT_COLOR_WHITE	0x00629C48
#define FONT_COLOR_GRAY		0x0062AAF0

#define FONT_EXTCOLOR_GOLD  0x0062B088
#define FONT_EXTCOLOR_BROWN 0x0062B048
#define FONT_EXTCOLOR_GOLDBRIGHT 0x0062AF8C

#define FONT_ALIGN_RIGHT 1
#define FONT_ALIGN_CENTER 2
#define FONT_ALIGN_LEFT 0

namespace zxmgr
{
	unsigned long AfxGetMainWnd();

	namespace GUI
	{
		unsigned long _stdcall GetWindowLong(unsigned long cptr, unsigned long which);
		void _stdcall RepaintWindow(unsigned long cptr);
		void _stdcall RepaintWindowIndirect(unsigned long cptr);
		void GetClientRect(unsigned long cptr, RECT* rec);
	}

	namespace Font
	{
		unsigned long GetColor(unsigned long t);
		void DrawText(unsigned long cptr, int x, int y, const char* string, unsigned long align, unsigned long color, unsigned long shadowpos);
		unsigned long MeasureTextWidth(unsigned long cptr, const char* string);
	}

	void FillRect(int left, int top, int right, int bottom, unsigned long color);
	void UpdateScreen();
	void LockBuffer();
	void UnlockBuffer();

	char* GetPatchString(unsigned long idx);

	void WriteChatRaw(const char*);
	void WriteChat(const char*, ...);
	void WriteChatA(const char*, ...);

	int GlobalReceive();
	void DisplayMouse();

	void DoMessageLoop();

	HWND GetHWND();

	void LockHighSurface();
	void UnlockHighSurface();

	void AfxAbort();
}