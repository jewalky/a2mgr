void __declspec(naked) SYSTEM_setThreadPriority()
{
	__asm retn 8;
}

static char aAllods2[] = "\\Allods2.eu";
void __declspec(naked) REG_allods2eu()
{
	// @ 4863F6
	__asm
	{
		push	offset aAllods2
		lea		ecx, [ebp-0x740]
		push	ecx
		mov		ecx, 0x00486402
		jmp		ecx
	}
}