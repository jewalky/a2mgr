#include <windows.h>
#include "a2mgr.h"

DWORD _stdcall CheckValid(LPVOID what, DWORD count = 4)
{
	if(!IsBadReadPtr(what, count)) return (DWORD)what;
	return 0;
}

DWORD _stdcall CheckValidW(LPVOID what, DWORD count = 4)
{
	if(!IsBadWritePtr(what, count)) return (DWORD)what;
	return 0;
}

void __declspec(naked) CRASH_fix_414994()
{
	__asm
	{
		push	4
		push	eax
		call	CheckValid
		test	eax, eax
		jz		ret_bad

		mov		eax, [eax]
		push	0x38
		push	eax
		call	CheckValid
		test	eax, eax
		jz		ret_bad

		mov		ecx, [eax+0x34]
		and		ecx, 1
		
		mov		edx, 0x0041499C
		jmp		edx

ret_bad:
		mov		edx, 0x00417BE5
		jmp		edx
	}
}

void __declspec(naked) CRASH_fix_42353A()
{
	__asm
	{
		push	8
		push	eax
		call	CheckValid
		test	eax, eax
		jz		ret_bad

		mov		ecx, [eax+4]
		mov		edx, [ebp+0x08]
		lea		eax, [ecx+edx*4]
		mov		esp, ebp
		pop		ebp
		retn	4

ret_bad:
		xor		eax, eax
		mov		esp, ebp
		pop		ebp
		retn	4
	}
}

void __declspec(naked) CRASH_fix_523295()
{
	__asm
	{
		push	4
		push	eax
		call	CheckValid
		test	eax, eax
		jz		ret_bad

		mov		ecx, [ebp-0x0C]
		mov		dl, [eax]
		mov		[ecx+0x09], dl
		mov		eax, [ebp-0x0C]
		xor		ecx, ecx
		mov		cl, [eax+0x08]
		cmp		ecx, 0x17

		mov		edx, 0x005232A5
		jmp		edx

ret_bad:
		mov		edx, 0x005234A3
		jmp		edx
	}
}

void __declspec(naked) CRASH_fix_46AD10()
{
	__asm
	{
		mov		edx, [ebp-0x58]
		movsx	eax, word ptr [edx+0x104]
		movsx	ecx, word ptr [edx+0x108]
		cmp		eax, ecx
		jle		skip_replace

		mov		word ptr [edx+0x104], cx

skip_replace:
		mov		edx, [ebp-0x58]
		movsx	eax, word ptr [edx+0x104]
		mov		ecx, [ebp-0x20]
		sub		ecx, [ebp-0x18]
		sub		ecx, 8
		imul	eax, ecx
		mov		edx, [ebp-0x58]
		movsx	ecx, word ptr [edx+0x108]
		cdq

		cmp		ecx, 0
		jz		skip_div
		idiv	ecx
		mov		[ebp-0x08], eax
		jmp		retc

skip_div:
		mov		[ebp-0x08], 0

retc:
		cmp		[ebp-0x08], 0
		mov		edx, 0x0046AD3A
		jmp		edx
	}
}

void __declspec(naked) CRASH_fix_420450()
{
	__asm
	{
		push	0x0C
		push	ecx
		call	CheckValid
		test	eax, eax
		jz		ret_bad

		mov		edx, [eax+0x08]
		mov		eax, [ebp+0x08]
		shl		eax, 8
		lea		eax, [edx+eax*2]
		mov		esp, ebp
		pop		ebp
		retn	4

ret_bad:
		xor		eax, eax
		mov		esp, ebp
		pop		ebp
		retn	4
	}
}

void __declspec(naked) CRASH_fix_45FC3B()
{
	__asm
	{
		push	0x14
		push	ecx
		call	CheckValid
		test	eax, eax
		jz		ret_bad
		mov		[ebp-0x10], eax
		mov		eax, 0x006297E4
		cmp		[eax], 0
		jz		loc_45FC4E

		mov		edx, 0x0045FC47
		jmp		edx

ret_bad:
loc_45FC4E:
		mov		edx, 0x0045FC4E
		jmp		edx
	}
}

void __declspec(naked) CRASH_fix_458CF9()
{
	__asm
	{
		push	[ebp-0x04]
		push	[ebp-0x08]
		call	CheckValidW
		test	eax, eax
		jz		ret_bad

		push	[ebp-0x04]
		push	[ebp-0x0C]
		call	CheckValid
		test	eax, eax
		jz		ret_bad

		push	[ebp-0x04]
		push	[ebp-0x0C]
		push	[ebp-0x08]
		mov		edx, 0x00458D05
		jmp		edx

ret_bad:
		xor		eax, eax
		mov		edx, 0x00458D24
		jmp		edx
	}
}

void __declspec(naked) CRASH_fix_4657AD()
{
	__asm
	{
		push	1
		push	ecx
		call	CheckValid
		test	eax, eax
		jz		ret_bad

		movsx	edx, byte ptr [eax]
		mov		[ebp-0x10], edx
		mov		eax, [ebp-0x28]
		add		eax, 1
		mov		[ebp-0x28], eax

		mov		edx, 0x004657BC
		jmp		edx

ret_bad:
		xor		eax, eax
		mov		edx, 0x0046584C
		jmp		edx
	}
}

void __declspec(naked) CRASH_fix_46B685()
{
	__asm
	{
		push	0x2C
		push	ecx
		call	CheckValid
		test	eax, eax
		jz		ret_bad

		cmp		dword ptr [eax+0x28], 0
		jle		loc_46B698

		mov		edx, 0x0046B68B
		jmp		edx

loc_46B698:
		mov		edx, 0x0046B698
		jmp		edx

ret_bad:
		xor		eax, eax
		mov		edx, 0x0046C246
		jmp		edx
	}
}

void __declspec(naked) shop_init_2(void)
{
	_asm
	{
		mov	dword ptr [edx+0x164], 0
		mov	dword ptr [edx+0x168], 0
		mov	dword ptr [edx+0x16C], 0
		mov	dword ptr [edx+0x170], 0
		mov	dword ptr [edx+0x174], 0
		mov	dword ptr [edx+0x178], 0
		mov	dword ptr [edx+0x17C], 0
		retn
	}
}

void __declspec(naked) CRASH_fix_4BEE2D()
{
	__asm
	{
		call	shop_init_2
		mov		edx, 0x004BEE34
		jmp		edx
	}
}

void __declspec(naked) CRASH_fix_4AAB7E()
{
	__asm
	{
		test	ecx, ecx
		jz		ret_bad

		mov		edx, [ecx]
		mov		ecx, [ebp-0x114]
		mov		ebx, 0x004AAB86
		jmp		ebx

ret_bad:
		mov		edx, 0x004AAD31
		jmp		edx
	}
}

void __declspec(naked) CRASH_fix_multiwindow_1()
{
	__asm
	{
		push	u_pid_string
		lea		edx, [ebp-0x230]
		push	edx
		mov		edx, 0x0049C15F
		jmp		edx
	}
}

void __declspec(naked) CRASH_fix_multiwindow_2()
{
	__asm
	{
		push	u_pid_string
		lea		ecx, [ebp-0x66C]
		push	ecx
		mov		edx, 0x0049C66D
		jmp		edx
	}
}

void __declspec(naked) CRASH_fix_multiwindow_3()
{
	__asm
	{
		push	u_pid_string
		lea		edx, [ebp-0x288]
		push	edx
		mov		edx, 0x004B7BA0
		jmp		edx
	}
}

void __declspec(naked) CRASH_fix_multiwindow_4()
{
	__asm
	{
		push	u_pid_string_delete
		lea		ecx, [ebp-0x340]
		push	ecx
		mov		edx, 0x0048724A
		jmp		edx
	}
}