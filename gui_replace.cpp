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

// write registry from switch option, remap
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
		// 2. Sandbox		 // new
		// 3. Arena          // was 1

		cmp		dword ptr [eax], 2
		jnz		not_sand
		mov		dword ptr [eax], 4
		jmp		cont

not_sand:
		cmp		dword ptr [eax], 3
		jnz		not_arena
		jmp		cont
		//mov		dword ptr [eax], 3
		//jmp		cont

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

// read from registry, remap
void __declspec(naked) GUI_softcoreResetSwitch()
{
	__asm
	{
		mov		eax, [ebp-0x18]
		mov		eax, [eax+0x14]

		cmp		byte ptr [z_softcore], 1
		jnz		sc1_no
		mov		eax, 2
		jmp		sc2_no
sc1_no:
		cmp		byte ptr [z_softcore], 2
		jnz		sc2_no
		mov		eax, 4
sc2_no:

		cmp		eax, 3
		jnz		not_arena
		//mov		eax, 2
		jmp		cont

not_arena:
		cmp		eax, 0
		jnz		not_pvp
		jmp		cont // 0 is 0
		/*cmp		byte ptr [z_softcore], 0
		jz		cont
		mov		eax, 2*/

not_pvp:
		cmp		eax, 2
		jnz		not_pve
		mov		eax, 1
		jmp		cont

not_pve:
		cmp		eax, 4
		jnz		not_sandbox
		mov		eax, 2
		jmp		cont

not_sandbox:
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
		cmp		byte ptr [z_softcore], 2
		jnz		not_sand
		mov		dword ptr [z_gamemode], 4
		jmp		not_pvp

not_sand:
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

void __declspec(naked) GUI_sandboxSwitch()
{
	__asm
	{ // 44CB2D
		push	0
		push	0x005FA5A0
		mov		ecx, 0x00629C88
		push	[ecx]
		push	0xDC // bottom
		mov		edx, [ebp+0x0C]
		push	[edx+8] // right
		push	0x7C
		mov		edx, [ebp+0x0C]
		push	[edx] // left
		push	3
		mov		ecx, [ebp-0x2C]
		mov		edx, 0x004536F0
		call	edx
		mov		[ebp-0x6C], eax
		mov		[ebp-0x10], eax
		mov		[ebp-0x30], eax

		push	0x68 // hardcore
		mov		ecx, 0x0062BA40
		mov		edx, 0x00479BA3
		call	edx
		push	eax
		mov		ecx, [ebp-0x10]
		mov		edx, 0x00452B30
		call	edx

		push	0xD5 // softcore
		mov		ecx, 0x0062BA40
		mov		edx, 0x00479BA3
		call	edx
		push	eax
		mov		ecx, [ebp-0x10]
		mov		edx, 0x00452B30
		call	edx

		push	0xED // sandbox
		mov		ecx, 0x0062BA40
		mov		edx, 0x00479BA3
		call	edx
		push	eax
		mov		ecx, [ebp-0x10]
		mov		edx, 0x00452B30
		call	edx

		push	0x6D // arena
		mov		ecx, 0x0062BA40
		mov		edx, 0x00479BA3
		call	edx
		push	eax
		mov		ecx, [ebp-0x10]
		mov		edx, 0x00452B30
		call	edx

		mov		edx, 0x0044CBBB
		jmp		edx
	}
}

void __declspec(naked) GUI_removeServerList()
{
	__asm
	{ // 44CC1E
		push	0
		push	0x005FA5A0
		mov		ecx, 0x00629C88
		push	[ecx]
		//push	0xF0
		push	0x80FFFFC0 // bottom; minus something (a lot)
		mov		edx, [ebp+0x0C]
		push	[edx+8]
		push	0x80FFFFF0 // top
		mov		edx, [ebp+0x0C]
		push	[edx]
		push	4
		mov		ecx, [ebp-0x34]
		mov		edx, 0x004536F0
		call	edx
		mov		[ebp-0x70], eax
		mov		[eax+0x18], 0 // disabled
		mov		edx, 0x0044CC51
		jmp		edx
	}
}