#ifndef GL_H_INCLUDED
#define GL_H_INCLUDED

#include <GL/GL.h>
#include <stdint.h>

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

struct GLtex
{
    uint32_t Context;
    GLuint Texture;
};

#endif // GL_H_INCLUDED