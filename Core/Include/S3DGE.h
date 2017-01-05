#pragma once

#include "../Source/Common.h"

namespace s3dge
{
	class S3DGE
	{
	private:
		Graphics::Window* _window;
		Timer* _timer;
		RNG* _rng;
		uint _fps;
		uint _ups;

	protected:
		S3DGE();
		virtual ~S3DGE();

	public:
		void Run();
		inline uint GetFPS() const { return _fps; }

	protected:
		Graphics::Window* CreateGameWindow(cstring name, uint width, uint height, bool fullscreen = false, bool vsync = true);
		double GetNextRNG();
		float GetElapsedMS();

		virtual void Initialize() = 0; // Runs upon startup.
		virtual void UpdateInput() = 0; // Runs 60 times a second.
		virtual void Render() = 0; // Runs the number of times the current frame rate.
		virtual void Dispose() = 0; // Runs upon closing the main window.

	private:
		void RunGameLoop(); // Starts the actual gameloop
		void InitializeInternalSystems();
		void InitializeResourceManagers();
		void UpdateResourceManagers();
		void DisposeResourceManagers();
		void DisposeInternalSystems();
	};
}