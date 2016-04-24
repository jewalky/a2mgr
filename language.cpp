#include "config.h"
#include <cstdlib>
#include "utils.h"
#include <windows.h>

char* aMainTextCredit = NULL;
char* aMainTextGlobal = NULL;
char* aMainTextHelp_t = NULL;
char* aMainTextMain_t = NULL;
char* aMainTextHeropi = NULL;
char* aMainTextStats_ = NULL;
char* aMainTextSpells = NULL;
char* aMainTextSpell_ = NULL;
char* aMainTextDialog = NULL;
char* aMainTextUnitna = NULL;
char* aMainTextBuildi = NULL;
char* aMainTextItemna = NULL;
char* aMainTextNpcnam = NULL;
char* aMainTextCutsce = NULL;
char* aMainTextCutpat = NULL;
char* aMainTextTunes_ = NULL;
char* aPatchPatch_txt = NULL;
char* aMainTextTown_t = NULL;
char* aMainTextMissio = NULL;
char* aMainTextQuest_ = NULL;
char* aMainTextDocsD_ = NULL;

char* aMainGraphicsCh = NULL;
char* aMainGraphicsLo = NULL;
char* aMainGraphicsFa = NULL;
char* aMainGraphicsGl = NULL;
char* aMainGraphics_0 = NULL;
char* aMainGraphics_1 = NULL;
char* aMainGraphicsMa = NULL;
char* aMainGraphics_2 = NULL;
char* aMainGraphics_3 = NULL;
char* aMainGraphics_4 = NULL;
char* aMainGraphics_5 = NULL;

void DecideLanguage()
{
	if(!z_lang_override)
	{
		LANGID lid = GetSystemDefaultUILanguage();
		if(lid == 0x419) z_russian = true;
		else z_russian = false;
	}

	if(z_russian)
	{
		aMainTextCredit = "locale\\ru\\credits.txt";
		aMainTextGlobal = "locale\\ru\\globalmap.txt";
		aMainTextHelp_t = "locale\\ru\\help.txt";
		aMainTextMain_t = "locale\\ru\\main.txt";
		aMainTextHeropi = "locale\\ru\\heropicture.txt";
		aMainTextStats_ = "locale\\ru\\stats.txt";
		aMainTextSpells = "locale\\ru\\spells.txt";
		aMainTextSpell_ = "locale\\ru\\spell.txt";
		aMainTextDialog = "locale\\ru\\dialogs.txt";
		aMainTextUnitna = "locale\\ru\\unitname.txt";
		aMainTextBuildi = "locale\\ru\\building.txt";
		aMainTextItemna = "locale\\ru\\itemname.txt";
		aMainTextNpcnam = "locale\\ru\\npcnames.txt";
		aMainTextCutsce = "locale\\ru\\cutscene.txt";
		aMainTextCutpat = "locale\\ru\\cutpaths.txt";
		aMainTextTunes_ = "locale\\ru\\tunes.txt";
		aPatchPatch_txt = "locale\\ru\\patch.txt";
		aMainTextTown_t = "locale\\ru\\town.txt";
		aMainTextMissio = "locale\\ru\\mission%d.txt";
		aMainTextQuest_ = "locale\\ru\\quest.txt";
		aMainTextDocsD_ = "locale\\ru\\docs\\%d.txt";

		aMainGraphicsCh = "locale\\ru\\graphics\\chrgen\\leftup.bmp";
		aMainGraphicsLo = "locale\\ru\\graphics\\logo\\";
		aMainGraphicsFa = "locale\\ru\\graphics\\famehall\\hall.bmp";
		aMainGraphicsGl = "locale\\ru\\graphics\\Global.Map\\Umoir.bmp";
		aMainGraphics_0 = "locale\\ru\\graphics\\Global.Map\\GMap.bmp";
		aMainGraphics_1 = "locale\\ru\\graphics\\logo\\allods.bmp";
		aMainGraphicsMa = "locale\\ru\\graphics\\MainMenu\\MenuMask.bmp";
		aMainGraphics_2 = "locale\\ru\\graphics\\MainMenu\\menu_.bmp";
		aMainGraphics_3 = "locale\\ru\\graphics\\MainMenu\\button%up.bmp";
		aMainGraphics_4 = "locale\\ru\\graphics\\MainMenu\\button%u.bmp";
		aMainGraphics_5 = "locale\\ru\\graphics\\MainMenu\\text%u.bmp";
	}
	else
	{
		aMainTextCredit = "locale\\en\\credits.txt";
		aMainTextGlobal = "locale\\en\\globalmap.txt";
		aMainTextHelp_t = "locale\\en\\help.txt";
		aMainTextMain_t = "locale\\en\\main.txt";
		aMainTextHeropi = "locale\\en\\heropicture.txt";
		aMainTextStats_ = "locale\\en\\stats.txt";
		aMainTextSpells = "locale\\en\\spells.txt";
		aMainTextSpell_ = "locale\\en\\spell.txt";
		aMainTextDialog = "locale\\en\\dialogs.txt";
		aMainTextUnitna = "locale\\en\\unitname.txt";
		aMainTextBuildi = "locale\\en\\building.txt";
		aMainTextItemna = "locale\\en\\itemname.txt";
		aMainTextNpcnam = "locale\\en\\npcnames.txt";
		aMainTextCutsce = "locale\\en\\cutscene.txt";
		aMainTextCutpat = "locale\\en\\cutpaths.txt";
		aMainTextTunes_ = "locale\\en\\tunes.txt";
		aPatchPatch_txt = "locale\\en\\patch.txt";
		aMainTextTown_t = "locale\\en\\town.txt";
		aMainTextMissio = "locale\\en\\mission%d.txt";
		aMainTextQuest_ = "locale\\en\\quest.txt";
		aMainTextDocsD_ = "locale\\en\\docs\\%d.txt";

		aMainGraphicsCh = "locale\\en\\graphics\\chrgen\\leftup.bmp";
		aMainGraphicsLo = "locale\\en\\graphics\\logo\\";
		aMainGraphicsFa = "locale\\en\\graphics\\famehall\\hall.bmp";
		aMainGraphicsGl = "locale\\en\\graphics\\Global.Map\\Umoir.bmp";
		aMainGraphics_0 = "locale\\en\\graphics\\Global.Map\\GMap.bmp";
		aMainGraphics_1 = "locale\\en\\graphics\\logo\\allods.bmp";
		aMainGraphicsMa = "locale\\en\\graphics\\MainMenu\\MenuMask.bmp";
		aMainGraphics_2 = "locale\\en\\graphics\\MainMenu\\menu_.bmp";
		aMainGraphics_3 = "locale\\en\\graphics\\MainMenu\\button%up.bmp";
		aMainGraphics_4 = "locale\\en\\graphics\\MainMenu\\button%u.bmp";
		aMainGraphics_5 = "locale\\en\\graphics\\MainMenu\\text%u.bmp";
	}
}

void __declspec(naked) LANG_change_43EDEB()
{
	__asm
	{
		push	aMainTextCredit
		mov		ecx, [ebp-0x30]
		add		ecx, 0x84
		mov		edx, 0x004799F5
		call	edx
		mov		[ebp-0x10], 0
		mov		edx, 0x0043EE10
		jmp		edx
	}
}

void __declspec(naked) LANG_change_474F27()
{
	__asm
	{
		push	aMainTextGlobal
		mov		edx, 0x0047961D
		call	edx
		add		esp, 8
		mov		edx, 0x00474F34
		jmp		edx
	}
}

void __declspec(naked) LANG_change_48626D()
{
	__asm
	{
		push	aMainTextHelp_t
		mov		edx, 0x0047961D
		call	edx
		add		esp, 8

		push	aMainTextMain_t
		mov		ecx, 0x0062C8F8
		mov		edx, 0x004799F5
		call	edx

		push	aMainTextHeropi
		mov		ecx, 0x0062B5F8
		mov		edx, 0x004799F5
		call	edx

		push	aMainTextStats_
		mov		ecx, 0x0062B608
		mov		edx, 0x004799F5
		call	edx

		push	aMainTextSpells
		mov		ecx, 0x0062C7B8
		mov		edx, 0x004799F5
		call	edx

		push	aMainTextSpell_
		mov		ecx, 0x0062C8A8
		mov		edx, 0x004799F5
		call	edx

		push	aMainTextDialog
		mov		ecx, 0x0062BA50
		mov		edx, 0x004799F5
		call	edx

		push	aMainTextUnitna
		mov		ecx, 0x0062C8B8
		mov		edx, 0x004799F5
		call	edx

		push	aMainTextBuildi
		mov		ecx, 0x0062C838
		mov		edx, 0x004799F5
		call	edx

		push	aMainTextItemna
		mov		ecx, 0x0062C7A8
		mov		edx, 0x004799F5
		call	edx

		push	aMainTextNpcnam
		mov		ecx, 0x0062BA30
		mov		edx, 0x004799F5
		call	edx

		push	aMainTextCutsce
		mov		ecx, 0x0062C828
		mov		edx, 0x004799F5
		call	edx

		push	aMainTextCutpat
		mov		ecx, 0x0062C878
		mov		edx, 0x004799F5
		call	edx

		push	aMainTextTunes_
		mov		ecx, 0x0062C990
		mov		edx, 0x004799F5
		call	edx

		push	aPatchPatch_txt
		mov		ecx, 0x0062BA40
		mov		edx, 0x004799F5
		call	edx

		mov		edx, 0x00486D18
		jmp		edx
	}
}

void __declspec(naked) LANG_change_48BA07()
{
	__asm
	{
		push	aMainTextTown_t
		mov		edx, 0x0047961D
		call	edx
		add		esp, 8
		mov		edx, 0x0048BA14
		jmp		edx
	}
}

void __declspec(naked) LANG_change_48CA55()
{
	__asm
	{
		push	aMainTextTown_t
		mov		edx, 0x0047961D
		call	edx
		add		esp, 8
		mov		edx, 0x0048CA62
		jmp		edx
	}
}

void __declspec(naked) LANG_change_491CDF()
{
	__asm
	{
		push	aMainTextTown_t
		mov		edx, 0x0047961D
		call	edx
		add		esp, 8
		mov		edx, 0x00491CEC
		jmp		edx
	}
}

void __declspec(naked) LANG_change_4939A2()
{
	__asm
	{
		push	aMainTextMissio
		lea		eax, [ebp-0x60]
		push	eax
		mov		edx, 0x004939AB
		jmp		edx
	}
}

void __declspec(naked) LANG_change_493B23()
{
	__asm
	{
		push	aMainTextQuest_
		mov		edx, 0x0047961D
		call	edx
		add		esp, 8
		mov		edx, 0x00493B30
		jmp		edx
	}
}

void __declspec(naked) LANG_change_4B018F()
{
	__asm
	{
		push	aMainTextDocsD_
		lea		ecx, [ebp-0x40C]
		push	ecx
		mov		edx, 0x004B019B
		jmp		edx
	}
}

void __declspec(naked) LANG_change_42B3D5()
{
	__asm
	{
		push	aMainGraphicsCh
		mov		ecx, [ebp-0x14]
		mov		edx, 0x0042B3DD
		jmp		edx
	}
}

void __declspec(naked) LANG_change_43EEC3()
{
	__asm
	{
		push	aMainGraphicsLo
		lea		ecx, [ebp-0x24]
		push	ecx
		mov		edx, 0x0043EECC
		jmp		edx
	}
}

void __declspec(naked) LANG_change_460F9B()
{
	__asm
	{
		push	aMainGraphicsFa
		mov		ecx, [ebp-0x10]
		mov		edx, 0x00460FA3
		jmp		edx
	}
}

void __declspec(naked) LANG_change_474375()
{
	__asm
	{
		push	aMainGraphicsGl
		mov		ecx, [ebp-0x10]
		mov		edx, 0x0047437D
		jmp		edx
	}
}

void __declspec(naked) LANG_change_4743C9()
{
	__asm
	{
		push	aMainGraphics_0
		mov		ecx, [ebp-0x18]
		mov		edx, 0x004743D1
		jmp		edx
	}
}

void __declspec(naked) LANG_change_496638()
{
	__asm
	{
		push	aMainGraphics_1
		mov		ecx, [ebp-0x34]
		mov		edx, 0x00496640
		jmp		edx
	}
}

void __declspec(naked) LANG_change_4AEE20()
{
	__asm
	{
		push	aMainGraphicsMa
		mov		ecx, [ebp-0x1C]
		mov		edx, 0x004AEE28
		jmp		edx
	}
}

void __declspec(naked) LANG_change_4AEE6D()
{
	__asm
	{
		push	aMainGraphics_2
		mov		ecx, [ebp-0x24]
		mov		edx, 0x004AEE75
		jmp		edx
	}
}

void __declspec(naked) LANG_change_4AEF2A()
{
	__asm
	{
		push	aMainGraphics_3
		lea		eax, [ebp-0x10]
		push	eax
		mov		edx, 0x004AEF33
		jmp		edx
	}
}

void __declspec(naked) LANG_change_4AEF9C()
{
	__asm
	{
		push	aMainGraphics_4
		lea		edx, [ebp-0x10]
		push	edx
		mov		edx, 0x004AEFA5
		jmp		edx
	}
}

void __declspec(naked) LANG_change_4AF00E()
{
	__asm
	{
		push	aMainGraphics_5
		lea		ecx, [ebp-0x10]
		push	ecx
		mov		edx, 0x004AF017
		jmp		edx
	}
}

void __declspec(naked) LANG_fix_codepage()
{
	__asm
	{
		mov		eax, 0x0062C9C4
		mov		[eax], 1
		push	0x10
		mov		edx, 0x00479D82
		jmp		edx
	}
}