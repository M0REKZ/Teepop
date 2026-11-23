#include "opengl_include.h"

#include "ext_ext_swap_control.h"

extern void *GLExtInit_GetProcAddress(const char *_pName);

PFNSWAPINTERVALEXTPROC glextSwapIntervalEXT = 0;
PFNGETSWAPINTERVALEXTPROC glextGetSwapIntervalEXT = 0;


bool GLExtInit_ext_swap_control()
{
	glextSwapIntervalEXT = (PFNSWAPINTERVALEXTPROC)GLExtInit_GetProcAddress("wglSwapIntervalEXT");
	glextGetSwapIntervalEXT = (PFNGETSWAPINTERVALEXTPROC)GLExtInit_GetProcAddress("wglGetSwapIntervalEXT");


	if(
		glextSwapIntervalEXT &&
		glextGetSwapIntervalEXT &&
		1
	)
	{
		return true;
	}

	return false;
}
