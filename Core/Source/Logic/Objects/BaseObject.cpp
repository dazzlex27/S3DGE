/*
===========================================================================
BaseObject.cpp

Implements the BaseObject class.
===========================================================================
*/

#include "BaseObject.h"
#include "Utilities/IDGenerator.h"

using namespace s3dge;

BaseObject::BaseObject()
{
	_id = IDGenerator::GetNewID();
}

BaseObject::~BaseObject()
{
}