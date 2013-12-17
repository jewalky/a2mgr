#include <cstring>

char* _stdcall GetFirstName(unsigned long dch)
{
	char* longname = (char*)(*(unsigned long*)(dch + 0x14) + 0x14);
	if(strstr(longname, "Administrator ") == longname) return "Administrator";
	if(strstr(longname, "GameMaster ") == longname) return "GameMaster";
	if(strstr(longname, "TournamentMaster ") == longname) return "TournamentMaster";
	return "";
}

char* _stdcall GetSecondName(unsigned long dch)
{
	int position = 0;
	char* longname = (char*)(*(unsigned long*)(dch + 0x14) + 0x14);
	if(strstr(longname, "Administrator ") == longname) position = 14;
	else if(strstr(longname, "GameMaster ") == longname) position = 11;
	else if(strstr(longname, "TournamentMaster ") == longname) position = 17;
	return (longname + position);
}

bool _stdcall CheckNickname(unsigned long dunit)
{
	char* longname = (char*)(*(unsigned long*)(dunit + 0x14) + 0x14);
	if(*(char*)(dunit + 0xF8) != 0) return false;
	if((strstr(longname, "Administrator ") == longname) ||
	   (strstr(longname, "GameMaster ") == longname) ||
	   (strstr(longname, "TournamentMaster ") == longname)) return true;
	return false;
}

// (+ 14) + 14 = nickname

void __declspec(naked) ADMINISTRAT()
{
	__asm
	{
		push	[ebp-0x58]
		call	CheckNickname
		test	eax, eax
		jz		def_proc

		mov		edx, [ebp-0x58]
		mov		eax, [edx+0x14]
		mov		ecx, [eax+0x0C]
		shl		ecx, 5
		add		ecx, 0x00629C90
		push	ecx
		push	2
		push	[ebp-0x58]
		call	GetSecondName
		push	eax
		mov		eax, [ebp-0x1C]
		sub		eax, 0x0F
		push	eax
		mov		ecx, [ebp-0x18]
		add		ecx, [ebp-0x20]
		sar		ecx, 1
		push	ecx
		mov		edx, 0x0062A6C8
		mov		ecx, [edx]
		mov		edx, [ecx]
		call	dword ptr [edx+0x14]

		mov		edx, [ebp-0x58]
		mov		eax, [edx+0x14]
		mov		ecx, [eax+0x0C]
		shl		ecx, 5
		add		ecx, 0x00629C90
		push	ecx
		push	2
		push	[ebp-0x58]
		call	GetFirstName
		push	eax
		mov		eax, [ebp-0x1C]
		sub		eax, 0x19
		push	eax
		mov		ecx, [ebp-0x18]
		add		ecx, [ebp-0x20]
		sar		ecx, 1
		push	ecx
		mov		edx, 0x0062A6C8
		mov		ecx, [edx]
		mov		edx, [ecx]
		call	dword ptr [edx+0x14]

		mov		edx, 0x0046B1BC
		jmp		edx

def_proc:
		mov		eax, [ebp-0x58]
		mov		ecx, [eax+0x14]
		mov		edx, [ecx+0x0C]
		shl		edx, 5
		add		edx, 0x00629C90
		push	edx

		mov		edx, 0x0046B191
		jmp		edx
	}
}