#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "utils.h"
#include "network_crypt.h"
#include "network_protocol_ext.h"
#include "config.h"
#include "CRC_32.h"
#include <ctime>

#include "zxmgr.h"

using namespace std;

#include "a2mgr.h"

typedef unsigned char u_char;

unsigned long _neI_SessionKey = 0, _neI_SessionID = 0;

void _stdcall crcmain(char* data)
{
	// data:0 = allods2.exe ^ key
	// data:4 = a2mgr.dll ^ key + allods2.exe
	// data:8 = patch.res ^ a2mgr.dll
	// data:C = world.res ^ key - patch.res
	// data:10 = graphics.res + allods2.exe ^ key (nulled/unused)
	// data:14 = key
	// data:18 = UUID
	// data:2C = UUID CRC32

	net_key = 0x6CDB248D;
	//net_key = _neI_SessionKey;

	CRC_32 crc;
	crc32_allods2_exe = crc.CalcCRC(sha1_allods2_exe, 20);
	crc32_a2mgr_dll = crc.CalcCRC(sha1_a2mgr_dll, 20);
	crc32_patch_res = crc.CalcCRC(sha1_patch_res, 20);
	crc32_world_res = crc.CalcCRC(sha1_world_res, 20);
	//crc32_graphics_res = crc.CalcCRC(sha1_graphics_res, 20);

	unsigned long lrc_allods2_exe = crc32_allods2_exe ^ net_key;
	unsigned long lrc_a2mgr_dll = (crc32_a2mgr_dll ^ net_key) + crc32_allods2_exe;
	unsigned long lrc_patch_res = crc32_patch_res ^ crc32_a2mgr_dll;
	unsigned long lrc_world_res = (crc32_world_res ^ net_key) - crc32_patch_res;
	//unsigned long lrc_graphics_res = (crc32_graphics_res + crc32_allods2_exe) ^ net_key;

	memset(data, 0, 0x18);
	*(unsigned long*)(data) = lrc_allods2_exe;
	*(unsigned long*)(data + 4) = lrc_a2mgr_dll;
	*(unsigned long*)(data + 8) = lrc_patch_res;
	*(unsigned long*)(data + 0x0C) = lrc_world_res;
	*(unsigned long*)(data + 0x10) = net_key;//lrc_graphics_res;
	*(unsigned long*)(data + 0x14) = _neI_SessionID ^ 0xDEADBEEF;
	memcpy(data + 0x18, sha1_uuid, 20);

	*(unsigned long*)(data + 0x2C) = crc.CalcCRC((LPVOID)(data + 0x18), 20) ^ net_key;
	for(int i = 0; i < 5; i++)
		*(unsigned long*)(data + 0x18 + i * 4) ^= net_key;
}

int NETPROTO_sendAuthorizationPacket() 
/* окончательное формирование пакета авторизации на хете и отправка */
{ // calc size & send
	_asm {
		push	ebp
		mov	ebp, esp
		push	ecx
		mov	[ebp-0x04], ecx
		mov	ecx, [ebp-0x04]
		mov	eax, ecx
		cmp	byte ptr [ecx+9], 0
		jnz	norm_p // не пакет авторизации, обработать по человечески
		// обработать с доп. размером
		add	eax, 50h 
		push	eax		// char *
		mov	edx, 0x58D7B0
		call	edx
		add	esp, 4
		mov	ecx, [ebp-0x04]
		mov	[ecx+4Eh], ax
		mov	edx, [ebp-0x04]
		xor	eax, eax
		mov	ax, [edx+4Eh]
		add	eax, 0x48
		jmp	join
norm_p:		add	eax, 10h 
		push	eax		// char *
		mov	edx, 0x58D7B0
		call	edx
		add	esp, 4
		mov	ecx, [ebp-0x04]
		mov	[ecx+0Eh], ax
		mov	edx, [ebp-0x04]
		xor	eax, eax
		mov	ax, [edx+0Eh]
		add	eax, 0x8
join:		push	eax
		mov	ecx, [ebp-0x04]
		add	ecx, 9
		push	ecx
		mov	ecx, [ebp+0x08]
		mov	edx, 0x508C6A
		call	edx
		mov	esp, ebp
		pop	ebp
		retn	4
	}
}

#pragma warning (disable: 4733)
int NETPROTO_createAuthorizationPacket()
/* подготовка пакета авторизации к отправке */
{ // fill packet
	_asm {
		push	ebp
		mov	ebp, esp
		push	0FFFFFFFFh
		push	0x5CF28A
		mov	eax, fs:0
		push	eax
		mov	fs:0, esp
		sub	esp, 14h
		mov	[ebp-0x018], ecx
		mov	[ebp-0x04], 1
		mov	dword ptr [ebp-0x010], 0x699FC8
		lea	eax, [ebp+0x0C]
		push	eax
		lea	ecx, [ebp+0x08]
		push	ecx
		lea	edx, [ebp-0x014]
		push	edx
		mov	edx, 0x5ABA87
		call	edx
		mov	[ebp-0x01C], eax
		mov	eax, [ebp-0x01C]
		mov	[ebp-0x020], eax
		mov	byte ptr [ebp-0x04], 2
		mov	ecx, [ebp-0x020]
		mov	edx, 0x401860
		call	edx
		push	eax		// char *
		mov	ecx, [ebp-0x010]
		add	ecx, 50h
		push	ecx		// char *
		mov	edx, 0x58CDF0
		call	edx
		add	esp, 8
		mov	byte ptr [ebp-0x04], 1
		lea	ecx, [ebp-0x014]
		mov	edx, 0x5AB84A
		call	edx
		mov	edx, [ebp-0x010]
		mov	byte ptr [edx+9], 0 /* заголовок пакета */
		//////// заполнить 0x30 байт
		//crc
		//lea eax, patchsign
		//push eax
		//lea eax, [edx+0x0A]
		//push eax
		//call crcmain
		//crc
		lea		eax, [edx+0x0A]
		push	eax
		call	crcmain
		lea	ecx, [ebp+0x08]
		mov	edx, 0x402970
		call	edx
		mov	ecx, [ebp+0x010]
		shl	ecx, 10h
		or	eax, ecx
		mov	ecx, PROTO_VER
		shl	ecx, 24
		or	eax, ecx /* версия 11 */
		mov	edx, [ebp-0x010]
		mov	[edx+0x4A], eax
		mov	eax, [ebp-0x010]
		mov	word ptr [eax+7], 0
		mov	ecx, [ebp-0x010]
		push	ecx
		mov	ecx, [ebp-0x018]
		mov	edx, 0x50A218
		call	edx
		mov	byte ptr [ebp-0x04], 0
		lea	ecx, [ebp+0x08]
		mov	edx, 0x5AB84A
		call	edx
		mov	[ebp-0x04], 0FFFFFFFFh
		lea	ecx, [ebp+0x0C]
		mov	edx, 0x5AB84A
		call	edx
		mov	ecx, [ebp-0x0C]
		mov	fs:0, ecx
		mov	esp, ebp
		pop	ebp
		retn	0Ch
	}
}

bool recv_lastCustom = false;
bool recv_lastIncomplete = false;
int recv_lastRemaining = -1;

int PASCAL recv_(SOCKET s, char* buf, int len, int flags)
{
	if(recv_lastIncomplete)
	{
		int r_ = recv(s, buf, recv_lastRemaining, 0);
		if(r_ == recv_lastRemaining)
		{
			recv_lastIncomplete = false;
			recv_lastRemaining = -1;
		}
		
		return r_;
	}

	int r = recv(s, buf, 4, 0);
	if(r != 4)
	{
		//log_format("received incomplete packet (oct1 = %d).\n", r);
		return -1;
	}
	if(!*(int*)(buf))
	{
		//log_format("received null packet.\n");
		return 0;
	}

	r = recv(s, buf+4, 4, 0);
	if(r != 4)
	{
		//log_format("received incomplete packet (oct2 = %d).\n", r);
		return -1;
	}

	int pkt_size = *(int*)(buf);
	int pkt_flags = *(int*)(buf+4);
	
	if((pkt_flags & 0x80100000) == 0x80100000)
	{
		//log_format("received special packet (size = %u, flags = %08X).\n", pkt_size, pkt_flags);

		uint8_t* rd = new uint8_t[pkt_size];
		r = recv(s, (char*)rd, pkt_size, 0);
		if(r != pkt_size)
		{
			delete[] rd;
			return -1;
		}

		Packet pack;
		pack.Reset();
		for(uint32_t i = 0; i < pkt_size; i++)
			pack.WriteUInt8(rd[i]);
		pack.Seek(0);

		delete[] rd;

		if(!Cl_ProcessServerPacket(pack))
			return -1;

		memset(buf, 0, len);
		return recv_(s, buf, len, flags);
	}
	else
	{
		//log_format("received allods packet (size = %u, flags = %08X).\n", pkt_size, pkt_flags);
		r = recv(s, buf+8, pkt_size, 0);
		if(r != pkt_size)
		{
			recv_lastCustom = false;
			recv_lastIncomplete = true;
			recv_lastRemaining = pkt_size - r;
		}
		r += 8;
	}

	return r;
}

void __declspec(naked) NETPROTO_fixRecv()
{
	__asm
	{
//40DB2C
		mov		[ebp-0x11BF4], edx

		mov		eax, [ebp-0x11BF4]
		sub		eax, 3
		mov		[ebp-0x11BF4], eax
		mov		edx, 0x0040DB41
		jmp		edx

skip_packet:
		mov		edx, 0x00417C03
		jmp		edx
	}
}

//0 100 005122D1
//0 101 00512314

char newNetBuffer[51200];

void __declspec(naked) NETPROTO_enlargeBuffer()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		mov		eax, 0x201C
		mov		edx, 0x0058D720
		call	edx // __alloca_probe
		mov		[ebp-0x2018], ecx
		push	0
		push	0xC800 // 51200
		lea		eax, newNetBuffer
		push	eax
		mov		edx, 0x005122F2
		jmp		edx
	}
}

void __declspec(naked) NETPROTO_enlargeBuffer2()
{
	__asm
	{
		lea		eax, newNetBuffer
		mov		[ebp-0x08], eax
		mov		edx, 0x0051231D
		jmp		edx
	}
}