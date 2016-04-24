#include "config.h"

unsigned long lastTime;
unsigned long delta;

void __declspec(naked) SCROLL_part1()
{
	__asm
	{

		mov		edx, 0x006DC0B0
		mov		edx, [edx]
		call	edx // timeGetTime

		sub		eax, lastTime
		mov		delta, eax

		cmp		eax, g_scrollintv
		jnb		ssp1_skip_1

		mov		edx, 0x0040712C
		jmp		edx

ssp1_skip_1:
		mov		ecx, [ebp-0x2F4]
		mov		eax, [ecx+0x78]
		or		eax, [ecx+0x7C]
		test	eax, eax
		jnz		ssp1_skip_2

		mov		edx, 0x004070FC
		jmp		edx

ssp1_skip_2:
		mov		eax, delta
		add		lastTime, eax

		mov		edx, 0x4070FC
		mov		ecx, [ebp-0x2F4]
		jmp		edx
	}
}

void __declspec(naked) SCROLL_part2()
{
	__asm
	{
		mov		eax, delta
		cmp		eax, g_scrollintv
		jnb		ssp2_skip

		mov		edx, 0x004074DB
		jmp		edx

ssp2_skip:
		mov		edx, [ebp-0x2F4]
		mov		ecx, 0x004073CF
		jmp		ecx
	}
}
