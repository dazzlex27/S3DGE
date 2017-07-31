#pragma once

#include <vector>
#include "Renderable.h"

namespace s3dge
{
	class Mesh;

	class Model : public Renderable
	{
	private:
		std::vector<Mesh*> _meshes;

	public:
		bool LoadModel(cstring path);
		virtual void Submit(Renderer* renderer) const override;
	};
}