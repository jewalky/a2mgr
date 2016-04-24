#include <windows.h>
#include "zxmgr.h"
#include "zxmgr_gui.h"


namespace zxmgr
{
	namespace GUI
	{
		unsigned long DefCWindowVtbl[36] = {
				0x005AC95E, // get object name
				0x00453C60, // destructor
				0x004019D0,
				0x004019E0,
				0x004DCBC3,
				0x004DC525,
				0x004DC509,
				0x004DC313,
				0x004DC2FD,
				0x004DC34A,
				0x004DC3B0,
				0x004E3FBE,
				0x004E3FFD,
				0x004DC5D8,
				0x004DC613,
				0x004DC6A9,
				0x004DC647,
				0x004DC712,
				(unsigned long)CWindow::OnMessage, // 18: message
				0x00420950,
				0x004201A0,
				0x004E4868,
				0x0043A7D0,
				0x0043A7E0,
				0x0043A600,
				0x0043A610,
				0x004201B0,
				0x004E4833,
				0x004201C0,
				0x0043A630,
				(unsigned long)CWindow::IdleProcessing, // 30: not sure what this is, idle processing or sumfuck like auto-height
				0x0043A9E0,
				0x004E44D0,
				0x004E450B,
				(unsigned long)CWindow::PopulateWindow, // 34: here should be default function for populating window
				0x0044C815,
			};

		void CWindow::ShowWindow()
		{
			unsigned long pthis = (unsigned long)this;
			__asm
			{
				push	[pthis]
				mov		edx, 0x00401870
				call	edx
				mov		ecx, eax
				mov		edx, 0x004913EB
				call	edx
			}
		}

		void CWindow::InitializeWindow(unsigned long unkint, unsigned long left, unsigned long top, unsigned long right, unsigned long bottom, unsigned long unkint2)
		{
			char aNoData[] = "";
			unsigned long pthis = (unsigned long)this;
			__asm
			{
				push	aNoData // sub-header
				push	1
				push	aNoData // header
				push	[unkint2]
				push	[bottom]
				push	[right]
				push	[top]
				push	[left]
				push	[unkint]
				mov		ecx, [pthis]
				mov		edx, 0x004465FB
				call	edx
				mov		edx, 0x00401870
				call	edx
				add		eax, 0x3B4
				mov		[unkint2], eax
				mov		eax, [pthis]
				mov		ebx, [unkint2]
				mov		[eax+0x78], ebx
			}
			this->ReplaceRoutines();
		}
	}
}