#if defined(FOO_PLATFORM_WIN32)
    #ifndef _WINDOWS_
            #define WINGDIAPI
            #define APIENTRY __stdcall
            #define CALLBACK __stdcall
            
            #define _WCHAR_T_DEFINED
            #define wchar_t short
    #endif

	#define GLFUNC __stdcall

	// glheaders (needed? I think not)
    #include <GL/gl.h>
    #include <GL/glu.h>

#elif defined(FOO_PLATFORM_DARWIN)
	#include </System/Library/Frameworks/AGL.framework/Headers/gl.h>
	#include </System/Library/Frameworks/AGL.framework/Headers/glu.h>
	#define GLFUNC

#elif defined(FOO_PLATFORM_LINUX) || defined(FOO_PLATFORM_FREEBSD)
	// glheaders
    #include <GL/gl.h>
    #include <GL/glu.h>
	#define GLFUNC
#else
	#error no opengl implementation
#endif

