#include "utils.h"
#include "zxmgr.h"
#include <windows.h>
#include <GL/GL.h>

#ifndef GL_TEXTURE_RECTANGLE_ARB
    #define GL_TEXTURE_RECTANGLE_ARB 0x84F5
#endif

#ifndef GL_BGRA
    #define GL_BGRA 0x80E1
#endif

#ifndef GL_COMBINE
    #define GL_COMBINE 0x8570
#endif

#ifndef GL_COMBINE_RGB
    #define GL_COMBINE_RGB 0x8571
#endif

#ifndef GL_COMBINE_ALPHA
    #define GL_COMBINE_ALPHA 0x8572
#endif

#ifndef GL_UNSIGNED_SHORT_5_6_5
	#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#endif

extern int rwid, rhei;

#pragma comment (lib, "opengl32")

unsigned short ddP[1310720];

bool r_openGLReady = false;
HDC r_hDC = 0;
HGLRC r_hRC = 0;

RECT r_viewport;

GLuint r_texture = 0;

void GL_SetViewport(int x, int y, int w, int h)
{
	r_viewport.left = x;
	r_viewport.top = y;
	r_viewport.right = x+w;
	r_viewport.bottom = y+h;

    int y2 = rhei - (h + y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(x, y2, w, h);
    glOrtho(x, x+w, y+h, y, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void _stdcall GL_ResetViewport()
{
	GL_SetViewport(0, 0, rwid, rhei);
}

void _stdcall OpenGLProc()
{
	unsigned short* pixelsPtr = *(unsigned short**)(0x0062571C);
	
	if(!r_openGLReady)
	{
		HWND hWnd = zxmgr::GetHWND();
		r_hDC = GetDC(hWnd);
		PIXELFORMATDESCRIPTOR pfd;
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 16;
		pfd.cDepthBits = 0;
		pfd.iLayerType = PFD_MAIN_PLANE;
		int iFormat = ChoosePixelFormat(r_hDC, &pfd);
		SetPixelFormat(r_hDC, iFormat, &pfd);
		r_hRC = wglCreateContext(r_hDC);
		log_format("r_hDC = %08X, format = %d, r_hRC = %08X\n", r_hDC, iFormat, r_hRC);
		//ReleaseDC(hWnd, r_hDC);
		bool okay = wglMakeCurrent(r_hDC, r_hRC);
		log_format("wglMakeCurrent = %02X\n", okay);

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// create main texture
		glGenTextures(1, &r_texture);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, r_texture);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, rwid, rhei, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixelsPtr);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

		r_openGLReady = true;
	}

	wglMakeCurrent(r_hDC, r_hRC);
	GL_ResetViewport();

	if(r_texture)
	{
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, r_texture);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, rwid, rhei, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixelsPtr);

		glColor4ub(255, 255, 255, 255);
		glBegin(GL_QUADS);
			glTexCoord2i(0, 0);
			glVertex2i(0, 0);
			glTexCoord2i(rwid, 0);
			glVertex2i(rwid, 0);
			glTexCoord2i(rwid, rhei);
			glVertex2i(rwid, rhei);
			glTexCoord2i(0, rhei);
			glVertex2i(0, rhei);
		glEnd();

		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	}

	/*glBegin(GL_QUADS);
		glColor4ub(0, 0, 255, 255);
		glVertex2i(32, 32);
		glVertex2i(32, 64);
		glVertex2i(64, 64);
		glVertex2i(64, 32);
	glEnd();*/

	SwapBuffers(r_hDC);
}

void __declspec(naked) old_sub_unlock()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		push	ecx
		mov		edx, 0x00631770
		cmp		dword ptr [edx], 0
		mov		eax, 0x00457FC3
		jmp		eax
	}
}

unsigned short prevArrSaved[327680];

void _stdcall ConsoleProc(bool load, int locks)
{
	if(locks > 1) return;
	unsigned short* pixelsPtr = *(unsigned short**)(0x0062571C);
	if(!load)
	{
		memcpy(prevArrSaved, pixelsPtr, 327680*2);
		memset(pixelsPtr, 0, 327680*2);
	}
	else
	{
		memcpy(pixelsPtr, prevArrSaved, 327680*2);
	}
}

void __declspec(naked) GL_blockOldUpdate1()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		push	ecx
		mov		edx, 0x00631770
		cmp		dword ptr [edx], 0
		jz		bou1_unlock
		xor		eax, eax
		jmp		bou1_exit

bou1_unlock:
		mov		edx, 0x0062565C
		cmp		dword ptr [edx], 0
		jz		bou1_break

		push	dword ptr [edx]
		push	0
		call	ConsoleProc

		mov		eax, 0x0062571C
		push	dword ptr [eax]
		mov		ecx, 0x006256F0
		mov		ecx, [ecx]
		mov		edx, [ecx]
		push	ecx
		call	[edx+0x80]
		neg		eax
		sbb		eax, eax
		inc		eax
		mov		[ebp-0x04], eax

		mov		edx, 0x0062565C
		dec		dword ptr [edx]

		// unlocked
		mov		edx, 0x0062565C
		push	dword ptr [edx]
		push	1
		call	ConsoleProc

bou1_break:
		mov		eax, [ebp-0x04]

bou1_exit:
		mov		esp, ebp
		pop		ebp
		retn
	}
}

void __declspec(naked) GL_blockOldUpdate2()
{
	__asm retn; // stub
}

void __declspec(naked) GL_blockOldUpdate3()
{
	__asm retn; // stub
}

void __declspec(naked) GL_setFlags()
{
	__asm retn; // stub
}