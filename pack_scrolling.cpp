#include <windows.h>
#include "utils.h"

unsigned long _stdcall PSCRL_rep(unsigned long parm1)
{
	unsigned long pthis = _get_this();

	bool ctrlp = (GetAsyncKeyState(VK_CONTROL));

	if(parm1 == 0x21 && ctrlp)
	{
		for(int i = 0; i < 6; i++)
			_call_member_func(0x004A937F, pthis);
		_call_func(0x0043A790, 1, pthis + 0x20BC);
		_call_func(0x0043A750, 1, pthis + 0x20BC);
		return 1;
	}
	else if(parm1 == 0x22 && ctrlp)
	{
		for(int i = 0; i < 6; i++)
			_call_member_func(0x004A91F8, pthis);
		_call_func(0x0043A790, 1, pthis + 0x20BC);
		_call_func(0x0043A750, 1, pthis + 0x20BC);
		return 1;
	}

	return 0;
}

char logwhere[] = "ret = %08X\n";

void _declspec(naked) PSCRL_gameInventory()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		sub		esp, 0x0C
		mov		[ebp-4], ecx

		push	0

		mov		eax, 0x0062B5E0
		mov		eax, [eax]
		sub		eax, 240
		mov		ecx, 80
		xor		edx, edx
		div		ecx
		sub		eax, 1
		cmp		eax, 1
		jg		gi_count_ok
		mov		eax, 1

gi_count_ok:
		mov		[ebp-0x0C], eax

		mov		eax, [ebp+8]
		cmp		eax, 0x21
		jz		gi_move_left
		cmp		eax, 0x22
		jz		gi_move_right
//		cmp		eax, 0x23
//		jz		gi_move_last
//		cmp		eax, 0x24
//		jz		gi_move_first
		xor		eax, eax
		jmp		gi_exit

gi_move_left:
		xor		eax, eax
		mov		[ebp-8], eax

gi_loop_left:
		mov		eax, [ebp-8]
		cmp		eax, [ebp-0x0c]
		jge		gi_exit_1
		mov		edx, 0x004A937F
		mov		ecx, [ebp-4]
		call	edx
		inc		dword ptr [ebp-8]
		jmp		gi_loop_left

gi_move_right:
		xor		eax, eax
		mov		[ebp-8], eax

gi_loop_right:
		mov		eax, [ebp-8]
		cmp		eax, [ebp-0x0c]
		jge		gi_exit_1
		mov		edx, 0x04A91F8
		mov		ecx, [ebp-4]
		call	edx
		inc		dword ptr [ebp-8]
		jmp		gi_loop_right

gi_exit_1:
		mov		eax, 1

gi_exit:
		mov		esp, ebp
		pop		ebp
		retn	4
	}
}

void _declspec(naked) PSCRL_shopShelfOnKeyDown()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		sub		esp, 8
		mov		[ebp-4], ecx

		mov		edx, 0x05A4D36 // GetAsyncKeyState
		mov		eax, VK_CONTROL
		push	eax
		call	edx
		test	eax, eax
		jnz		ss_continue
		xor		eax, eax
		jmp		ss_exit
ss_continue:
		push	dword ptr [ebp+8]
		mov		ecx, [ebp-4]
		mov		edx, 0x04BC146
		call	edx
ss_exit:
		mov		esp, ebp
		pop		ebp
		retn	4
	}
}

void PSCRL_InitializeInjection()
{
	unsigned long func1 = (unsigned long)PSCRL_rep;
	unsigned long func2 = (unsigned long)PSCRL_gameInventory;
	unsigned long func3 = (unsigned long)PSCRL_shopShelfOnKeyDown;
	memcpy((void*)0x005D92F4, &func1, 4);
	memcpy((void*)0x005D8BAC, &func2, 4);
	memcpy((void*)0x005D92F4, &func3, 4);
}