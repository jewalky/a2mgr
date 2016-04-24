#include "config.h"

void __declspec(naked) GRID_part1()
{
	__asm
	{
		cmp		byte ptr [r_grid], 0
		jz		gp1_skip

		// horizontal
		push	r_grid_color
		push	[ebp-0x58]
		mov		eax, [ebp-0x24]
		add		eax, 1
		shl		eax, 5
		push	eax
		push	[ebp-0x50]
		mov		eax, [ebp-0x24]
		shl		eax, 5
		push	eax
		mov		edx, 0x0045BA0C
		call	edx
		add		esp, 0x14

		// vertical
		push	r_grid_color
		push	[ebp-0x64]
		mov		eax, [ebp-0x24]
		shl		eax, 5
		push	eax
		push	[ebp-0x50]
		mov		eax, [ebp-0x24]
		shl		eax, 5
		push	eax
		mov		edx, 0x0045BA0C
		call	edx
		add		esp, 0x14

gp1_skip:
		mov		edx, 0x004052E9
		jmp		edx
	}
}

void __declspec(naked) GRID_part2()
{
	__asm
	{
		cmp		byte ptr [r_grid], 0
		jz		gp2_skip

		// horizontal
		push	r_grid_color
		push	[ebp-0x60]
		mov		eax, [ebp-0x40]
		add		eax, 1
		shl		eax, 5
		push	eax
		push	[ebp-0x58]
		mov		eax, [ebp-0x40]
		shl		eax, 5
		push	eax
		mov		edx, 0x0045BA0C
		call	edx
		add		esp, 0x14

		// vertical
		push	r_grid_color
		push	[ebp-0x6C]
		mov		eax, [ebp-0x40]
		shl		eax, 5
		push	eax
		push	[ebp-0x58]
		mov		eax, [ebp-0x40]
		shl		eax, 5
		push	eax
		mov		edx, 0x0045BA0C
		call	edx
		add		esp, 0x14

gp2_skip:
		mov		edx, 0x00405D8B
		jmp		edx
	}
}

void __declspec(naked) GRID_part3()
{
	__asm
	{
		cmp		byte ptr [r_grid], 0
		jz		gp3_skip

		// horizontal
		push	r_grid_color
		push	[ebp-0x6C]
		mov		eax, [ebp-0x20]
		add		eax, 1
		shl		eax, 5
		push	eax
		push	[ebp-0x64]
		mov		eax, [ebp-0x20]
		shl		eax, 5
		push	eax
		mov		edx, 0x0045BA0C
		call	edx
		add		esp, 0x14

		// vertical
		push	r_grid_color
		push	[ebp-0x78]
		mov		eax, [ebp-0x20]
		shl		eax, 5
		push	eax
		push	[ebp-0x64]
		mov		eax, [ebp-0x20]
		shl		eax, 5
		push	eax
		mov		edx, 0x0045BA0C
		call	edx
		add		esp, 0x14

gp3_skip:
		mov		edx, 0x004063B7
		jmp		edx
	}
}

void __declspec(naked) GRID_part4()
{
	__asm
	{
		mov		edx, 0x0045C8CE
		call	edx
		add		esp, 0x30

		cmp		byte ptr [r_grid], 0
		jz		gp4_skip

		// horizontal
		push	r_grid_color
		push	[ebp-0x50]
		mov		eax, [ebp-0x2C]
		add		eax, 1
		shl		eax, 5
		push	eax
		push	[ebp-0x44]
		mov		eax, [ebp-0x2C]
		shl		eax, 5
		push	eax
		mov		edx, 0x0045BA0C
		call	edx
		add		esp, 0x14

		// vertical
		push	r_grid_color
		push	[ebp-0x5C]
		mov		eax, [ebp-0x2C]
		shl		eax, 5
		push	eax
		push	[ebp-0x44]
		mov		eax, [ebp-0x2C]
		shl		eax, 5
		push	eax
		mov		edx, 0x0045BA0C
		call	edx
		add		esp, 0x14

gp4_skip:
		mov		edx, 0x004058F7
		jmp		edx
	}
}

void __declspec(naked) GRID_part5()
{
	__asm
	{
		add		esp, 0x20

		cmp		byte ptr [r_grid], 0
		jz		gp4_skip

		// horizontal
		push	r_grid_color
		push	[ebp-0x50]
		mov		eax, [ebp-0x2C]
		add		eax, 1
		shl		eax, 5
		push	eax
		push	[ebp-0x44]
		mov		eax, [ebp-0x2C]
		shl		eax, 5
		push	eax
		mov		edx, 0x0045BA0C
		call	edx
		add		esp, 0x14

		// vertical
		push	r_grid_color
		push	[ebp-0x5C]
		mov		eax, [ebp-0x2C]
		shl		eax, 5
		push	eax
		push	[ebp-0x44]
		mov		eax, [ebp-0x2C]
		shl		eax, 5
		push	eax
		mov		edx, 0x0045BA0C
		call	edx
		add		esp, 0x14

gp4_skip:
		mov		edx, 0x004058F7
		jmp		edx
	}
}