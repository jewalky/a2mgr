#include "Image.h"
#include "File.h"
#include "utils.h"
#include <windows.h>

void* ipng_out_primary = NULL;
void* ipng_out_secondary = NULL;

void _stdcall IImage_CopyTo(int arg1, int arg2, int arg3, int arg4)
{
	char* c = NULL;
	__asm mov c, ecx;

	Image* image = *(Image**)(c+4);
	if(!image) return;
	if(image->GetWidth() != 160 ||
		image->GetHeight() != 240) return;

	uint8_t* upx = *(uint8_t**)(0x0062571C);
	uint32_t* px = image->GetPixels();
	for(uint32_t i = 0; i < 160*240; i++)
	{
		uint32_t src_color = px[i];
		uint8_t src_a = (src_color & 0xFF000000) >> 24;

		if(src_a < 127) continue;

		upx[i] = arg4;
	}
}

void _stdcall IImage_Display(int arg1, int arg2, int arg3, int arg4, int arg5)
{
	char* c = NULL;
	__asm mov c, ecx;

	Image* image = *(Image**)(c+4);
	if(!image) return;
	if(image->GetWidth() != 160 ||
		image->GetHeight() != 240) return;

	uint16_t* upx = *(uint16_t**)(0x0062571C);
	uint32_t* px = image->GetPixels();
	bool no_alpha = false;
	for(uint32_t i = 0; i < 160*240; i++)
	{
		uint32_t src_color = px[i];
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

		if(!src_a) continue;

		uint16_t dst_color = upx[i];
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
			float alpha = (float)(src_a) / 255.0;
			float Fout_r = (src_r * alpha) + (dst_r * (1.0-alpha));
			float Fout_g = (src_g * alpha) + (dst_g * (1.0-alpha));
			float Fout_b = (src_b * alpha) + (dst_b * (1.0-alpha));

			out_r = max(min((uint8_t)Fout_r, 0x1F), 1);
			out_g = max(min((uint8_t)Fout_g, 0x3F), 1);
			out_b = max(min((uint8_t)Fout_b, 0x1F), 1);
		}
		else
		{
			out_r = max(src_r, 1);
			out_g = max(src_g, 1);
			out_b = max(src_b, 1);
		}

		dst_color = (out_r & 0x1F) << 11;
		dst_color |= (out_g & 0x3F) << 5;
		dst_color |= (out_b);
		upx[i] = dst_color;
	}
}

struct IImageUse
{
	void* ptr;
	int ref;
	std::string filename;
};

std::vector<IImageUse> IImages;

void _stdcall IImage_Destruct(unsigned long final)
{
	return;
	/*
	char* c = NULL;
	__asm mov c, ecx;
	Image* image = *(Image**)(c+4);
	if (image) delete image;
	delete[] c;*/
}

void* _stdcall IImage_Construct(std::string filename = "")
{
	for (std::vector<IImageUse>::iterator it = IImages.begin();
		 it != IImages.end(); ++it)
	{
		IImageUse& use = (*it);
		if (use.filename == filename)
		{
			use.ref++;
			return use.ptr;
		}
	}

	static unsigned long* vtable = NULL;
	if (!vtable)
	{
		vtable = new unsigned long[17];
		memset(vtable, 0, sizeof(unsigned long)*17);
		vtable[1] = (unsigned long)&IImage_Destruct;
		vtable[16] = (unsigned long)&IImage_CopyTo;
		vtable[6] = (unsigned long)&IImage_Display;
	}

	char* c = new char[8];
	memset(c, 0, 8);
	*(unsigned long**)(c) = vtable;
	Image* image = (filename.length() > 0) ? new Image(filename) : NULL;
	*(Image**)(c+4) = image;

	IImageUse use;
	use.ptr = c;
	use.ref = 1;
	use.filename = filename;
	IImages.push_back(use);

	return c;
}

void* _stdcall ITEMPNG_doLoadPngChar(const char* location, const char* character)
{
	int chrN = *(int*)(character+0x28);
	std::string img = Format("data\\%s%d.png", location, chrN);
	std::string oimg = Format("data\\%s%d.256", location, chrN);

	void* v = NULL;

	if(FileExistsEx(img))
	{
		v = IImage_Construct(img);
	}
	else
	{
		for (std::vector<IImageUse>::iterator it = IImages.begin();
			 it != IImages.end(); ++it)
		{
			IImageUse& use = (*it);
			if (use.filename == oimg)
			{
				use.ref++;
				return use.ptr;
			}
		}

		const char* coimg = oimg.c_str();
		__asm
		{
			push	0x24
			mov		edx, 0x00401840
			call	edx
			push	[coimg]
			mov		ecx, eax
			mov		edx, 0x00426DA9
			call	edx
			mov		[v], eax
			push	0
			push	1
			push	1
			mov		ecx, eax
			mov		edx, 0x00426BE0
			call	edx
		}

		IImageUse use;
		use.ptr = v;
		use.ref = 1;
		use.filename = oimg;
		IImages.push_back(use);
	}

	return v;
}

void _stdcall ITEMPNG_doLoadPng(const char* character, int i, const char* str_primary, const char* str_secondary)
{
	std::string i_pr = "data\\" + std::string(str_primary) + ".png";
	std::string i_sc = "data\\" + std::string(str_secondary) + ".png";

	std::string oi_pr = "data\\" + std::string(str_primary) + ".256";
	std::string oi_sc = "data\\" + std::string(str_secondary) + ".256";

	ipng_out_primary = NULL;
	ipng_out_secondary = NULL;

	if(FileExistsEx(i_pr))
	{
		//log_format("loading replacement for \"%s\"...\n", i_pr.c_str());
		ipng_out_primary = IImage_Construct(i_pr);
	}
	else
	{
		for (std::vector<IImageUse>::iterator it = IImages.begin();
			 it != IImages.end(); ++it)
		{
			IImageUse& use = (*it);
			if (use.filename == oi_pr)
			{
				use.ref++;
				ipng_out_primary = use.ptr;
				break;
			}
		}

		if (!ipng_out_primary)
		{
			const char* coi_pr = oi_pr.c_str();
			//log_format("loading original for \"%s\"...\n", coi_pr);
			__asm
			{
				push	0x24
				mov		edx, 0x00401840
				call	edx
				push	[coi_pr]
				mov		ecx, eax
				mov		edx, 0x00426DA9
				call	edx
				mov		[ipng_out_primary], eax
				push	0
				push	1
				push	1
				mov		ecx, eax
				mov		edx, 0x00426BE0
				call	edx
			}

			IImageUse use;
			use.ptr = ipng_out_primary;
			use.ref = 1;
			use.filename = oi_pr;
			IImages.push_back(use);
		}
	}
	
	if(i == 3 || i == 8 || i == 9 || (i == 7 && (*(uint32_t*)(character+0x1B8) & 2)))
	{
		if(FileExistsEx(i_sc))
		{
			//log_format("loading replacement for \"%s\"...\n", i_sc.c_str());
			ipng_out_secondary = IImage_Construct(i_sc);
		}
		else
		{
			for (std::vector<IImageUse>::iterator it = IImages.begin();
				 it != IImages.end(); ++it)
			{
				IImageUse& use = (*it);
				if (use.filename == oi_sc)
				{
					use.ref++;
					ipng_out_secondary = use.ptr;
					break;
				}
			}

			if (!ipng_out_secondary)
			{
				const char* coi_sc = oi_sc.c_str();
				//log_format("loading original for \"%s\"...\n", coi_sc);
				__asm
				{
					push	0x24
					mov		edx, 0x00401840
					call	edx
					push	[coi_sc]
					mov		ecx, eax
					mov		edx, 0x00426DA9
					call	edx
					mov		[ipng_out_secondary], eax
					push	0
					push	1
					push	1
					mov		ecx, eax
					mov		edx, 0x00426BE0
					call	edx
				}

				IImageUse use;
				use.ptr = ipng_out_secondary;
				use.ref = 1;
				use.filename = oi_sc;
				IImages.push_back(use);
			}
		}
	}

	static void* iimg_null = NULL;
	if (!iimg_null) iimg_null = IImage_Construct();

	if(ipng_out_primary == NULL) ipng_out_primary = iimg_null;
	if(ipng_out_secondary == NULL) ipng_out_secondary = iimg_null;

	//log_format("spawned item: %08X %08X\n", ipng_out_primary, ipng_out_secondary);
}

// -> 46E18B
void __declspec(naked) ITEMPNG_loadPng()
{
	__asm
	{
		push	eax
		push	ebx
		push	ecx
		push	edx
		push	edi
		push	esi

		lea		edx, [ebp-0x77C]
		lea		ecx, [ebp-0x87C]
		push	ecx // secondary
		push	edx // primary
		push	[ebp-0x27C]
		push	[ebp-0x8C8]
		call	ITEMPNG_doLoadPng

		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		pop		esi
		pop		edi

		mov		eax, [ebp-0x27C]
		mov		ebx, [ipng_out_primary]
		mov		[ebp-0x244+eax*4], ebx
		mov		ebx, [ipng_out_secondary]
		mov		[ebp-0x274+eax*4], ebx
		mov		edx, 0x0046DE9E
		jmp		edx
	}
}

// -> 46E1B0
void __declspec(naked) ITEMPNG_loadPngChar()
{
	__asm
	{
		push	eax
		push	ebx
		push	ecx
		push	edx
		push	edi
		push	esi

		push	[ebp-0x8C8]
		lea		ecx, [ebp-0x214]
		push	ecx
		call	ITEMPNG_doLoadPngChar
		mov		[ebp-0x14], eax

		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		pop		esi
		pop		edi

		mov		edx, 0x0046E21B
		jmp		edx
	}
}

bool _stdcall ITEMPNG_doCachePng(void* ptr)
{
	for (std::vector<IImageUse>::iterator it = IImages.begin();
		 it != IImages.end(); ++it)
	{
		IImageUse& use = (*it);
		if (use.ptr == ptr)
			return true;
	}

	return false;
}

// -> 42AAC0
void __declspec(naked) ITEMPNG_cachePng()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		push	ecx
		mov		[ebp-0x04], ecx

		push	ecx
		call	ITEMPNG_doCachePng
		test	eax, eax
		jnz		do_nothing

		mov		ecx, [ebp-0x04]
		mov		edx, 0x0042AAF0
		call	edx

		mov		edx, 0x0042AACF
		jmp		edx

do_nothing:
		mov		esp, ebp
		pop		ebp
		retn	0x0004
	}
}