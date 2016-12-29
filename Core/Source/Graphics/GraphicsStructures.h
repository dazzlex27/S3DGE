/*
===========================================================================
GraphicsStructures.h

This file contains the necessary structures used by the Graphics namespace
===========================================================================
*/

#pragma once

#include "CustomTypes.h"
#include "Maths/MathsHeader.h"

namespace s3dge
{
	struct VertexData
	{
		Maths::vec3f Vertex;
		uint Color;
		Maths::vec2f UV;
		float TextureID;
	};
}