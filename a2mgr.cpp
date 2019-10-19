#define _WIN32_WINNT 0x500
#include <windows.h>
#include <psapi.h>
#include "utils.h"
#include "debug.h"
#include <cstring>
#include <ctime>

#include "a2mgr.h"

#include "config.h"
#include "language.h"
#include "pack_scrolling.h"
#include "CRC_32.h"

#include "File.h"

#include "network_protocol_ext.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

using namespace std;

#pragma comment(lib, "winmm")
#pragma comment(lib, "psapi")

unsigned long u_pid = 0;

char* u_pid_string = NULL;
char* u_pid_string_delete = NULL;

BOOL DirectoryExists(const char* dirName)
{
	DWORD attribs = ::GetFileAttributesA(dirName);
	if(attribs == INVALID_FILE_ATTRIBUTES) return false;
	return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

char exe_path[_MAX_PATH];
char dll_path[_MAX_PATH];

BOOL _stdcall BASE_setCurrentDirectory1(char* newd)
{
	std::string curdir = exe_path;
	size_t whd = curdir.find_last_of("\\");
	if(whd != std::string::npos)
		curdir.erase(whd);

	return SetCurrentDirectory(curdir.c_str());
}

#include "lib/sha1.h"

string DumpSHA(unsigned char buf[])
{
	string string2 = "";
	for(int i = 0; i < 20; i++)
		string2 += Format("%02x", buf[i]);
	return string2;
}

bool SDLInitialized = false;
HANDLE SDLInitMutex = NULL;
bool InitSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		log_format("ERROR: Failed to initialize SDL video (%s)\n", SDL_GetError());
		return false;
	}
	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF) !=
				(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF)) {
		log_format("ERROR: Failed to initialize SDL image (%s)\n", IMG_GetError());
		return false;
	}
	if (TTF_Init() == -1) {
		log_format("ERROR: Failed to initialize SDL-ttf (%s)\n", TTF_GetError());
		return false;
	}
	return true;
}

void TryInitSDL()
{
	if (!SDLInitialized)
	{
		if (!SDLInitMutex) SDLInitMutex = CreateMutex(NULL, TRUE, NULL);
		else WaitForSingleObject(SDLInitMutex, INFINITE);
		if (!SDLInitialized)
		{
			InitSDL();
			SDLInitialized = true;
		}
		ReleaseMutex(SDLInitMutex);
	}
}

bool _stdcall DllMain_Init(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	//__network_ext_Initialize();

	z_handle = (unsigned long)hModule;

	memset(exe_path, 0, _MAX_PATH);
	memset(dll_path, 0, _MAX_PATH);

	GetModuleFileName(NULL, exe_path, _MAX_PATH-1);
	GetModuleFileName(hModule, dll_path, _MAX_PATH-1);

	string basep = Dirname(exe_path);
	SetCurrentDirectory((basep+".").c_str());
	string patchr = "patch.res";
	string worldr = "world.res";
	//string graphr = "graphics.res";

	if(sha1_file(exe_path, sha1_allods2_exe) == POLARSSL_ERR_SHA1_FILE_IO_ERROR) memset(sha1_allods2_exe, 0, 20);
	if(sha1_file(dll_path, sha1_a2mgr_dll) == POLARSSL_ERR_SHA1_FILE_IO_ERROR) memset(sha1_a2mgr_dll, 0, 20);
	if(sha1_file(patchr.c_str(), sha1_patch_res) == POLARSSL_ERR_SHA1_FILE_IO_ERROR) memset(sha1_patch_res, 0, 20);
	if(sha1_file(worldr.c_str(), sha1_world_res) == POLARSSL_ERR_SHA1_FILE_IO_ERROR) memset(sha1_world_res, 0, 20);
	//if(sha1_file(graphr.c_str(), sha1_graphics_res) == POLARSSL_ERR_SHA1_FILE_IO_ERROR) memset(sha1_graphics_res, 0, 20);

	ParseConfig();
	if(z_logsdir && !DirectoryExists("logs")) CreateDirectory("logs", NULL);
	if(!DirectoryExists("maps")) CreateDirectory("maps", NULL);
	if(z_screendir && !DirectoryExists("screenshots")) CreateDirectory("screenshots", NULL);
	string wherelog = "allods2";
	if(z_logsdir)
	{
		time_t t = time(NULL);
		struct tm *tm = localtime(&t);
		if(tm) wherelog = Format("logs\\allods2_%02u-%02u-%04u", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);
		else wherelog = "logs\\allods2";
	}

	u_pid = GetProcessId(GetCurrentProcess());

	char procnamet[MAX_PATH];
	GetModuleFileName(0, procnamet, MAX_PATH);
	string tstrt = Basename(procnamet);

	DWORD processLists[1024];
	DWORD processListsSize = sizeof(processLists) * sizeof(DWORD);
	DWORD processListsRet = 0;
	if(!EnumProcesses(processLists, processListsSize, &processListsRet))
		processListsRet = 0;
	processListsRet = processListsRet / sizeof(DWORD);
	unsigned long procsiz = 0;
	for(DWORD i = 0; i < processListsRet; i++)
	{
		HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processLists[i]);
		if(!proc) continue;
		char procname[MAX_PATH];
		GetModuleFileNameEx(proc, 0, procname, MAX_PATH);
		string tstr = Basename(procname);
		if(tstr == tstrt) procsiz++;
		CloseHandle(proc);
	}
	if(procsiz) procsiz--;
	if(procsiz) wherelog += Format(".%u", u_pid);
	wherelog += ".log";
	bool pre_exists = FileExists(wherelog);
	_LOG_NAME = wherelog;
	if(pre_exists) log_format2("\n");
	log_format("Rage of Mages II (v1.07)\n");
	log_format("a2mgr (v1.13)/zxmgr (v2.0) started.\n");
	setup_handler();

	//log_format("SHA1 (allods2.exe):   %s\n", DumpSHA(sha1_allods2_exe).c_str());
	//log_format("SHA1 (a2mgr.dll):     %s\n", DumpSHA(sha1_a2mgr_dll).c_str());
	//log_format("SHA1 (patch.res):     %s\n", DumpSHA(sha1_patch_res).c_str());
	//log_format("SHA1 (world.res):     %s\n", DumpSHA(sha1_world_res).c_str());
	//log_format("SHA1 (graphics.res):  %s\n", DumpSHA(sha1_graphics_res).c_str());

	char tmpUid[64];
	tmpUid[0] = 0;
	HANDLE tmpHandle = FindFirstVolumeA(tmpUid, sizeof(tmpUid));
	if(tmpHandle) sha1((const unsigned char*)tmpUid, strlen(tmpUid), sha1_uuid);
	else memset(sha1_uuid, 0, 20);
	if(tmpHandle) FindVolumeClose(tmpHandle);

	u_pid_string = new char[128];
	u_pid_string[127] = 0;
	_snprintf(u_pid_string, 127, "allods-%u-%%u.$$$", u_pid);

	u_pid_string_delete = new char[128];
	u_pid_string_delete[127] = 0;
	_snprintf(u_pid_string_delete, 127, "%%sallods-%u-*.$$$", u_pid);

	DecideLanguage();

	PSCRL_InitializeInjection();

/*
	Archives.Open("./graphics.res", "graphics");
	Archives.Open("./main.res", "main");
	Archives.Open("./patch.res", "patch");*/

	char* commandline_raw = GetCommandLineA();
	vector<string> parms = ParseCommandLine(commandline_raw);

	for (size_t i = 0; i < parms.size(); i++)
	{
		log_format("arg: %s\n", parms[i].c_str());
		if (parms[i] == "-delay" &&
			i+1 < parms.size())
		{
			int delay = StrToInt(parms[i+1]);
			if (delay)
				Sleep(delay); // before we try to initialize video mode, etc. to not make the old allods be like wtf.
		}
	}

	return true;
}

void _stdcall DllMain_Shutdown(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	log_format("a2mgr closed.\n");
	if(u_pid_string_delete) delete u_pid_string_delete;
	if(u_pid_string) delete u_pid_string;
}

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch(ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
				if(!DllMain_Init(hModule, ul_reason_for_call, lpReserved))
					return false;
				break;
			}
		case DLL_PROCESS_DETACH:
			{
				DllMain_Shutdown(hModule, ul_reason_for_call, lpReserved);
				break;
			}
	}
	return TRUE;
}

void __declspec(naked) _fix_launcher()
{
	__asm
	{
		cmp		[ebp-0x04], 0
		jz		loc_58203E

		mov		edx, 0x0058200D
		jmp		edx

loc_58203E:
		mov		edx, 0x0058203E
		jmp		edx
	}
}