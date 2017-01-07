/*
===========================================================================
Converters.h

Lists a few functions for mathematical conversions. Might be refactored later.
===========================================================================
*/

#pragma once

#include "CustomTypes.h"
#include "Maths/vec4.h"

namespace s3dge
{
	inline uint ConvertColorToUint(const maths::vec4f& color)
	{
		int red = (int)(color.x * 255);
		int green = (int)(color.y * 255);
		int blue = (int)(color.z * 255);
		int alpha = (int)(color.w * 255);

		return alpha << 24 | blue << 16 | green << 8 | red;
	}

	inline maths::vec4f ConvertColorToVec4(uint color)
	{
		maths::vec4f result;

		result.x = (float)(color >> 0);
		result.y = (float)(color >> 8);
		result.z = (float)(color >> 16);
		result.w = (float)(color >> 24);
		
		return result;
	}
}