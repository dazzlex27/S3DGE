/*
===========================================================================
ImageUtils.cpp

Implements the functions declared in the header file.
Image loading done via stb_image library by Sean T. Barrett.
===========================================================================
*/

#include "ImageUtils.h"

// WARNING! MUST ONLY BE DEFINED ONCE!
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// image loading via stb_image. Supports *.bmp, *.png and whatnot.
byte* s3dge::LoadImage(cstring path, int* width, int* height, int* components)
{
	stbi_set_flip_vertically_on_load(1);
	return stbi_load(path, width, height, components, 0);
}