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
#include "lib/socket.hpp"

//#define API_LOCAL

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

	std::string filename;
	int ftotal;
	int fnow;

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
		filename = "";
		ftotal = 0;
		fnow = 0;
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

HANDLE hProgLock = 0;
void LockProgress()
{
	if (!hProgLock) hProgLock = CreateMutex(0, TRUE, NULL);
	WaitForSingleObject(hProgLock, INFINITE);
}

void UnlockProgress()
{
	ReleaseMutex(hProgLock);
}

std::vector<std::string> oldfilenames;
NETPROTO_fileProgress gprogress;

void NETPROTO_hhReadReply(const happyhttp::Response* r, void* userdata, const unsigned char* data, int n)
{
	using namespace zxmgr;

	NETPROTO_fileProgress* progress = (NETPROTO_fileProgress*)(userdata);

	if (!progress->file && !progress->is_string)
	{
		DisplayMouse();
		DoMessageLoop();
		return;
	}

	if(progress->is_string)
	{
		for(int i = 0; i < n; i++)
			progress->string.push_back(data[i]);
		if (GetTickCount()-progress->last_ticks > 30)
		{
			DisplayMouse();
			DoMessageLoop();
			progress->last_ticks = GetTickCount();
		}
	}
	else
	{
		uint32_t ticks = GetTickCount();
		bool updateScreen = false;

		RECT* rcSrc2 = (RECT*)(0x0062B5D8);
		size_t count = 0;
		//if (n) count = fwrite(data, 1, n, progress->file);
		progress->string.append((const char*)data, n);

		progress->dlnow += n;
		progress->read_bytes += n;

		if (progress->dlnow == progress->dltotal)
		{
			fwrite(progress->string.data(), progress->dltotal, 1, progress->file);
			progress->string = "";
		}

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

		uint32_t complete = progress->dltotal ? (progress->dlnow * 100.0 / progress->dltotal) : 100;

		if (progress->filename != "")
		{
			LockProgress();
			gprogress = *progress;
			UnlockProgress();
		}
		else if(progress->complete != complete || updateScreen)
		{
			LockBuffer();
			FillRect(rcSrc2->left, rcSrc2->top, rcSrc2->right, rcSrc2->bottom, 0);
			Font::DrawText(FONT1, rcSrc2->right / 2, rcSrc2->bottom / 2 - 8, GetPatchString(218), FONT_ALIGN_CENTER, FONT_COLOR_WHITE, 0);
			Font::DrawText(FONT1, rcSrc2->right / 2, rcSrc2->bottom / 2 + 8, Format(GetPatchString(230), complete, progress->speed_string.c_str()).c_str(), FONT_ALIGN_CENTER, FONT_COLOR_GRAY, 0);
			UnlockBuffer();
			UpdateScreen();
			progress->complete = complete;
			DisplayMouse();
			DoMessageLoop();
		}
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

#ifndef API_LOCAL
#define APIU_NAME "api.allods2.eu"
#define APIU_PORT 8080
#define APIU_NAME2 "api.allods2.eu"
#define APIU_PORT2 80
#else
#define APIU_NAME "127.0.0.1"
#define APIU_PORT 80
#define APIU_NAME2 "127.0.0.1"
#define APIU_PORT2 80
#endif

std::string NETPROTO_hhGetBlockingReply(std::string method, std::string url, std::string body)
{
	happyhttp::Connection hhC(APIU_NAME, APIU_PORT);

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

	try
	{
		hhC.request(method.c_str(), url.c_str(), headers, (const unsigned char*)body.c_str(), body.length());

		while(hhC.outstanding())
			hhC.pump();

		hhC.close();
	}
	catch (happyhttp::Wobbly ex)
	{
		/* pass */
		return "";
	}

	//log_format("HH reply:\n%s\nHH status: %d\n\n", prog.string.c_str(), prog.status);

	return prog.string;
}

#include <atlstr.h>

struct VersionFileEntry
{
	VersionFileEntry(CString a, CString b) : name(a), checksum(b) {}
	CString name;
	CString checksum;
};

struct VersionFile
{
	void LoadFromString(std::string w)
	{
		files.clear();
		std::vector<std::string> rows = Explode(w, "\n");
		for (size_t i = 0; i < rows.size(); i++)
		{
			if (Trim(rows[i]).length() == 0)
				continue;
			std::vector<std::string> row = Explode(Trim(rows[i]), " ");
			if (row.size() != 2)
				files.push_back(VersionFileEntry(row[0].c_str(), ""));
			else files.push_back(VersionFileEntry(row[0].c_str(), row[1].c_str()));
		}
	}

	std::string SaveToString()
	{
		std::string out;
		for (size_t i = 0; i < files.size(); i++)
			out += files[i].name + " " + files[i].checksum + "\n";
		return out;
	}

	std::vector<VersionFileEntry> files;
};

bool NETPROTO_CreateRecursiveCheck(std::string wat)
{
	DWORD dwAttrib = GetFileAttributes(wat.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
           (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#include <direct.h>

bool NETPROTO_CreateRecursive(std::string wat)
{
	if (NETPROTO_CreateRecursiveCheck(wat))
		return true;

	std::string fp = "";
	std::vector<std::string> dirs = Explode(wat, "/");
	for (size_t i = 0; i < dirs.size(); i++)
	{
		if (fp.length()) fp += "/";
		fp += dirs[i];
		if (!NETPROTO_CreateRecursiveCheck(fp))
		{
			if (_mkdir(fp.c_str()) != 0)
				return false;
			if (!NETPROTO_CreateRecursiveCheck(fp))
				return false;
		}
	}

	return true;
}

struct UpdaterInfo
{
	int result;
};

int NETPROTO_updater(UpdaterInfo* updater)
{
	// first off make sure that we want to do anything even relatively long
	std::string id_own;
	std::string id_remote = NETPROTO_hhGetBlockingReply("GET", "/allods/version_id", "");
	FILE* vfo = fopen("data/version_id", "rb");
	if (vfo)
	{
		fseek(vfo, 0, SEEK_END);
		size_t len = ftell(vfo);
		char* tmpbuf = new char[len+1];
		tmpbuf[len] = 0;
		fseek(vfo, 0, SEEK_SET);
		fread(tmpbuf, len, 1, vfo);
		id_own = tmpbuf;
		delete[] tmpbuf;
		fclose(vfo);
	}

	if (id_remote.size() && id_own.size() && id_own == id_remote)
	{
		updater->result = 1;
		return 1; // updated successfully
	}

	using namespace zxmgr;
	// update the resources
	std::string md5_own;
	//data/version = own
	//allods/version http = remote

	vfo = fopen("data/version", "rb");
	if (vfo)
	{
		fseek(vfo, 0, SEEK_END);
		size_t len = ftell(vfo);
		char* tmpbuf = new char[len+1];
		tmpbuf[len] = 0;
		fseek(vfo, 0, SEEK_SET);
		fread(tmpbuf, len, 1, vfo);
		md5_own = tmpbuf;
		delete[] tmpbuf;
		fclose(vfo);
	}

	std::string md5_remote = NETPROTO_hhGetBlockingReply("GET", "/allods/version", "");

	if (!md5_remote.size())
	{
		updater->result = 1;
		return 1; // we aren't using this mechanism
	}

	//log_format("updater(): md5_own = %d bytes, md5_remote = %d bytes\n", md5_own.size(), md5_remote.size());

	VersionFile vf_own;
	vf_own.LoadFromString(md5_own);
	VersionFile vf_remote;
	vf_remote.LoadFromString(md5_remote);

	std::vector<CString> files_to_delete;
	std::vector<VersionFileEntry> files_to_download;

	for (size_t i = 0; i < vf_own.files.size(); i++)
	{
		bool found = false;
		for (size_t j = 0; j < vf_remote.files.size(); j++)
		{
			if (vf_remote.files[j].name == vf_own.files[i].name)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			files_to_delete.push_back(vf_own.files[i].name);
			//log_format("updater(): marked to delete file \"%s\"...\n", vf_own.files[i].name.GetBuffer());
		}
	}

	bool have_update = false;
	bool have_dll = false;

	for (size_t i = 0; i < vf_remote.files.size(); i++)
	{
		bool matches = false;
		for (size_t j = 0; j < vf_own.files.size(); j++)
		{
			if (vf_own.files[j].name == vf_remote.files[i].name)
			{
				if (vf_own.files[j].checksum == vf_remote.files[i].checksum)
					matches = true;
				break;
			}
		}

		if (!matches)
		{
			files_to_download.push_back(vf_remote.files[i]);
			if (vf_remote.files[i].name == "allods2.exe")
				have_update = true;
			else if (vf_remote.files[i].name == "a2mgr.dll")
				have_dll = true;
			//log_format("updater(): marked to download file \"%s\"...\n", vf_remote.files[i].name.GetBuffer());
		}
	}

	if (files_to_download.size() || files_to_delete.size())
	{
		// perform downloading
		for (size_t i = 0; i < files_to_download.size(); i++)
		{
			std::string target = "data/"+files_to_download[i].name;
			std::string source = "/allods/"+files_to_download[i].name;

			std::string target_dir = Dirname(target);
			if (NETPROTO_CreateRecursive(target_dir))
			{
				FILE* file_data = fopen(target.c_str(), "wb");

				// fast check for null hash, to avoid random happyhttp error
				if (file_data)
				{
					if (files_to_download[i].checksum == "da39a3ee5e6b4b0d3255bfef95601890afd80709")
					{
						LockProgress();
						oldfilenames.push_back(files_to_download[i].name.GetBuffer());
						UnlockProgress();
						vf_own.files.push_back(files_to_download[i]);
						fclose(file_data);
						continue;
					}

					happyhttp::Connection hhC(APIU_NAME2, APIU_PORT2);

					NETPROTO_fileProgress prog;
					prog.file = file_data;
					prog.complete = 0;
					prog.last_ticks = GetTickCount();
					prog.speed = 0;
					prog.read_bytes = 0;
					prog.speed_string = "N/A";
					prog.filename = files_to_download[i].name;
					prog.ftotal = files_to_download.size();
					prog.fnow = i;

					prog.is_string = false;
					prog.string = "";
					prog.status = 0;

					const char* headers[] = 
					{
						"Connection", "close",
						//"Content-type", "application/x-www-form-urlencoded",
						"Accept", "text/plain",
						0
					};

					hhC.setcallbacks(NETPROTO_hhStartReply, NETPROTO_hhReadReply, NETPROTO_hhEndReply, &prog);

					bool error = false;
					try
					{
						hhC.request("GET", source.c_str(), headers);

						while(hhC.outstanding())
							hhC.pump();
						fclose(file_data);

						hhC.close();
					}
					catch (happyhttp::Wobbly ex)
					{
						error = true;
					}

					if (!error && (prog.dlnow == prog.dltotal))
					{
						bool found = false;
						for (size_t j = 0; j < vf_own.files.size(); j++)
						{
							if (vf_own.files[j].name == files_to_download[i].name)
							{
								vf_own.files[j].checksum = files_to_download[i].checksum;
								found = true;
								break;
							}
						}

						vf_own.files.push_back(files_to_download[i]);
						continue;
					}
				}
			}

			FILE* file_ver = fopen("data/version", "wb");
			if (file_ver)
			{
				md5_own = vf_own.SaveToString();
				fwrite(md5_own.data(), md5_own.size(), 1, file_ver);
				fclose(file_ver);
			}

			LockProgress();
			gprogress.filename = "";
			UnlockProgress();
			updater->result = 0;
			return 0;
		}

		// and deletion
		for (size_t i = 0; i < files_to_delete.size(); i++)
		{
			DeleteFile(("data/"+files_to_delete[i]).GetBuffer());
			for (size_t j = 0; j < vf_own.files.size(); j++)
			{
				if (vf_own.files[j].name == files_to_delete[i])
				{
					vf_own.files.erase(vf_own.files.begin()+j);
					j--;
				}
			}
		}
		
		FILE* file_ver = fopen("data/version", "wb");
		if (file_ver)
		{
			md5_own = vf_own.SaveToString();
			fwrite(md5_own.data(), md5_own.size(), 1, file_ver);
			fclose(file_ver);
		}
	}

	if (id_remote.size())
	{
		vfo = fopen("data/version_id", "w");
		if (vfo)
		{
			fwrite(id_remote.data(), id_remote.size(), 1, vfo);
			fclose(vfo);
		}
	}

	if (have_update || // updated patch
		have_dll || // updated a2mgr
		files_to_download.size()) // have any other NEW/replaced files
	{
		if (have_update)
		{
			MoveFileEx("allods2.exe", "allods2.exe.old", MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH); // can't delete
			//rename("allods2.exe", "allods2.exe.old");
			CopyFile("data/allods2.exe", "allods2.exe", FALSE);
		}

		if (have_dll)
		{
			MoveFileEx("a2mgr.dll", "a2mgr.dll.old", MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
			//rename("a2mgr.dll", "a2mgr.dll.old");
			CopyFile("data/a2mgr.dll", "a2mgr.dll", FALSE);
		}

		// we are restarting the client anyway, because the updated resources could have been already loaded
		char* commandline_raw = GetCommandLineA();
		vector<string> parms = ParseCommandLine(commandline_raw);
		std::string newcmd = "";
		for (size_t i = 0; i < parms.size(); i++)
			newcmd += parms[i]+" ";
		newcmd += "-delay 4000"; // wait 4s before restarting

		/* CreateProcess API initialization */
		STARTUPINFO siStartupInfo;
		PROCESS_INFORMATION piProcessInfo;
		memset(&siStartupInfo, 0, sizeof(siStartupInfo));
		memset(&piProcessInfo, 0, sizeof(piProcessInfo));
		siStartupInfo.cb = sizeof(siStartupInfo);

		::CreateProcessA("allods2.exe", // application name/path
			(LPSTR)newcmd.c_str(), // command line (optional)
			NULL, // no process attributes (default)
			NULL, // default security attributes
			false,
			CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE,
			NULL, // default env
			NULL, // default working dir
			&siStartupInfo,
			&piProcessInfo);

		::TerminateProcess(GetCurrentProcess(), 0);
		::ExitProcess(0); // exit this process
	}

	updater->result = 1;
	return 1;
}

int _stdcall NETPROTO_loginToHat()
{
	char* pthis;
	__asm mov pthis, ecx;

	gprogress.filename = "";

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

	std::string post_data = Format("action=check_version&language=%s&sum_allods2_exe=%08X&sum_a2mgr_dll=%08X&l=%s&p=%s",
		z_russian ? "rus" : "eng", crc32_allods2_exe, crc32_a2mgr_dll, CurlEscape2(login).c_str(), CurlEscape2(password).c_str());

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

	UpdaterInfo updater;
	updater.result = 0;
	hProgLock = CreateMutex(0, FALSE, NULL);
	HANDLE thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)NETPROTO_updater, &updater, 0, NULL);
	std::string lastName = "";
	while (WaitForSingleObject(thread, 1) != WAIT_OBJECT_0)
	{
		LockProgress();
		NETPROTO_fileProgress* progress = &gprogress;

		if (progress->filename != "")
		{
			if (progress->filename != lastName)
			{
				if (lastName != "")
					oldfilenames.push_back(lastName);
				lastName = progress->filename;
			}

			int wd = rcSrc2->right;
			int ht = rcSrc2->bottom;
			int lines = ht / 2 / 16 + 1;
			int lines_px = lines * 16;
			int startx = wd/2 - wd/4;
			int start = ht/2 - lines_px/2 - 16;
			LockBuffer();
			FillRect(rcSrc2->left, rcSrc2->top, rcSrc2->right, rcSrc2->bottom, 0);
			Font::DrawText(FONT1, rcSrc2->right / 2, start, GetPatchString(234), FONT_ALIGN_CENTER, FONT_COLOR_WHITE, 0);
			start += 16;

			for (int i = int(oldfilenames.size()-1)-(lines-2); i < int(oldfilenames.size()); i++)
			{
				if (i < 0) continue;
				std::string up = Format("%s: %s", GetPatchString(235), oldfilenames[i].c_str());
				if (Font::MeasureTextWidth(*(unsigned long*)FONT1, up.c_str()) >= wd/2)
					Font::DrawText(FONT1, rcSrc2->right / 2, start, up.c_str(), FONT_ALIGN_CENTER, FONT_COLOR_GRAY, 0);
				else
				{
					Font::DrawText(FONT1, rcSrc2->left + startx, start, Format("%s:", GetPatchString(235)).c_str(), FONT_ALIGN_LEFT, FONT_COLOR_GRAY, 0);
					Font::DrawText(FONT1, rcSrc2->right - startx, start, oldfilenames[i].c_str(), FONT_ALIGN_RIGHT, FONT_COLOR_GRAY, 0);
				}

				start += 16;
			}
			std::string up = Format("file: %s", progress->filename.c_str());
			if (Font::MeasureTextWidth(*(unsigned long*)FONT1, up.c_str()) >= wd/2)
				Font::DrawText(FONT1, rcSrc2->right / 2, start, up.c_str(), FONT_ALIGN_CENTER, FONT_COLOR_GRAY, 0);
			else
			{
				Font::DrawText(FONT1, rcSrc2->left + startx, start, Format("%s:", GetPatchString(235)).c_str(), FONT_ALIGN_LEFT, FONT_COLOR_WHITE, 0);
				Font::DrawText(FONT1, rcSrc2->right - startx, start, progress->filename.c_str(), FONT_ALIGN_RIGHT, FONT_COLOR_WHITE, 0);
			}
			start += 16;
			uint32_t complete = progress->dltotal ? (progress->dlnow * 100.0 / progress->dltotal) : 100;
			Font::DrawText(FONT1, rcSrc2->right - startx, start, (Format(GetPatchString(230), complete, progress->speed_string.c_str())+", "+Format(GetPatchString(236), progress->fnow, progress->ftotal)).c_str(), FONT_ALIGN_RIGHT, FONT_COLOR_WHITE, 0);
			UnlockBuffer();
			UpdateScreen();
		}

		UnlockProgress();
		DisplayMouse();
		DoMessageLoop();
		Sleep(1);
	}

	if (updater.result != 1)
	{
		NETPROTO_showErrorDialog(GetPatchString(216));
		DoMessageLoop();
		return updater.result;
	}

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

struct ScreenshotInfo
{
	std::string Login;
	uint32_t UID;
	int Width;
	int Height;
	uint16_t* Data;
	bool Flip;
};

#include "lib/lodepng.h"

extern char aHat[];
int Cl_ScreenshotThread(const ScreenshotInfo* si)
{
	SOCKET s = SOCK_Connect("hat.allods2.eu", 8000, "0.0.0.0", 0);

	Packet p;
	p.WriteUInt32(0x5C0EE250);
	p.WriteString(si->Login);
	p.WriteUInt32(si->UID);
	p.WriteUInt8(0);
	SOCK_SendPacket(s, p, 24);

	//zxmgr::WriteChat("screenshot: converting");

	uint32_t* ndata = new uint32_t[si->Width*si->Height];
	uint32_t* ndatap = ndata;

	uint16_t* orp = si->Data;

	for (int y = si->Flip ? si->Height-1 : 0; si->Flip ? (y >= 0) : (y < si->Height); si->Flip ? y-- : y++)
	{
		uint32_t* rowp = ndatap + y * si->Width;
		for (int x = 0; x < si->Width; x++)
		{
			uint16_t a = *orp++;
			unsigned long r = (a & 0xF800)>>11;
			unsigned long g = (a & 0x07E0)>>5;
			unsigned long b = (a & 0x001F);
			r = r * 255 / 31;
			g = g * 255 / 63;
			b = b * 255 / 31;
			uint32_t col = 0xFF000000 | (b << 16) | (g << 8) | r;
			*rowp++ = col;
		}
	}

	delete[] si->Data;

	std::string post_reply = "";

	unsigned char* buf = NULL;
	size_t size = 0;
	unsigned int ov = lodepng_encode32(&buf, &size, (const unsigned char*)ndata, si->Width, si->Height);
	delete[] ndata;
	if (ov)
	{
		p = Packet();
		p.WriteUInt32(0x5C0EE250);
		p.WriteString(si->Login);
		p.WriteUInt32(si->UID);
		p.WriteUInt8(1);
		p.WriteString(post_reply);
		SOCK_SendPacket(s, p, 24); // report failure
		SOCK_Destroy(s);
		delete si;
		return 0;
	}

	std::string pre = "------WebKitFormBoundaryePkpFF7tjBAqx29L\r\nContent-Disposition: form-data; name=\"p\"; filename=\"ps.png\"\r\nContent-Type: image/png\r\n\r\n";
	std::string post = "\r\n------WebKitFormBoundaryePkpFF7tjBAqx29L--\r\n";
	int post_size = pre.size()+post.size()+size;
	//zxmgr::WriteChat("screenshot: sending %d bytes", post_size);
	happyhttp::Connection hhC(APIU_NAME, APIU_PORT);

	NETPROTO_fileProgress prog;
	prog.is_string = true;
	prog.string = "";
	hhC.setcallbacks(NETPROTO_hhStartReply, NETPROTO_hhReadReply, NETPROTO_hhEndReply, &prog);

	hhC.putrequest("POST", Format("/posts.php?login=%s&uid=%d", si->Login.c_str(), si->UID).c_str());
	hhC.putheader("Connection", "close");
	hhC.putheader("Content-Length", post_size);
	hhC.putheader("Content-Type", "multipart/form-data; boundary=----WebKitFormBoundaryePkpFF7tjBAqx29L");
	hhC.putheader("Accept", "text/plain");
	hhC.endheaders();

	try
	{
		//hhC.send((const unsigned char*)post_data.data(), post_data.size());
		hhC.send((unsigned const char*)pre.data(), pre.size());
		hhC.send(buf, size);
		buf = NULL;
		hhC.send((unsigned const char*)post.data(), post.size());

		while(hhC.outstanding())
			hhC.pump();
	}
	catch( happyhttp::Wobbly ex )
	{
		p = Packet();
		p.WriteUInt32(0x5C0EE250);
		p.WriteString(si->Login);
		p.WriteUInt32(si->UID);
		p.WriteUInt8(1);
		p.WriteString(post_reply);
		SOCK_SendPacket(s, p, 24);
		SOCK_Destroy(s);
		if (buf) free(buf);
		delete si;
		return 0;
	}

	post_reply = prog.string;
	if (buf) free(buf);

	//zxmgr::WriteChat("screenshot: finished");
	// once we're done, signal end to the hat
	p = Packet();
	p.WriteUInt32(0x5C0EE250);
	p.WriteString(si->Login);
	p.WriteUInt32(si->UID);
	p.WriteUInt8(1);
	p.WriteString(post_reply);
	SOCK_SendPacket(s, p, 24);
	SOCK_Destroy(s);
	delete si;
	return 0;
}

ScreenshotInfo* LastSI = NULL;
void _stdcall ProcLastSI();

#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
bool Cl_ProcessServerPacket(Packet& pack)
{
	uint8_t type = pack.ReadUInt8();
	if (type == 0x01) // screenshot
	{
		std::string login = pack.ReadString();
		uint32_t uid = pack.ReadUInt32();
		RECT* rcSrc2 = (RECT*)(0x0062B5D8);
		ScreenshotInfo* si = new ScreenshotInfo();
		si->Login = login;
		si->UID = uid;
		si->Width = rcSrc2->right - rcSrc2->left;
		si->Height = rcSrc2->bottom - rcSrc2->top;
		si->Data = new uint16_t[si->Width*si->Height];
		si->Flip = false;

		bool iconic = IsIconic(zxmgr::GetHWND());

		LastSI = si;
		if (iconic)
			ProcLastSI(); // iconic = we can try reading from buffer right now

		return true;
	}

	return false;
}

void _stdcall ProcLastSI()
{
	if (LastSI)
	{
		//zxmgr::WriteChat("scr: deferred activated");
		ScreenshotInfo* si = LastSI;
		//memcpy(si->Data, *(uint16_t**)(0x0062571C), si->Width*si->Height*2);

		uint8_t* Data = (uint8_t*)si->Data;
		uint8_t* SData = *(uint8_t**)(0x0062571C);
		uint32_t SPitch = *(int32_t*)(0x00625708);

		for (int y = 0; y < si->Height; y++)
		{
			uint8_t* SDataT = SData + y * SPitch;
			for (int x = 0; x < si->Width; x++)
			{
				*Data++ = *SDataT++;
				*Data++ = *SDataT++;
			}
		}

		HANDLE hSSThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Cl_ScreenshotThread, si, 0, NULL);
		LastSI = NULL;
	}
}

void __declspec(naked) imp_MakeScreenshotNet()
{
	__asm
	{	// 457FF4
		push	eax
		push	ebx
		push	ecx
		push	edx
		push	edi
		push	esi
		call	ProcLastSI
		pop		esi
		pop		edi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		mov		ecx, 0x0062565C
		sub		dword ptr [ecx], 1
		mov		ecx, 0x00458003
		jmp		ecx
	}
}