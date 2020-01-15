#include "utils.h"
#include "lib/utils.hpp"

// allocates more space in draw font function (for local variables)
// stores [ebp+0x10] in the new space, so that we can change palette
void __declspec(naked) TEXTCOLO_allocMoreSpace() {
	// @ 4637F1
	__asm {
		push	ebp
		mov		ebp, esp
		sub		esp, 0x18
		mov		[ebp-0x14], ecx
		mov		eax, [ebp+0x18]
		mov		[ebp-0x18], eax
		mov		edx, 0x004637FA
		jmp		edx
	}
}

static unsigned short TempPalette[16];

// makes RGB 565 RGB color from 0-255 RGB values
#define MAKECOLOR(r,g,b) (((r * 31 / 255) << 11) | ((g * 63 / 255) << 5) | ((b * 31 / 255)))

void _stdcall PreCharDraw(const char* s, int* idx, unsigned short** out_pal, unsigned short* pal) {
	if (!pal) return; // ???

	bool isShadow = (pal == (unsigned short*)(0x0062AFB8)); // shadow palette. returned in sub_463A06
	
	// parse input
	int i = *idx;
	// format: %[RRGGBB] or %[RGB] or %[R,G,B] or %[*] (revert to original palette)
	// this is used in the middle of rendering, so refraining from the use of heavy operations
	if (s[i] == '%') {
		int start = -1;
		int end = -1;
		bool isDec = false;
		int colors[3] = { -1 };
		if (s[i + 1] == '[') {
			i += 2;
			start = end = i;
			for (; s[i]; i++) {
				end = i;
				if (s[i] == ']') break;
				if (s[i] == ',') isDec = true;
			}
			if (!s[i]) return; // invalid sequence, does not have closing ]
		}
		if (start < 0 || end < 0) return;
		// check for reset:
		int seqlen = end - start;
		if (seqlen == 1 && s[start] == '*') {
			*idx = end + 1;
			if (!isShadow) *out_pal = pal;
			return;
		}
		else if (!isDec) {
			char hc[3];
			if (seqlen == 3) {
				for (int j = 0; j < 3; j++) {
					hc[0] = s[start + j];
					hc[1] = 0;
					int c = HexToInt(ToUpper(hc));
					c |= c << 4;
					colors[j] = c;
				}
			} else if (seqlen == 6) {
				for (int j = 0; j < 3; j++) {
					hc[0] = s[start + j * 2];
					hc[1] = s[start + j * 2 + 1];
					hc[2] = 0;
					int c = HexToInt(ToUpper(hc));
					colors[j] = c;
				}
			}
		} else {
			char hi[4];
			int hj = 0;
			int hc = 0;
			for (int j = start; j <= end; j++) {
				if (s[j] == ',' || j == end) {
					if (hc >= 3) { // more than 3 color components. invalid
						return;
					}
					hi[hj] = 0;
					colors[hc] = StrToInt(hi);
					hj = 0;
					hc++;
					continue;
				}
				if (hj >= 3) { // int longer than 3 chars. invalid
					return;
				}
				hi[hj] = s[j];
				hj++;
			}
		}
		// done, colors = base for new palette
		if (colors[0] < 0 || colors[1] < 0 || colors[2] < 0)
			return; // some of the colors was invalid or incomplete seq
		if (!isShadow) {
			// generate palette. null color is not allowed for w/e reason (breaks everything)
			for (int j = 0; j < 16; j++) {
				TempPalette[j] = MAKECOLOR(colors[0] * j / 15, colors[1] * j / 15, colors[2] * j / 15);
			}
			*out_pal = TempPalette;
		}
		*idx = end + 1;
	}

}

// switches palette based on results of parsing the string
// [ebp-8] : index into the string
// [ebp+0x10] : string
// [ebp+0x18] : new palette
// [ebp-0x18] : original palette (saved)
// warning: may clash with ^ fix
void __declspec(naked) TEXTCOLO_preCharDraw() {
	// @ 4638B1
	__asm {
		push	[ebp-0x18]
		lea		eax, [ebp+0x18]
		push	eax
		lea		eax, [ebp-8]
		push	eax
		push	[ebp+0x10]
		call	PreCharDraw
		// check if we cannot proceed (i>=len)
		mov		ecx, [ebp-8]
		cmp		ecx, [ebp-4]
		jge		tc_stop_draw

		// resume normally
		mov		edx, [ebp+0x10]
		add		edx, [ebp-8]
		mov		al, [edx]
		push	eax
		mov		ecx, [ebp-0x14]
		mov		edx, 0x00461DA1
		call	edx

		mov		edx, 0x004638C2
		jmp		edx

tc_stop_draw:
		mov		edx, 0x00463A00
		jmp		edx
	}
}