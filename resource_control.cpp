#include "utils.h"

char aPatchC[] = "patch\\";

char aLocale_res[] = "locale.res";

bool _stdcall TestGraphics(const char* file)
{
	return false;
	std::string filename = TruncateSlashes(FixSlashes(ToLower(file)));
	if(((filename.find("graphics/") == 0) ||
		(filename.find("main/") == 0) ||
		(filename.find("patch/") == 0) ||
		(filename.find("world/") == 0) ||
		(filename.find("locale/") == 0)) && FileExists(filename)) return true;

	return false;
}

int __declspec(naked) RES_loadFile(int x)
/* измененная процедура загрузки ресурсов;
 * сначала пытаемся загрузить файл из patch11.res
 * потом из a25graph.res, если он подгружен,
 * после из оригинального ресурса */
// ZZYZX EDIT: a25graph.res не используется.
{
	__asm
	{ // 004EBEDA
		call	sub_4EB99D

		mov		[ebp-0x814], eax
		cmp		eax, 0
		jz		loc_4EBF9B
		mov		edx, 0x004EBEEE
		jmp		edx
loc_4EBF9B:
		mov		edx, 0x004EBF9B
		jmp		edx

sub_4EB99D:
		push	ebp
		mov		ebp, esp

		sub		esp, 8
		mov		[ebp-4], ecx

		push	[ebp+8]
		call	TestGraphics
		and		eax, 0xFF
		test	eax, eax
		jnz		loc_retzero

		push	0x200
		mov		edx, 0x0058CA80
		call	edx	/// malloc
		add		esp, 4
		mov		[ebp-8], eax

		push	offset aPatchC
		push	eax
		mov		edx, 0x0058CDF0
		call	edx	// strcpy
		add		esp, 8

		push	dword ptr [ebp+8]
		push	dword ptr [ebp-8]
		mov		edx, 0x0058CE00 // strcat
		call	edx
		add		esp, 8

		push	dword ptr [ebp-8]
		mov		ecx, [ebp-4]
		mov		edx, 0x004EB99D
		call	edx
		test	eax, eax
		jnz		en

		push	dword ptr [ebp+8]
		mov		ecx, [ebp-4]
		mov		edx, 0x004EB99D
		call	edx

en:

		mov		[ebp-4], eax

		push	dword ptr [ebp-8]
		mov		edx, 0x0058CB50
		call	edx /// free
		add		esp, 4

		mov		eax, [ebp-4]

		mov		esp, ebp
		pop		ebp
		retn	4

loc_retzero:
		mov		eax, 0
		
		mov		esp, ebp
		pop		ebp
		retn	4
	}
}

void __declspec(naked) RES_loadException()
{ // 004EB723
	__asm
	{
		mov	edx, 0x004EB72E
		jmp	edx
	}
}

void __declspec(naked) RES_loadDirectories()
{ // 004867E6
	__asm
	{
		push	offset aLocale_res
		mov		edx, 0x004EB741
		call	edx
		add		esp, 4

		mov		byte ptr [ebp-0x04], 4

		push	0x005FD168 // offset aMusic_res; "music.res"
		mov		edx, 0x004EB741
		call	edx
		add		esp, 4

		mov		edx, 0x0048681A
		jmp		edx
	}
}