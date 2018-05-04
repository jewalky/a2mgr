#include "Image.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <windows.h>
#include "zxmgr.h"
#include "lib\utils.hpp"
#include "File.h"

Image::Image(std::string filename)
{
	myPixels = NULL;
	myWidth = 0;
	myHeight = 0;

	File fil;
	if(!fil.Open(filename))
	{
		// produce system error
		MessageBoxA(zxmgr::GetHWND(), Format("FATAL ERROR: can't load %s", filename.c_str()).c_str(),
			"Allods2", MB_ICONWARNING | MB_OK);
		zxmgr::AfxAbort();
	}

	uint8_t* buffer = new uint8_t[fil.GetLength()];
	memset(buffer, 0, fil.GetLength());
	fil.Seek(0);
	uint32_t count_read = fil.Read(buffer, fil.GetLength());
	SDL_RWops* rw = SDL_RWFromMem(buffer, fil.GetLength());
	fil.Close();

	SDL_Surface* img = IMG_Load_RW(rw, false);
	if(!img || !img->w || !img->h)
	{
		if(img) SDL_FreeSurface(img);
		delete[] buffer;
		MessageBoxA(zxmgr::GetHWND(), Format("FATAL ERROR: can't load %s", filename.c_str()).c_str(),
			"Allods2", MB_ICONWARNING | MB_OK);
		zxmgr::AfxAbort();
	}

	SDL_PixelFormat pfd;
	pfd.palette = NULL;
	pfd.BitsPerPixel = 32;
	pfd.BytesPerPixel = 4;
	pfd.Rmask = 0xFF000000;
	pfd.Gmask = 0x00FF0000;
	pfd.Bmask = 0x0000FF00;
	pfd.Amask = 0x000000FF;
	pfd.Rshift = 24;
	pfd.Gshift = 16;
	pfd.Bshift = 8;
	pfd.Ashift = 0;
	pfd.Rloss = 0;
	pfd.Gloss = 0;
	pfd.Bloss = 0;
	pfd.Aloss = 0;
	pfd.alpha = 255;
	pfd.colorkey = 0;

	SDL_Surface* img_r = SDL_ConvertSurface(img, &pfd, 0);
	if(!img_r)
	{
		SDL_FreeSurface(img);
		delete[] buffer;
		MessageBoxA(zxmgr::GetHWND(), Format("FATAL ERROR: can't load %s", filename.c_str()).c_str(),
			"Allods2", MB_ICONWARNING | MB_OK);
		zxmgr::AfxAbort();
	}

	myWidth = img_r->w;
	myHeight = img_r->h;
	myPixels = new uint32_t[myWidth*myHeight];

	memcpy(myPixels, img_r->pixels, myWidth*myHeight*4);
	SDL_FreeSurface(img_r);
	delete[] buffer;
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
			uint8_t src_r = (src_color & 0xFF000000) >> 27;
			uint8_t src_g = (src_color & 0x00FF0000) >> 18;
			uint8_t src_b = (src_color & 0x0000FF00) >> 11;
			uint8_t src_a = (src_color & 0x000000FF);
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

Image* Image::RenderText(TTF_Font* font, std::string text, int r, int g, int b)
{
	Uint16* utext = new Uint16[text.length()+1];
	utext[text.length()] = 0;
	for (int i = 0; i < text.size(); i++)
	{
		unsigned long ch = (unsigned char)text[i];
		if (ch >= 0x80 && ch <= 0xAF)
			ch += 0x390;
		else if (ch >= 0xE0 && ch <= 0xEF)
			ch += 0x360;
		else if (ch == 0xF0)
			ch = 0x401;
		else if (ch == 0xF1)
			ch = 0x451;
		utext[i] = ch; // for now. note: do unicode translate later
	}

	SDL_Color c = {255,255,255}; SDL_Color bgc = {0,0,0};
	//SDL_Surface* srf = TTF_RenderUNICODE_Blended(font, utext, c);
	SDL_Surface* srf = TTF_RenderUNICODE_Shaded(font, utext, c, bgc);
	//SDL_Surface* srf = TTF_RenderUNICODE_Solid(font, utext, c);
	delete[] utext;
	
	if (!srf) return NULL;
	// copy data. srf->pixels should have rgba in it.
	Image* img = new Image();
	img->myWidth = srf->w;
	img->myHeight = srf->h;
	img->myPixels = new uint32_t[srf->w*srf->h];
	//memcpy(img->myPixels, srf->pixels, srf->w*srf->h*4);

	int cnt = srf->w*srf->h;
	uint8_t* pixels = (uint8_t*)img->myPixels;
	for (int y = 0; y < srf->h; y++)
	{
		uint8_t* opixels = ((uint8_t*)srf->pixels) + y * srf->pitch;
		for (int x = 0; x < srf->w; x++)
		{
			int cb = b;
			int cg = g;
			int cr = r;
			//int ca = !*opixels++?0:255;
			int ca = *opixels++;
			*pixels++ = ca;
			*pixels++ = cb;
			*pixels++ = cg;
			*pixels++ = cr;
		}
	}

	SDL_FreeSurface(srf);
	return img;
}

int Image::RenderTextWidth(TTF_Font* font, std::string text)
{
	//int TTF_SizeUNICODE(TTF_Font *font, const Unit16 *text, int *w, int *h)
	Uint16* utext = new Uint16[text.length()+1];
	utext[text.length()] = 0;
	for (int i = 0; i < text.size(); i++)
	{
		unsigned long ch = (unsigned char)text[i];
		if (ch >= 0x80 && ch <= 0xAF)
			ch += 0x390;
		else if (ch >= 0xE0 && ch <= 0xEF)
			ch += 0x360;
		else if (ch == 0xF0)
			ch = 0x401;
		else if (ch == 0xF1)
			ch = 0x451;
		utext[i] = ch; // for now. note: do unicode translate later
	}

	int w;
	int h;
	TTF_SizeUNICODE(font, utext, &w, &h);
	delete[] utext;
	return w;
}