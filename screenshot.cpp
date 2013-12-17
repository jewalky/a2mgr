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

void SCREENSHOT_take()
{
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