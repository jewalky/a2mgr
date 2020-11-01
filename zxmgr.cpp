#include <cstdlib>
#include "zxmgr.h"
#include <cstdarg>
#include <cstring>
#include <cstdio>

unsigned long _memcpy = (unsigned long)memcpy;

namespace zxmgr
{
	unsigned long AfxGetMainWnd()
	{
		unsigned long retv = 0;
		__asm
		{
			mov		edx, 0x00401870
			call	edx
			mov		retv, eax
		}
		return retv;
	}

	namespace GUI
	{
		unsigned long _stdcall GetWindowLong(unsigned long cptr, unsigned long which)
		{
			unsigned long retval;
			__asm
			{
				push	[which]
				mov		ecx, [cptr]
				mov		edx, [ecx]
				call	[edx+0x20]
				mov		[retval], eax
			}
			return retval;
		}

		void _stdcall RepaintWindow(unsigned long cptr)
		{
			__asm
			{
				mov		ecx, [cptr]
				mov		edx, [ecx]
				call	[edx+0x34]
			}
		}

		void _stdcall RepaintWindowIndirect(unsigned long cptr)
		{
			__asm
			{
				push	1
				push	[cptr]
				mov		ecx, [cptr]
				add		ecx, 0x48
				mov		edx, 0x004DC126
				call	edx
			}
		}

		void _declspec(naked) GetClientRect(unsigned long cptr, RECT* rec)
		{
			__asm
			{
				push	ebp
				mov		ebp, esp
				push	[ebp+0x0C]
				mov		edx, 0x00402800
				call	edx
				mov		eax, [ebp+0x08]
				add		eax, 0x08
				push	eax
				push	[ebp+0x0C]
				mov		ecx, [cptr]
				mov		edx, 0x004DBB93
				call	edx
				mov		esp, ebp
				pop		ebp
				retn
			}
		}
	}

	namespace Font
	{
		unsigned long __declspec(naked) GetColor(unsigned long t)
		{
			__asm
			{
				/*push    0
				mov     ecx, dword_62B088
				call    sub_420440
				*/
				push	ebp
				mov		ebp, esp
				push	0
				mov		ecx, [ebp+0x08]
				//mov		ecx, 0x0062B088
				mov		ecx, [ecx]
				mov		edx, 0x00420440
				call	edx
				mov		esp, ebp
				pop		ebp
				retn
			}
		}

		void DrawText(unsigned long cptr, int x, int y, const char* string, unsigned long align, unsigned long color, unsigned long shadowpos)
		{
			__asm
			{
				push	[shadowpos]
				push	[color]
				push	[align]
				push	[string]
				push	[y]
				push	[x]
				mov		ecx, [cptr]
				mov		ecx, [ecx]
				mov		edx, 0x004627F6
				call	edx
			}
		}

		unsigned long MeasureTextWidth(unsigned long cptr, const char* string)
		{
			unsigned long retval;
			__asm
			{
				push	[string]
				mov		ecx, [cptr]
				mov		edx, 0x00461E19
				call	edx
				mov		[retval], eax
			}
			return retval;
		}
	}

	void __declspec(naked) WriteChatRaw(const char* msg)
	{
		__asm
		{
			push	ebp
			mov		ebp, esp
			sub		esp, 0x08
			mov		edx, 0x00401870
			call	edx
			mov		[ebp-0x08], eax
			test	eax, eax
			jz		skipsend
			push	0x7530
			push	0x0062AAF0
			push	[ebp+0x08]
			mov		ecx, [ebp-0x08]
			mov		ecx, [ecx+0xD0] // chat?
			add		ecx, 0x0A28
			mov		edx, 0x004021FE
			call	edx
		skipsend:
			mov		esp, ebp
			pop		ebp
			retn
		}
	}

	void WriteChat(const char* s, ...)
	{
		char line[2048];
		va_list va;
		va_start(va, s);
		_vsnprintf(line, 1255, s, va);
		WriteChatRaw(line);
		va_end(va);
	}

	void WriteChatA(const char* s, ...)
	{
		char line[2048];
		va_list va;
		va_start(va, s);
		_vsnprintf(line, 1255, s, va);
		AnsiToOem(line, line);
		WriteChatRaw(line);
		va_end(va);
	}

	void __declspec(naked) DrawLine(int x1, int y1, int x2, int y2, unsigned long color)
	{
		__asm
		{
			push	ebp
			mov		ebp, esp
			push	[ebp+0x18]
			push	[ebp+0x14]
			push	[ebp+0x10]
			push	[ebp+0x0C]
			push	[ebp+0x08]
			mov		edx, 0x0045BA0C
			call	edx
			mov		esp, ebp
			pop		ebp
			retn
		}
	}

	void __declspec(naked) FillRect(int left, int top, int right, int bottom, unsigned long color)
	{
		__asm
		{
			push	ebp
			mov		ebp, esp
			push	[ebp+0x18]
			push	[ebp+0x14]
			push	[ebp+0x10]
			push	[ebp+0x0C]
			push	[ebp+0x08]
			mov		edx, 0x0045B618
			call	edx
			mov		esp, ebp
			pop		ebp
			retn
		}
	}

	void __declspec(naked) UpdateScreen()
	{
		__asm
		{
			mov		edx, 0x00458A58
			call	edx
			retn
		}
	}

	const char* GetTranslationString(unsigned long trans_id, unsigned long idx)
	{
		char* retv;
		__asm
		{
			push	[idx]
			mov		ecx, [trans_id]
			mov		edx, 0x00479BA3
			call	edx
			mov		retv, eax
		}
		return retv;
	}

	const char* GetPatchString(unsigned long idx)
	{
		return GetTranslationString(TRANS_PATCH, idx);
	}

	void LockBuffer()
	{
		__asm
		{
			mov		edx, 0x00457EAE
			call	edx
		}
	}

	void UnlockBuffer()
	{
		__asm
		{
			mov		edx, 0x00457F0A
			call	edx
		}
	}

	int __declspec(naked) GlobalReceive()
	{
		__asm
		{
			push	ebp
			mov		ebp, esp
			push	0x064
			call	AfxGetMainWnd
			mov		ecx, [eax+0xD0]
			mov		edx, 0x0040DAC9
			call	edx
			mov		esp, ebp
			pop		ebp
			retn
		}
	}

	void __declspec(naked) DisplayMouse()
	{
		__asm
		{
			mov		ecx, 0x0060E6C0
			mov		edx, 0x00429114
			call	edx
			retn
		}
	}

	HWND GetHWND()
	{
		HWND hWnd;
		unsigned long mainwnd = zxmgr::AfxGetMainWnd();
		if(!mainwnd) return 0;

		hWnd = *(HWND*)(mainwnd + 0x1C);
		return hWnd;
	}

	void _stdcall DoMessageLoop()
	{
		MSG msg;
		while(PeekMessage(&msg, 0, 0, 0, 1))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void LockHighSurface()
	{
		__asm
		{
			mov		edx, 0x00457F5C
			call	edx
		}
	}

	void UnlockHighSurface()
	{
		__asm
		{
			mov		edx, 0x00457FB8
			call	edx
		}
	}

	void AfxAbort()
	{
		__asm
		{
			mov		edx, 0x005A8B96
			call	edx
		}
	}
}