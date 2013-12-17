#include <vector>
#include <string>
#include "client_cmd.h"
#include "utils.h"
#include "zxmgr.h"
#include "lib\utils.hpp"

using namespace std;

vector< vector<string> > __chat_history;
unsigned long __chat_history_index = 0;

void __declspec(naked) __cdecl str_empty(char** cstr)
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		mov		ecx, [ebp+0x08]
		mov		edx, 0x005AB7D5
		call	edx
		mov		esp, ebp
		pop		ebp
		ret
	}
}


void __declspec(naked) __cdecl str_assign(char** cstr, const char* str)
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		mov		ecx, [ebp+0x08]
		push	dword ptr [ebp+0x0C]
		mov		edx, 0x005AB9E0
		call	edx
		mov		esp, ebp
		pop		ebp
		ret
	}
}

void __declspec(naked) __cdecl str_array_resize(char* pthis, unsigned long size)
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		mov		ecx, [ebp+0x08]
		push	0xFFFFFFFF
		push	dword ptr [ebp+0x0C]
		mov		edx, 0x005A8C4A
		call	edx
		mov		esp, ebp
		pop		ebp
		ret
	}
}

void addString(char* pthis)
{
	if(!pthis) return;
	char* arrptr = (char*)(pthis+0x70);
	if(!arrptr) return;

	unsigned long str_count = *(unsigned long*)(arrptr+0x08);
	if(str_count)
	{
		vector<string> strings;
		unsigned long* stringsa = *(unsigned long**)(arrptr+0x04);
		for(int i = 0; i < str_count; i++)
		{
			char* stringc = (char*)stringsa[i];
			strings.push_back(stringc);
		}
		__chat_history.push_back(strings);
		__chat_history_index = __chat_history.size();
	}
}

unsigned long setString(char* pthis)
{
	if(!pthis) return 0;
	char* arrptr = (char*)(pthis+0x70);
	if(!arrptr) return 0;

	vector<string> &strings = __chat_history[__chat_history_index];

	unsigned long str_count = strings.size();
	if(str_count)
	{
		str_array_resize(arrptr, str_count-1);
		unsigned long* stringsa = *(unsigned long**)(arrptr+0x04);
		for(int i = 0; i < str_count-1; i++)
			stringsa[i] = (unsigned long)_strdup(strings[i].c_str());

		char** cstr = (char**)(pthis+0x84);
		str_empty(cstr);
		str_assign(cstr, _strdup(strings[strings.size()-1].c_str()));
	}
	return str_count;
}

unsigned long _stdcall consoleDown(char* pthis)
{
	if(!pthis) return 0;
	char* arrtptr = *(char**)(pthis+0x168);
	if(!arrtptr) return 0;

	char** cstr = (char**)(pthis+0x84);
	if(!cstr) return 0;

	if(__chat_history_index < __chat_history.size()-1)
	{
		__chat_history_index++;
		return setString(arrtptr);
	}
	return 0;
}

unsigned long _stdcall consoleUp(char* pthis)
{
	if(!pthis) return 0;
	char* arrtptr = *(char**)(pthis+0x168);
	if(!arrtptr) return 0;

	char** cstr = (char**)(pthis+0x84);
	if(!cstr) return 0;

	if(__chat_history_index)
	{
		__chat_history_index--;
		return setString(arrtptr);
	}
	return 0;
}

string _stdcall getStringLastT()
{
	vector<string> vec = __chat_history[__chat_history.size()-1];
	string retv = "";
	for(vector<string>::iterator i = vec.begin(); i != vec.end(); ++i)
	{
		string &stra = (*i);
		retv += stra;
	}
	return retv;
}

void _stdcall getStringLast(char** cstr)
{
	string last = getStringLastT();
	char* asto = (char*)last.c_str();
	if(strlen(asto) && asto[0] == '/')
	{
		CCMD_processCommand(asto);
		asto[0] = 0;
	}

	str_empty(cstr);
	str_assign(cstr, asto);
}

unsigned long _stdcall consoleEnter(char* pthis)
{
	if(!pthis) return 0;
	char* arrtptr = *(char**)(pthis+0x168);
	if(!arrtptr) return 0;

	unsigned long cnt = *(unsigned long*)(arrtptr+0x78);
	if(cnt) addString(arrtptr);
	return cnt;
}

void _stdcall consoleTab(void* pthis)
{

}

void _declspec(naked) CHAT_keyDown()
{ // 0043E1FB
	__asm
	{
		push	ebp
		mov		ebp, esp
		sub		esp, 8
		mov		[ebp-4], ecx

		mov		eax, [ebp+8]
		cmp		eax, 0x26
		jz		c_up
		cmp		eax, 0x28
		jz		c_down
		cmp		eax, 0x0D
		jz		c_enter
		push	dword ptr [ebp+8]
		mov		ecx, [ebp-4]
		call	sub_43E1FB
		jmp		c_exit

c_down:
		push	dword ptr [ebp-4]
		call	consoleDown
		jmp		c_exit

c_up:
		push	dword ptr [ebp-4]
		call	consoleUp
		jmp		c_exit

c_tab:
		push	dword ptr [ebp-4]
		call	consoleTab
		jmp		c_exit_1

c_enter:
		push	dword ptr [ebp-4]
		call	consoleEnter
		test	eax, eax
		jnz		c_msg_send
		push	0
		push	0
		push	0x0446
		mov		ecx, [ebp-0x04]
		mov		edx, [ecx]
		call	[edx+0x48]
		jmp		c_exit_1

c_msg_send:
		push	0
		push	0
		push	0x0445
		mov		ecx, [ebp-0x04]
		mov		edx, [ecx]
		call	[edx+0x48]
		
c_exit_1:
		mov		eax, 1
c_exit:
		mov		esp, ebp
		pop		ebp
		retn	4

sub_43E1FB:
		push	ebp
		mov		ebp, esp
		sub		esp, 8
		mov		edx, 0x0043E201
		jmp		edx
	}
}

void __declspec(naked) CHAT_getMessageLast()
{ // 48F167
	__asm
	{ 
		lea		eax, [ebp-0x10]
		push	eax
		call	getStringLast
		mov		edx, 0x0048F188
		jmp		edx
	}
}

void _stdcall logOpen(unsigned long addr)
{
	log_format("window open: %08X\n", addr);
}

void _stdcall logEscape(unsigned long addr, unsigned char ch)
{
	/*SYSTEMTIME tm;
	GetLocalTime(&tm);
	std::string strf = Format("AfxGetMainWnd_%02u-%02u-%04u_%02u-%02u-%02u.dat", tm.wDay, tm.wMonth, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond);

	FILE* fil = fopen(strf.c_str(), "wb");
	if(!fil) return;

	const char* ch2 = (const char*)zxmgr::AfxGetMainWnd();
	for(int i = 0; i < 0x400; i++)
		fwrite(&ch2[i], 1, 1, fil);

	fclose(fil);*/
}

bool _stdcall ConsoleProc(unsigned char ch, int unk);

void __declspec(naked) CHAT_fixEscape()
{ // 48A0F7
	__asm
	{
		mov		[ebp-0x30], edx

		mov		eax, [ebp-0x2C]
		mov		eax, [eax+0xCC]
		mov		eax, [eax]
		push	[ebp+0x08]
		push	eax
		call	logEscape

		push	[ebp-0x30] // key?
		push	[ebp+0x08] // key
		call	ConsoleProc
		and		eax, 0xFF
		test	eax, eax
		jnz		cfe_skipall

		cmp		[ebp-0x30], 0xB8
		ja		loc_48A5BC

		cmp		[ebp+0x08], 0x1B
		jz		cfe_skipesc

cfe_continue:
		mov		edx, 0x0048A107
		jmp		edx

loc_48A5BC:
		mov		edx, 0x0048A5BC
		jmp		edx

cfe_skipesc:
		mov		edx, 0x00401870
		call	edx
		mov		ebx, [eax+0xC0]
		test	ebx, ebx
		jz		cfe_continue

		mov		ecx, [eax+0xCC] // chat object
		test	ecx, ecx
		jz		cfe_continue

		push	0
		push	0
		push	0x446
		mov		edx, [ecx]
		call	[edx+0x48]

		jmp		loc_48A5BC

cfe_skipall:
		jmp		loc_48A5BC
	}
}

void __declspec(naked) CHAT_dbg()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		push	ecx
		mov		[ebp-0x04], ecx
		
		push	[ebp+0x08]
		call	logOpen

		mov		edx, 0x004913F2
		jmp		edx
	}
}