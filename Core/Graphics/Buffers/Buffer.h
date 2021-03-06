/*
===========================================================================
Buffer.h

Defines base class for object buffers.
===========================================================================
*/

#pragma once

#include <CustomTypes.h>
#include "BufferEnums.h"

namespace sedge
{
	class Buffer
	{
	protected:
		ID BufferID;
		void* DataPtr;
		uint ElementSize;
		uint ElementCount;
		BufferTarget Target;
		DrawingMode Mode;

	protected:
		Buffer(const BufferTarget target, const uint elementSize, const uint elementCount, void*const dataPtr = nullptr, const DrawingMode drawingMode = Static);

	public:
		virtual ~Buffer();

	public:
		inline const ID GetBufferID() const { return BufferID; }
		inline const void* GetDataPointer() { return DataPtr; }
		inline const uint GetCount() const { return ElementCount; }
		inline const uint GetElementSize() const { return ElementSize; }
		inline const uint GetTotalLength() const { return ElementCount * ElementSize; }

		virtual void SetDataPointer(void*const dataPtr) { DataPtr = dataPtr; }

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual void Map();
		virtual void Unmap();
	};
}