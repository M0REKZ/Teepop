#import <AppKit/AppKit.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

#include <dlfcn.h>
#include <foo/base/base.h>
#include <foo/ext/input/keyboard.h>
#include <foo/ext/input/mouse.h>

#include "opengl.h"

using namespace NFoo;
using namespace NFoo::NCore;
using namespace NFoo::N3D;
using namespace NFoo::N3D::NOpenGL;

static class CMouse_OpenGLDarwin *g_pMouse = NULL;
static BOOL g_CursorHidden = NO;

class CMouse_OpenGLDarwin : public CMouse
{
public:
	CMouse_OpenGLDarwin()
	{
		SetMode(EMode_Capture);
	}

	virtual bool Update()
	{
		g_pMouse = this;
		m_pAxis[0] = 0;
		m_pAxis[1] = 0;
		return CMouse::Update();
	}

	void MoveBy(float x, float y)
	{
		m_pAxis[0] += x;
		m_pAxis[1] -= y;
	}
};

MACRO_REGISTER_CLASS(CMouse_OpenGLDarwin, "mouse")

static int MapKeyCode(unsigned short keyCode)
{
	switch(keyCode)
	{
	case 36: return EBtn_Key_Enter;
	case 48: return EBtn_Key_Tab;
	case 49: return EBtn_Key_Space;
	case 51: return EBtn_Key_Backspace;
	case 53: return EBtn_Key_Escape;
	case 115: return EBtn_Key_Home;
	case 116: return EBtn_Key_Page_Up;
	case 117: return EBtn_Key_Delete;
	case 119: return EBtn_Key_End;
	case 121: return EBtn_Key_Page_Down;
	case 123: return EBtn_Key_Left;
	case 124: return EBtn_Key_Right;
	case 125: return EBtn_Key_Down;
	case 126: return EBtn_Key_Up;
	case 122: return EBtn_Key_F1;
	case 120: return EBtn_Key_F2;
	case 99: return EBtn_Key_F3;
	case 118: return EBtn_Key_F4;
	case 96: return EBtn_Key_F5;
	case 97: return EBtn_Key_F6;
	case 98: return EBtn_Key_F7;
	case 100: return EBtn_Key_F8;
	case 101: return EBtn_Key_F9;
	case 109: return EBtn_Key_F10;
	case 103: return EBtn_Key_F11;
	case 111: return EBtn_Key_F12;
	default:
		return -1;
	}
}

static int GetKey(NSEvent *event)
{
	int key = MapKeyCode([event keyCode]);
	if(key >= 0)
		return key;

	NSString *characters = [event charactersIgnoringModifiers];
	if([characters length] == 0)
		return -1;

	unichar character = [characters characterAtIndex:0];
	if(character < 128)
		return toupper((unsigned char)character);
	return (int)character;
}

static int8 GetCharacter(NSEvent *event)
{
	NSString *characters = [event characters];
	if([characters length] == 0)
		return 0;
	unichar character = [characters characterAtIndex:0];
	return character < 128 ? (int8)character : 0;
}

static void SetCursorHidden(BOOL hidden)
{
	if(hidden == g_CursorHidden)
		return;
	g_CursorHidden = hidden;
	if(hidden)
		[NSCursor hide];
	else
		[NSCursor unhide];
}

@interface TeepopOpenGLView : NSOpenGLView <NSWindowDelegate>
@end

@implementation TeepopOpenGLView

- (BOOL)acceptsFirstResponder
{
	return YES;
}

- (void)updateTrackingAreas
{
	[super updateTrackingAreas];
	NSTrackingArea *trackingArea = [[NSTrackingArea alloc]
		initWithRect:[self bounds]
		options:NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved |
			NSTrackingActiveInKeyWindow | NSTrackingInVisibleRect
		owner:self
		userInfo:nil];
	[self addTrackingArea:trackingArea];
	[trackingArea release];
}

- (void)keyDown:(NSEvent *)event
{
	CKeyboard *keyboard = CInput::Keyboard();
	int key = GetKey(event);
	if(keyboard && key >= 0 && key < 256)
		keyboard->PressButton(key, GetCharacter(event));
}

- (void)keyUp:(NSEvent *)event
{
	CKeyboard *keyboard = CInput::Keyboard();
	int key = GetKey(event);
	if(keyboard && key >= 0 && key < 256)
		keyboard->ReleaseButton(key, GetCharacter(event));
}

- (void)flagsChanged:(NSEvent *)event
{
	CKeyboard *keyboard = CInput::Keyboard();
	if(!keyboard)
		return;

	int key = -1;
	NSEventModifierFlags flag = 0;
	switch([event keyCode])
	{
	case 56: case 60: key = EBtn_Key_Shift; flag = NSEventModifierFlagShift; break;
	case 59: case 62: key = EBtn_Key_Control; flag = NSEventModifierFlagControl; break;
	case 58: case 61: key = EBtn_Key_LMenu; flag = NSEventModifierFlagOption; break;
	case 54: case 55: key = EBtn_Key_LSpecial; flag = NSEventModifierFlagCommand; break;
	default: return;
	}

	if([event modifierFlags] & flag)
		keyboard->PressButton(key);
	else
		keyboard->ReleaseButton(key);
}

- (void)mouseDown:(NSEvent *)event
{
	(void)event;
	if(g_pMouse)
		g_pMouse->PressButton(0);
}

- (void)mouseUp:(NSEvent *)event
{
	(void)event;
	if(g_pMouse)
		g_pMouse->ReleaseButton(0);
}

- (void)rightMouseDown:(NSEvent *)event
{
	(void)event;
	if(g_pMouse)
		g_pMouse->PressButton(1);
}

- (void)rightMouseUp:(NSEvent *)event
{
	(void)event;
	if(g_pMouse)
		g_pMouse->ReleaseButton(1);
}

- (void)otherMouseDown:(NSEvent *)event
{
	(void)event;
	if(g_pMouse)
		g_pMouse->PressButton(2);
}

- (void)otherMouseUp:(NSEvent *)event
{
	(void)event;
	if(g_pMouse)
		g_pMouse->ReleaseButton(2);
}

- (void)mouseMoved:(NSEvent *)event
{
	if(g_pMouse)
		g_pMouse->MoveBy((float)[event deltaX], (float)[event deltaY]);
}

- (void)mouseDragged:(NSEvent *)event
{
	[self mouseMoved:event];
}

- (void)rightMouseDragged:(NSEvent *)event
{
	[self mouseMoved:event];
}

- (void)otherMouseDragged:(NSEvent *)event
{
	[self mouseMoved:event];
}

- (void)mouseEntered:(NSEvent *)event
{
	(void)event;
	SetCursorHidden(YES);
}

- (void)mouseExited:(NSEvent *)event
{
	(void)event;
	SetCursorHidden(NO);
}

- (void)windowDidResignKey:(NSNotification *)notification
{
	(void)notification;
	CKeyboard *keyboard = CInput::Keyboard();
	if(keyboard)
	{
		for(int key = 0; key < 256; ++key)
		{
			if(keyboard->IsButtonPressed(key))
				keyboard->ReleaseButton(key);
		}
	}
	if(g_pMouse)
	{
		for(int button = 0; button < 3; ++button)
			g_pMouse->ReleaseButton(button);
	}
	SetCursorHidden(NO);
}

- (BOOL)windowShouldClose:(id)sender
{
	(void)sender;
	NCore::AddMessage(NCore::CMessage(NCore::EMsg_Quit));
	return NO;
}

@end

void *GLExtInit_GetProcAddress(const char *name)
{
	return dlsym(RTLD_DEFAULT, name);
}

class COpenGLDevice_Darwin : public COpenGLDevice
{
	NSWindow *m_Window;
	NSOpenGLContext *m_RenderContext;

public:
	COpenGLDevice_Darwin()
	{
		m_Active = false;
		m_Window = nil;
		m_RenderContext = nil;
	}

	virtual int32 EnumDisplayModes(TDynamicArray<CMode> &modes, CMode requested)
	{
		NSScreen *screen = [[NSScreen screens] count] ? [[NSScreen screens] objectAtIndex:0] : nil;
		NSRect screenFrame = screen ? [screen frame] : NSMakeRect(0, 0, 1024, 768);
		const int32 sizes[][2] = {
			{640, 480}, {800, 600}, {1024, 768}, {1280, 720},
			{(int32)screenFrame.size.width, (int32)screenFrame.size.height}
		};

		for(int32 i = 0; i < 5; ++i)
		{
			for(int32 fullscreen = 0; fullscreen <= 1; ++fullscreen)
			{
				if(fullscreen && i != 4)
					continue;

				CMode mode;
				mode.Specify(CMode::EWidth, sizes[i][0]);
				mode.Specify(CMode::EHeight, sizes[i][1]);
				mode.Specify(CMode::ERefreshRate, 60);
				mode.Specify(CMode::EFullscreen, fullscreen);
				mode.Specify(CMode::EColorBits, 32);
				mode.Specify(CMode::EAlphaBits, 8);
				mode.Specify(CMode::EDepthBits, 24);
				mode.Specify(CMode::EStencilBits, 8);
				mode.Specify(CMode::EAdapter, CMode::EAdapter_Default);
				if(requested.Acceptable(mode))
					modes.Add(mode);
			}
		}
		return modes.Len();
	}

	virtual bool Init(CMode requested)
	{
		CMode mode;
		if(!GetBestMode(requested, mode))
		{
			if(!requested.Get(CMode::EFullscreen))
				return false;

			CMode windowed(requested.Get(CMode::EWidth), requested.Get(CMode::EHeight), false, 0);
			if(!GetBestMode(windowed, mode))
				return false;

			NLog::Write("OpenGL: requested fullscreen mode is unavailable; using windowed mode\n");
		}

		[NSApplication sharedApplication];
		[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
		[NSApp finishLaunching];

		NSScreen *screen = [[NSScreen screens] count] ? [[NSScreen screens] objectAtIndex:0] : nil;
		NSRect screenFrame = screen ? [screen frame] : NSMakeRect(0, 0, 1024, 768);
		BOOL fullscreen = mode.Get(CMode::EFullscreen) != 0;
		NSRect frame;
		if(fullscreen)
		{
			frame = screenFrame;
		}
		else
		{
			frame = NSMakeRect(
				NSMidX(screenFrame) - mode.Get(CMode::EWidth) / 2,
				NSMidY(screenFrame) - mode.Get(CMode::EHeight) / 2,
				mode.Get(CMode::EWidth),
				mode.Get(CMode::EHeight));
		}

		NSOpenGLPixelFormatAttribute attributes[] = {
			NSOpenGLPFAAccelerated,
			NSOpenGLPFADoubleBuffer,
			NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute)mode.Get(CMode::EColorBits),
			NSOpenGLPFAAlphaSize, (NSOpenGLPixelFormatAttribute)mode.Get(CMode::EAlphaBits),
			NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)mode.Get(CMode::EDepthBits),
			NSOpenGLPFAStencilSize, (NSOpenGLPixelFormatAttribute)mode.Get(CMode::EStencilBits),
			(NSOpenGLPixelFormatAttribute)0
		};
		NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
		if(!pixelFormat)
			return false;

		TeepopOpenGLView *view = [[TeepopOpenGLView alloc] initWithFrame:NSMakeRect(0, 0, frame.size.width, frame.size.height)
			pixelFormat:pixelFormat];
		[pixelFormat release];
		if(!view)
			return false;

		NSWindowStyleMask style = fullscreen ? NSWindowStyleMaskBorderless :
			(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable);
		m_Window = [[NSWindow alloc] initWithContentRect:frame
			styleMask:style
			backing:NSBackingStoreBuffered
			defer:NO
			screen:screen];
		if(!m_Window)
		{
			[view release];
			return false;
		}

		[m_Window setTitle:@"Teepop"];
		[m_Window setReleasedWhenClosed:NO];
		[m_Window setDelegate:view];
		[m_Window setContentView:view];
		[m_Window makeFirstResponder:view];
		[m_Window makeKeyAndOrderFront:nil];
		[NSApp activateIgnoringOtherApps:YES];

		m_RenderContext = [view openGLContext];
		[m_RenderContext makeCurrentContext];
		GLint swapInterval = 1;
		[m_RenderContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
		m_Active = true;
		return COpenGLDevice::Init(mode);
	}

	virtual bool Update()
	{
		if(!m_Active)
			return true;

		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		NSEvent *event;
		while((event = [NSApp nextEventMatchingMask:NSEventMaskAny
			untilDate:[NSDate distantPast]
			inMode:NSDefaultRunLoopMode
			dequeue:YES]) != nil)
		{
			[NSApp sendEvent:event];
		}
		[m_RenderContext update];
		[m_RenderContext makeCurrentContext];
		[pool drain];
		return true;
	}

	virtual bool SwapBuffers()
	{
		[m_RenderContext flushBuffer];
		return true;
	}

	virtual bool Shutdown()
	{
		m_Active = false;
		if(m_RenderContext)
			[m_RenderContext clearDrawable];
		if(m_Window)
		{
			[m_Window setDelegate:nil];
			[m_Window close];
			[m_Window release];
			m_Window = nil;
		}
		m_RenderContext = nil;
		SetCursorHidden(NO);
		return true;
	}

	virtual ~COpenGLDevice_Darwin()
	{
		Shutdown();
	}
};

MACRO_REGISTER_CLASS(COpenGLDevice_Darwin, N3D::aDeviceClassTag)
