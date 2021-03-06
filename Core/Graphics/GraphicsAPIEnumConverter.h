/*
===========================================================================
GraphicsAPIEnumParser.h

Declares parsers to convert sedge graphics enums to API-dependent ones.
===========================================================================
*/

#pragma once

#include "Graphics/Buffers/BufferEnums.h"
#include "Graphics/Textures/TextureEnums.h"
#include "Graphics/Shaders/ShaderEnums.h"
#include "DrawingEnums.h"

namespace sedge
{
	class GraphicsAPIEnumConverter
	{
	public:
		static const int GetDrawingModeValue(const DrawingMode drawingMode);
		static const int GetBufferTarget(const BufferTarget target);

		static const int GetTextureTarget(const TextureTarget target);
		static const int GetTextureWrap(const TextureWrap wrap);
		static const int GetTextureWrapMode(const TextureWrapMode mode);
		static const int GetTextureFilter(const TextureFilter filter);
		static const int GetTextureFilterMode(const TextureFilterMode filterMode);
		static const int GetColorCode(const ColorCode code);

		static const int GetShaderTarget(const ShaderTarget target);

		static const int GetPrimitiveType(const PrimitiveType type);
		static const int GetValueType(const ValueType type);

		static const int GetWindingOrder(const WindingOrder order);
	};
}