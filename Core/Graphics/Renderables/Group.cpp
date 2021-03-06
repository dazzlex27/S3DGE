/*
===========================================================================
Group.cpp

Implements the Group class
===========================================================================
*/

#include "Group.h"
#include "System/MemoryManagement.h"
#include "Graphics/Renderers/Renderer2D.h"

using namespace sedge;

Group::Group()
{
}

Group::~Group()
{
	for (auto item : _renderables)
		SafeDelete(item);
}

void Group::Add(Renderable2D*const renderable)
{
	_renderables.push_back(renderable);
}

void Group::Submit(Renderer2D*const renderer) const
{
	for (auto item : _renderables)
		renderer->Submit(item);
}