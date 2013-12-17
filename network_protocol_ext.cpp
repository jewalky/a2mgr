#include <winsock2.h>
#include "utils.h"
#include "network_crypt.h"
#include "config.h"
#include "zxmgr.h"
#include "network_protocol_ext.h"
//#include <curl/curl.h>
#include "lib/happyhttp.h"
#include <windows.h>
#include "a2mgr.h"
#include "CRC_32.h"

using namespace std;

int _declspec(naked) real_4949CD()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		push	0xFFFFFFFF
		push	0x005C9A39
		mov		edx, 0x004949D7
		jmp		edx
	}
}

#include "lib\socket.hpp"

int __declspec(naked) NETPROTO_showErrorDialog(const char* text, int msgtype = 0)
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		sub		esp, 8

		push	0x78
		mov		edx, 0x00401840
		call	edx
		test	eax, eax
		jz		ned_exit
		
		push	msgtype
		push	0
		push	text
		push	0x252
		push	0x17C
		push	0x064
		push	0x040
		push	1
		mov		ecx, eax
		mov		edx, 0x0044775F
		call	edx

		push	eax
		mov		[ebp-0x04], eax
		mov		edx, 0x00401870
		call	edx
		mov		ecx, eax
		mov		edx, 0x0049958C
		call	edx

		mov		ecx, [ebp-0x04]
		mov		edx, 0x0043AB60
		call	edx

ned_exit:
		mov		esp, ebp
		pop		ebp
		retn
	}
}

struct NETPROTO_fileProgress
{
	FILE* file;
	double dltotal;
	double dlnow;
	uint32_t complete;
	uint32_t last_ticks;
	uint32_t speed;
	uint32_t read_bytes;
	std::string speed_string;

	bool is_string;
	std::string string;

	int status;

	NETPROTO_fileProgress()
	{
		file = NULL;
		dltotal = 0;
		dlnow = 0;
		complete = 0;
		last_ticks = 0;
		speed = 0;
		read_bytes = 0;
		speed_string = "";
		is_string = false;
		string = "";
		status = 0;
	}
};

void NETPROTO_hhStartReply(const happyhttp::Response* r, void* userdata)
{
	NETPROTO_fileProgress* prog = (NETPROTO_fileProgress*)(userdata);
	if(prog->is_string)
	{
		prog->string = "";
	}
	else
	{
		const char* strLC = r->getheader("Content-Length");
		std::string strL = "0";
		if(strLC) strL = strLC;

		prog->dltotal = StrToInt(strL);
		prog->dlnow = 0;
	}
}

void NETPROTO_hhReadReply(const happyhttp::Response* r, void* userdata, const unsigned char* data, int n)
{
	NETPROTO_fileProgress* progress = (NETPROTO_fileProgress*)(userdata);
	if(progress->is_string)
	{
		for(int i = 0; i < n; i++)
			progress->string.push_back(data[i]);
	}
	else
	{
		uint32_t ticks = GetTickCount();
		bool updateScreen = false;

		using namespace zxmgr;
		RECT* rcSrc2 = (RECT*)(0x0062B5D8);
		size_t count = fwrite(data, 1, n, progress->file);

		progress->dlnow += n;
		progress->read_bytes += n;
		if(ticks-progress->last_ticks > 1000 || !progress->speed)
		{
			if(ticks-progress->last_ticks > 1000)
			{
				updateScreen = true;
				progress->speed = progress->read_bytes;
				progress->read_bytes = 0;
				progress->last_ticks = ticks;
			}

			uint32_t speed = progress->speed;
			if(!speed) speed = progress->read_bytes;
			if(!speed) progress->speed_string = "N/A";
			else
			{
				if(speed > 1048576) // mb
					progress->speed_string = Format(GetPatchString(233), (double)(speed / 1048576.0));
				else if(speed > 1024) // kb
					progress->speed_string = Format(GetPatchString(232), (double)(speed / 1024.0));
				else progress->speed_string = Format(GetPatchString(231), (double)speed);
			}
		}

		uint32_t complete = progress->dlnow * 100.0 / progress->dltotal;

		if(progress->complete != complete || updateScreen)
		{
			LockBuffer();
			FillRect(rcSrc2->left, rcSrc2->top, rcSrc2->right, rcSrc2->bottom, 0);
			Font::DrawText(FONT1, rcSrc2->right / 2, rcSrc2->bottom / 2 - 8, GetPatchString(218), FONT_ALIGN_CENTER, FONT_COLOR_WHITE, 0);
			Font::DrawText(FONT1, rcSrc2->right / 2, rcSrc2->bottom / 2 + 8, Format(GetPatchString(230), complete, progress->speed_string.c_str()).c_str(), FONT_ALIGN_CENTER, FONT_COLOR_GRAY, 0);
			UnlockBuffer();
			UpdateScreen();
			progress->complete = complete;
		}
		
		DisplayMouse();
		DoMessageLoop();
	}
}

void NETPROTO_hhEndReply(const happyhttp::Response* r, void* userdata)
{
	NETPROTO_fileProgress* prog = (NETPROTO_fileProgress*)(userdata);
	prog->status = r->getstatus();
}

size_t NETPROTO_readTextRply(const char* ptr, size_t size, size_t nmemb, std::string* out)
{
	for(size_t i = 0; i < size * nmemb; i++)
		out->push_back(ptr[i]);
	return size * nmemb;
}

std::string CurlEscape2(std::string what)
{
	std::string out;
	for(size_t i = 0; i < what.length(); i++)
		out += Format("%%%02X", what[i]);
	return out;
}

happyhttp::Connection hhC("api.allods2.eu", 8080);

std::string NETPROTO_hhGetBlockingReply(std::string method, std::string url, std::string body)
{
	NETPROTO_fileProgress prog;
	prog.is_string = true;
	prog.string = "";
	hhC.setcallbacks(NETPROTO_hhStartReply, NETPROTO_hhReadReply, NETPROTO_hhEndReply, &prog);

	const char* headers[] = 
	{
		"Connection", "close",
		"Content-type", "application/x-www-form-urlencoded",
		"Accept", "text/plain",
		0
	};

	hhC.request(method.c_str(), url.c_str(), headers, (const unsigned char*)body.c_str(), body.length());

	while(hhC.outstanding())
		hhC.pump();

	hhC.close();

	//log_format("HH reply:\n%s\nHH status: %d\n\n", prog.string.c_str(), prog.status);

	return prog.string;
}

int _stdcall NETPROTO_loginToHat()
{
	char* pthis;
	__asm mov pthis, ecx;

	std::string login = *(const char**)(pthis + 0x3C0);
	std::string password = *(const char**)(pthis + 0x3C4);

	WSADATA wsd;
	WSAStartup(0x0101, &wsd);

	using namespace zxmgr;
	DoMessageLoop();

	RECT* rcSrc2 = (RECT*)(0x0062B5D8);

	LockBuffer();
	FillRect(rcSrc2->left, rcSrc2->top, rcSrc2->right, rcSrc2->bottom, 0);
	//void DrawText(unsigned long cptr, int x, int y, const char* string, unsigned long align, unsigned long color, unsigned long shadowpos)
	Font::DrawText(FONT1, rcSrc2->right / 2, rcSrc2->bottom / 2, GetPatchString(227), FONT_ALIGN_CENTER, FONT_COLOR_WHITE, 0);
	UnlockBuffer();
	UpdateScreen();

	char* uh;
	__asm mov uh, offset aHat;

	CRC_32 crc;
	uint32_t crc32_allods2_exe = crc.CalcCRC(sha1_allods2_exe, 20);
	uint32_t crc32_a2mgr_dll = crc.CalcCRC(sha1_a2mgr_dll, 20);
	uint32_t crc32_patch_res = crc.CalcCRC(sha1_patch_res, 20);
	uint32_t crc32_world_res = crc.CalcCRC(sha1_world_res, 20);

	std::string post_data = Format("action=check_version&language=%s&sum_allods2_exe=%08X&sum_patch_res=%08X&sum_world_res=%08X&sum_a2mgr_dll=%08X&l=%s&p=%s",
		z_russian ? "rus" : "eng", crc32_allods2_exe, crc32_patch_res, crc32_world_res, crc32_a2mgr_dll, CurlEscape2(login).c_str(), CurlEscape2(password).c_str());

	std::string text_reply = "";
	text_reply = NETPROTO_hhGetBlockingReply("POST", "/redhat.php", post_data);
	
	text_reply = Trim(text_reply);
	std::string lower_text_reply = ToLower(text_reply);
	if(lower_text_reply.find("string|") == 0) // хэт что-то ответил
	{
		text_reply.erase(0, 7);
		NETPROTO_showErrorDialog(text_reply.c_str());
		DoMessageLoop();
		return 0;
	}
	else if(lower_text_reply == "patch_update") // версия не совпадает
	{
		int reply = NETPROTO_showErrorDialog(GetPatchString(217), 4);
		if(reply != 0x447) return 0;
		DoMessageLoop();

		FILE* file_data = fopen("patch_setup.exe", "wb");
		if(!file_data)
		{
			NETPROTO_showErrorDialog(GetPatchString(229));
			DoMessageLoop();
			return 0;
		}

		NETPROTO_fileProgress prog;
		prog.file = file_data;
		prog.complete = 0;
		prog.last_ticks = GetTickCount();
		prog.speed = 0;
		prog.read_bytes = 0;
		prog.speed_string = "N/A";

		prog.is_string = false;
		prog.string = "";
		prog.status = 0;

		const char* headers[] = 
		{
			"Connection", "close",
			"Content-type", "application/x-www-form-urlencoded",
			"Accept", "text/plain",
			0
		};

		hhC.setcallbacks(NETPROTO_hhStartReply, NETPROTO_hhReadReply, NETPROTO_hhEndReply, &prog);

		std::string post_data_file = Format("action=get_latest&language=%s", z_russian ? "rus" : "eng");
		hhC.request("POST", "/redhat.php", headers, (const unsigned char*)post_data_file.c_str(), post_data_file.length());
		while(hhC.outstanding())
			hhC.pump();
		fclose(file_data);

		hhC.close();

		if(prog.dlnow == prog.dltotal) // easiest way to check if D/L completed
		{
			if(prog.dlnow <= 0 ||
				prog.dltotal <= 0)
			{
				NETPROTO_showErrorDialog(GetPatchString(229));
				DoMessageLoop();
				return 0;
			}

			// a2patch20_inst.exe /D=thisdir
			system(Format("start patch_setup.exe /D=%s", current_directory.c_str()).c_str());
			ExitProcess(0);
		}
		else
		{
			NETPROTO_showErrorDialog(GetPatchString(216));
			DoMessageLoop();
			return 0;
		}
	}
	else if(lower_text_reply == "login_banned")
	{
		NETPROTO_showErrorDialog(GetPatchString(22));
		DoMessageLoop();
		return 0;
	}
	else if(lower_text_reply == "login_banned_fvr")
	{
		NETPROTO_showErrorDialog(GetPatchString(219));
		DoMessageLoop();
		return 0;
	}
	else if(lower_text_reply == "login_invalid")
	{
		NETPROTO_showErrorDialog(GetPatchString(20));
		DoMessageLoop();
		return 0;
	}
	else if(lower_text_reply == "ip_banned")
	{
		NETPROTO_showErrorDialog(GetPatchString(34));
		DoMessageLoop();
		return 0;
	}
	else if(lower_text_reply == "fuck_off")
	{
		NETPROTO_showErrorDialog(GetPatchString(118));
		DoMessageLoop();
		return 0;
	}
	else if(lower_text_reply == "unblock_error")
	{
		NETPROTO_showErrorDialog(GetPatchString(234));
		DoMessageLoop();
		return 0;
	}

	text_reply = NETPROTO_hhGetBlockingReply("POST", "/redhat.php", post_data);

	//
	// if everything is OK with first stage

	int retval = 0;
	__asm
	{
		mov		ecx, pthis
		call	real_4949CD
		mov		retval, eax
	}

	return retval;
}

bool Cl_ProcessServerPacket(Packet& pack)
{
	return true;
}