#include <windows.h>
#include "utils.h"
#include "debug.h"
#include "zxmgr.h"

#define MGR_DEBUG

void log_str(char* string, unsigned long ref)
{
	log_format("string: \"%s\"; return address 0x%08X\n", string, ref);
}

void __declspec(naked) DBG_logStrings()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		push	ecx
		mov		[ebp-0x04], ecx
		mov		eax, [ecx+0x0C]
		add		eax, [ebp+0x08]
		push	eax
		mov		ecx, 0x0062C7C8
		mov		edx, 0x00421580
		call	edx
		mov		eax, [eax]
#ifdef MGR_DEBUG
		mov		[ebp-0x04], eax
		push	[ebp+0x04]
		push	eax
		call	log_str
		mov		eax, [ebp-0x04]
#endif
		mov		esp, ebp
		pop		ebp
		retn	0x0004
	}
}

DWORD exc_handler_run(struct _EXCEPTION_POINTERS *info)
{
	__try
	{
		// dump info
		log_format("EXCEPTION DUMP:\neax=%08Xh,ebx=%08Xh,ecx=%08Xh,edx=%08Xh,\nesp=%08Xh,ebp=%08Xh,esi=%08Xh,edi=%08Xh;\neip=%08Xh;\naddr=%08Xh,code=%08Xh,flags=%08Xh\n",
				info->ContextRecord->Eax,
				info->ContextRecord->Ebx,
				info->ContextRecord->Ecx,
				info->ContextRecord->Edx,
				info->ContextRecord->Esp,
				info->ContextRecord->Ebp,
				info->ContextRecord->Esi,
				info->ContextRecord->Edi,
				info->ContextRecord->Eip,
				info->ExceptionRecord->ExceptionAddress,
				info->ExceptionRecord->ExceptionCode,
				info->ExceptionRecord->ExceptionFlags);

		log_format("BEGIN STACK TRACE: %08Xh [%08Xh] <= ", info->ExceptionRecord->ExceptionAddress, *(unsigned long*)(info->ContextRecord->Esp));
		unsigned long stebp = *(unsigned long*)(info->ContextRecord->Ebp);
		while(true)
		{
			bool bad_ebp = false;
			if(stebp & 3) bad_ebp = true;
			if(!bad_ebp || IsBadReadPtr((void*)stebp, 8)) bad_ebp = true;

			if(bad_ebp) /* ? */ break;

			log_format2("%08Xh <= ", *(unsigned long*)(stebp+4));
			stebp = *(unsigned long*)(stebp); // o_O
		}
		log_format2("END STACK TRACE\n");

		log_format("a2mgr crashed.\n\n");
		
		//if(_LOG_FILE.is_open()) _LOG_FILE.close();
		TerminateProcess(GetCurrentProcess(), 1); // гарантированно ёбнет
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		log_format("a2mgr crashed TWICE.\n\n");
		TerminateProcess(GetCurrentProcess(), 1);
	}

	// will not get to this point
	// but compiler requires that to be written
	return EXCEPTION_EXECUTE_HANDLER;
}

void setup_handler()
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)&exc_handler_run);
}

/*void _stdcall log_retn(unsigned ww, char* str)
{
	log_format("log_retn: %08X, %s\n", ww, str);
}

void __declspec(naked) DBG_tempS()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		push	ecx
		mov		[ebp-0x04], ecx
		push	[ebp+0x10]
		push	[ebp+4]
		call	log_retn
		mov		eax, 0x004627FD
		jmp		eax
	}
}*/

void __declspec(naked) DBG_tempS()
{
	__asm
	{
		push	0x20
		push	0x10
		lea		ecx, [ebp-0x14]
		mov		edx, 0x0042ABF0
		call	edx
		mov		edx, 0x004741CC
		jmp		edx
	}
}

// THE ULTIMATE "DRAWN-FROM"

#include <map>
#include <string>
std::map<unsigned long, std::string> d_strings;

void _stdcall D_AddAddr(unsigned long addr, char* string)
{
	d_strings[addr] = std::string(string);
}

void _stdcall D_LogAddr(unsigned long addr, unsigned long addr2)
{
	log_format("drawn sprite: %s; at: %08X\n", d_strings[addr].c_str(), addr2);
}

void __declspec(naked) dbg_16a_addaddrstring()
{
	__asm
	{
		//42A71B
		push	ebp
		mov		ebp, esp
		push	ecx
		mov		[ebp-0x04], ecx

		push	[ebp+0x08]
		push	ecx
		call	D_AddAddr

		mov		edx, 0x0042A722
		jmp		edx
	}
}

void __declspec(naked) dbg_16a_readaddrstring()
{
	__asm
	{
		//42A740
		push	ebp
		mov		ebp, esp
		sub		esp, 0x0C
		mov		[ebp-0x0C], ecx

		push	[ebp+4]
		push	ecx
		call	D_LogAddr

		mov		edx, 0x0042A749
		jmp		edx
	}
}

void _stdcall logVision(char* struc, char* unit)
{
	unsigned short vis = *(unsigned short*)(unit + 0x10A);
	unsigned long shift = *(unsigned long*)(struc + 0x3F50);

	unsigned short vis2 = (1 << (shift - 1)) + (vis >> (8 - shift));
	log_format("vis = %u (%04X); shift = %u; vis2 = %u (%04X)\n", vis, vis, shift, vis2, vis2);
}

void __declspec(naked) dbg_logVision()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		sub		esp, 0x28
		mov		[ebp-0x28], ecx
		push	[ebp+0x08]
		push	ecx
		call	logVision
		mov		edx, 0x00403D50
		jmp		edx
	}
}

void _stdcall logSectionLoaded(char* what)
{
	
}

// 4F7B90
void __declspec(naked) DBG_logSectionLoaded()
{
	__asm
	{
		push	0x00674620
		call	logSectionLoaded
		mov		eax, [ebp+0x1C]
		mov		dword ptr [eax], 0x00674620
		mov		edx, 0x004F7BA2
		jmp		edx
	}
}

#include <map>
std::map<const char*, std::string> Names;

void _stdcall _load256(const char* name, const char* pthis)
{
	if(!name || !pthis) return;
	Names[pthis] = name;
}

// 426DA9
void __declspec(naked) DBG_load256()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		push	0xFFFFFFFF
		push	0x005C5232
		mov		eax, fs:[0]
		push	eax
		mov		fs:[0], esp
		sub		esp, 0x138
		mov		[ebp-0x144], ecx
		push	ecx
		push	[ebp+0x08]
		call	_load256

		mov		edx, 0x00426DCD
		jmp		edx
	}
}

void _stdcall _disp256(const char* pthis, unsigned long retaddr)
{
	std::string fname = "N/A";
	for(std::map<const char*, std::string>::iterator it = Names.begin();
		it != Names.end(); ++it)
	{
		if(it->first == pthis)
		{
			fname = it->second;
			break;
		}
	}

	log_format(".256 displayed (\"%s\", [ebp+4]=%08Xh)\n", fname.c_str(), retaddr);
}

// 427014
void __declspec(naked) DBG_disp256()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		sub		esp, 0x08
		mov		[ebp-0x08], ecx
		push	[ebp+4]
		push	ecx
		call	_disp256

		mov		edx, 0x0042701D
		jmp		edx
	}
}

// 427140
void __declspec(naked) DBG_di2p256()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		sub		esp, 0x08
		mov		[ebp-0x08], ecx
		push	[ebp+4]
		push	ecx
		call	_disp256

		mov		edx, 0x00427149
		jmp		edx
	}
}

// 4271D0
void __declspec(naked) DBG_di3p256()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		sub		esp, 0x08
		mov		[ebp-0x08], ecx
		push	[ebp+4]
		push	ecx
		call	_disp256

		mov		edx, 0x004271D9
		jmp		edx
	}
}

void _stdcall _decideState(unsigned long num)
{
	log_format("idle: frame: %u\n", num);
}

// 4688CF
void __declspec(naked) DBG_decideState()
{
	__asm
	{
		mov		eax, [ebp-0x90]
		test	eax, eax
		jz		dds_skip
		push	[eax+0x74]
		call	_decideState

dds_skip:
		mov		eax, [ebp-0x50]
		cmp		dword ptr [eax+0x28], 0
		jz		loc_4688D6
		mov		edx, 0x004688DC
		jmp		edx

loc_4688D6:
		mov		edx, 0x004688D6
		jmp		edx
	}
}
