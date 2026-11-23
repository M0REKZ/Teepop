#include <foo/func/classregistry.h>

#ifdef FOO_PLATFORM_DARWIN
	//MACRO_TOUCH_CLASS(CMouse_OpenGLDarwin);
	MACRO_TOUCH_CLASS(COpenGLDevice_Darwin)
#endif

#ifdef FOO_PLATFORM_WIN32
	MACRO_TOUCH_CLASS(COpenGLDevice_Win32)
	//MACRO_TOUCH_CLASS(CKeyboard_OpenGLWin32);
#endif

#if defined(FOO_PLATFORM_LINUX) || defined(FOO_PLATFORM_FREEBSD)
	MACRO_TOUCH_CLASS(COpenGLDevice_X11)
#endif

