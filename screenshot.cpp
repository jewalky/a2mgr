#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>

#include <cstring>
#include <ctime>

#include "utils.h"
#include "debug.h"

#include "config.h"

#include "zxmgr.h"

#pragma comment(lib, "gdiplus")

using namespace Gdiplus;
using namespace std;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

#include "lib/lodepng.h"
#include "lib/stdint.h"

void SCREENSHOT_takeManual()
{
	RECT* rcSrc2 = (RECT*)(0x0062B5D8);

	int siWidth = rcSrc2->right - rcSrc2->left;
	int siHeight = rcSrc2->bottom - rcSrc2->top;

	uint16_t* siData = new uint16_t[siWidth*siHeight];

	uint8_t* Data = (uint8_t*)siData;
	uint8_t* SData = *(uint8_t**)(0x0062571C);
	uint32_t SPitch = *(int32_t*)(0x00625708);

	for (int y = 0; y < siHeight; y++)
	{
		uint8_t* SDataT = SData + y * SPitch;
		for (int x = 0; x < siWidth; x++)
		{
			*Data++ = *SDataT++;
			*Data++ = *SDataT++;
		}
	}

	uint32_t* ndata = new uint32_t[siWidth*siHeight];
	uint32_t* ndatap = ndata;

	uint16_t* orp = siData;
	bool siFlip = false;

	for (int y = siFlip ? siHeight-1 : 0; siFlip ? (y >= 0) : (y < siHeight); siFlip ? y-- : y++)
	{
		uint32_t* rowp = ndatap + y * siWidth;
		for (int x = 0; x < siWidth; x++)
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

	delete[] siData;

	std::string post_reply = "";

	unsigned char* buf = NULL;
	size_t size = 0;
	unsigned int ov = lodepng_encode32(&buf, &size, (const unsigned char*)ndata, siWidth, siHeight);
	delete[] ndata;

	// buf and size is the screenshot
	string str = "screenshot";
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	if(tm)
	{
		str = Format("screenshot_%02u-%02u-%04u_%02u-%02u-%02u", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
		if(z_screendir) str = "screenshots\\" + str;
	}

	str += ".png";

	FILE* outf = fopen(str.c_str(), "wb");
	fwrite(buf, size, 1, outf);
	fclose(outf);

	char* strc = zxmgr::GetPatchString(226);
	zxmgr::WriteChat(strc, str.c_str());
}

void SCREENSHOT_take()
{
	SCREENSHOT_takeManual();
	return;

	RECT rcSrc2(*(RECT*)(0x0062B5D8));

	string strtype;
	switch(v_screentype)
	{
	default:
	case SCREENTYPE_PNG:
		strtype = "png";
		break;
	case SCREENTYPE_GIF:
		strtype = "gif";
		break;
	case SCREENTYPE_BMP:
		strtype = "bmp";
		break;
	case SCREENTYPE_JPG:
		strtype = "jpg";
		break;
	}

	WCHAR* fname = new WCHAR[256];
	memset(fname, 0, 256*sizeof(WCHAR));
	string str = "screenshot";
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	if(tm)
	{
		str = Format("screenshot_%02u-%02u-%04u_%02u-%02u-%02u", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
		if(z_screendir) str = "screenshots\\" + str;
	}

	str += "." + strtype;

	mbstowcs(fname, str.c_str(), str.length());

	using namespace Gdiplus;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{
		HDC scrdc, memdc;
		HBITMAP membit;
		scrdc = ::GetDC(0);
		int Height = rcSrc2.bottom - rcSrc2.top;
		int Width = rcSrc2.right - rcSrc2.left;
		memdc = CreateCompatibleDC(scrdc);
		membit = CreateCompatibleBitmap(scrdc, Width, Height);
		HBITMAP hOldBitmap =(HBITMAP) SelectObject(memdc, membit);
		BitBlt(memdc, 0, 0, Width, Height, scrdc, rcSrc2.left, rcSrc2.top, SRCCOPY);
		Gdiplus::Bitmap bitmap(membit, NULL);
		CLSID clsid;
		switch(v_screentype)
		{
		default:
		case SCREENTYPE_PNG:
			GetEncoderClsid(L"image/png", &clsid);
			break;
		case SCREENTYPE_GIF:
			GetEncoderClsid(L"image/gif", &clsid);
			break;
		case SCREENTYPE_BMP:
			GetEncoderClsid(L"image/bmp", &clsid);
			break;
		case SCREENTYPE_JPG:
			GetEncoderClsid(L"image/jpeg", &clsid);
			break;
		}
		bitmap.Save(fname, &clsid);
		SelectObject(memdc, hOldBitmap);
		DeleteObject(memdc);
		DeleteObject(membit);
		::ReleaseDC(0,scrdc);
	}
	GdiplusShutdown(gdiplusToken);

	char* strc = zxmgr::GetPatchString(226);
	zxmgr::WriteChat(strc, str.c_str());
}