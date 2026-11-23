#include "opengl_include.h"

#include "ext_arb_multitexture.h"

extern void *GLExtInit_GetProcAddress(const char *_pName);

PFNACTIVETEXTUREARBPROC glextActiveTextureARB = 0;
PFNCLIENTACTIVETEXTUREARBPROC glextClientActiveTextureARB = 0;
PFNMULTITEXCOORD1DARBPROC glextMultiTexCoord1dARB = 0;
PFNMULTITEXCOORD1DVARBPROC glextMultiTexCoord1dvARB = 0;
PFNMULTITEXCOORD1FARBPROC glextMultiTexCoord1fARB = 0;
PFNMULTITEXCOORD1FVARBPROC glextMultiTexCoord1fvARB = 0;
PFNMULTITEXCOORD1IARBPROC glextMultiTexCoord1iARB = 0;
PFNMULTITEXCOORD1IVARBPROC glextMultiTexCoord1ivARB = 0;
PFNMULTITEXCOORD1SARBPROC glextMultiTexCoord1sARB = 0;
PFNMULTITEXCOORD1SVARBPROC glextMultiTexCoord1svARB = 0;
PFNMULTITEXCOORD2DARBPROC glextMultiTexCoord2dARB = 0;
PFNMULTITEXCOORD2DVARBPROC glextMultiTexCoord2dvARB = 0;
PFNMULTITEXCOORD2FARBPROC glextMultiTexCoord2fARB = 0;
PFNMULTITEXCOORD2FVARBPROC glextMultiTexCoord2fvARB = 0;
PFNMULTITEXCOORD2IARBPROC glextMultiTexCoord2iARB = 0;
PFNMULTITEXCOORD2IVARBPROC glextMultiTexCoord2ivARB = 0;
PFNMULTITEXCOORD2SARBPROC glextMultiTexCoord2sARB = 0;
PFNMULTITEXCOORD2SVARBPROC glextMultiTexCoord2svARB = 0;
PFNMULTITEXCOORD3DARBPROC glextMultiTexCoord3dARB = 0;
PFNMULTITEXCOORD3DVARBPROC glextMultiTexCoord3dvARB = 0;
PFNMULTITEXCOORD3FARBPROC glextMultiTexCoord3fARB = 0;
PFNMULTITEXCOORD3FVARBPROC glextMultiTexCoord3fvARB = 0;
PFNMULTITEXCOORD3IARBPROC glextMultiTexCoord3iARB = 0;
PFNMULTITEXCOORD3IVARBPROC glextMultiTexCoord3ivARB = 0;
PFNMULTITEXCOORD3SARBPROC glextMultiTexCoord3sARB = 0;
PFNMULTITEXCOORD3SVARBPROC glextMultiTexCoord3svARB = 0;
PFNMULTITEXCOORD4DARBPROC glextMultiTexCoord4dARB = 0;
PFNMULTITEXCOORD4DVARBPROC glextMultiTexCoord4dvARB = 0;
PFNMULTITEXCOORD4FARBPROC glextMultiTexCoord4fARB = 0;
PFNMULTITEXCOORD4FVARBPROC glextMultiTexCoord4fvARB = 0;
PFNMULTITEXCOORD4IARBPROC glextMultiTexCoord4iARB = 0;
PFNMULTITEXCOORD4IVARBPROC glextMultiTexCoord4ivARB = 0;
PFNMULTITEXCOORD4SARBPROC glextMultiTexCoord4sARB = 0;
PFNMULTITEXCOORD4SVARBPROC glextMultiTexCoord4svARB = 0;


bool GLExtInit_arb_multitexture()
{
	glextActiveTextureARB = (PFNACTIVETEXTUREARBPROC)GLExtInit_GetProcAddress("glActiveTextureARB");
	glextClientActiveTextureARB = (PFNCLIENTACTIVETEXTUREARBPROC)GLExtInit_GetProcAddress("glClientActiveTextureARB");
	glextMultiTexCoord1dARB = (PFNMULTITEXCOORD1DARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord1dARB");
	glextMultiTexCoord1dvARB = (PFNMULTITEXCOORD1DVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord1dvARB");
	glextMultiTexCoord1fARB = (PFNMULTITEXCOORD1FARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord1fARB");
	glextMultiTexCoord1fvARB = (PFNMULTITEXCOORD1FVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord1fvARB");
	glextMultiTexCoord1iARB = (PFNMULTITEXCOORD1IARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord1iARB");
	glextMultiTexCoord1ivARB = (PFNMULTITEXCOORD1IVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord1ivARB");
	glextMultiTexCoord1sARB = (PFNMULTITEXCOORD1SARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord1sARB");
	glextMultiTexCoord1svARB = (PFNMULTITEXCOORD1SVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord1svARB");
	glextMultiTexCoord2dARB = (PFNMULTITEXCOORD2DARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord2dARB");
	glextMultiTexCoord2dvARB = (PFNMULTITEXCOORD2DVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord2dvARB");
	glextMultiTexCoord2fARB = (PFNMULTITEXCOORD2FARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord2fARB");
	glextMultiTexCoord2fvARB = (PFNMULTITEXCOORD2FVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord2fvARB");
	glextMultiTexCoord2iARB = (PFNMULTITEXCOORD2IARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord2iARB");
	glextMultiTexCoord2ivARB = (PFNMULTITEXCOORD2IVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord2ivARB");
	glextMultiTexCoord2sARB = (PFNMULTITEXCOORD2SARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord2sARB");
	glextMultiTexCoord2svARB = (PFNMULTITEXCOORD2SVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord2svARB");
	glextMultiTexCoord3dARB = (PFNMULTITEXCOORD3DARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord3dARB");
	glextMultiTexCoord3dvARB = (PFNMULTITEXCOORD3DVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord3dvARB");
	glextMultiTexCoord3fARB = (PFNMULTITEXCOORD3FARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord3fARB");
	glextMultiTexCoord3fvARB = (PFNMULTITEXCOORD3FVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord3fvARB");
	glextMultiTexCoord3iARB = (PFNMULTITEXCOORD3IARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord3iARB");
	glextMultiTexCoord3ivARB = (PFNMULTITEXCOORD3IVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord3ivARB");
	glextMultiTexCoord3sARB = (PFNMULTITEXCOORD3SARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord3sARB");
	glextMultiTexCoord3svARB = (PFNMULTITEXCOORD3SVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord3svARB");
	glextMultiTexCoord4dARB = (PFNMULTITEXCOORD4DARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord4dARB");
	glextMultiTexCoord4dvARB = (PFNMULTITEXCOORD4DVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord4dvARB");
	glextMultiTexCoord4fARB = (PFNMULTITEXCOORD4FARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord4fARB");
	glextMultiTexCoord4fvARB = (PFNMULTITEXCOORD4FVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord4fvARB");
	glextMultiTexCoord4iARB = (PFNMULTITEXCOORD4IARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord4iARB");
	glextMultiTexCoord4ivARB = (PFNMULTITEXCOORD4IVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord4ivARB");
	glextMultiTexCoord4sARB = (PFNMULTITEXCOORD4SARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord4sARB");
	glextMultiTexCoord4svARB = (PFNMULTITEXCOORD4SVARBPROC)GLExtInit_GetProcAddress("glMultiTexCoord4svARB");


	if(
		glextActiveTextureARB &&
		glextClientActiveTextureARB &&
		glextMultiTexCoord1dARB &&
		glextMultiTexCoord1dvARB &&
		glextMultiTexCoord1fARB &&
		glextMultiTexCoord1fvARB &&
		glextMultiTexCoord1iARB &&
		glextMultiTexCoord1ivARB &&
		glextMultiTexCoord1sARB &&
		glextMultiTexCoord1svARB &&
		glextMultiTexCoord2dARB &&
		glextMultiTexCoord2dvARB &&
		glextMultiTexCoord2fARB &&
		glextMultiTexCoord2fvARB &&
		glextMultiTexCoord2iARB &&
		glextMultiTexCoord2ivARB &&
		glextMultiTexCoord2sARB &&
		glextMultiTexCoord2svARB &&
		glextMultiTexCoord3dARB &&
		glextMultiTexCoord3dvARB &&
		glextMultiTexCoord3fARB &&
		glextMultiTexCoord3fvARB &&
		glextMultiTexCoord3iARB &&
		glextMultiTexCoord3ivARB &&
		glextMultiTexCoord3sARB &&
		glextMultiTexCoord3svARB &&
		glextMultiTexCoord4dARB &&
		glextMultiTexCoord4dvARB &&
		glextMultiTexCoord4fARB &&
		glextMultiTexCoord4fvARB &&
		glextMultiTexCoord4iARB &&
		glextMultiTexCoord4ivARB &&
		glextMultiTexCoord4sARB &&
		glextMultiTexCoord4svARB &&
		1
	)
	{
		return true;
	}

	return false;
}

