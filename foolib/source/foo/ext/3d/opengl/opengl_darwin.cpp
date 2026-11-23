#ifdef FOO_PLATFORM_DARWIN

	#ifdef FOO_COMPILER_GCC
		// System Headers
		#define __CARBONSOUND__
		#define __OPENSCRIPTING__
		#define __PRINT__
		#define __NAVIGATIONSERVICES__
		#define __COMMONPANELS__
		#define __HTMLRENDERING__
		#define __SPEECHRECOGNITION__
		#define __SECURITYHI__
		#define __HELP__
		#define __IMAGECAPTURE__
		
		#include <Carbon/Carbon.h>
	#else
		#include <Carbon/Carbon.h>
	#endif

	#include <AGL/agl.h>
	#include <AGL/gl.h>
	#include <AGL/glu.h>
	#include <AGL/aglMacro.h>
	#include <mach-o/dyld.h>

#include <foo/base/base.h>
#include <foo/base/log.h>
#include <foo/func/classregistry.h>
#include <foo/ext/input/input.h>
#include <foo/ext/input/mouse.h>
//#include <foo/ext/input/input.h>


#include "opengl.h"

//
using namespace NFoo;
using namespace NFoo::N3D;
using namespace NFoo::N3D::NOpenGL;


void *GLExtInit_GetProcAddress(const char *name)
{

// #import <mach-o/dyld.h>
// #import <stdlib.h>
// #import <string.h>
//void *NSGLGetProcAddress(const char *name)
//{
	// code snatched from http://developer.apple.com/qa/qa2001/qa1188.html
    NSSymbol symbol;
    int8 *symbolName;
	
    // Prepend a '_' for the Unix C symbol mangling convention
    symbolName = new int8[strlen (name) + 2];
    strcpy(symbolName + 1, name);
    symbolName[0] = '_';
    symbol = NULL;
    if (NSIsSymbolNameDefined (symbolName))
        symbol = NSLookupAndBindSymbol (symbolName);
    free (symbolName);
    return symbol ? NSAddressOfSymbol (symbol) : NULL;
}

/*
	return aglGetProcAddress(_pString);
	return NULL;
}
*/
class CMouse_OpenGLDarwin *g_pMouse = NULL;

class CMouse_OpenGLDarwin : public CMouse
{ 
	friend class COpenGLDevice_Darwin;
public:
	virtual bool Update()
	{
		g_pMouse = this;
		m_pAxis[0] = 0;
		m_pAxis[1] = 0;
		return CMouse::Update();
	}
};
MACRO_REGISTER_CLASS(CMouse_OpenGLDarwin, "mouse")

class COpenGLDevice_Darwin : public COpenGLDevice
{
public:
    AGLContext m_RenderContext;	
    WindowRef m_Window;
	bool m_GotFocus;
	bool m_HideCursor;
	
	//CMouse_OpenGLDarwin *m_pMouse;

	//
	//
	//
	COpenGLDevice_Darwin()
	{
		m_Active = false;
		m_GotFocus = false;
		m_HideCursor = false;
		//m_pMouse = (CMouse_OpenGLDarwin*)NClassRegistry::CreateByName("CMouse_OpenGLDarwin");
	}
	
	// get a long value from a dictionary
	static long getDictLong(const void *p, CFStringRef key)
	{
		CFDictionaryRef refDict = (CFDictionaryRef)p;
		long int_value;
		CFNumberRef number_value = (CFNumberRef) CFDictionaryGetValue(refDict, key);
		if (!number_value) // if can't get a number for the dictionary
			return -1;  // fail
		if (!CFNumberGetValue(number_value, kCFNumberLongType, &int_value)) // or if cant convert it
			return -1; // fail
		return int_value; // otherwise return the long value
	}

	virtual int32 EnumDisplayModes(TDynamicArray<CMode> &_rModes, CMode _Mode)
	{
		// get all displays/adapters
		CGDisplayCount MaxDisplays = 0, NumDisplays = 0;
		CGDirectDisplayID *pDisplays = NULL;
		CGGetActiveDisplayList(0 , NULL, &MaxDisplays);
		pDisplays = new CGDirectDisplayID[MaxDisplays];
		CGGetActiveDisplayList(MaxDisplays, pDisplays, &NumDisplays);
		
		bool Log = false;

		/*
		int32 Count = 0;
		AGLPixelFormat PixFormat = aglNextPixelFormat(NULL);
		while(PixFormat)
		{
			Count++;
			PixFormat = aglNextPixelFormat(PixFormat);
		}*/

		for(int32 d = 0; d < NumDisplays; d++)
		{
			//AGLRendererInfo kAGLInfo = aglQueryRendererInfo(pDisplays[d], 1);
			
			int32 c_width = CGDisplayPixelsWide(pDisplays[d]);
			int32 c_height = CGDisplayPixelsHigh(pDisplays[d]);
			int32 c_color = CGDisplayBitsPerPixel(pDisplays[d]);
			if(Log)
				NLog::WriteF("Display #%d: %dx%dx%d\n", d, c_width, c_height, c_color);
			
			CFArrayRef aModes;
			aModes = CGDisplayAvailableModes(pDisplays[d]);
			if(aModes == NULL) // check for useless displays
				continue;
			
			for(int32 f = 0; f <= 1; f++) // fullscreen
			{
				int32 NumModes = CFArrayGetCount(aModes);
				if(Log)
					NLog::WriteF("\t%d modes\n", NumModes);
				for(int32 j = 0; j < NumModes; j++)
				{
					int32 width = getDictLong(CFArrayGetValueAtIndex(aModes, j),  kCGDisplayWidth);
					int32 height = getDictLong(CFArrayGetValueAtIndex(aModes, j),  kCGDisplayHeight);
					int32 colorbits = getDictLong(CFArrayGetValueAtIndex(aModes, j),  kCGDisplayBitsPerPixel);
					int32 refreshrate = getDictLong(CFArrayGetValueAtIndex(aModes, j),  kCGDisplayRefreshRate);
					if(Log)
						NLog::WriteF("\t#%d %dx%dx%d %dhz\n", j, width, height, colorbits, refreshrate);

					// we don't allow windowed resolutions that are higher then the current
					if(f && (c_width <= width || c_width <= width) && (colorbits != c_color))
						continue;
					
					
					for(int32 stencilbits = 0; stencilbits <= 8; stencilbits+=8)
					for(int32 depthbits = 16; depthbits <= 32; depthbits+=16)
					{
						CMode Params;
						Params.Specify(CMode::EWidth, width);
						Params.Specify(CMode::EHeight, height);
						Params.Specify(CMode::ERefreshRate, refreshrate);
						Params.Specify(CMode::EFullscreen, f); // windowed or fullscreen
						
						Params.Specify(CMode::EColorBits, colorbits);
						Params.Specify(CMode::EAlphaBits, 0);
						Params.Specify(CMode::EDepthBits, depthbits);
						Params.Specify(CMode::EStencilBits, 8);
		
						Params.Specify(CMode::EAdapter, d);
						
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
		}
		
		return 0;
	}
	
	virtual bool Init(CMode _Params)
	{
		CMode Mode;
		if(!GetBestMode(_Params, Mode))
		{
			NLog::WriteF("COpenGLDevice_DARWIN::Init(): Couldn't find a displaymode that fits\n");
			return false;
		}
			
		int32 Width = _Params.Get(CMode::EWidth);
		int32 Height = _Params.Get(CMode::EHeight);
		
		NLog::WriteF("COpenGLDevice_DARWIN::Init(): Initing display, %dx%dx%d fullscreen=%d\n",
						Width, Height, 
						_Params.Get(CMode::EColorBits),
						_Params.Get(CMode::EFullscreen));
	
		//
		// Create Window
		//
		NLog::Write("COpenGLDevice_DARWIN::Init(): Creating window\n");
		Rect wRect;
		SetRect(&wRect,50,50,50+Width,50+Height);
		m_Window = NewCWindow(nil, &wRect, (const uint8*)"\4Test", true, zoomNoGrow, (WindowRef) -1, true, 0);
	
		// Init OpenGL
		AGLPixelFormat PixelFormat;
		GLint Attribs[] = {AGL_RGBA, AGL_DOUBLEBUFFER, AGL_DEPTH_SIZE, 32, AGL_ACCELERATED, AGL_NONE};
		//AGL_STENCIL_SIZE
		NLog::Write("COpenGLDevice_DARWIN::Init(): Choosing pixel format\n");
		PixelFormat = aglChoosePixelFormat(NULL, 0, Attribs);
		
		NLog::Write("COpenGLDevice_DARWIN::Init(): Creating render context\n");
		m_RenderContext = aglCreateContext(PixelFormat, NULL);
		aglSetDrawable(m_RenderContext, GetWindowPort(m_Window));
		aglSetCurrentContext(m_RenderContext);
		
		aglDestroyPixelFormat(PixelFormat);
	
		m_Active = true;

		// add mouse
		//NCore::GetCore()->m_lMouse.Add(m_pMouse);

		//InitExts();

		NLog::WriteF("COpenGLDevice_DARWIN::Init(): Final report, rendercontext=%.8x\n", m_RenderContext);
		COpenGLDevice::Init(Mode);
		return true;
	}
	
	virtual bool Shutdown()
	{
		return true;
	}
	
	virtual bool Update()
	{
		if(!m_Active)
			return true;
	
		EventRecord event;
	
		// handle mouse
//		if(WaitNextEvent(0, &event, 0, nil))
//			PollMouse();
//			DoEvent(&event, this);
			
		// the core handles the keyboard messages
		while(WaitNextEvent(everyEvent&(~(keyDownMask|keyUpMask)), &event, 0, nil))
			DoEvent(&event);
	
		// Mouse Evenets
		bool LooseFocus = false;
		
		if(IsWindowActive(m_Window))
		//if(m_Window == FrontWindow())
		{
			// get windows size
			Rect TempRect;
			TVector2<int32> Center;
			GetWindowBounds(m_Window, kWindowContentRgn, &TempRect);
			Center.x = TempRect.left + (TempRect.right-TempRect.left)/2;
			Center.y = TempRect.top + (TempRect.bottom-TempRect.top)/2;
						
			// get mouse location
			Point Location;
			GetMouse(&Location);
			TVector2<int32> Pos(Location.h-Center.x, Location.v-Center.y);

			
			//CGMouseDelta dx, dy ; 
			//CGGetLastMouseDelta( &dx, &dy ) ; 
			
			if(g_pMouse)
			{
				g_pMouse->m_pAxis[0] += Pos.x;
				g_pMouse->m_pAxis[1] += Pos.y;
			}
			/*
			// Send Message
			NCore::CMessage Message;
			Message.m_MessageID = NCore::EMsg_MouseMove;
			Message.m_aParameters[0] = Pos.x;
			Message.m_aParameters[1] = Pos.y;
			NCore::AddMessage(Message);
			*/
			
			
			// Move mouse again
			CGPoint NewPoint;
			NewPoint.x = Center.x;
			NewPoint.y = Center.y;

			CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, NewPoint);
			CGAssociateMouseAndMouseCursorPosition(TRUE); // must be here for some strange reason
	
	
			if(!m_GotFocus)
			{
				HideCursor();
				//NLog::Write("opengl: got focus\n");
			}
			
			m_GotFocus = true;
			LooseFocus = false;
		}
		else
			LooseFocus = true;
			
		if(LooseFocus)
		{
			if(m_GotFocus)
			{
				ShowCursor();
				//NLog::Write("opengl: lost focus\n");
			}
				
			m_GotFocus = false;
		}
		
	
		return true;
	}

	virtual bool SwapBuffers()
	{
		aglSwapBuffers(m_RenderContext);
		return true;
	}

	//
	//
	//
	void DrawWindow()
	{
	    GrafPtr curPort;
		
	    GetPort(&curPort);
	    SetPort(GetWindowPort(m_Window));
	    BeginUpdate(m_Window);
	    DrawControls(m_Window);
	    DrawGrowIcon(m_Window);
	    EndUpdate(m_Window);
				
	    SetPort(curPort);
	}

	void PollMouse()
	{
		CGMouseDelta dx, dy ;
		CGGetLastMouseDelta(&dx, &dy);
		
		NCore::CMessage Msg(NCore::EMsg_MouseMove);
		Msg.m_aParameters[0] = dx; //Pos.x - Center.x; 
		Msg.m_aParameters[1] = dy; //Pos.y - Center.y; 
		Msg.m_aParameters[2] = 1; 
		NCore::AddMessage(Msg);
	}

	//
	//
	//
	void DoEvent(EventRecord *_pEvent)
	{
	    short part;
	    Boolean hit;
	    //char key;
	    Rect tempRect;
	    WindowRef whichWindow;
		//HideCursor
	    switch (_pEvent->what) 
	    {
		case mouseUp:
			{
				if(g_pMouse)
					g_pMouse->ReleaseButton(0);
			} break;
			 
		case mouseDown:
			part = FindWindow(_pEvent->where, &whichWindow);
			switch (part)
			{
			case inContent:
				{
					if (whichWindow != FrontWindow()) 
						SelectWindow(whichWindow);
					
					if(g_pMouse)
						g_pMouse->PressButton(0);
				}
				break;
				
			case inDrag:	// pass screenBits.bounds
				GetRegionBounds(GetGrayRgn(), &tempRect);
				DragWindow(whichWindow, _pEvent->where, &tempRect);
				break;
				
			case inGoAway:
				if (TrackGoAway(whichWindow, _pEvent->where))
					DisposeWindow(whichWindow);
				break;
				
			case inZoomIn:
			case inZoomOut:
				hit = TrackBox(whichWindow, _pEvent->where, part);
				if (hit) 
				{
					SetPort(GetWindowPort(whichWindow));   // window must be current port
					EraseRect(GetWindowPortBounds(whichWindow, &tempRect));   // inval/erase because of ZoomWindow bug
					ZoomWindow(whichWindow, part, true);
					InvalWindowRect(whichWindow, GetWindowPortBounds(whichWindow, &tempRect));	
				}
				break;
			}
			break;
			
		/*
		case keyDown:
		case autoKey:
			key = _pEvent->message & charCodeMask;
			if (_pEvent->modifiers & cmdKey)
				if (_pEvent->what == keyDown)
					DoMenuCommand(MenuKey(key));
			*/					
		case updateEvt:
			DrawWindow();
			break;
			
		case kHighLevelEvent:
			AEProcessAppleEvent(_pEvent);
			break;
		}
	}
};

MACRO_REGISTER_CLASS(COpenGLDevice_Darwin, N3D::aDeviceClassTag)

#endif

