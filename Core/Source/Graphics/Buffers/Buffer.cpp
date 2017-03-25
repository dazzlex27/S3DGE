/*
===========================================================================
Buffer.cpp

Implements the Buffer class
===========================================================================
*/

#include "Buffer.h"

using namespace s3dge;
using namespace graphics;
	
Buffer::Buffer(void* data, int componentSize, uint componentCount)
	: _componentCount(componentCount)
{
	glGenBuffers(1, &_bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, _bufferID);
	glBufferData(GL_ARRAY_BUFFER, componentCount * componentSize, data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &_bufferID);
}

void Buffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, _bufferID);
}

void Buffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}