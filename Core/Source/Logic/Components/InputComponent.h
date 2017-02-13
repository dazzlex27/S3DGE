/*
===========================================================================
InputComponent.h

Defines a component for handling input on entities.
Designed to fit component-based entity model.
Should be composed by the entity in question to be enable that entity to respond to input command, may that be AI-generated input or used input.
===========================================================================
*/

#pragma once

#include "Logic/Objects/Entity.h"

namespace s3dge
{
	namespace logic
	{
		class InputComponent
		{
		public:
			virtual void Update(Entity* entity) = 0;
			virtual ~InputComponent() { }
		};
	}
}