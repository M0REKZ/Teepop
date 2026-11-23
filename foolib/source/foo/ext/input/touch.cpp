//#include "opengl.h"
#include <foo/func/classregistry.h>

#ifdef FOO_PLATFORM_WIN32
	//MACRO_TOUCH_CLASS(CMouse_Win32);
	//MACRO_TOUCH_CLASS(CMouse_DirectInput8);
	MACRO_TOUCH_CLASS(CKeyboard_Win32)
#endif

#ifdef FOO_PLATFORM_DARWIN
	MACRO_TOUCH_CLASS(CKeyboard_Darwin)
#endif
