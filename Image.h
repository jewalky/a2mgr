#include <SDL/SDL.h>
#include <string>
#include <stdint.h>

class Image
{
public:
	Image(std::string filename);
	~Image();
	void Display(int16_t x, int16_t y);
	void DisplayEx(int16_t x, int16_t y, int16_t inX, int16_t inY, int16_t w, int16_t h, bool no_alpha);
	uint32_t* GetPixels();
	uint32_t GetWidth();
	uint32_t GetHeight();
	uint32_t GetPixelAt(int32_t x, int32_t y);

private:
	uint32_t* myPixels;
	uint32_t myWidth;
	uint32_t myHeight;
};