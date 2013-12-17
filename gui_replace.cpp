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