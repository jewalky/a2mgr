char *aWorld = "world.res";
char *aWorldData = "world\\data\\";
char *aWorldDataDatabin = "world\\data\\data.bin";
char *aWorldDataAireg = "world\\data\\ai.reg";
char *aWorldDataMapreg = "world\\data\\map.reg";
char *aWorldMissionScr = "world\\mission\\scr_";
char *aWorldDataItemnamebin = "world\\data\\itemname.bin";
char *aWorldDataItemnamepkt = "world\\data\\itemname.pkt";
char *aPatch = "patch.res";
char *aPatchPatchtxt = "patch\\patch.txt";

void _declspec(naked) FIL_world0()
{
// 00502C6B
	__asm
	{
		mov	dword ptr [ebp-4], 2
		push	aWorld
		mov	edx, 0x4EB741
		call	edx
		add	esp, 4
		mov	edx, 0x0502C7F
		jmp	edx
	}
}

void _declspec(naked) FIL_world1()
{ // 00486DCD
	__asm
	{
		push	aWorldData
		lea	ecx, [ebp-0x790]
		mov	edx, 0x486DD8
		jmp	edx
	}
}

void _declspec(naked) FIL_world2()
{ // 004F41F2
	__asm
	{
		push	aWorldData
		lea	ecx, [ebp-0x120]
		mov	edx, 0x4F41FD
		jmp	edx
	}
}

void _declspec(naked) FIL_world3()
{ // 00502CF4
	__asm
	{
		push	0x20
		push	aWorldDataDatabin
		mov	edx, 0x0502CFB
		jmp	edx
	}
}

void _declspec(naked) FIL_world4()
{ // 00572315
	__asm
	{
		push	0
		push	aWorldDataAireg
		mov	edx, 0x057231C
		jmp	edx
	}
}

void _declspec(naked) FIL_world5()
{ // 0055E216
	__asm
	{
		push	0
		push	aWorldDataMapreg
		mov	edx, 0x0055E21D
		jmp	edx
	}
}

void _declspec(naked) FIL_world6()
{ // 0055D705
	__asm
	{
		push	0
		push	aWorldDataMapreg
		mov	edx, 0x0055D70C
		jmp	edx
	}
}

void _declspec(naked) FIL_world7()
{ // 0057F722
	__asm
	{
		push	aWorldMissionScr
		mov	eax, [ebp-0x5B4]
		mov	edx, 0x0057F72D
		jmp	edx
	}
}

void _declspec(naked) FIL_world8()
{ // 004867D2
	__asm
	{
		add	esp, 4
		push	aWorld
		mov	edx, 0x004867DA
		jmp	edx
	}
}

void _declspec(naked) FIL_world9()
{ // 00479704
	__asm
	{
		push	0
		push	aWorldDataItemnamebin
		mov	edx, 0x0047970B
		jmp	edx
	}
}

void _declspec(naked) FIL_worldA()
{ // 00479799
	__asm
	{
		push	0
		push	aWorldDataItemnamepkt
		mov	edx, 0x004797A0
		jmp	edx
	}
}

void _declspec(naked) FIL_patch0()
{ // 004867C5
	__asm
	{
		add	esp, 4
		push	aPatch
		mov	edx, 0x04867CD
		jmp	edx
	}
}

void _declspec(naked) FIL_patch1()
{ // 00486D09
	__asm
	{
		push	aPatchPatchtxt
		mov	ecx, 0x0062BA40
		mov	edx, 0x00486D13
		jmp	edx
	}
}