/*
===========================================================================
Common.h

Aggregates all the basic components of the engine in a single header file.
This file is included by the main header sedge.h
===========================================================================
*/

#pragma once

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"
#include "Math/Size2D.h"
#include "Math/Size3D.h"

#include "Platform/KeyCodes.h"
#include "Platform/Window.h"

#include "Graphics/Layer2D.h"

#include "Graphics/GraphicsAPI.h"

#include "Graphics/Shaders/ShaderProgram.h"
#include "Graphics/Shaders/ShaderFactory.h"
#include "Graphics/Renderables/Sprite.h"
#include "Graphics/Renderables/Mesh.h"
#include "Graphics/Renderables/Model.h"
#include "Graphics/Renderables/Skybox.h"
#include "Graphics/Renderables/SpriteFactory.h"
#include "Graphics/Renderables/MeshFactory.h"
#include "Graphics/Terrain/Terrain.h"
#include "Graphics/Renderables/Primitives/Cube.h"

#include "Graphics/AssetManagers/TextureManager.h"
#include "Graphics/AssetManagers/Renderable2DManager.h"
#include "Graphics/AssetManagers/Renderable3DManager.h"
#include "Graphics/AssetManagers/ShaderManager.h"
#include "Graphics/Structures/VertexData.h"
#include "Graphics/Structures/VertexLayout.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"

#include "Logic/Objects/Actor.h"
#include "Logic/Objects/Scene.h"
#include "Logic/Objects/Entity.h"
#include "Logic/Cameras/FPSCamera.h"
#include "Logic/Cameras/TPSCamera.h"

#include "System/InputManager.h"
#include "System/MemoryManagement.h"
#include "System/Stopwatch.h"
#include "System/RNG.h"
#include "System/Logger.h"
#include "System/DateTime.h"

#include "Graphics/Renderables/GraphicsObjectFactorySet.h"