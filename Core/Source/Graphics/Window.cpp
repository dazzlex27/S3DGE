/*
===========================================================================
Window.cpp

Contains basic high-level window management functions.
===========================================================================
*/

#include "Window.h"
#include "Internal/Log.h"
#include "Internal/DeleteMacros.h"
#include "Platforms/KeyCodes.h"
#include "Utilities/Timers/Timer.h"

using namespace s3dge;
using namespace graphics;

std::map<void*, Window*> Window::_windowInstances;

Window::Window(cstring title, uint width, uint height, bool fullscreen, bool vsync)
	: _title(title), _width(width), _height(height), _fullScreen(fullscreen), _vSync(vsync), _isClosed(false)
{
	if (!Initialize())
	{
		LOG_FATAL("Could not initialize window!");
		abort();
	}
}

bool Window::Initialize()
{
	if (!CreateMainWindow())
	{
		LOG_FATAL("Could not create window!");
		return false;
	}

	if (!CreateContext())
	{
		LOG_FATAL("Could not create context!");
		return false;
	}

	SetupContext();
	SetFullScreen(_fullScreen);
	SetVSync(_vSync);
	InitializeInput();

	return true;
}

void Window::InitializeInput()
{
	memset(&_keysDown, 0, sizeof(_keysDown));
	memset(&_keysClicked, 0, sizeof(_keysClicked));
	memset(&_buttonsDown, 0, sizeof(_buttonsDown));
	memset(&_buttonsClicked, 0, sizeof(_buttonsClicked));
	memset(&_buttonsDoubleClicked, 0, sizeof(_buttonsDoubleClicked));
	for (int i = 0; i < MAX_BUTTONS; ++i)
		_doubleClickTimers[i] = new Timer();

	_elapsedDoubleClickThreshold = 1.0f;
}

Window::~Window()
{
	Dispose();
}

void Window::Dispose()
{
	for (int i = 0; i < MAX_BUTTONS; ++i)
		SafeDelete(_doubleClickTimers[i]);
}

// Triggers once every 1/60 of a second.
// Resets all click and double-click states.
// Also resets mouse wheel rotation state
void Window::UpdateInputState()
{
	if (_buttonsDown[S3_KEY_MWUP])
		_buttonsDown[S3_KEY_MWUP] = false;
	if (_buttonsDown[S3_KEY_MWDOWN])
		_buttonsDown[S3_KEY_MWDOWN] = false;

	memset(&_keysClicked, 0, sizeof(_keysClicked));
	memset(&_buttonsClicked, 0, sizeof(_buttonsClicked));
	for (int i = 0; i < MAX_BUTTONS; ++i)
		if (_doubleClickTimers[i]->IsRunning())
			if (_doubleClickTimers[i]->ElapsedS() > _elapsedDoubleClickThreshold)
				_doubleClickTimers[i]->Stop();

	memset(&_buttonsDoubleClicked, 0, sizeof(_buttonsDoubleClicked));
}

bool Window::KeyDown(uint key) const
{
	return _keysDown[key];
}

bool Window::KeyClicked(uint key) const
{
	return _keysClicked[key];
}

bool Window::MouseButtonDoubleClicked(uint key) const
{
	return _buttonsDoubleClicked[key];
}

bool Window::MouseButtonDown(uint button) const
{
	return _buttonsDown[button];
}

bool Window::MouseButtonClicked(uint button) const
{
	return _buttonsClicked[button];
}

// Inserts a window instance into a static window collection
void Window::SetHandle(void* instance, Window* window)
{
	_windowInstances[instance] = window;
}

// Returns a window instance of a specified index
Window* Window::GetWindowClassInstance(void* windowInstance)
{
	return _windowInstances[windowInstance];
}