#include "Renderer2D.h"

namespace S3DGE
{
	namespace Graphics
	{
		Renderer2D::Renderer2D()
		{			
			Initialize();
		}

		Renderer2D::~Renderer2D()
		{
			SafeDelete(m_IBO);
			glDeleteBuffers(1, &m_VBO);
			glDeleteVertexArrays(1, &m_VAO);
		}

		void Renderer2D::Initialize()
		{
			glGenVertexArrays(1, &m_VAO);
			glBindVertexArray(m_VAO);

			glGenBuffers(1, &m_VBO);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

			glBufferData(GL_ARRAY_BUFFER, RENDERER_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const void*)(offsetof(VertexData, VertexData::vertex)));
			glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, RENDERER_VERTEX_SIZE, (const void*)(offsetof(VertexData, VertexData::color)));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const void*)(offsetof(VertexData, VertexData::uv)));
			glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const void*)(offsetof(VertexData, VertexData::textureID)));

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			uint* indices = new uint[RENDERER_INDICES_SIZE];

			int offset = 0;

			for (int i = 0; i < RENDERER_INDICES_SIZE; i += 6)
			{
				indices[i] = offset + 0;
				indices[i + 1] = offset + 1;
				indices[i + 2] = offset + 2;

				indices[i + 3] = offset + 2;
				indices[i + 4] = offset + 3;
				indices[i + 5] = offset + 0;

				offset += 4;
			}

			m_IBO = new IndexBuffer(indices, RENDERER_INDICES_SIZE);

			glBindVertexArray(0);
		}

		void Renderer2D::Begin()
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			m_Buffer = (VertexData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		}

		void Renderer2D::SubmitRenderable(const Renderable2D* renderable)
		{
			const Maths::vec3f& position = renderable->GetPosition();
			const Maths::vec2f& size = renderable->GetSize();
			const uint color = renderable->GetColor();
			const std::vector<Maths::vec2f> uv = renderable->GetUV();
			const uint textureID = renderable->GetTextureID();

			float textureSlot = 0.0f;

			if (textureID > 0)
			{
				bool ok = false;
				for (uint i = 0; i < m_Textures.size(); ++i)
				{
					if (m_Textures[i] == textureID)
					{
						textureSlot = (float)(i + 1);
						ok = true;
						break;
					}
				}

				if (!ok)
				{
					if (m_Textures.size() >= 32)
					{
						End();
						Flush();
						Begin();
					}

					m_Textures.push_back(textureID);
					textureSlot = (float)(m_Textures.size());
				}
			}

			m_Buffer->vertex = position;
			m_Buffer->color = color;
			m_Buffer->uv = uv[0];
			m_Buffer->textureID = textureSlot;
			m_Buffer++;

			m_Buffer->vertex = Maths::vec3f(position.x, position.y + size.y, position.z);
			m_Buffer->color = color;
			m_Buffer->uv = uv[1];
			m_Buffer->textureID = textureSlot;
			m_Buffer++;

			m_Buffer->vertex = Maths::vec3f(position.x + size.x, position.y + size.y, position.z);
			m_Buffer->color = color;
			m_Buffer->uv = uv[2];
			m_Buffer->textureID = textureSlot;
			m_Buffer++;

			m_Buffer->vertex = Maths::vec3f(position.x + size.x, position.y, position.z);
			m_Buffer->color = color;
			m_Buffer->uv = uv[3];
			m_Buffer->textureID = textureSlot;
			m_Buffer++;

			m_IndexCount += 6;
		}

		void Renderer2D::Flush()
		{
			for (uint i = 0; i < m_Textures.size(); ++i)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D,m_Textures[i]);
			}

			glBindVertexArray(m_VAO);
			m_IBO->Bind();

			glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, NULL);

			m_IBO->Unbind();
			glBindVertexArray(0);

			m_IndexCount = 0;
		}

		void Renderer2D::End()
		{
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
}