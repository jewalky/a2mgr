#include "config.h"
#include <Windows.h>

void __declspec(naked) GUI_setServColumnSize()
{ // 44E9D5
	__asm
	{
		mov		eax, [ebp-0x40]
		cmp		dword ptr [eax+0x03C8], 2
		jnz		loc_44EB70
		push	0x98
		mov		edx, 0x00401840
		call	edx
		mov		[ebp-0x6C], eax
		mov		[ebp-4], 4
		push	0
		push	0x0A
		push	0x005FA5A4
		push	0x005FA5A0
		mov		eax, 0x00629C88
		push	[eax]
		mov		ecx, [ebp-0xBC]
		add		ecx, 8
		mov		edx, 0x00402880
		call	edx
		
		// !!!!
		sub		eax, 0xC0
		// !!!!

		push	eax
		mov		ecx, [ebp-0xBC]
		add		ecx, 8
		mov		edx, 0x00420220
		call	edx

		// !!!!
		sub		eax, 0x40
		// !!!!

		push	eax
		push	0x5C
		push	0x28
		push	1
		mov		ecx, [ebp-0x6C]
		mov		edx, 0x00453F60
		call	edx
		mov		[ebp-0xD0], eax

loc_44EB70:
		mov		edx, 0x0044EF11
		jmp		edx
	}
}

void __declspec(naked) GUI_softcoreSwitch()
{
	__asm
	{
		push	0x03
		mov		ecx, [ebp-0x108]
		mov		edx, 0x004DC183
		call	edx
		mov		[ebp-0x11C], eax

		mov		ebx, [ebp-0x108]
		mov		ebx, [ebx+0x78]
		add		ebx, 0x14
		push	ebx
		mov		edx, [eax]
		mov		ecx, eax
		call	dword ptr [edx+0x3C]
		
		mov		eax, [ebp-0x108]
		mov		eax, [eax+0x78]
		add		eax, 0x14
		
		// the order has changed
		// 0. PvP (hardcore) // was 0
		// 1. PvE (softcore) // was 2
		// 2. Arena          // was 1

		cmp		dword ptr [eax], 2
		jnz		not_arena
		mov		dword ptr [eax], 3
		jmp		cont

not_arena:
		cmp		dword ptr [eax], 1
		jnz		not_pve
		mov		dword ptr [eax], 2
		jmp		cont

not_pve:
		mov		dword ptr [eax], 0

cont:
		mov		edx, 0x0044C9A9
		jmp		edx
	}
}

void __declspec(naked) GUI_softcoreResetSwitch()
{
	__asm
	{
		mov		eax, [ebp-0x18]
		mov		eax, [eax+0x14]
		cmp		eax, 3
		jnz		not_arena
		mov		eax, 2
		jmp		cont

not_arena:
		cmp		eax, 0
		jnz		not_pvp
		cmp		byte ptr [z_softcore], 0
		jz		cont
		mov		eax, 2

not_pvp:
		cmp		eax, 2
		jnz		not_pve
		mov		eax, 1
		jmp		cont

not_pve:
		mov		eax, 0

cont:
		mov		[ebp-0x20], eax
		lea		eax, [ebp-0x20]
		push	eax
		mov		ecx, [ebp-0x10]
		mov		edx, [ecx]
		call	dword ptr [edx+0x44]
		mov		edx, 0x0044CBEC
		jmp		edx
	}
}

unsigned long z_gamemode = 0;

void __declspec(naked) GUI_softcoreSaveSwitch()
{
	__asm
	{
		push	[ebp-0x10]
		
		mov		eax, [ebp-0x124]
		add		eax, 0x14
		mov		ebx, dword ptr [eax]
		mov		dword ptr [z_gamemode], ebx
		lea		eax, z_gamemode
		cmp		ebx, 0
		jnz		not_pvp
		cmp		byte ptr [z_softcore], 0
		jz		not_pvp
		mov		dword ptr [z_gamemode], 2

not_pvp:
		push	eax
		push	4
		push	0
		push	0x005FA6A4 // offset aDeathmatch // "deathmatch"
		mov		ecx, [ebp+0x08]
		push	[ecx]
		call	ds:RegSetValueExA

		mov		edx, 0x0043FA64
		jmp		edx
	}
}