/*
===========================================================================
Sprite.cpp

Implements the Sprite class
===========================================================================
*/

#include "Sprite.h"

using namespace s3dge;
	
Sprite::Sprite(const Point2D& position, const float zIndex, const Size2D& size, const Color& color)
	: Renderable2D(Point3D(position.x, position.y, zIndex), size, color)
{
}

Sprite::Sprite(const Point2D& position, const float zIndex, const Size2D& size, Texture2D* texture)
	: Renderable2D(Point3D(position.x, position.y, zIndex), size, texture)
{
}