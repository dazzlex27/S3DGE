#pragma once

#include "CustomTypes.h"
#include "Graphics/Buffers/Buffer.h"

namespace s3dge
{
	struct Point3D;
	struct Size3D;

	class Mesh;
	class VertexBuffer;
	class ElementBuffer;
	class Texture2D;
	struct Color;

	class MeshFactory
	{
	public:
		static Mesh* CreateMesh(VertexBuffer* vbo, ElementBuffer* ibo);

		static Mesh* CreateCuboid(const Size3D& size, Texture2D* texture, DrawingMode drawingMode);
		static Mesh* CreateCuboid(const Size3D& size, const Color& color, DrawingMode drawingMode);

		static Mesh* CreateCube(float size, Texture2D* texture, DrawingMode drawingMode);
		static Mesh* CreateCube(float size, const Color& color, DrawingMode drawingMode);
	};
}