// draw spb1024l to the left of spellbook, instead of left border
void __declspec(naked) HIRES_fixSpellbook()
{
	// @ 4CECDB
	__asm
	{
		push	eax

		push	0
		mov		ecx, 0x00630E24
		mov		ecx, [ecx]
		mov		edx, [ecx]
		call	[edx + 0x20]
		mov		ebx, eax
		pop		eax

		push	eax
		push	0
		push	0
		push	[ebp-0x10]
		mov		eax, [ebp-0x70]
		sub		eax, ebx
		push	eax
		mov		ecx, 0x00630E24
		mov		ecx, [ecx]
		mov		edx, [ecx]
		call	[edx+0x38]

		mov		edx, 0x004CECF9
		jmp		edx
	}
}

// draw textbackl at the bottom of the screen, matching how the actual textback is drawn
void __declspec(naked) HIRES_fixTextback()
{
	// @ 40A919
	__asm
	{
		// I don't know what's this value, but it matches the location of spellbook
		mov		edx, 0x00401870
		call	edx
		mov		eax, [eax+0xD0]
		add		eax, 8
		mov		ecx, [eax+0x0C]
		sub		ecx, 0xF2

		push	0xF2 // 242
		push	0x10 // 16
		push	0
		push	0
		push	ecx
		mov		eax, [ebp - 0x40]
		sub		eax, 0x10
		push	eax
		mov		ecx, 0x00630DD0
		mov		ecx, [ecx]
		mov		edx, [ecx]
		call	[edx+0x38]

		mov		edx, 0x0040A9A1
		jmp		edx
	}
}

// draw textbackr according to the map spacing
void __declspec(naked) HIRES_fixTextback2()
{
	// @ 4889E9
	__asm
	{
		// construct textbackr with correct alignment
		mov		edx, 0x00401870
		call	edx
		mov		eax, [eax + 0xD0]
		add		eax, 8
		mov		eax, [eax + 0x0C]
		push	eax
		push	0xA0
		push	0x1E0
		push	0
		push	9
		mov		ecx, [ebp-0x40]
		mov		edx, 0x004B8974
		call	edx
		mov		[ebp-0x114], eax

		mov		edx, 0x00488A18
		jmp		edx
	}
}

// draw map to the far right
void __declspec(naked) HIRES_extendMap()
{
	// @ 4887ED
	__asm
	{
		mov		edx, 0x0062B5F4
		push	[edx]
		mov		edx, 0x0062B5F0
		push	[edx]
		push	0
		push	0
		mov		ecx, [ebp-0x18]
		mov		edx, 0x00402AF6
		call	edx
		mov		[ebp-0x100], eax
		mov		edx, 0x0048881D
		jmp		edx
	}
}