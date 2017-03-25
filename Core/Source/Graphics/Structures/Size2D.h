/*
===========================================================================
Size2D.h

Represents a two-dimensional size.
===========================================================================
*/

#pragma once

#include "Math/Vector2.h"

namespace s3dge
{
	namespace graphics
	{
		struct Size2D
		{
			float width;
			float height;

			Size2D(float width, float height) 
				: width(width), height(height) { }
			Size2D(math::Vector2 vector) 
				: width(vector.x), height(vector.y) { }
		};
	}
}