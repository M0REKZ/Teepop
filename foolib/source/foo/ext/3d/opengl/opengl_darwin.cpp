#ifdef FOO_PLATFORM_DARWIN

#include <SDL3/SDL.h>

#include <foo/core/core_darwin.h>
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
	return (void *)SDL_GL_GetProcAddress(name);
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
    SDL_GLContext m_RenderContext = NULL;
	SDL_Window * m_pSDLWindow = nullptr;
	bool m_GotFocus = false;
	bool m_QuitEventReceived = false;
	
	//CMouse_OpenGLDarwin *m_pMouse;

	//
	//
	//
	COpenGLDevice_Darwin()
	{
		m_Active = false;
		m_GotFocus = false;
		//m_pMouse = (CMouse_OpenGLDarwin*)NClassRegistry::CreateByName("CMouse_OpenGLDarwin");
	}

	virtual int32_t EnumDisplayModes(TDynamicArray<CMode> &_rModes, CMode _Mode)
	{
		// get all displays/adapters
		int MaxDisplays = 0, NumDisplays = 0;
		SDL_DisplayID * pDisplays = SDL_GetDisplays(&NumDisplays);
		if(!pDisplays)
		{
			return 0;
		}
		
		bool Log = false;
		for(int32_t d = 0; d < NumDisplays; d++)
		{
			//AGLRendererInfo kAGLInfo = aglQueryRendererInfo(pDisplays[d], 1);
			
			int c_width;
			int c_height;

			SDL_GetWindowSizeInPixels(m_pSDLWindow, &c_width, &c_height);
			
			const SDL_DisplayMode * pMode;
			pMode = SDL_GetDesktopDisplayMode(pDisplays[d]);
			if(!pMode) // check for useless displays
				continue;
				
			for(int32_t f = 0; f <= 1; f++) // fullscreen
			{
				CMode Params;
				Params.Specify(CMode::EWidth, c_width);
				Params.Specify(CMode::EHeight, c_height);
				Params.Specify(CMode::ERefreshRate, pMode->refresh_rate);
				Params.Specify(CMode::EFullscreen, f); // windowed or fullscreen
				
				Params.Specify(CMode::EColorBits, 32); //RGBA8888
				Params.Specify(CMode::EAlphaBits, 8); //alpha for RGBA
				Params.Specify(CMode::EDepthBits, 32);
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
		
		return 0;
	}
	
	virtual bool Init(CMode _Params)
	{		
		int32_t Width = _Params.Get(CMode::EWidth);
		int32_t Height = _Params.Get(CMode::EHeight);
		int32_t FullScreen = _Params.Get(CMode::EFullscreen);
		
		NLog::WriteF("COpenGLDevice_DARWIN::Init(): Initing display, %dx%dx%d fullscreen=%d\n",
						Width, Height, 
						_Params.Get(CMode::EColorBits),
						FullScreen);
	
		//
		// Create Window
		//
		NLog::Write("COpenGLDevice_DARWIN::Init(): Creating window\n");
		if(!SDL_Init(SDL_INIT_VIDEO))
		{
			NLog::WriteF("COpenGLDevice_DARWIN::Init(): Error while trying to init SDL3: %s\n", SDL_GetError());
			return false;
		}


		SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

		m_pSDLWindow = SDL_CreateWindow("\4Test", Width, Height, SDL_WINDOW_OPENGL | (FullScreen ? SDL_WINDOW_FULLSCREEN : 0));
		if(!m_pSDLWindow)
		{
			NLog::WriteF("COpenGLDevice_DARWIN::Init(): Error while trying to create SDL3 Window: %s\n", SDL_GetError());
			return false;
		}

		NLog::Write("COpenGLDevice_DARWIN::Init(): Creating render context\n");
		m_RenderContext = SDL_GL_CreateContext(m_pSDLWindow);
		if(!m_RenderContext)
		{
			NLog::WriteF("COpenGLDevice_DARWIN::Init(): Error while trying to create OpenGL Context %s\n", SDL_GetError());
			return false;
		}

		CMode Mode;
		if(!GetBestMode(_Params, Mode))
		{
			NLog::WriteF("COpenGLDevice_DARWIN::Init(): Couldn't find a displaymode that fits\n");
			return false;
		}
	
		m_Active = true;
		SDL_HideCursor();

		// add mouse
		//NCore::GetCore()->m_lMouse.Add(m_pMouse);

		//InitExts();

		NLog::WriteF("COpenGLDevice_DARWIN::Init(): Final report, rendercontext=%.8x\n", m_RenderContext);
		COpenGLDevice::Init(Mode);
		return true;
	}
	
	virtual bool Shutdown()
	{
		if(m_RenderContext)
		{
			SDL_GL_DestroyContext(m_RenderContext);
			m_RenderContext = NULL;
		}
		if(m_pSDLWindow)
		{
			SDL_DestroyWindow(m_pSDLWindow);
			m_pSDLWindow = nullptr;
		}
		SDL_Quit();
		return true;
	}
	
	virtual bool Update()
	{
		if(!m_Active)
			return true;
	
		NCore::CCore_OSX *pCore = NCore::CCore_OSX::Get();

		for(int32_t i = 0; i < pCore->m_lSDLEvents.Len(); i++)
		{
			DoEvent(pCore->m_lSDLEvents[i]);
		}

		// Mouse Evenets
		bool LooseFocus = false;
		
		if(g_pMouse)
		{
			float mouseX, mouseY;
    		SDL_GetGlobalMouseState(&mouseX, &mouseY);
			int windowX, windowY;
    		SDL_GetWindowPosition(m_pSDLWindow, &windowX, &windowY);
			int windowW, windowH;
    		SDL_GetWindowSizeInPixels(m_pSDLWindow, &windowW, &windowH);

			// Mouse position relative to the window
			float localMouseX = mouseX - windowX;
			float localMouseY = mouseY - windowY;

			// Is the mouse inside the actual rendered window area?
			if (localMouseX >= 0 &&
				localMouseX <  windowW &&
				localMouseY >= 0 &&
				localMouseY <  windowH)
			{
				g_pMouse->m_pAxis[0] += localMouseX;
				g_pMouse->m_pAxis[1] += localMouseY;
				m_GotFocus = true;
			}
			else
			{
				// Ignore clicks outside the game area
				m_GotFocus = false;
				LooseFocus = true;
			}

			
		}
		
		
			
		if(LooseFocus)
		{
			if(m_GotFocus)
			{
				//ShowCursor();
				//NLog::Write("opengl: lost focus\n");
			}
				
			m_GotFocus = false;
		}
		
	
		return !m_QuitEventReceived;
	}

	virtual bool SwapBuffers()
	{
		SDL_GL_SwapWindow(m_pSDLWindow);
		return true;
	}

	//
	//
	//

	//
	//
	//
	void DoEvent(SDL_Event & sdlevent)
	{
		//HideCursor
	    switch (sdlevent.type) 
	    {
		case SDL_EVENT_QUIT:
		{
			m_QuitEventReceived = true;
		}
		break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			if(g_pMouse)
				g_pMouse->ReleaseButton(0);
		}
		break;
			 
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{	
			if(m_GotFocus && g_pMouse)
				g_pMouse->PressButton(0);
		}
		break;
		}
	}
};

MACRO_REGISTER_CLASS(COpenGLDevice_Darwin, N3D::aDeviceClassTag)

#endif

