#include "opengl_include.h"

#include "ext_arb_vertex_buffer_object.h"

extern void *GLExtInit_GetProcAddress(const char *_pName);

PFNBINDBUFFERARBPROC glextBindBufferARB = 0;
PFNDELETEBUFFERSARBPROC glextDeleteBuffersARB = 0;
PFNGENBUFFERSARBPROC glextGenBuffersARB = 0;
PFNISBUFFERARBPROC glextIsBufferARB = 0;
PFNBUFFERDATAARBPROC glextBufferDataARB = 0;
PFNBUFFERSUBDATAARBPROC glextBufferSubDataARB = 0;
PFNGETBUFFERSUBDATAARBPROC glextGetBufferSubDataARB = 0;
PFNMAPBUFFERARBPROC glextMapBufferARB = 0;
PFNUNMAPBUFFERARBPROC glextUnmapBufferARB = 0;
PFNGETBUFFERPARAMETERIVARBPROC glextGetBufferParameterivARB = 0;
PFNGETBUFFERPOINTERVARBPROC glextGetBufferPointervARB = 0;


bool GLExtInit_arb_vertex_buffer_object()
{
	glextBindBufferARB = (PFNBINDBUFFERARBPROC)GLExtInit_GetProcAddress("glBindBufferARB");
	glextDeleteBuffersARB = (PFNDELETEBUFFERSARBPROC)GLExtInit_GetProcAddress("glDeleteBuffersARB");
	glextGenBuffersARB = (PFNGENBUFFERSARBPROC)GLExtInit_GetProcAddress("glGenBuffersARB");
	glextIsBufferARB = (PFNISBUFFERARBPROC)GLExtInit_GetProcAddress("glIsBufferARB");
	glextBufferDataARB = (PFNBUFFERDATAARBPROC)GLExtInit_GetProcAddress("glBufferDataARB");
	glextBufferSubDataARB = (PFNBUFFERSUBDATAARBPROC)GLExtInit_GetProcAddress("glBufferSubDataARB");
	glextGetBufferSubDataARB = (PFNGETBUFFERSUBDATAARBPROC)GLExtInit_GetProcAddress("glGetBufferSubDataARB");
	glextMapBufferARB = (PFNMAPBUFFERARBPROC)GLExtInit_GetProcAddress("glMapBufferARB");
	glextUnmapBufferARB = (PFNUNMAPBUFFERARBPROC)GLExtInit_GetProcAddress("glUnmapBufferARB");
	glextGetBufferParameterivARB = (PFNGETBUFFERPARAMETERIVARBPROC)GLExtInit_GetProcAddress("glGetBufferParameterivARB");
	glextGetBufferPointervARB = (PFNGETBUFFERPOINTERVARBPROC)GLExtInit_GetProcAddress("glGetBufferPointervARB");


	if(
		glextBindBufferARB &&
		glextDeleteBuffersARB &&
		glextGenBuffersARB &&
		glextIsBufferARB &&
		glextBufferDataARB &&
		glextBufferSubDataARB &&
		glextGetBufferSubDataARB &&
		glextMapBufferARB &&
		glextUnmapBufferARB &&
		glextGetBufferParameterivARB &&
		glextGetBufferPointervARB &&
		1
	)
	{
		return true;
	}

	return false;
}
