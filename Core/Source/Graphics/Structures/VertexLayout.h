#pragma once

#include <vector>
#include "CustomTypes.h"

namespace s3dge
{
	enum ElementType
	{
		Float,
		Ubyte,
	};

	struct LayoutAttribute
	{
		const char* name;
		int index;
		int size;
		int type;
		int normalized;
		int stride;
		const void* offset;
	};

	class VertexLayout
	{
	private:
		std::vector<LayoutAttribute*> _attributes;

	public:
		inline const std::vector<LayoutAttribute*>& GetAttributes() const { return _attributes; }

		void AddEntry(LayoutAttribute*const attribute);
		void AddEntry(const char* name, const int index, const int size, const ElementType type, const bool normalized, const int stride, const void*const offset);

		static VertexLayout GetDefaultVertexLayout();
		static VertexLayout GetDefaultVertexLayoutSprite();
		static VertexLayout GetDefaultVertexLayoutCb();
	};
}