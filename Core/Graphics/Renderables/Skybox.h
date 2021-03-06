/*
===========================================================================
Skybox.h

Declares the Skybox class, made for rendering cubic skybox textures.
===========================================================================
*/

#pragma once

#include "Renderable3D.h"

namespace sedge
{
	class VertexBuffer;
	class IndexBuffer;
	class Cubemap;

	class Skybox : public Renderable3D
	{
	private:
		VertexBuffer* _vbo;
		IndexBuffer* _ibo;
		Cubemap* _texture;

	public:
		Skybox(Cubemap*const texture);

		virtual void Draw() const override;
	};
}