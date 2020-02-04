#include "a2mgr.h"
#include "Image.h"
#include <windows.h>
#include "zxmgr.h"
#include "lib\utils.hpp"
#include "File.h"

#include <atlbase.h>
#include <gdiplus.h>

Image::Image(std::string filename)
{

	TryInitGraphics();

	myPixels = NULL;
	myWidth = 0;
	myHeight = 0;

	File fil;
	if(!fil.Open(filename))
	{
		// produce system error
		MessageBoxA(zxmgr::GetHWND(), Format("FATAL ERROR: can't load %s (not found)", filename.c_str()).c_str(),
			"Allods2", MB_ICONWARNING | MB_OK);
		zxmgr::AfxAbort();
		return;
	}

	std::vector<uint8_t> buffer;
	buffer.resize(fil.GetLength());
	fil.Seek(0);
	uint32_t count_read = fil.Read(buffer.data(), buffer.size());
	fil.Close();

	IStream* stream = SHCreateMemStream(buffer.data(), buffer.size());
	Gdiplus::Bitmap* image = Gdiplus::Bitmap::FromStream(stream);
	
	stream->Release();

	if (!image || image->GetWidth() <= 0 || image->GetHeight() <= 0)
	{
		MessageBoxA(zxmgr::GetHWND(), Format("FATAL ERROR: can't load %s (GDI failed)", filename.c_str()).c_str(),
			"Allods2", MB_ICONWARNING | MB_OK);
		zxmgr::AfxAbort();
		return;
	}

	myWidth = image->GetWidth();
	myHeight = image->GetHeight();
	myPixels = new uint32_t[myWidth * myHeight];

	Gdiplus::BitmapData bitmapData;
	memset(&bitmapData, 0, sizeof(bitmapData));
	Gdiplus::Rect bitmapRect;
	bitmapRect.X = bitmapRect.Y = 0;
	bitmapRect.Width = myWidth;
	bitmapRect.Height = myHeight;
	if (image->LockBits(&bitmapRect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData) != Gdiplus::Ok)
	{
		MessageBoxA(zxmgr::GetHWND(), Format("FATAL ERROR: can't load %s (LockBits failed)", filename.c_str()).c_str(),
			"Allods2", MB_ICONWARNING | MB_OK);
		zxmgr::AfxAbort();
		return;
	}

	// now the problem here is that bitmapData is not guaranteed to be in the format we want (orientation, stride)
	// as such, we convert line by line and Y-flip
	uint32_t* myptr = myPixels;
	BYTE* gdiptr = (BYTE*)bitmapData.Scan0;
	for (int y = 0; y < myHeight; y++)
	{
		memcpy(myptr, gdiptr, sizeof(uint32_t) * myWidth);
		myptr += myWidth;
		gdiptr += bitmapData.Stride;
	}

	image->UnlockBits(&bitmapData);
	delete image;

}

Image::~Image()
{
	if(myPixels) delete[] myPixels;
	myPixels = NULL;
	myWidth = 0;
	myHeight = 0;
}

void Image::Display(int16_t x, int16_t y)
{
	DisplayEx(x, y, 0, 0, myWidth, myHeight, false);
}

#define SCALENUM(NUM, ACTUAL, MAX) (NUM ? (NUM * ACTUAL / MAX) : 0)

void Image::DisplayEx(int16_t x, int16_t y, int16_t inX, int16_t inY, int16_t w, int16_t h, bool no_alpha)
{
	if(!myPixels) return;
	if(*(uint32_t*)(0x00631770)) return;

	uint16_t* pPixels = *(uint16_t**)(0x0062571C);
	uint32_t pPitch = *(uint32_t*)(0x00625708);

	if(x < 0)
	{
		int16_t oldX = x;
		x = 0;
		inX = -oldX;
	}

	if(y < 0)
	{
		int16_t oldY = y;
		y = 0;
		inY = -oldY;
	}

	if(inX+w > myWidth)
		w = (myWidth-inX);
	if(inY+h > myHeight)
		h = (myHeight-inY);

	RECT pRect = *(RECT*)(0x00625768);
	if(x > pRect.right ||
		y > pRect.bottom) return;
	if(x+w < pRect.left ||
		y+h < pRect.top) return;

	int32_t pTotalWidth = pPitch / 2;
	int32_t inBorderLeft = x;
	int32_t inBorderRight = pTotalWidth-(w+inBorderLeft);

	if(inBorderLeft < 0 ||
		inBorderRight < 0) return;

	int32_t rBorderLeft = inX;
	int32_t rBorderRight = myWidth-(w+rBorderLeft);
	uint32_t* rPixels = myPixels;

	pPixels += y * pTotalWidth;
	rPixels += inY * myWidth;
	for(int y = 0; y < h; y++)
	{
		rPixels += rBorderLeft;
		pPixels += inBorderLeft;
		for(int x = 0; x < w; x++)
		{
			uint32_t src_color = *rPixels++;
			uint8_t src_a = (src_color & 0xFF000000) >> 24;
			uint8_t src_r = (src_color & 0x00FF0000) >> 16;
			uint8_t src_g = (src_color & 0x0000FF00) >> 8;
			uint8_t src_b = (src_color & 0x000000FF);
			
			// preprocess for 16bit
			src_r >>= 3;
			src_b >>= 3;
			src_g >>= 2;

			if(no_alpha)
			{
				if(src_a > 127) src_a = 255;
				else src_a = 0;
			}

			if(!src_a)
			{
				pPixels++;
				continue;
			}

			uint16_t dst_color = *pPixels;
			uint8_t dst_r = (dst_color & 0xF800) >> 11;
			uint8_t dst_g = (dst_color & 0x07E0) >> 5;
			uint8_t dst_b = (dst_color & 0x001F);
			// max for blue&red = 31.0
			// max for green = 63.0
			
			uint8_t out_r;
			uint8_t out_g;
			uint8_t out_b;

			if(src_a != 255)
			{
				//int alpha5 = src_a * 31 / 255;
				//int alpha6 = src_a * 63 / 255;
				
				float alpha = (float)(src_a) / 255.0;
				float Fout_r = (src_r * alpha) + (dst_r * (1.0-alpha));
				float Fout_g = (src_g * alpha) + (dst_g * (1.0-alpha));
				float Fout_b = (src_b * alpha) + (dst_b * (1.0-alpha));

				
				//out_r = min(SCALENUM(src_r, alpha5, 0x1F)+SCALENUM(dst_r, 31-alpha5, 0x1F), 0x1F);
				//out_g = min(SCALENUM(src_g, alpha6, 0x3F)+SCALENUM(dst_g, 63-alpha6, 0x3F), 0x3F);
				//out_b = min(SCALENUM(src_b, alpha5, 0x1F)+SCALENUM(dst_b, 31-alpha5, 0x1F), 0x1F);

				out_r = min(Fout_r, 0x1F);
				out_g = min(Fout_g, 0x3F);
				out_b = min(Fout_b, 0x1F);
			}
			else
			{
				out_r = src_r;
				out_g = src_g;
				out_b = src_b;
			}

			dst_color = (out_r & 0x1F) << 11;
			dst_color |= (out_g & 0x3F) << 5;
			dst_color |= (out_b);
			*pPixels++ = dst_color;
		}
		rPixels += rBorderRight;
		pPixels += inBorderRight;
	}
}

uint32_t Image::GetWidth()
{
	return myWidth;
}

uint32_t Image::GetHeight()
{
	return myHeight;
}

uint32_t Image::GetPixelAt(int32_t x, int32_t y)
{
	if(!myPixels) return 0;
	if(x < 0 || x >= myWidth) return 0;
	if(y < 0 || y >= myHeight) return 0;
	return myPixels[y*myWidth+x];
}

uint32_t* Image::GetPixels()
{
	return myPixels;
}

// this is the private struct used in ROMFont
struct ROMFontData
{

	Gdiplus::PrivateFontCollection* fontCollection;
	Gdiplus::FontFamily family;
	Gdiplus::Font* font;

	int fontSize;

	bool wasBold;
	bool wasItalic;
	
	bool isBold;
	bool isItalic;

	Gdiplus::Font* GetFont()
	{
		if (!font || isBold != wasBold || isItalic != wasItalic)
		{
			if (font) delete font;
			int fs = 0;
			if (isBold) fs |= Gdiplus::FontStyleBold;
			if (isItalic) fs |= Gdiplus::FontStyleItalic;
			font = new Gdiplus::Font(&family, fontSize, fs, Gdiplus::UnitPixel);
		}

		return font;
	}

};

static std::vector<wchar_t> InternalStringToWCHAR(std::string s)
{
	std::vector<wchar_t> ws;
	ws.resize(s.length() + 1);
	ws[s.length()] = 0;
	MultiByteToWideChar(866, MB_PRECOMPOSED, s.data(), -1, ws.data(), ws.size());
	return ws;
}

ROMFont::ROMFont(std::string filename, int size)
{

	TryInitGraphics();

	std::vector<wchar_t> ws = InternalStringToWCHAR(filename);

	ROMFontData* data = new ROMFontData();
	data->fontCollection = new Gdiplus::PrivateFontCollection();
	if (data->fontCollection->AddFontFile(ws.data()) != Gdiplus::Ok)
	{
		delete data;
		MessageBoxA(zxmgr::GetHWND(), Format("FATAL ERROR: can't load %s (AddFontFile failed)", filename.c_str()).c_str(),
			"Allods2", MB_ICONWARNING | MB_OK);
		zxmgr::AfxAbort();
		return;
	}

	// read one font family.
	int totalFamilies;
	if (data->fontCollection->GetFamilies(1, &data->family, &totalFamilies) != Gdiplus::Ok)
	{
		delete data;
		MessageBoxA(zxmgr::GetHWND(), Format("FATAL ERROR: can't load %s (GetFamilies failed)", filename.c_str()).c_str(),
			"Allods2", MB_ICONWARNING | MB_OK);
		zxmgr::AfxAbort();
		return;
	}

	data->fontSize = size;
	data->font = NULL;
	data->wasBold = data->isBold = false;
	data->wasItalic = data->isItalic = false;

	myData = data;

}

bool ROMFont::IsBold()
{
	ROMFontData* data = static_cast<ROMFontData*>(myData);
	if (!data) return false;
	return data->isBold;
}

bool ROMFont::IsItalic()
{
	ROMFontData* data = static_cast<ROMFontData*>(myData);
	if (!data) return false;
	return data->isItalic;
}

void ROMFont::SetBold(bool bold)
{
	ROMFontData* data = static_cast<ROMFontData*>(myData);
	if (!data) return;
	data->isBold = bold;
}

void ROMFont::SetItalic(bool italic)
{
	ROMFontData* data = static_cast<ROMFontData*>(myData);
	if (!data) return;
	data->isItalic = italic;
}

ROMFont::~ROMFont()
{
	if (myData)
	{
		ROMFontData* data = static_cast<ROMFontData*>(myData);
		delete data;
		myData = NULL;
	}
}

Image* Image::RenderText(ROMFont* font, std::string text, int r, int g, int b)
{

	// get graphics for HWND. we will not use this for drawing
	Gdiplus::Graphics graphicsHWND(zxmgr::GetHWND());

	// so the problem is that we don't know the exact dimensions of this text.
	// we need to first guess this
	ROMFontData* data = static_cast<ROMFontData*>(font->myData);
	Gdiplus::Font* gdifont = data->GetFont();
	
	std::vector<wchar_t> ws = InternalStringToWCHAR(text);
	Gdiplus::RectF measuredSize;
	if (graphicsHWND.MeasureString(ws.data(), ws.size(), gdifont, Gdiplus::PointF(0, 0), &measuredSize) != Gdiplus::Ok)
		return NULL;

	int outputWidth = ceil(measuredSize.Width);
	int outputHeight = ceil(measuredSize.Height);
	if (outputWidth < 1)
		outputWidth = 1;
	if (outputHeight < 1)
		outputHeight = 1;

	Gdiplus::Bitmap bmp(outputWidth, outputHeight);
	Gdiplus::Graphics graphics(&bmp);

	graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	Gdiplus::SolidBrush brush(Gdiplus::Color(255, r, g, b));
	if (graphics.DrawString(ws.data(), ws.size(), gdifont, Gdiplus::PointF(0, 0), &brush) != Gdiplus::Ok)
		return NULL;

	// convert image from bitmap
	Gdiplus::BitmapData bitmapData;
	Gdiplus::Rect bitmapRect(0, 0, outputWidth, outputHeight);
	if (bmp.LockBits(&bitmapRect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData) != Gdiplus::Ok)
		return NULL;

	Image* img = new Image();
	img->myWidth = outputWidth;
	img->myHeight = outputHeight;
	img->myPixels = new uint32_t[outputWidth * outputHeight];
	
	BYTE* gdiptr = (BYTE*)bitmapData.Scan0;
	uint32_t* myptr = img->myPixels;

	for (int y = 0; y < outputHeight; y++)
	{
		memcpy(myptr, gdiptr, sizeof(uint32_t) * outputWidth);
		myptr += outputWidth;
		gdiptr += bitmapData.Stride;
	}

	bmp.UnlockBits(&bitmapData);
	return img;

}

int Image::RenderTextWidth(ROMFont* font, std::string text)
{
	
	// get graphics for HWND. we will not use this for drawing
	Gdiplus::Graphics graphicsHWND(zxmgr::GetHWND());

	ROMFontData* data = static_cast<ROMFontData*>(font->myData);
	Gdiplus::Font* gdifont = data->GetFont();

	std::vector<wchar_t> ws = InternalStringToWCHAR(text);
	Gdiplus::RectF measuredSize;
	if (graphicsHWND.MeasureString(ws.data(), ws.size(), gdifont, Gdiplus::PointF(0, 0), &measuredSize) != Gdiplus::Ok)
		return 0;

	int outputWidth = ceil(measuredSize.Width);
	if (outputWidth < 1)
		outputWidth = 1;
	
	return outputWidth-3;

}