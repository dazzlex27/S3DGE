/*
===========================================================================
Renderer2D.cpp

Implements the Renderer2D class
Set up to process up to 250,000 vertices per frame.
===========================================================================
*/

#include "Renderer2D.h"
#include "Internal/DeleteMacros.h"
#include "Graphics/Renderables/Renderable2D.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Fonts/Font.h"
#include "Graphics/GraphicsStructures.h"
#include "Graphics/Structures/Color.h"

using namespace s3dge;
using namespace graphics;

#define MAX_VERTICES 250000
#define VERTEX_SIZE sizeof(VertexData)
#define VERTEX_BUFFER_SIZE VERTEX_SIZE * MAX_VERTICES
#define INDEX_BUFFER_SIZE VERTEX_BUFFER_SIZE
	
Renderer2D::Renderer2D()
{			
	Initialize();
}

Renderer2D::~Renderer2D()
{
	SafeDelete(_ibo);
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}

void Renderer2D::Initialize()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (const void*)(offsetof(VertexData, VertexData::Vertex)));
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_SIZE, (const void*)(offsetof(VertexData, VertexData::Color)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (const void*)(offsetof(VertexData, VertexData::UV)));
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (const void*)(offsetof(VertexData, VertexData::TextureID)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	uint* indices = new uint[INDEX_BUFFER_SIZE];

	int offset = 0;

	for (int i = 0; i < INDEX_BUFFER_SIZE; i += 6)
	{
		indices[i] = offset + 0;
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;

		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0;

		offset += 4;
	}

	_ibo = new IndexBuffer(indices, INDEX_BUFFER_SIZE);

	_indexCount = 0;

	glBindVertexArray(0);
}

void Renderer2D::Begin()
{
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	_buffer = (VertexData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void Renderer2D::Submit(const Renderable2D* renderable)
{
	const Point3D& position = renderable->GetPosition();
	const Size2D& size = renderable->GetSize();
	const Color& color = renderable->GetColor();
	const std::vector<Point2D> uv = renderable->GetUV();
	const uint textureID = renderable->GetTextureID();

	float textureSlot = 0.0f;

	if (textureID > 0)
	{
		bool ok = false;
		for (uint i = 0; i < _textures.size(); ++i)
		{
			if (_textures[i] == textureID)
			{
				textureSlot = (float)(i + 1);
				ok = true;
				break;
			}
		}

		if (!ok)
		{
			if (_textures.size() >= 32)
			{
				End();
				Flush();
				Begin();
			}

			_textures.push_back(textureID);
			textureSlot = (float)(_textures.size());
		}
	}

	_buffer->Vertex = math::vec3f(position.x, position.y, position.z);
	_buffer->Color = color.value;
	_buffer->UV = math::vec2f(uv[0].x, uv[0].y);
	_buffer->TextureID = textureSlot;
	_buffer++;

	_buffer->Vertex = math::vec3f(position.x, position.y + size.height, position.z);
	_buffer->Color = color.value;
	_buffer->UV = math::vec2f(uv[1].x, uv[1].y);
	_buffer->TextureID = textureSlot;
	_buffer++;

	_buffer->Vertex = math::vec3f(position.x + size.width, position.y + size.height, position.z);
	_buffer->Color = color.value;
	_buffer->UV = math::vec2f(uv[2].x, uv[2].y);
	_buffer->TextureID = textureSlot;
	_buffer++;

	_buffer->Vertex = math::vec3f(position.x + size.width, position.y, position.z);
	_buffer->Color = color.value;
	_buffer->UV = math::vec2f(uv[3].x, uv[3].y);
	_buffer->TextureID = textureSlot;
	_buffer++;

	_indexCount += 6;
}

void Renderer2D::DrawString(const std::string& text, Font* font, const math::vec3f& position, const Color& color)
{
	using namespace ftgl;

	float x = position.x;
	float textureSlot = 0.0f;
	bool ok = false;

	for (uint i = 0; i < _textures.size(); i++)
	{
		if (_textures[i] == font->GetAtlasID())
		{
			textureSlot = (float)(i + 1);
			ok = true;
			break;
		}
	}

	if (!ok)
	{
		if (_textures.size() >= 32)
		{
			End();
			Flush();
			Begin();
		}

		_textures.push_back(font->GetAtlasID());
		textureSlot = (float)(_textures.size());
	}

	const float scaleX = 40.0f;
	const float scaleY = 40.0f;

	for (uint i = 0; i < text.length(); i++)
	{
		texture_glyph_t* glyph = texture_font_get_glyph(font->GetFontFace(), text[i]);
		if (glyph != NULL)
		{
			if (i > 0)
			{
				float kerning = texture_glyph_get_kerning(glyph, text[i - 1]);
				x += kerning / scaleX;
			}

			float x0 = x + glyph->offset_x / scaleX;
			float y0 = position.y + glyph->offset_y / scaleY;
			float x1 = x0 + glyph->width / scaleX;
			float y1 = y0 - glyph->height / scaleY;

			float u0 = glyph->s0;
			float v0 = glyph->t0;
			float u1 = glyph->s1;
			float v1 = glyph->t1;

			_buffer->Vertex = math::vec3f(x0, y0, 0);
			_buffer->UV = math::vec2f(u0, v0);
			_buffer->TextureID = textureSlot;
			_buffer->Color = color.value;
			_buffer++;

			_buffer->Vertex = math::vec3f(x0, y1, 0);
			_buffer->UV = math::vec2f(u0, v1);
			_buffer->TextureID = textureSlot;
			_buffer->Color = color.value;
			_buffer++;

			_buffer->Vertex = math::vec3f(x1, y1, 0);
			_buffer->UV = math::vec2f(u1, v1);
			_buffer->TextureID = textureSlot;
			_buffer->Color = color.value;
			_buffer++;

			_buffer->Vertex = math::vec3f(x1, y0, 0);
			_buffer->UV = math::vec2f(u1, v0);
			_buffer->TextureID = textureSlot;
			_buffer->Color = color.value;
			_buffer++;

			_indexCount += 6;

			x += glyph->advance_x / scaleX;
		}

	}
}

void Renderer2D::Flush()
{
	for (uint i = 0; i < _textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, _textures[i]);
	}

	glBindVertexArray(_vao);
	_ibo->Bind();

	glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, NULL);

	_ibo->Unbind();
	glBindVertexArray(0);

	_indexCount = 0;
}

void Renderer2D::End()
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}