#define URL_EU "hat.allods2.eu"
#define URL_COM "hat.allods2.com"
#define URL_LOCAL "127.0.0.1"
#define URL_ATV "atvision.net"
#define URL_TESTING "hat.allods2.eu:18000"

char aHat[] = URL_EU;

void __declspec(naked) HATENT_url_text()
{
	__asm
	{
		mov		eax, offset aHat
		push	eax
		mov		ecx, [ebp-0x124]
		add		ecx, 4
		mov		edx, 0x005AB9E0
		call	edx
		mov		edx, 0x0043F755
		jmp		edx
	}
}

void __declspec(naked) HATENT_url_connect()
{
	__asm
	{
		add		esp, 4
		mov     ecx, 0x0069C208
		mov		edx, 0x0051151C
		push	offset aHat // "hat.allods2.eu"
		call    edx
		mov		dword ptr [ebp-0x224], eax
		mov		edx, 0x00494AF4
		jmp		edx
	}
}