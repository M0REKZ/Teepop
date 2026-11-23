#if defined(FOO_PLATFORM_LINUX) || defined(FOO_PLATFORM_FREEBSD)

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/xf86vmode.h>

#include <GL/glx.h>

#include <foo/base/base.h>
#include <foo/base/log.h>
#include <foo/base/string.h>
#include <foo/base/sys.h>
#include <foo/core/messages.h>
#include <foo/func/classregistry.h>
#include "opengl.h"

using namespace NFoo;
using namespace NFoo::N3D;
using namespace NFoo::N3D::NOpenGL;

//
// Keyboard
//
class CKeyboard_OpenGLX11 : public NCore::CKeyboard 
{ 
	friend class COpenGLDevice_X11;
public: 
};
MACRO_REGISTER_CLASS(CKeyboard_OpenGLX11, NCore::aKeyboardClassTag)

int32 g_iOpenGLKeyboardClass = 0;

//
// Load the extension
//
void * GLExtInit_GetProcAddress(const int8 *_pString)
{
	//NLog::WriteF("Checking extension %s\n", _pString);	
	return (void *)glXGetProcAddressARB((GLubyte *)_pString);
}

inline int32 CalcRefreshRate(XF86VidModeModeInfo *_modeinfo) 
{
	return _modeinfo->dotclock * 1000 / (_modeinfo->htotal * _modeinfo->vtotal); 
}

//
// Helper class for display 
//
class DisplayHelper 
{
public:
	DisplayHelper():m_pDisplay(NULL) { }
	
	explicit DisplayHelper(Display *_disp):
	m_pDisplay(_disp) {}
	
	~DisplayHelper() {	close(); }
	
	inline Display *operator *() const { return m_pDisplay; }


	// returns true if display connection was opened successfully
	bool open(const char *_str) 
	{
		// close old display connection before we open a new one
		close();
		m_pDisplay = XOpenDisplay(_str);
		if(m_pDisplay == NULL)
			return false;
		return true;
	}	
	
	void close() {
		if(m_pDisplay != NULL)
		{
			XCloseDisplay(m_pDisplay);
			m_pDisplay = NULL;
		}
	}

private:  
	// we are not copyable
	DisplayHelper(const DisplayHelper &rhs);
	// we are not copyable
    DisplayHelper &operator=(const DisplayHelper &rhs);

	Display *m_pDisplay;
};

//
// OpenGL device for X11
//
class COpenGLDevice_X11 : public COpenGLDevice
{
public:
	COpenGLDevice_X11():
	    m_Window(0),
	    m_ppAllModes(0),
	    m_NewMode(0),
	    m_OldMode(0) { }


	virtual bool Init(CMode _Params);
	virtual bool Shutdown();
	virtual bool SwapBuffers();
	virtual bool Update();

private:
	int CheckVisual(Display *_pDisplay, XVisualInfo *_pVisual, int32 _Attrib);
	int CheckVideoModes(int _Width, int _Height, int _RefreshRate, 
    		XF86VidModeModeInfo **_ppVideoModes, int32 _NumVideoModes);
	
	virtual int32 EnumDisplayModes(TDynamicArray<CMode> &_rModes, CMode _Mode);
			
	bool DoEvents();
	void handleKeyEvent(XKeyEvent &event);

	DisplayHelper m_Display;
	Window	m_Window;
	GLXContext m_GLXContext;
	
	XF86VidModeModeInfo **m_ppAllModes;
	
	int32 m_NewMode;
	int32 m_OldMode;
	
	XVisualInfo m_Visual;		// The visual to use.

	CMode m_Mode;	
};

MACRO_REGISTER_CLASS(COpenGLDevice_X11, N3D::aDeviceClassTag)

int COpenGLDevice_X11::CheckVisual(Display *_pDisplay, XVisualInfo *_pVisual, int32 _Attrib) 
{  
	if(_pDisplay == NULL || _pVisual == NULL)  
		return -1; 
		
	int Ret; 
	glXGetConfig(_pDisplay, _pVisual, _Attrib, &Ret); 
	return Ret; 
}
	
int COpenGLDevice_X11::CheckVideoModes(int _Width, int _Height, int _RefreshRate, 
    	XF86VidModeModeInfo **_ppVideoModes, int32 _NumVideoModes) 
{ 
	if(_ppVideoModes == NULL || _NumVideoModes <= 0) 
		return -1; 

	intr index = -1; 
	for(intr i = 0; i < _NumVideoModes; i++) 
	{ 
		if(_Width == _ppVideoModes[i]->hdisplay && 
		   _Height == _ppVideoModes[i]->vdisplay && 
		   _RefreshRate == CalcRefreshRate(_ppVideoModes[i]))
		{ 
			index = i; break; 
		} 
	} 
	
	return index; 
}
	
//
// EnumDisplayModes
//
// 
int32 COpenGLDevice_X11::EnumDisplayModes(TDynamicArray<CMode> &_rModes, CMode _Mode) 
{ 
	if(*m_Display == NULL) {
		NLog::Write("No display connection!");
		return -1;
	}

	XF86VidModeModeInfo **ppVideoModes;
	int NumVideoModes;
	int Depth;
	int ret;

	TDynamicArray<XVisualInfo> pVisuals;
	int NumVisuals;
		
	XF86VidModeGetAllModeLines(*m_Display, XDefaultScreen(*m_Display), 
					&NumVideoModes, &ppVideoModes);
		
	// Got a bad error :/
	if(ppVideoModes == NULL || NumVideoModes <= 0)
	{
		NLog::Write("No video modes found!");
		return -1;
	}

	Depth = XDefaultDepthOfScreen(XDefaultScreenOfDisplay(*m_Display));

	// Why even bother?
	if(Depth < 16)
	{
		NLog::Write("Depth greater or equal to 16 required!");
		return -1;
	}

	// Temporary visual to match against and a temporary list
	XVisualInfo vis_template;
	XVisualInfo *tmpVisualInfo;
		
	vis_template.depth = Depth;
	vis_template.c_class = TrueColor;
	vis_template.screen = DefaultScreen(*m_Display);

	tmpVisualInfo = XGetVisualInfo(*m_Display, 
				VisualDepthMask | VisualClassMask | VisualScreenMask,
				&vis_template, &NumVisuals);
		
	for(intr i = 0; i < NumVisuals; i++)
	{
		if(!CheckVisual(*m_Display, &tmpVisualInfo[i], GLX_USE_GL))
			continue;

		if(!CheckVisual(*m_Display, &tmpVisualInfo[i], GLX_DOUBLEBUFFER))
			continue;
		
		pVisuals.Add(tmpVisualInfo[i]);
	}

	XFree(tmpVisualInfo);
		
	if(pVisuals.Len() == 0)
	{
		NLog::WriteF("opengl_x11: No visuals capable of opengl found!\n");
		return -1;
	}

	NumVisuals = pVisuals.Len();
			
	NLog::WriteF("Found %d Display Modes and %d Visuals with Depth of %d\n", 
		NumVideoModes, NumVisuals, Depth);
	
	for(intr f = 0; f < 2; f++)
	{
		for(intr i = 0; i < NumVideoModes; i++)
		{
			for(intr vis = 0; vis < NumVisuals; vis++)
			{
				CMode Params;

				// Exclude the visual if it does not support opengl
				if(!CheckVisual(*m_Display, &pVisuals[i], GLX_USE_GL))
					continue;

				// Exclude if double buffer not supported
				if(!CheckVisual(*m_Display, &pVisuals[i], GLX_DOUBLEBUFFER))
					continue;
					
				Params.Specify(CMode::EWidth, ppVideoModes[i]->hdisplay);
				Params.Specify(CMode::EHeight, ppVideoModes[i]->vdisplay);
				Params.Specify(CMode::ERefreshRate, CalcRefreshRate(ppVideoModes[i]));
				Params.Specify(CMode::EFullscreen, f);
				// for clean/readable code, undefined a few lines down
				#define CVIS(x) CheckVisual(*m_Display, &pVisuals[i], x)
				Params.Specify(CMode::EColorBits, CVIS(GLX_BUFFER_SIZE));
				Params.Specify(CMode::EAlphaBits, CVIS(GLX_ALPHA_SIZE));
				Params.Specify(CMode::EDepthBits, CVIS(GLX_DEPTH_SIZE));					
				Params.Specify(CMode::EStencilBits, CVIS(GLX_STENCIL_SIZE));
				#undef CVIS
				
				Params.Specify(CMode::EAdapter, CMode::EAdapter_Default);

				if(_Mode.Acceptable(Params))
				{
					bool Found = false;

					for(intr m = 0; m < _rModes.Len(); m++)
					{
						if(Params.Compare(_rModes[m]))
						{
							Found = true;
							break;
						}
					}

					if(!Found)
						_rModes.Add(Params);
				}
			}
		}
	}
		
	// return num modes
	return _rModes.Len();
}

//
//
//
bool COpenGLDevice_X11::Init(CMode _Params)
{ 
		
	if(!m_Display.open(NULL))
	{
		NLog::WriteF("opengl_x11: Unable to connect to X server\n");
		return false;
	}

	if(!GetBestMode(_Params, m_Mode))
	{
		NLog::WriteF("opengl_x11: unable to find a good mode\n");
		return false;
	}
		
	XVisualInfo vis_template;
	XVisualInfo *tmpVisuals;
	int NumVisuals;
		
	int Depth = XDefaultDepthOfScreen(XDefaultScreenOfDisplay(*m_Display));

	vis_template.depth = Depth;
	vis_template.c_class = TrueColor;
	vis_template.screen = XDefaultScreen(*m_Display);

	tmpVisuals = XGetVisualInfo(*m_Display, 
			VisualDepthMask | VisualClassMask | VisualScreenMask, 
			&vis_template, &NumVisuals);

	if(tmpVisuals == NULL)
	{
		NLog::WriteF("opengl_x11: Unable to get visual info.\n");
		return false;
	}

	bool VisualFound = false;
	// try to match a visual to requested 
	for(intr i = 0; i < NumVisuals; i++)
	{
		// Match the mode up perfectly .. almost
			
		// ugly macro, but cleaner code (only used for this if-statement anyways 
		#define MACRO_CVIS(x) CheckVisual(*m_Display, &tmpVisuals[i], x)
		if(m_Mode.Get(CMode::EColorBits) == MACRO_CVIS(GLX_BUFFER_SIZE) &&
		   m_Mode.Get(CMode::EAlphaBits) == MACRO_CVIS(GLX_ALPHA_SIZE) &&
		   m_Mode.Get(CMode::EDepthBits) == MACRO_CVIS(GLX_DEPTH_SIZE) &&
		   m_Mode.Get(CMode::EStencilBits) == MACRO_CVIS(GLX_STENCIL_SIZE) &&
		   MACRO_CVIS(GLX_DOUBLEBUFFER) == 1 &&
		   MACRO_CVIS(GLX_USE_GL) == 1)
		{
			#undef MACRO_CVIS
			// Copy the visual to use to keep it :>
			NLog::WriteF("Found a visual matching requested mode\n");
			NFoo::MemoryCopy(&m_Visual, &tmpVisuals[i], sizeof(XVisualInfo));
			VisualFound = true;
			break;
		}
	}
				
	XFree(tmpVisuals);

	int NumModes;
		
	if(!XF86VidModeGetAllModeLines(*m_Display, XDefaultScreen(*m_Display), 
				&NumModes, &m_ppAllModes))
	{
		NLog::WriteF("opengl_x11: Unable to get video modes.\n");
		return false;
	}
		
	m_OldMode = 0;

	bool VideoModeFound = false;

	if(m_Mode.Get(CMode::EFullscreen))
	{
		// Find a mode that fits.			
		int mode = CheckVideoModes(
				m_Mode.Get(CMode::EWidth), m_Mode.Get(CMode::EHeight),
				m_Mode.Get(CMode::ERefreshRate), m_ppAllModes, NumModes);
			
		if(mode != -1)
		{
			NLog::WriteF("Found a video mode matching requested mode\n");
			m_NewMode = mode;
			VideoModeFound = true;
		}
	}
	else
	{
		VideoModeFound = true;
		m_NewMode = -1;
	}
	
	if(!VisualFound || !VideoModeFound)
	{
		NLog::WriteF("opengl_x11: Unable to get display info\n");
		return false;
	}

	NLog::WriteF("Mode Width: %d, Height: %d, RefreshRate: %d"
				", Depth: %d, AlphaBits: %d, DepthBits: %d, StencilBits: %d\n", 
				m_Mode.Get(CMode::EWidth), m_Mode.Get(CMode::EHeight), 
		        m_Mode.Get(CMode::ERefreshRate), m_Mode.Get(CMode::EColorBits), 
	   		    m_Mode.Get(CMode::EAlphaBits), m_Mode.Get(CMode::EDepthBits), 	
				m_Mode.Get(CMode::EStencilBits));
		
	if(m_Mode.Get(CMode::EFullscreen))
	{
		// TODO: Fullscreen code.

		XF86VidModeSwitchToMode(*m_Display, XDefaultScreen(*m_Display), m_ppAllModes[m_NewMode]);
		// WindowAtrrib for fullscreen etc
	}
	else
	{
		// windowattrib for windowed etc
	}

	// TODO: Add window create code/Opengl init code here.
	Colormap cmap = XCreateColormap(*m_Display, 
	XDefaultRootWindow(*m_Display), 
	m_Visual.visual, AllocNone);
		
	XSetWindowAttributes WinAttr;
	WinAttr.colormap = cmap;
	WinAttr.event_mask = 
			ResizeRedirectMask | 
			KeyPressMask | KeyReleaseMask |
			EnterWindowMask | LeaveWindowMask |
			FocusChangeMask;

	m_Window = XCreateWindow(*m_Display, 
						XDefaultRootWindow(*m_Display), 
						0, 0,
						m_Mode.Get(CMode::EWidth), m_Mode.Get(CMode::EHeight),
						0,
						m_Visual.depth,
						CopyFromParent,
						m_Visual.visual,
						CWColormap | CWEventMask,
						&WinAttr);
					

	XMapWindow(*m_Display, m_Window);

	XSetStandardProperties(*m_Display, m_Window, 
        					"Foolib opengl", "Foolib opengl", 
                           	None, NULL, 0, NULL);
								
	XFlush(*m_Display);

	// setup glx for this window
		
	m_GLXContext = glXCreateContext(*m_Display, &m_Visual, NULL, true);

	if(m_GLXContext == NULL)
	{
		NLog::WriteF("opengl_x11: Unable to create GLX Context.\n");
		Shutdown();
		return false;
	}

	glXMakeCurrent(*m_Display, m_Window, m_GLXContext);

	COpenGLDevice::Init(m_Mode);

	return true; 
}
	
//
//
//
bool COpenGLDevice_X11::Shutdown()
{
	if(*m_Display == NULL)
		return true;

	// switch back to window mode		
	if(m_Mode.Get(CMode::EFullscreen))
		XF86VidModeSwitchToMode(*m_Display, XDefaultScreen(*m_Display), 
								m_ppAllModes[m_OldMode]);
	
	if(m_GLXContext)
		glXMakeCurrent(*m_Display, None, NULL);	// Release the context

	if(m_Window)
		XDestroyWindow(*m_Display, m_Window);
		
	m_Display.close();
	return true;
}

//
// SwapBuffer
//
// Display what you have drawn
// 
bool COpenGLDevice_X11::SwapBuffers()
{ 
	if(*m_Display == NULL)
		return false;
	
	// Swap the buffers.		
	glXSwapBuffers(*m_Display, m_Window);
	return true;
}

//
//
//
bool COpenGLDevice_X11::Update()
{
	if(*m_Display == NULL || !m_Window)
		return false;

	return DoEvents(); 
}

//
//
//
bool COpenGLDevice_X11::DoEvents()
{
	static XEvent event; 
		
	while(XPending(*m_Display) != 0) 
	{ 
		XNextEvent(*m_Display, &event); 
		switch(event.type) 
		{ 
			case ResizeRequest: 
				NLog::WriteF("FixMe: ResizeRequest\n"); 

				// For now, dont allow window resizing.
				XResizeWindow(*m_Display, m_Window, 
                   	m_Mode.Get(CMode::EWidth), m_Mode.Get(CMode::EHeight)); 
				break; 
			case KeyPress:
			case KeyRelease:
				handleKeyEvent(event.xkey);
				break; 
			case EnterNotify: 
				NLog::WriteF("FixMe: EnterNotify\n"); 
				break; 
			case LeaveNotify: 
				NLog::WriteF("FixMe: LeaveNotify\n"); 
				break; 
			case FocusIn: 
				NLog::WriteF("FixMe: FocusIn\n"); 
				break; 
			case FocusOut: 
				NLog::WriteF("FixMe: FocusOut\n"); 
				break; 
		} 
	}

	return true;
}

//
// translates x keyevents to foolib keyevents
//
void COpenGLDevice_X11::handleKeyEvent(XKeyEvent &event)
{
	using namespace NCore;
		
	struct STransTable 
	{
		int xkey, fookey;
	};
	// translator table for xkey -> fookey
	static STransTable table[] = 
	{
        // arrows
		{XK_Up, EBtn_Key_Up},
		{XK_Down, EBtn_Key_Down},
		{XK_Left, EBtn_Key_Left},
		{XK_Right, EBtn_Key_Right},
		// special keys
		{XK_Escape, EBtn_Key_Escape},
		{XK_Delete, EBtn_Key_Delete},
		{XK_Return, EBtn_Key_Enter},
		{XK_Tab, EBtn_Key_Tab},
		{XK_BackSpace, EBtn_Key_Backspace},
		{XK_Control_L, EBtn_Key_Control},
		{XK_Control_R, EBtn_Key_Control},
		{XK_space, EBtn_Key_Space},
		{XK_Shift_L, EBtn_Key_Shift},
		{XK_Shift_R, EBtn_Key_Shift},
		{XK_Menu, EBtn_Key_LMenu},
		{XK_Multi_key, EBtn_Key_RMenu},
		
		{XK_Page_Up, EBtn_Key_Page_Up},
		{XK_Page_Down, EBtn_Key_Page_Down},
		{XK_Home, EBtn_Key_Home},
		{XK_End, EBtn_Key_End},
		{XK_Delete, EBtn_Key_Delete},
		{XK_Insert, EBtn_Key_Insert},
		// f1..f12
		{XK_F1, EBtn_Key_F1},
		{XK_F2, EBtn_Key_F2},
		{XK_F3, EBtn_Key_F3},
		{XK_F4, EBtn_Key_F4},
		{XK_F5, EBtn_Key_F5},
		{XK_F6, EBtn_Key_F6},
		{XK_F7, EBtn_Key_F7},
		{XK_F8, EBtn_Key_F8},
		{XK_F9, EBtn_Key_F9},
		{XK_F10, EBtn_Key_F10},
		{XK_F11, EBtn_Key_F11},
		{XK_F12, EBtn_Key_F12},
		{0, 0} // marks end of table
	};
	// for each keyboard, translate xkey to fookey
	for(int32 i = 0; i < NCore::GetNumKeyboards(); i++)
	{
		KeySym keysym;
		char keychar[1];
		XLookupString(&event, keychar, 1, &keysym, 0);
		int32 iTable = 0;
		// find xkey in table
		while(table[iTable].xkey != 0 && table[iTable].fookey != 0)
		{
           	if(table[iTable].xkey == keysym)
               	break;
			iTable++;
		}
            
		int key = 0;
		bool special_key = false;
		// if we were at the end of the table (no key found)
		// then set key to our keysym.
		//Why you say?...well, I dont know, what else should we set it to?
		if(table[iTable].xkey == 0 && table[iTable].fookey == 0)
			key = keysym;
		else
		{
			special_key = true;
			key = table[iTable].fookey;
		}
			
		// determine keypress or release and call correct function
		if(event.type == KeyPress)
			NCore::GetKeyboard(i)->PressButton(key, special_key ? 0 : key);
		else // type is release
			NCore::GetKeyboard(i)->ReleaseButton(key, special_key ? 0 : key);
	}
}

#endif // defined(FOO_PLATFORM_LINUX) || defined(FOO_PLATFORM_FREEBSD)
