/*
===========================================================================
SoundManager.h

Defines a class responsible for managing Sound objects.

TODO: standartize all of the manager classes
===========================================================================
*/

#pragma once

#include <vector>
#include <stdio.h>
#include "Sound.h"

namespace s3dge
{
	namespace Audio
	{
		class SoundManager
		{
		private:
			static std::vector<Sound*> _sounds;
			static bool _initialized;

			static gau_Manager* _manager;
			static ga_Mixer* _mixer;

		public:
			static void Initialize(); // must be called upon engine startup
			static void Add(cstring name, cstring path, bool overrideExisting = false);
			static Sound* Get(cstring name);
			static void Update(); // must be called in the engine's main loop
			static void Dispose(); // must be called upon engine shutdown

			static inline uint GetCount() { return _sounds.size(); }

		private:
			SoundManager();
			SoundManager(const SoundManager& tRef) = delete;
			SoundManager& operator = (const SoundManager& tRef) = delete;
			~SoundManager(void) {}

			friend class Sound;
		};
	}
}