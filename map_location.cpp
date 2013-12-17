/// ме хяонкэгсеряъ дн ксвьху бпел╗м. янбяел.

#include <windows.h>
#include <string>
#include "utils.h"

using namespace std;

char mapFileName[256];

const char* _stdcall get_map_name(const char* oldfile)
{
	string cnewfile = ("maps\\"+string(oldfile));
	memset(mapFileName, 0, 256);
	strncpy(mapFileName, cnewfile.c_str(), 255);

	return mapFileName;
}

void _stdcall move_map_file(const char* oldfile, const char* newfile)
{
	string coldfile = oldfile;
	if(coldfile.find("maps\\") != 0)
		coldfile = "maps\\" + coldfile;

	string cnewfile = newfile;
	if(cnewfile.find("maps\\") != 0)
		cnewfile = "maps\\" + cnewfile;

	DeleteFile(cnewfile.c_str());
	MoveFile(coldfile.c_str(), cnewfile.c_str());
}

const char* _stdcall check_map_file(const char* newfile)
{
	
	return newfile;
}

void __declspec(naked) MAP_loadFromServer()
{
	__asm
	{ // 40E2BA
		mov		eax, [ebp-0x4C]
		add		eax, 0x0A
		push	eax
		push	[ebp-0x48] // existing
		call	move_map_file
		mov		edx, 0x0040E2E0
		jmp		edx
	}
}

void __declspec(naked) MAP_CFileOpen1()
{
	__asm
	{ // 40E0E3
		push	0
		push	1
		push	[ebp-0x48]
		call	get_map_name
		push	eax
		lea		ecx, [ebp-0x5C]
		mov		edx, 0x0040E0F3
		jmp		edx
	}
}

void __declspec(naked) MAP_CFileOpen2()
{
	__asm
	{ // 40E0C9
		push	0
		push	0x1001
		push	[ebp-0x48]
		call	get_map_name
		push	eax
		lea		ecx, [ebp-0x5C]
		mov		edx, 0x0040E0DC
		jmp		edx
	}
}

void __declspec(naked) MAP_CalcCRC()
{
	__asm
	{ // 40DEA4
		mov		ecx, [ebp-0x44]
		add		ecx, 0x10
		push	ecx
		call	check_map_file
		push	eax
		mov		edx, 0x0040DEAB
		jmp		edx
	}
}