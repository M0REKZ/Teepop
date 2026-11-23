#include <foo/func/classregistry.h>

#ifdef FOO_PLATFORM_WIN32
	//MACRO_TOUCH_CLASS(CSoundDevice_WaveOut)
	MACRO_TOUCH_CLASS(CSoundDevice_DirectSound)
#endif

#ifdef FOO_PLATFORM_DARWIN
	MACRO_TOUCH_CLASS(CSoundDevice_OpenAL)
#endif
