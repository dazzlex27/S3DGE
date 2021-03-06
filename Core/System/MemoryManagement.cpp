/*
===========================================================================
MemoryManager.cpp

Custom memory manager implementation
===========================================================================
*/

#include "MemoryManagement.h"

namespace sedge
{
	bool MemoryManager::_initialized;
	void* MemoryManager::_head;
	uint MemoryManager::_allocations;
	size_t MemoryManager::_size;

	bool MemoryManager::Initialize(size_t size)
	{
		_size = size;
		_head = new bool[size];
		_initialized = true;

		return true;
	}

	void MemoryManager::Allocate(size_t size)
	{
		// TODO:

		_allocations++;
	}

	void MemoryManager::Deallocate(void* address)
	{
		// TODO:

		_allocations--;
	}

	bool MemoryManager::Dispose()
	{
		_size = 0;
		SafeDeleteArray(_head);
		_initialized = false;

		_allocations = 0;

		return true;
	}
}