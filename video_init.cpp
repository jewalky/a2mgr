#include <windows.h>
#include <string>
#include <vector>
#include "utils.h"
#include "config.h"

using namespace std;

int rwid, rhei;

void _stdcall get_res(int *w, int *h)
{
	// absolute default
	*w = 640;
	*h = 480;

	if(!z_resolution)
	{
		// "smart" default, video mode guessed from desktop video mode
		DEVMODE mode;
		mode.dmSize = sizeof(mode);
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode);

		if(mode.dmPelsWidth >= 1024 && mode.dmPelsHeight >= 768)
		{
			*w = 1024;
			*h = 768;
		}
		else if(mode.dmPelsWidth >= 800 && mode.dmPelsHeight >= 600)
		{
			*w = 800;
			*h = 600;
		}
	}
	else
	{
		*w = r_width;
		*h = r_height;
	}

	char* commandline_raw = GetCommandLineA();
	vector<string> parms = ParseCommandLine(commandline_raw);

	for(vector<string>::iterator i = parms.begin(); i != parms.end(); ++i)
	{
		if((*i) == "-640")
		{
			*w = 640;
			*h = 480;
		}
		else if((*i) == "-800")
		{
			*w = 800;
			*h = 600;
		}
		else if((*i) == "-1024")
		{
			*w = 1024;
			*h = 768;
		}
		else if((*i).find("--res:") == 0)
		{
			const char* tmpr = (*i).c_str();
			int w1, h1;
			if(sscanf(tmpr, "--res:%dx%d", &w1, &h1) == 2)
			{
				*w = w1;
				*h = h1;
			}
		}
	}

	rwid = *w;
	rhei = *h;
}

void _declspec(naked) VIDEO_checkVideoMode(void) 
{
	__asm {
		lea		ecx, dword ptr [ebp-0x014] // H
		push	ecx
		lea		ecx, dword ptr [ebp-0x010] // W
		push	ecx
		call	get_res
		test	eax, eax
		mov		edx, 0x00487757
		jmp		edx 
	}
}