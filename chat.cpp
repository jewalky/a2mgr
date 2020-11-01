#include "utils.h"
#include "config.h"

void _declspec(naked) CHAT_wmCharHandler(void) 
{
	_asm {
		push	ebp
		mov		ebp, esp
		push	ecx
		mov		[ebp - 4], ecx
		cmp		[ebp + 8], 0x20
		jb		wch_skip
		cmp		[ebp + 8], 0x7F
		jz		wch_skip
		mov		edx, 0x43D664
		jmp		edx
wch_skip:
		mov		edx, 0x43D680
		jmp		edx
	}
}

char *__stdcall conv_double_tilde(char *str)
{
	static char tilda_buf[4000];
	char *t = tilda_buf;
	while (*str && t < (tilda_buf + sizeof(tilda_buf) - 3))
	{
		*t++ = *str;
		if (*str == '~')
			*t++ = *str;
		str++;
	}
	*t = 0;
	return tilda_buf;
}

void _declspec(naked) CHAT_tilde0(void)
{
	__asm
	{
		mov		eax, [ecx]
		push	eax
		call	conv_double_tilde
		push	eax
		mov		edx, 0x043D97C
		jmp		edx
	}
}

void _declspec(naked) CHAT_tilde1(void)
{
	__asm
	{
		mov		eax, [ecx]
		push	eax
		call	conv_double_tilde
		push	eax
		mov		edx, 0x0402710
		jmp		edx
	}
}

void _declspec(naked) CHAT_tilde2(void)
{
	__asm
	{
		mov		eax, [ecx]
		push	eax
		call	conv_double_tilde
		push	eax
		mov		edx, 0x0402764
		jmp		edx
	}
}

void _declspec(naked) CHAT_convWinToDos(void)
{
	__asm
	{
		xor		edx, edx
		mov		dl, [ebp + 8]
		cmp		edx, 0x80
		jnz		w2d_n0
		mov		edx, 0x80
		mov		[ebp + 8], dl
		jmp w2d_skip
w2d_n0:
		cmp		edx, 0xA8
		jnz		w2d_n1
		mov		edx, 0xF0
		mov		[ebp + 8], dl
		jmp		w2d_skip
w2d_n1:
		cmp		edx, 0xB8
		jnz		w2d_n2
		mov		edx, 0xF1
		mov		[ebp + 8], dl
		jmp		w2d_skip
w2d_n2:
		mov		ecx, 0x047A127
		jmp		ecx
w2d_skip:
		mov		edx, 0x047A162
		jmp		edx
	}
}

void _declspec(naked) CHAT_convDosToIdx(void)
{
	__asm
	{
		xor		eax, eax
		mov		al, [ebp + 8]
		cmp		eax, 0xC1
		jnz		d2a_n0
		mov		eax, 0x80
		mov		[ebp + 8], al
		jmp		d2a_skip
d2a_n0:
		cmp		eax, 0xF0
		jnz		d2a_n1
		mov		eax, 192+32
		mov		[ebp + 8], al
		jmp		d2a_skip
d2a_n1:
		cmp		eax, 0xF1
		jnz		d2a_n2
		mov		eax, 105+32
		mov		[ebp + 8], al
		jmp		d2a_skip
d2a_n2:
		mov		ecx, 0x0461DBE
		jmp		ecx
d2a_skip:
		mov		edx, 0x0461E10
		jmp		edx
	}
}

char pr_c_2_str[] = "%s\n";

int _declspec(naked) CHAT_logStrings(char *s, int unk_1, int unk_2) {
	__asm 
	{
		push	ebp
		mov		ebp, esp
		sub		esp, 4

		push	dword ptr [ebp+0x10]
		push	dword ptr [ebp+0x0C]
		push	dword ptr [ebp+8]
		call	sub_4021FE
		mov		[ebp-4], eax

		push	dword ptr [ebp+8]
		push	offset pr_c_2_str
		call	log_format

		mov		eax, [ebp-4]

		mov		esp, ebp
		pop		ebp
		retn	0Ch

sub_4021FE:
		push	ebp
		mov		ebp, esp
		push	0xFFFFFFFF
		push	0x005C479E
		mov		edx, 0x00402208
		jmp		edx
	}
}

#include "zxmgr.h"

char * _stdcall chatAddStringCpp(char *msg, int type)
{
	if (g_altchat)
	{
		static char chatBuf[3000];
		if (type == 1)
		{
			// аля
			const char* str_type = zxmgr::GetPatchString(223);
			static char chatB2[128];
			chatB2[127] = 0;
			_snprintf(chatB2, 127, "[%s] ", str_type);
			strcpy(chatBuf, chatB2);
			strncat(chatBuf, msg, sizeof(chatBuf)-1);
			return chatBuf;
		}
		else if (type == 2)
		{
			// приват
			const char* str_type = zxmgr::GetPatchString(224);
			static char chatB2[128];
			chatB2[127] = 0;
			_snprintf(chatB2, 127, "[%s] ", str_type);
			strcpy(chatBuf, chatB2);
			strncat(chatBuf, msg, sizeof(chatBuf)-1);
			return chatBuf;
		}
		else if (type == 3)
		{
			// вопль
			const char* str_type = zxmgr::GetPatchString(225);
			static char chatB2[128];
			chatB2[127] = 0;
			_snprintf(chatB2, 127, "[%s] ", str_type);
			strcpy(chatBuf, chatB2);
			strncat(chatBuf, msg, sizeof(chatBuf)-1);
			return chatBuf;
		}
	}
	return msg;
}


void _declspec(naked) CHAT_prependType_part1()
{
	__asm
	{
		mov	ecx, [ebp-0x77C] // second byte
		push	ecx
		push	eax
		call	chatAddStringCpp
		push	eax

		mov	ecx, [ebp-0x11BF0]
		add	ecx, 0xA28

		mov	edx, 0x0413F93
		jmp	edx
	}
}

void _declspec(naked) CHAT_prependType_part2()
{
	__asm
	{
		mov	ecx, [ebp-0x77C] // second byte
		push	ecx
		push	eax
		call	chatAddStringCpp
		push	eax

		mov	ecx, [ebp-0x11BF0]
		add	ecx, 0xA28

		mov	edx, 0x413E04
		jmp	edx
	}
}
