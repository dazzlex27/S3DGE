#pragma once

#include "Maths/MathsHeader.h"
#include "Graphics/Texture.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Utilities/DeleteMacros.h"

namespace S3DGE
{
	namespace Graphics
	{
		class Renderable2D
		{
		protected:
			Maths::vec3f m_Position;
			Maths::vec2f m_Size;
			uint m_Color;
			Texture* m_Texture;

		protected:
			Renderable2D(const Maths::vec3f& position, const Maths::vec2f& size, uint color)
				: m_Position(position), m_Size(size), m_Color(color) 
			{
			}

			virtual ~Renderable2D() 
			{
			}

		public:
			inline const Maths::vec3f GetPosition() const { return m_Position; }
			inline const Maths::vec2f GetSize() const { return m_Size; }
			inline uint GetColor() const { return m_Color; }
		};
	}
}