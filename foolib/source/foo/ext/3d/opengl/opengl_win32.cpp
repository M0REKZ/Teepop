#ifdef FOO_PLATFORM_WIN32

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRA_LEAN
#include <windows.h> // damn it

#include <foo/base/base.h>
#include <foo/base/log.h>
#include <foo/base/string.h>
#include <foo/base/sys.h>
#include <foo/ext/input/mouse.h>
#include <foo/ext/input/mouse.h>
#include <foo/func/classregistry.h>
#include "opengl.h"

HWND g_WIN32_MainWindowHandle;

//
using namespace NFoo;
using namespace NFoo::N3D;
using namespace NFoo::N3D::NOpenGL;

void *GLExtInit_GetProcAddress(const char *_pString)
{
	return (void*)wglGetProcAddress(_pString);
}

class COpenGLDevice_Win32;



//
/*
class CKeyboard_OpenGLWin32 : public NCore::CKeyboard
{
	friend LRESULT CALLBACK OGLWndProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);
public:
};
MACRO_REGISTER_CLASS(CKeyboard_OpenGLWin32, NCore::aKeyboardClassTag)
*/

LRESULT CALLBACK OGLWndProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);

// Locals
namespace 
{
	int32_t g_iOpenGLKeyboardClass = 0;
	COpenGLDevice_Win32 *g_pOpengDevice_Win32 = NULL;
}

//
class COpenGLDevice_Win32 : public COpenGLDevice
{
public:
	HWND m_hWindow;
	HDC m_hDeviceContext;
	HGLRC m_hRenderContext;
	HINSTANCE m_hInstance;

	//EnumDisplaySettings

	COpenGLDevice_Win32()
	{
		g_pOpengDevice_Win32 = this;
		m_Active = false;
	}

	~COpenGLDevice_Win32()
	{
		Shutdown();

		g_pOpengDevice_Win32 = NULL;
	}

	//
	//
	//
	virtual int32_t EnumDisplayModes(TDynamicArray<CMode> &_rModes, CMode _Mode)
	{
		TDynamicArray<PIXELFORMATDESCRIPTOR> lPixelFormats;

		// get screen DC
		HDC hDC = GetDC(NULL);

		// get the screen format
		PIXELFORMATDESCRIPTOR ScreenFormat;
		ScreenFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		DescribePixelFormat(hDC, GetPixelFormat(hDC), sizeof(PIXELFORMATDESCRIPTOR), &ScreenFormat);

		// enumare all pixel formats
		PIXELFORMATDESCRIPTOR Format; 
		Format.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		int32_t iFormat = 1;
		while(DescribePixelFormat(hDC, iFormat++, sizeof(PIXELFORMATDESCRIPTOR), &Format))
		{
			if(	Format.dwFlags&PFD_SUPPORT_OPENGL &&
				Format.dwFlags&PFD_DOUBLEBUFFER &&
				Format.dwFlags&PFD_DRAW_TO_WINDOW)
			{
				lPixelFormats.Add(Format);
			}
		}

		// release the screen dc
		ReleaseDC(NULL, hDC);

		NLog::WriteF("opengl_win32: PixelFormats=%d\n", lPixelFormats.Len());

		// get current mode
		DEVMODE ScreenMode;
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &ScreenMode);

		// enumerate display modes
		DEVMODE Mode;
		Mode.dmSize = sizeof(DEVMODE);

		int32_t iMode = 0;
		while(EnumDisplaySettings(NULL, iMode++, &Mode))
		{	
			for(intr i = 0; i < lPixelFormats.Len(); i++)
			{
				if(lPixelFormats[i].cColorBits != Mode.dmBitsPerPel)
					continue;

				// two turns, first windowed, then fullscreen
				for(intr f = 0; f < 2; f++)
				{
					if(f == 0)
					{
						// in windowed we need same depth as the desktop
						if(Mode.dmBitsPerPel != ScreenMode.dmBitsPerPel)
							continue;

						// can't have higher resolution then the screen
						if(Mode.dmPelsWidth > ScreenMode.dmPelsWidth || Mode.dmPelsHeight > ScreenMode.dmPelsHeight)
							continue;

						// nullify display frequency
						Mode.dmDisplayFrequency = 0;
					}

					// no alpha when we are running in 16 bit
					if(ScreenMode.dmBitsPerPel == 16 && lPixelFormats[i].cAlphaBits)
						continue;

					// construct format
					CMode Params;
					Params.Specify(CMode::EWidth, Mode.dmPelsWidth);
					Params.Specify(CMode::EHeight, Mode.dmPelsHeight);
					Params.Specify(CMode::ERefreshRate, Mode.dmDisplayFrequency);
					Params.Specify(CMode::EFullscreen, f); // windowed or fullscreen
					
					Params.Specify(CMode::EColorBits, Mode.dmBitsPerPel);
					Params.Specify(CMode::EAlphaBits, lPixelFormats[i].cAlphaBits);
					Params.Specify(CMode::EDepthBits, lPixelFormats[i].cDepthBits);
					Params.Specify(CMode::EStencilBits, lPixelFormats[i].cStencilBits);

					Params.Specify(CMode::EAdapter, CMode::EAdapter_Default);

					//for(intr p = 0; p < CMode::ENumParams; p++)
					//	NLog::WriteF("%4d ", Params.m_aParams[p].m_Value);
					//NLog::WriteF("\n");
						

					if(_Mode.Acceptable(Params))
					{
						// avoid duplicate modes
						bool Found = false;
						for(intr m = 0; m < _rModes.Len(); m++)
						{
							if(Params.Compare(_rModes[m]))
							{
								Found = true;
								break;
							}
						}

						// add format
						if(!Found)
							_rModes.Add(Params);
					}
				}
			}
		}

		return _rModes.Len();
	}

	//
	//
	//
	virtual bool Update()
	{
		return true;
	}

	//
	//
	//
	virtual bool Init(CMode _Params)
	{
		CMode Mode;
		if(!GetBestMode(_Params, Mode))
			return false;

		// find keyboard class id
		g_iOpenGLKeyboardClass = NClassRegistry::GetClassID("CKeyboard_OpenGLWin32");

		// create window class
		RECT WindowRect;
		WNDCLASS WindowClass;
		WindowRect.left = (int32_t)0;
		WindowRect.right = (int32_t)Mode.Get(CMode::EWidth);
		WindowRect.top = (int32_t)0;
		WindowRect.bottom = (int32_t)Mode.Get(CMode::EHeight);

		WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		WindowClass.lpfnWndProc = (WNDPROC)OGLWndProc;
		WindowClass.cbClsExtra = 0;
		WindowClass.cbWndExtra = 0;
		WindowClass.hInstance = m_hInstance;
		WindowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WindowClass.hbrBackground = NULL;
		WindowClass.lpszMenuName = NULL;
		WindowClass.lpszClassName = "opengl_win32";

		// register window class
		if(!RegisterClass(&WindowClass))
		{
			NLog::Write("opengl_win32: Couldn't register window class");
			return false;
		}

		// fix stuff for fullscreenmode
		if(Mode.Get(CMode::EFullscreen))
		{
			// switch video mode
			DEVMODE dmScreenSettings;
			MemorySet(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = Mode.Get(CMode::EWidth);
			dmScreenSettings.dmPelsHeight = Mode.Get(CMode::EHeight);
			dmScreenSettings.dmBitsPerPel = Mode.Get(CMode::EColorBits);
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				NLog::Write("opengl_win32: Error changing display mode\n");
				return false;
			}
		}

		// set correct window params
		DWORD dwExStyle;
		DWORD dwStyle;
		if (Mode.Get(CMode::EFullscreen))
		{
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP;
			ShowCursor(FALSE);
		}
		else
		{
			//dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME;
			//dwStyle = WS_CAPTION | WS_SYSMENU |  WS_THICKFRAME;
			dwExStyle = WS_EX_DLGMODALFRAME;
			dwStyle = WS_CAPTION | WS_SYSMENU;
			
		}

		// adjust
		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

		RECT ScreenRect;
		GetWindowRect(GetDesktopWindow(),&ScreenRect);

		if (!(m_hWindow = CreateWindowEx(	dwExStyle, "opengl_win32", "FooOpenGL", WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
											ScreenRect.right / 2 - Mode.Get(CMode::EWidth) / 2, ScreenRect.bottom / 2 - Mode.Get(CMode::EHeight) / 2,
											WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top,
											NULL, NULL, m_hInstance, NULL)))
		{
			NLog::Write("opengl_win32: Error creating window");
			return false;
		}

		g_WIN32_MainWindowHandle = m_hWindow;
		
		// show the window
		ShowWindow(m_hWindow, SW_SHOW);

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		// Set
		static PIXELFORMATDESCRIPTOR PixelFormat =
		{
			sizeof(PIXELFORMATDESCRIPTOR),              // Structure size
			1,                                          // Version number
			PFD_DRAW_TO_WINDOW |                        //
			PFD_SUPPORT_OPENGL |                        // OpenGL support
			PFD_DOUBLEBUFFER,                           // Dubblebuffer support
			PFD_TYPE_RGBA,                              // RGBA format
			(intr)Mode.Get(CMode::EColorBits),          // Bits per pixel
			0, 0, 0, 0, 0, 0,                           // Pixel Format
			(intr)Mode.Get(CMode::EAlphaBits), 0,       // Alpha buffer
			0,0, 0, 0, 0,                               // Accumbuffer
			(intr)Mode.Get(CMode::EDepthBits),          // DepthBuffer (z-Buffer)
			(intr)Mode.Get(CMode::EStencilBits),        // stecil buffer
			0,                                          // auxiliary buffer
			PFD_MAIN_PLANE,                             // layer
			0,                                          // Reserverad
			0, 0, 0                                     // Layer Mask
		};

		if (!(m_hDeviceContext = GetDC(m_hWindow)))
		{
			NLog::Write("opengl_win32: Couldn't get device context.");
			return false;
		}

		GLuint PixelFormatNum;
		if (!(PixelFormatNum = ChoosePixelFormat(m_hDeviceContext, &PixelFormat)))
		{
			NLog::Write("opengl_win32: Can't find a suitable pixel format.");
			return false;
		}

		if(!SetPixelFormat(m_hDeviceContext, PixelFormatNum, &PixelFormat))
		{
			NLog::Write("opengl_win32: Can't set the pixel format.");
			return false;
		}

		if (!(m_hRenderContext = wglCreateContext(m_hDeviceContext)))
		{
			NLog::Write("opengl_win32: Can't create a GL rendering context.");
			return false;
		}

		if(!wglMakeCurrent(m_hDeviceContext, m_hRenderContext))
		{
			NLog::Write("opengl_win32: Can't activate the GL rendering context.");
			return false;
		}

		COpenGLDevice::Init(Mode);

		SetWindowLong(m_hWindow,GWL_USERDATA,(DWORD)this);
		SetClipRegion();

		m_Active = true;

		return true;
	}

	//
	//
	//
	virtual bool SetGammaRamp(uint16 *_pRamp)
	{
		uint16 Ramp[256*3];
		MemoryCopy(Ramp, _pRamp, 256*2);
		MemoryCopy(Ramp+256, _pRamp, 256*2);
		MemoryCopy(Ramp+512, _pRamp, 256*2);
		return SetDeviceGammaRamp(GetDC(NULL), Ramp) != FALSE;
	};

	//
	//
	//
	int64 m_LastSwapTime;
	virtual bool SwapBuffers()
	{
		int64 Time = NFoo::GetTickCount();
		int8 aBuf[128];
		StringCFormat(aBuf, "FPS: %d", (int32_t)(1/((Time-m_LastSwapTime)/(fp32)GetTickFrequency())));
		SetWindowText(m_hWindow, aBuf);
		m_LastSwapTime = Time;
		
		::SwapBuffers(m_hDeviceContext);
		return true;
	}

	//
	//
	//
	virtual bool Shutdown()
	{
		if (!m_Active)
			return false;

		ClearAllAllocatedIndexes();

		uint16 Ramp[256*3];
		for(int32_t i = 0; i < 256; i++)
		{
			Ramp[i] = (uint16)(0xFFFF*(i/255.0f));
			Ramp[i+256] = Ramp[i];
			Ramp[i+512] = Ramp[i];
		}

		SetDeviceGammaRamp(GetDC(NULL), Ramp);

		// render device context
		if(m_hRenderContext)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(m_hRenderContext);
			m_hRenderContext = NULL;
		}

		// release device context
		if(m_hDeviceContext)
			ReleaseDC(m_hWindow, m_hDeviceContext);

		// destroy window and unregister window class
		DestroyWindow(m_hWindow);
		UnregisterClass("OpenGL", m_hInstance); 

		//DEVMODE Mode;
		//EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &Mode);
		//ChangeDisplaySettings(&Mode, CDS_RESET);



		m_Active = false;

		return true;
	}

	//
	//
	//
	void SetClipRegion()
	{
		return;
		RECT	ClientRect;
		POINT	Point1,Point2;

		GetClientRect(m_hWindow,&ClientRect);

		Point1.x = ClientRect.left;
		Point2.x = ClientRect.right;
		Point1.y = ClientRect.top;
		Point2.y = ClientRect.bottom;

		ClientToScreen(m_hWindow,&Point1);
		ClientToScreen(m_hWindow,&Point2);

		ClientRect.left		= Point1.x;
		ClientRect.right	= Point2.x;
		ClientRect.top		= Point1.y;
		ClientRect.bottom	= Point2.y;

		ClipCursor(&ClientRect);
	}
};

MACRO_REGISTER_CLASS(COpenGLDevice_Win32, N3D::aDeviceClassTag)

//
//
//
LRESULT CALLBACK OGLWndProc(HWND _hWnd, UINT _Msg, WPARAM _wParam, LPARAM _lParam)
{
	if(_Msg == WM_MOUSEMOVE)
	{
		/*
		NCore::CMessage *pMsg = new NCore::CMessage(NCore::EMsg_MouseMove);
		pMsg->m_aParameters[0] = LOWORD(_lParam); 
		pMsg->m_aParameters[1] = HIWORD(_lParam); 
		pMsg->m_aParameters[2] = 0; 
		NCore::m_pCore->AddMessage(pMsg);
		*/
	}

	/*
	if(_Msg == WM_MOVE)
	{
		g_pOpengDevice_Win32->
	}*/

	/*
	if(_Msg == WM_KEYDOWN || _Msg == WM_KEYUP && g_iOpenGLKeyboardClass)
	{
		// find keyboard if any
		
		for(int32_t i = 0; i < NCore::GetNumKeyboards(); i++)
		{
			if(NCore::GetKeyboard(i)->m_pClassHandle->m_iClass == g_iOpenGLKeyboardClass)
			{
				if(_Msg == WM_KEYDOWN && !(_lParam&(1<<30)))
					NCore::GetKeyboard(i)->PressButton(_wParam);
				else if(_Msg == WM_KEYUP)
					NCore::GetKeyboard(i)->ReleaseButton(_wParam);
			}
		}
	}*/
	/*
	if(_msg == WM_LBUTTONDOWN || _msg == WM_RBUTTONDOWN)
	{
		NCore::CMessage *pMsg = new NCore::CMessage(NCore::EMsg_MouseBtnDown);
		pMsg->m_aParameters[0] = LOWORD(_lParam); 
		pMsg->m_aParameters[1] = HIWORD(_lParam); 
		pMsg->m_aParameters[2] = (_msg == WM_RBUTTONDOWN);
		NCore::m_pCore->AddMessage(pMsg);
	}

	if(_msg == WM_LBUTTONUP || _msg == WM_RBUTTONUP)
	{
		NCore::CMessage *pMsg = new NCore::CMessage(NCore::EMsg_MouseBtnUp);
		pMsg->m_aParameters[0] = LOWORD(_lParam); 
		pMsg->m_aParameters[1] = HIWORD(_lParam); 
		pMsg->m_aParameters[2] = (_msg == WM_RBUTTONUP);
		NCore::m_pCore->AddMessage(pMsg);
	}*/

	if(_Msg == WM_SETFOCUS)
		ShowCursor(FALSE);

	if(_Msg == WM_KILLFOCUS)
		ShowCursor(TRUE);

	if(_Msg == WM_CLOSE)
		PostQuitMessage(0);

	//
	// Clip mouse to window. This should be disabled in rooless gui mode (?)
	//
	if(_Msg == WM_ACTIVATE)
	{
		if (LOWORD(_wParam))
			g_pOpengDevice_Win32->SetClipRegion();
		else
			ClipCursor(NULL);
	}
	else if(_Msg == WM_MOVE)
	{
		g_pOpengDevice_Win32->SetClipRegion();
	}


	return DefWindowProc(_hWnd, _Msg, _wParam, _lParam);
}

//
//
//
//#define WIN32_LEAN_AND_MEAN
//#define VC_EXTRALEAN
//#include <windows.h> // oh crap
#include <foo/core/core_win32.h>
#include <foo/core/messages.h>

class CMouse_Win32_OpenGL : public CMouse
{
public:
	CMouse_Win32_OpenGL()
	{
		UpdateRect();
		SetCursorPos(m_Org.x, m_Org.y);
	}

	void UpdateRect()
	{
		RECT WindowRect;
		GetClientRect(g_pOpengDevice_Win32->m_hWindow, &WindowRect);
		ClientToScreen(g_pOpengDevice_Win32->m_hWindow, (POINT*)&WindowRect.left);
		ClientToScreen(g_pOpengDevice_Win32->m_hWindow, (POINT*)&WindowRect.right);

		if(m_Mode == EMode_Capture)
		{
			m_Org.x = WindowRect.left + ((WindowRect.right - WindowRect.left) >> 1);
			m_Org.y = WindowRect.top + ((WindowRect.bottom - WindowRect.top) >> 1);
		}
		else if(m_Mode == EMode_Loose)
		{
			m_Org.x = WindowRect.left;
			m_Org.y = WindowRect.top;
		}
	}

	virtual void SetMode(int32_t iMode)
	{
		CMouse::SetMode(iMode);
		UpdateRect();
	}

	virtual bool Update()
	{
		if(!g_pOpengDevice_Win32)
			return true;

		if(GetFocus() != g_pOpengDevice_Win32->m_hWindow)
			return true;

		UpdateRect();

		NCore::CCore_Win32 *pCore = NCore::CCore_Win32::Get();

		GetCursorPos(&m_CursorPos);

		if(m_Mode == EMode_Capture)
		{
			m_pAxis[0] += m_CursorPos.x - m_Org.x;
			m_pAxis[1] += m_CursorPos.y - m_Org.y;
			SetCursorPos(m_Org.x, m_Org.y);
		}
		else if(m_Mode == EMode_Loose)
		{
			m_pAxis[0] = m_CursorPos.x - m_Org.x;
			m_pAxis[1] = m_CursorPos.y - m_Org.y;
		}

		for(int32_t i = 0; i < pCore->m_lWinMessages.Len(); i++)
		{
			MSG WinMsg = pCore->m_lWinMessages[i];

			// keyboard
			if(WinMsg.message == WM_LBUTTONDOWN)
				PressButton(NCore::EBtn_Mouse_Left);
			else if(WinMsg.message == WM_LBUTTONUP)
				ReleaseButton(NCore::EBtn_Mouse_Left);

			else if(WinMsg.message == WM_RBUTTONDOWN)
				PressButton(NCore::EBtn_Mouse_Right);
			else if(WinMsg.message == WM_RBUTTONUP)
				ReleaseButton(NCore::EBtn_Mouse_Right);

			else if(WinMsg.message == WM_MBUTTONDOWN)
				PressButton(NCore::EBtn_Mouse_Middle);
			else if(WinMsg.message == WM_MBUTTONUP)
				ReleaseButton(NCore::EBtn_Mouse_Middle);
				//NLog::WriteF("(%f,%f)\n", m_pAxis[0], m_pAxis[1]);
		}

		return true;
	}

private:
	POINT m_Org;
	POINT m_CursorPos;
};
MACRO_REGISTER_CLASS(CMouse_Win32_OpenGL, "mouse")

#endif

