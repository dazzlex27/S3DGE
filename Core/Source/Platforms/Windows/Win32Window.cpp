/*
===========================================================================
Win32Window.cpp

Main Win32 implementation file.
===========================================================================
*/

#include <Windows.h>
#include "Platforms/Window.h"
#include "Internal/Log.h"

using namespace s3dge;
using namespace graphics;

// Forward declarations
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
PIXELFORMATDESCRIPTOR GetPixelFormatDescriptor();
bool SetupGLContext();

// Essential variables only
HWND window;
HINSTANCE instance;
HDC deviceContext;
WNDCLASSEX windowClass = {};
HGLRC renderingContext;
PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
WINDOWPLACEMENT wpc;

// Main message pump handler method
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Window* winInstance = Window::GetWindowClassInstance(window);

	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_DESTROY:
		wglMakeCurrent(deviceContext, NULL);	// Deselect rendering context.
		wglDeleteContext(renderingContext);
		PostQuitMessage(0);
		break;
	case WM_MENUCHAR:
		if (LOWORD(wParam) == VK_RETURN && winInstance)
			return MAKELRESULT(0, MNC_CLOSE);
		return MAKELRESULT(0, MNC_IGNORE);
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		if (winInstance)
			key_callback(winInstance, wParam, message);
		break;
	case WM_SIZE:
		if (winInstance)
			resize_callback(winInstance, LOWORD(lParam), HIWORD(lParam));
		break;
	default:
		if (message > 512 && message < 527 && winInstance)
			mousebutton_callback(winInstance, wParam, message);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

// Window initialization implementation
bool Window::InitializeWindow()
{
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProcedure;
	wc.hInstance = instance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = "S3DGE_Win32WindowClass";
	if (!RegisterClass(&wc))
		return 1;

	window = CreateWindowEx(
		WS_EX_APPWINDOW,
		wc.lpszClassName,
		_title,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0,
		0,
		_width,
		_height,
		HWND_DESKTOP,
		NULL,
		instance,
		NULL);

	if (!window)
	{
		LOG_FATAL("Could not create window!");
		return false;
	}

	if (!SetupGLContext())
		return false;

	Window::SetHandle(window, this);

	SetFullScreen(_fullScreen);
	SetVSync(_vSync);

	return true;
}

// Window update implementation
void Window::Update()
{
	POINT mousePosition;
	GetCursorPos(&mousePosition);
	ScreenToClient(window, &mousePosition);
	_mousePosition.x = (float)mousePosition.x;
	_mousePosition.y = (float)mousePosition.y;

	MSG messages;
	while (PeekMessage(&messages, NULL, 0, 0, PM_REMOVE))
	{
		if (messages.message == WM_QUIT)
			_isClosed = true;

		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

	SwapBuffers(deviceContext);
}

// Window clear implementation
void Window::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Switches between window fullscreen states
void Window::SetFullScreen(bool fullscreen)
{
	if (fullscreen)
	{
		GetWindowPlacement(window, &wpc);
		SetWindowLongPtr(window, GWL_STYLE, WS_POPUP);
		SetWindowLong(window, GWL_EXSTYLE, WS_EX_TOPMOST);
		ShowWindow(window, SW_SHOWMAXIMIZED);
		SetFocus(window);
	}
	else
	{
		SetWindowLongPtr(window, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		SetWindowLong(window, GWL_EXSTYLE, WS_EX_APPWINDOW);
		SetWindowPlacement(window, &wpc);
		ShowWindow(window, SW_SHOWDEFAULT);

	}

	SetFocus(window);
	_fullScreen = fullscreen;
}

// Switches window vsync state
void Window::SetVSync(bool vsync)
{
	typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

	cstring extensions = (char*)glGetString(GL_EXTENSIONS);

	if (strstr(extensions, "WGL_EXT_swap_control") == 0)
		return;

	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(vsync);

		this->_vSync = vsync;
	}
}

// Applies OpenGL context to a created window
bool SetupGLContext()
{
	deviceContext = GetDC(window);

	if (!deviceContext)
	{
		LOG_ERROR("Failed getting device context!");
		return false;
	}

	pixelFormatDescriptor = GetPixelFormatDescriptor();

	int pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDescriptor);

	if (!pixelFormat)
	{
		LOG_ERROR("Failed choosing pixel format!");
		return false;
	}

	if (!SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDescriptor))
	{
		LOG_ERROR("Failed setting pixel format!");
		return false;
	}

	renderingContext = wglCreateContext(deviceContext);

	if (!renderingContext)
	{
		LOG_ERROR("Failed to create rendering context");
		return false;
	}

	if (!wglMakeCurrent(deviceContext, renderingContext))
	{
		LOG_ERROR("Failed setting OpenGL context!");
		return false;
	}

	if (glewInit() != GLEW_OK)
	{
		LOG_ERROR("Failed to initialize GLEW!");
		return false;
	}

	return true;
}

// Triggers whenever a window changes size
void s3dge::graphics::resize_callback(Window* window, uint width, uint height)
{
	if (height == 0)
		height = 1;

	window->_width = width;
	window->_height = height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);	//  Set current Matrix to projection
	glLoadIdentity();				//reset projection matrix
	glMatrixMode(GL_MODELVIEW);		//set modelview matrix
	glLoadIdentity();				//reset modelview matrix
}

// Triggers whenever a mouse button is manipulated.
// Currently supports 9 mouse buttons
void s3dge::graphics::mousebutton_callback(Window* window, int key, int command)
{
	switch (command)
	{
	case WM_LBUTTONDOWN:
	{
		window->_buttonsDown[VK_LMB] = true;
		window->_buttonsClicked[VK_LMB] = true;
		return;
	}
	case WM_LBUTTONUP:
	{
		window->_buttonsDown[VK_LMB] = false;
		return;
	}
	case WM_RBUTTONDOWN:
	{
		window->_buttonsDown[VK_RMB] = true;
		window->_buttonsClicked[VK_RMB] = true;
		return;
	}
	case WM_RBUTTONUP:
	{
		window->_buttonsDown[VK_RMB] = false;
		return;
	}
	case WM_MBUTTONDOWN:
	{
		window->_buttonsDown[VK_MMB] = true;
		window->_buttonsClicked[VK_MMB] = true;
		return;
	}
	case WM_MBUTTONUP:
	{
		window->_buttonsDown[VK_MMB] = false;
		return;
	}
	case WM_XBUTTONDOWN:
	{
		if (GET_XBUTTON_WPARAM(key) == XBUTTON1)
		{
			window->_buttonsDown[VK_XBUTTON1] = true;
			window->_buttonsClicked[VK_XBUTTON1] = true;
		}
		else
		{
			window->_buttonsDown[VK_XBUTTON2] = true;
			window->_buttonsClicked[VK_XBUTTON2] = true;
		}
		return;
	}
	case WM_XBUTTONUP:
	{
		if (GET_XBUTTON_WPARAM(key) == XBUTTON1)
			window->_buttonsDown[VK_XBUTTON1] = false;
		else
			window->_buttonsDown[VK_XBUTTON2] = false;
		return;
	}
	case WM_MOUSEWHEEL:
	{
		short zDelta = (short)GET_WHEEL_DELTA_WPARAM(key);
		if (zDelta > 0)
		{
			window->_buttonsDown[VK_MWUP] = true;
			window->_buttonsClicked[VK_MWUP] = true;
		}
		if (zDelta < 0)
		{
			window->_buttonsDown[VK_MWDOWN] = true;
			window->_buttonsClicked[VK_MWDOWN] = true;
		}
		return;
	}
	}
}

// Triggers whenever a keyboard key is manipulated
void s3dge::graphics::key_callback(Window* window, int key, int command)
{
	switch (command)
	{
	case WM_KEYDOWN:
		window->_keysDown[key] = true;
		window->_keysClicked[key] = true;
		break;
	case WM_KEYUP:
		window->_keysDown[key] = false;
		break;
	case WM_SYSKEYDOWN:
		if (key == VK_RETURN)
			window->SetFullScreen(!window->IsFullScreen());
		break;
	}
}

// Returns a struct containing screen pixel format data.
// Required for window setup
PIXELFORMATDESCRIPTOR GetPixelFormatDescriptor()
{
	PIXELFORMATDESCRIPTOR pfd = { 0 };

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	return pfd;
}