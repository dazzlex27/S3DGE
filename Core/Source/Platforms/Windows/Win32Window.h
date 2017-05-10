/*
===========================================================================
Win32Window.h

Main Win32 implementation file.
===========================================================================
*/

#pragma once

#include <Windows.h>
#include <GL/glew.h>
#include "Internal/Log.h"
#include "Graphics/Window.h"
#include "Win32InputKeys.h"
#include "Utilities/Timers/Timer.h"

namespace s3dge
{
	namespace graphics
	{
		#ifdef S3_DEBUG
			#include "Win32OpenGLDebug.h"
		#endif

		HDC deviceContext;
		WINDOWPLACEMENT wpc;

		// Main message pump handler method
		LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			Window* winInstance = Window::GetInstance(hwnd);

			switch (message)
			{
				break;
			case WM_SETFOCUS:
				if (winInstance)
					focus_callback(winInstance, true);
				break;
			case WM_KILLFOCUS:
				if (winInstance)
					focus_callback(winInstance, false);
			case WM_CREATE:
				break;
			case WM_DESTROY:
				if (winInstance)
					winInstance->Dispose();
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

		// Returns a struct containing screen pixel format data. Required for window setup.
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

		// Window initialization implementation
		bool Window::CreateMainWindow()
		{
			WNDCLASS wc = { 0 };
			wc.lpfnWndProc = WindowProcedure;
			wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
			wc.lpszClassName = "S3DGE_Win32WindowClass";
			if (!RegisterClass(&wc))
				return 1;

			RECT clientArea;
			clientArea.left = 0;
			clientArea.top = 0;
			clientArea.bottom = _height;
			clientArea.right = _width;
			AdjustWindowRectEx(&clientArea, WS_OVERLAPPEDWINDOW | WS_VISIBLE, false, WS_EX_APPWINDOW);

			HWND window = CreateWindowEx(
				WS_EX_APPWINDOW,
				wc.lpszClassName,
				_title,
				WS_OVERLAPPEDWINDOW | WS_VISIBLE,
				0,
				0,
				clientArea.right - clientArea.left,
				clientArea.bottom - clientArea.top,
				HWND_DESKTOP,
				NULL,
				wc.hInstance,
				NULL);

			if (!window)
			{
				LOG_FATAL("Could not create window!");
				return false;
			}

			Window::SetInstance(window, this);

			return true;
		}

		// Applies OpenGL context to a created window
		bool Window::CreateContext()
		{
			deviceContext = GetDC((HWND)_handle);

			if (!deviceContext)
			{
				LOG_ERROR("Failed getting device context!");
				return false;
			}

			PIXELFORMATDESCRIPTOR pixelFormatDescriptor = GetPixelFormatDescriptor();

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

			HGLRC renderingContext = wglCreateContext(deviceContext);

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

		// Sets appropriate parameters to the OpenGL context
		void Window::SetupContext()
		{
			// Apply basic OpenGL setup
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glFrontFace(GL_CW);
			//glEnable(GL_CULL_FACE);

#ifdef S3_DEBUG
			// Enable the debug callback
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(openglCallbackFunction, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
#endif

			LOG_INFO("OpenGL v. ", (char*)glGetString(GL_VERSION));
			LOG_INFO("Renderer: ", (char*)glGetString(GL_RENDERER));
		}

		// Window update implementation
		void Window::Update()
		{
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

		// Triggers once every 1/60 of a second.
		// Resets all click and double-click states.
		// Also resets mouse wheel rotation state
		void Window::UpdateInputState()
		{
			POINT mousePosition;
			GetCursorPos(&mousePosition);

			_mousePosition.x = (float)mousePosition.x;
			_mousePosition.y = (float)mousePosition.y;

			if (_hasFocus)
				SetCursorPos(_width / 2, _height / 2);

			SetCursor(NULL);

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

		void Window::Clear()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void Window::SetFullScreen(bool fullscreen)
		{
			HWND window = (HWND)_handle;

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
				SetFocus(window);
			}

			SetFocus(window);
			_fullScreen = fullscreen;
		}

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

		void Window::DestroyContext()
		{
			HGLRC renderingContext;

			if (renderingContext = wglGetCurrentContext())
			{
				deviceContext = wglGetCurrentDC();
				// make the rendering context not current  
				wglMakeCurrent(NULL, NULL);
				// release the device context  
				ReleaseDC((HWND)_handle, deviceContext);
				// delete the rendering context  
				wglDeleteContext(renderingContext);
			}
		}

		// Triggers whenever a window changes size
		void s3dge::graphics::resize_callback(Window* window, uint width, uint height)
		{
			RECT rect = RECT();
			GetClientRect((HWND)window->_handle, &rect);

			window->_width = rect.right - rect.left;
			window->_height = rect.bottom - rect.top;

			glViewport(0, 0, window->_width, window->_height);
		}

		// Triggers whenever a mouse button is manipulated. Currently supports 9 mouse buttons.
		void s3dge::graphics::mousebutton_callback(Window* window, int key, int command)
		{
			switch (command)
			{
			case WM_LBUTTONDOWN:
			{
				window->_buttonsDown[S3_KEY_LMB] = true;
				window->_buttonsClicked[S3_KEY_LMB] = true;
				return;
			}
			case WM_LBUTTONUP:
			{
				window->_buttonsDown[S3_KEY_LMB] = false;
				return;
			}
			case WM_RBUTTONDOWN:
			{
				window->_buttonsDown[S3_KEY_RMB] = true;
				window->_buttonsClicked[S3_KEY_RMB] = true;
				return;
			}
			case WM_RBUTTONUP:
			{
				window->_buttonsDown[S3_KEY_RMB] = false;
				return;
			}
			case WM_MBUTTONDOWN:
			{
				window->_buttonsDown[S3_KEY_MMB] = true;
				window->_buttonsClicked[S3_KEY_MMB] = true;
				return;
			}
			case WM_MBUTTONUP:
			{
				window->_buttonsDown[S3_KEY_MMB] = false;
				return;
			}
			case WM_XBUTTONDOWN:
			{
				if (GET_XBUTTON_WPARAM(key) == XBUTTON1)
				{
					window->_buttonsDown[S3_KEY_XBUTTON1] = true;
					window->_buttonsClicked[S3_KEY_XBUTTON1] = true;
				}
				else
				{
					window->_buttonsDown[S3_KEY_XBUTTON2] = true;
					window->_buttonsClicked[S3_KEY_XBUTTON2] = true;
				}
				return;
			}
			case WM_XBUTTONUP:
			{
				if (GET_XBUTTON_WPARAM(key) == XBUTTON1)
					window->_buttonsDown[S3_KEY_XBUTTON1] = false;
				else
					window->_buttonsDown[S3_KEY_XBUTTON2] = false;
				return;
			}
			case WM_MOUSEWHEEL:
			{
				short zDelta = (short)GET_WHEEL_DELTA_WPARAM(key);
				if (zDelta > 0)
				{
					window->_buttonsDown[S3_KEY_MWUP] = true;
					window->_buttonsClicked[S3_KEY_MWUP] = true;
				}
				if (zDelta < 0)
				{
					window->_buttonsDown[S3_KEY_MWDOWN] = true;
					window->_buttonsClicked[S3_KEY_MWDOWN] = true;
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
				if (key == S3_KEY_RETURN)
					window->SetFullScreen(!window->IsFullScreen());
				break;
			}
		}

		// Triggers whenever window focus state is changed
		void s3dge::graphics::focus_callback(Window* window, bool hasFocus)
		{
			window->_hasFocus = hasFocus;
		}
	}
}