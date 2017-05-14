/*
===========================================================================
S3DGE.cpp

Implementation of the top level engine functions.
===========================================================================
*/

#include "../Include/S3DGEngine.h"
#include "System/Log.h"

using namespace s3dge;

S3DGEngine::S3DGEngine()
{
	_state = ENGINE_STATE::READY;
}

S3DGEngine::~S3DGEngine()
{
}

void S3DGEngine::CreateGameWindow(cstring name, uint width, uint height, bool fullscreen, bool vsync)
{
	if (!WindowInstance)
	{
		WindowInstance = new Window(name, width, height, fullscreen, vsync);

		if (!WindowInstance->Initialize())
		{
			LOG_FATAL("Could not initialize window!");
			abort();
		}
	}
}

void S3DGEngine::Run()
{
	LOG_INFO("Application started...");

	_state = ENGINE_STATE::INITIALIZING;
	LOG_INFO("Initializing components...");
	InitializeInternalSystems();
	InitializeResourceManagers();
	Initialize();
	_state = ENGINE_STATE::RUNNING;
	LOG_INFO("Running main loop...");
	RunGameLoop();
	_state = ENGINE_STATE::DISPOSING;
	LOG_INFO("Shutdown initiated...");
	Dispose();
	DisposeResourceManagers();
	DisposeInternalSystems();

	_state = ENGINE_STATE::DISPOSED;
	LOG_INFO("Application exited...");
}

void S3DGEngine::RunGameLoop()
{
	uint updates = 0;
	float updateTime = 0.0f;
	uint frames = 0;
	float renderTime = 0.0f;
	_timer->Start();

	WindowInstance->UpdateInputState();

	// The actual game loop.
	while (!WindowInstance->IsClosed())
	{
		WindowInstance->Clear();

		// Update input and managers.
		if (_timer->ElapsedS() - updateTime > (1.0f / 60.0f))
		{
			UpdateInput();
			WindowInstance->UpdateInputState();
			UpdateResourceManagers();
			++updates;
			updateTime += 1.0f / 60.0f;
		}

		Render();

		++frames;

		WindowInstance->Update();

		// Update service information.
		if (_timer->ElapsedS() - renderTime > 1.0f)
		{
			renderTime += 1.0f;
			_fps = frames;
			_ups = updates;
			frames = 0;
			updates = 0;
		}
	}
}

void S3DGEngine::InitializeInternalSystems()
{
	_timer = new Timer();
	_rng = new RNG();
}

void S3DGEngine::InitializeResourceManagers()
{
	GraphicsManager::Initialize();
	FontManager::Initialize();
	TextureManager::Initialize();
	SoundManager::Initialize();
}

void S3DGEngine::UpdateResourceManagers()
{
	SoundManager::Update();
}

void S3DGEngine::DisposeResourceManagers()
{
	GraphicsManager::Dispose();
	FontManager::Dispose();
	TextureManager::Dispose();
	SoundManager::Dispose();
}

void S3DGEngine::DisposeInternalSystems()
{
	SafeDelete(_rng);
	SafeDelete(_timer);
	SafeDelete(WindowInstance);
}

double S3DGEngine::GetNextRNG()
{
	return _rng->Next();
}

float S3DGEngine::GetElapsedMS()
{
	return _timer->ElapsedMS();
}