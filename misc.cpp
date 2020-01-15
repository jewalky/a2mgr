#include <cstring>
#include "zxmgr.h"
#include "lib/utils.hpp"
#include "a2mgr.h"

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

void __stdcall PreprocessItemName(unsigned char* item, const char* name)
{
	char* target = (char*)0x00624FB0;

	unsigned short itemId = *(unsigned short*)(item + 6);
	bool isSpecial = ((itemId & 0xFE00) == 0xE00);

	target[0] = 0;
	int magiclore = 0;
	// add stars
	unsigned char cnt = *(unsigned char*)(item + 0x09);
	unsigned char* stats = *(unsigned char**)(item + 0x0C);
	bool isMagic = false;
	bool price2 = false;
	for (int i = 0; i < cnt; i++)
	{
		int effect1 = *stats++;
		int effect2;
		int value1;
		int value2;
		// apparently each stat is packed differently.
		// this is so retarded...
		switch (effect1)
		{
		case 0x01:
			value1 = *(int*)(stats);
			stats += 4;
			break;
		case 0x0D: // damage X-Y
		case 0x2C:
		case 0x2D:
		case 0x2E:
		case 0x2F:
		case 0x30:
			value1 = *stats++;
			effect2 = *stats++;
			value2 = *stats++;
			i++;
			break;
		default:
			value1 = *stats++;
			break;
		}

		if (effect1 == 39)
		{
			magiclore = value1;
		}
		else if (effect1 == 0x33)
		{
			isMagic = true;
		}
		else if (effect1 == 1)
		{
			if (value1 == 2)
				price2 = true;
		}
	}

	if (magiclore < 0)
		magiclore = 0;

	int cr = -1;
	int cg = -1;
	int cb = -1;

	std::string colorstr = "";

	// logic:
	// has magic = 58 135 173
	// has magic and price 2 = 192 152 83
	// magiclore=1 = 185 74 72
	// magiclore=2 = 50 205 50
	// magiclore=3 = 255 0 255
	if (!isSpecial && isMagic)
	{
		if (magiclore == 1)
		{
			cr = 215; cg = 43; cb = 43;
		}
		else if (magiclore == 2)
		{
			cr = 50; cg = 205; cb = 50;
		}
		else if (magiclore >= 3)
		{
			cr = 255; cg = 0; cb = 255;
		}
		else if (price2)
		{
			cr = 84; cg = 82; cb = 255;
		}
		else
		{
			cr = 192; cg = 152; cb = 83;
		}
	}

	if (cr >= 0 && cg >= 0 && cb >= 0)
	{
		colorstr = Format("%%[%d,%d,%d]", cr, cg, cb);
	}

	strcat(target, colorstr.c_str());

	for (int i = 0; i < magiclore; i++)
		target[i+colorstr.length()] = 0xC1;

	target[magiclore+colorstr.length()] = 0;

	strcat(target, name);

	strcat(target, "%[*]");
}

// idea: don't display "magic:" label if all stats were hidden (i.e. all item has is magiclore)
bool _stdcall CheckRemoveMagic(unsigned char* item)
{
	int visibleStats = 0;
	bool reachedMagic = false;

	unsigned char cnt = *(unsigned char*)(item + 0x09);
	unsigned char* stats = *(unsigned char**)(item + 0x0C);
	for (int i = 0; i < cnt; i++)
	{

		int effect1 = *stats++;
		int effect2;
		int value1;
		int value2;
		// apparently each stat is packed differently.
		// this is so retarded...
		switch (effect1)
		{
		case 0x01:
			value1 = *(int*)(stats);
			stats += 4;
			break;
		case 0x0D: // damage X-Y
		case 0x2C:
		case 0x2D:
		case 0x2E:
		case 0x2F:
		case 0x30:
			value1 = *stats++;
			effect2 = *stats++;
			value2 = *stats++;
			i++;
			break;
		default:
			value1 = *stats++;
			break;
		}

		if (effect1 == 0x33) // magic:
		{
			reachedMagic = true;
			continue;
		}

		if (effect1 == 39) // magic lore
			continue;

		if (reachedMagic)
			visibleStats++;

	}

	return visibleStats > 0;
}

void __declspec(naked) ITEM_changeItemName()
{
	// @ 43BCE4
	__asm
	{
		push	[ebp-0xA0]
		push	[ebp-0xDC]
		call	PreprocessItemName
		mov		eax, 0x0043BCF8
		jmp		eax
	}
}

void __declspec(naked) ITEM_checkRemoveMagic()
{
	// @ 43C501
	__asm
	{
		push	[ebp - 0xDC]
		call	CheckRemoveMagic
		test	eax, eax
		jz		icrm_skipAdd

		push	0x005FA41C
		push	0x00624FB0
		mov		edx, 0x0058CE00 // strcat
		call	edx
		add		esp, 8

		mov		edx, 0x0043C513
		jmp		edx

icrm_skipAdd:
		mov		edx, 0x0043C532
		jmp		edx
	}
}

void _stdcall InitSDL()
{
	TryInitSDL();
}

void __declspec(naked) imp_InitSDL()
{
	// @ 5B44BA
	__asm
	{
		push	eax
		push	ebx
		push	ecx
		push	edx
		push	esi
		push	edi

		call	InitSDL

		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax

		mov		eax, [esi]
		mov		ecx, esi
		call	dword ptr [eax+0x8C]

		push	0x005B44C4
		ret
	}
}