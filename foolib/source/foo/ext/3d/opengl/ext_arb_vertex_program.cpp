#include "opengl_include.h"

#include "ext_arb_vertex_program.h"

extern void *GLExtInit_GetProcAddress(const char *_pName);

PFNVERTEXATTRIB1SARBPROC glextVertexAttrib1sARB = 0;
PFNVERTEXATTRIB1FARBPROC glextVertexAttrib1fARB = 0;
PFNVERTEXATTRIB1DARBPROC glextVertexAttrib1dARB = 0;
PFNVERTEXATTRIB2SARBPROC glextVertexAttrib2sARB = 0;
PFNVERTEXATTRIB2FARBPROC glextVertexAttrib2fARB = 0;
PFNVERTEXATTRIB2DARBPROC glextVertexAttrib2dARB = 0;
PFNVERTEXATTRIB3SARBPROC glextVertexAttrib3sARB = 0;
PFNVERTEXATTRIB3FARBPROC glextVertexAttrib3fARB = 0;
PFNVERTEXATTRIB3DARBPROC glextVertexAttrib3dARB = 0;
PFNVERTEXATTRIB4SARBPROC glextVertexAttrib4sARB = 0;
PFNVERTEXATTRIB4FARBPROC glextVertexAttrib4fARB = 0;
PFNVERTEXATTRIB4DARBPROC glextVertexAttrib4dARB = 0;
PFNVERTEXATTRIB4NUBARBPROC glextVertexAttrib4NubARB = 0;
PFNVERTEXATTRIB1SVARBPROC glextVertexAttrib1svARB = 0;
PFNVERTEXATTRIB1FVARBPROC glextVertexAttrib1fvARB = 0;
PFNVERTEXATTRIB1DVARBPROC glextVertexAttrib1dvARB = 0;
PFNVERTEXATTRIB2SVARBPROC glextVertexAttrib2svARB = 0;
PFNVERTEXATTRIB2FVARBPROC glextVertexAttrib2fvARB = 0;
PFNVERTEXATTRIB2DVARBPROC glextVertexAttrib2dvARB = 0;
PFNVERTEXATTRIB3SVARBPROC glextVertexAttrib3svARB = 0;
PFNVERTEXATTRIB3FVARBPROC glextVertexAttrib3fvARB = 0;
PFNVERTEXATTRIB3DVARBPROC glextVertexAttrib3dvARB = 0;
PFNVERTEXATTRIB4BVARBPROC glextVertexAttrib4bvARB = 0;
PFNVERTEXATTRIB4SVARBPROC glextVertexAttrib4svARB = 0;
PFNVERTEXATTRIB4IVARBPROC glextVertexAttrib4ivARB = 0;
PFNVERTEXATTRIB4UBVARBPROC glextVertexAttrib4ubvARB = 0;
PFNVERTEXATTRIB4USVARBPROC glextVertexAttrib4usvARB = 0;
PFNVERTEXATTRIB4UIVARBPROC glextVertexAttrib4uivARB = 0;
PFNVERTEXATTRIB4FVARBPROC glextVertexAttrib4fvARB = 0;
PFNVERTEXATTRIB4DVARBPROC glextVertexAttrib4dvARB = 0;
PFNVERTEXATTRIB4NBVARBPROC glextVertexAttrib4NbvARB = 0;
PFNVERTEXATTRIB4NSVARBPROC glextVertexAttrib4NsvARB = 0;
PFNVERTEXATTRIB4NIVARBPROC glextVertexAttrib4NivARB = 0;
PFNVERTEXATTRIB4NUBVARBPROC glextVertexAttrib4NubvARB = 0;
PFNVERTEXATTRIB4NUSVARBPROC glextVertexAttrib4NusvARB = 0;
PFNVERTEXATTRIB4NUIVARBPROC glextVertexAttrib4NuivARB = 0;
PFNVERTEXATTRIBPOINTERARBPROC glextVertexAttribPointerARB = 0;
PFNENABLEVERTEXATTRIBARRAYARBPROC glextEnableVertexAttribArrayARB = 0;
PFNDISABLEVERTEXATTRIBARRAYARBPROC glextDisableVertexAttribArrayARB = 0;
PFNPROGRAMSTRINGARBPROC glextProgramStringARB = 0;
PFNBINDPROGRAMARBPROC glextBindProgramARB = 0;
PFNDELETEPROGRAMSARBPROC glextDeleteProgramsARB = 0;
PFNGENPROGRAMSARBPROC glextGenProgramsARB = 0;
PFNPROGRAMENVPARAMETER4DARBPROC glextProgramEnvParameter4dARB = 0;
PFNPROGRAMENVPARAMETER4DVARBPROC glextProgramEnvParameter4dvARB = 0;
PFNPROGRAMENVPARAMETER4FARBPROC glextProgramEnvParameter4fARB = 0;
PFNPROGRAMENVPARAMETER4FVARBPROC glextProgramEnvParameter4fvARB = 0;
PFNPROGRAMLOCALPARAMETER4DARBPROC glextProgramLocalParameter4dARB = 0;
PFNPROGRAMLOCALPARAMETER4DVARBPROC glextProgramLocalParameter4dvARB = 0;
PFNPROGRAMLOCALPARAMETER4FARBPROC glextProgramLocalParameter4fARB = 0;
PFNPROGRAMLOCALPARAMETER4FVARBPROC glextProgramLocalParameter4fvARB = 0;
PFNGETPROGRAMENVPARAMETERDVARBPROC glextGetProgramEnvParameterdvARB = 0;
PFNGETPROGRAMENVPARAMETERFVARBPROC glextGetProgramEnvParameterfvARB = 0;
PFNGETPROGRAMLOCALPARAMETERDVARBPROC glextGetProgramLocalParameterdvARB = 0;
PFNGETPROGRAMLOCALPARAMETERFVARBPROC glextGetProgramLocalParameterfvARB = 0;
PFNGETPROGRAMIVARBPROC glextGetProgramivARB = 0;
PFNGETPROGRAMSTRINGARBPROC glextGetProgramStringARB = 0;
PFNGETVERTEXATTRIBDVARBPROC glextGetVertexAttribdvARB = 0;
PFNGETVERTEXATTRIBFVARBPROC glextGetVertexAttribfvARB = 0;
PFNGETVERTEXATTRIBIVARBPROC glextGetVertexAttribivARB = 0;
PFNGETVERTEXATTRIBPOINTERVARBPROC glextGetVertexAttribPointervARB = 0;
PFNISPROGRAMARBPROC glextIsProgramARB = 0;


bool GLExtInit_arb_vertex_program()
{
	glextVertexAttrib1sARB = (PFNVERTEXATTRIB1SARBPROC)GLExtInit_GetProcAddress("glVertexAttrib1sARB");
	glextVertexAttrib1fARB = (PFNVERTEXATTRIB1FARBPROC)GLExtInit_GetProcAddress("glVertexAttrib1fARB");
	glextVertexAttrib1dARB = (PFNVERTEXATTRIB1DARBPROC)GLExtInit_GetProcAddress("glVertexAttrib1dARB");
	glextVertexAttrib2sARB = (PFNVERTEXATTRIB2SARBPROC)GLExtInit_GetProcAddress("glVertexAttrib2sARB");
	glextVertexAttrib2fARB = (PFNVERTEXATTRIB2FARBPROC)GLExtInit_GetProcAddress("glVertexAttrib2fARB");
	glextVertexAttrib2dARB = (PFNVERTEXATTRIB2DARBPROC)GLExtInit_GetProcAddress("glVertexAttrib2dARB");
	glextVertexAttrib3sARB = (PFNVERTEXATTRIB3SARBPROC)GLExtInit_GetProcAddress("glVertexAttrib3sARB");
	glextVertexAttrib3fARB = (PFNVERTEXATTRIB3FARBPROC)GLExtInit_GetProcAddress("glVertexAttrib3fARB");
	glextVertexAttrib3dARB = (PFNVERTEXATTRIB3DARBPROC)GLExtInit_GetProcAddress("glVertexAttrib3dARB");
	glextVertexAttrib4sARB = (PFNVERTEXATTRIB4SARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4sARB");
	glextVertexAttrib4fARB = (PFNVERTEXATTRIB4FARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4fARB");
	glextVertexAttrib4dARB = (PFNVERTEXATTRIB4DARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4dARB");
	glextVertexAttrib4NubARB = (PFNVERTEXATTRIB4NUBARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4NubARB");
	glextVertexAttrib1svARB = (PFNVERTEXATTRIB1SVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib1svARB");
	glextVertexAttrib1fvARB = (PFNVERTEXATTRIB1FVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib1fvARB");
	glextVertexAttrib1dvARB = (PFNVERTEXATTRIB1DVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib1dvARB");
	glextVertexAttrib2svARB = (PFNVERTEXATTRIB2SVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib2svARB");
	glextVertexAttrib2fvARB = (PFNVERTEXATTRIB2FVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib2fvARB");
	glextVertexAttrib2dvARB = (PFNVERTEXATTRIB2DVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib2dvARB");
	glextVertexAttrib3svARB = (PFNVERTEXATTRIB3SVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib3svARB");
	glextVertexAttrib3fvARB = (PFNVERTEXATTRIB3FVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib3fvARB");
	glextVertexAttrib3dvARB = (PFNVERTEXATTRIB3DVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib3dvARB");
	glextVertexAttrib4bvARB = (PFNVERTEXATTRIB4BVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4bvARB");
	glextVertexAttrib4svARB = (PFNVERTEXATTRIB4SVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4svARB");
	glextVertexAttrib4ivARB = (PFNVERTEXATTRIB4IVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4ivARB");
	glextVertexAttrib4ubvARB = (PFNVERTEXATTRIB4UBVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4ubvARB");
	glextVertexAttrib4usvARB = (PFNVERTEXATTRIB4USVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4usvARB");
	glextVertexAttrib4uivARB = (PFNVERTEXATTRIB4UIVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4uivARB");
	glextVertexAttrib4fvARB = (PFNVERTEXATTRIB4FVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4fvARB");
	glextVertexAttrib4dvARB = (PFNVERTEXATTRIB4DVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4dvARB");
	glextVertexAttrib4NbvARB = (PFNVERTEXATTRIB4NBVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4NbvARB");
	glextVertexAttrib4NsvARB = (PFNVERTEXATTRIB4NSVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4NsvARB");
	glextVertexAttrib4NivARB = (PFNVERTEXATTRIB4NIVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4NivARB");
	glextVertexAttrib4NubvARB = (PFNVERTEXATTRIB4NUBVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4NubvARB");
	glextVertexAttrib4NusvARB = (PFNVERTEXATTRIB4NUSVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4NusvARB");
	glextVertexAttrib4NuivARB = (PFNVERTEXATTRIB4NUIVARBPROC)GLExtInit_GetProcAddress("glVertexAttrib4NuivARB");
	glextVertexAttribPointerARB = (PFNVERTEXATTRIBPOINTERARBPROC)GLExtInit_GetProcAddress("glVertexAttribPointerARB");
	glextEnableVertexAttribArrayARB = (PFNENABLEVERTEXATTRIBARRAYARBPROC)GLExtInit_GetProcAddress("glEnableVertexAttribArrayARB");
	glextDisableVertexAttribArrayARB = (PFNDISABLEVERTEXATTRIBARRAYARBPROC)GLExtInit_GetProcAddress("glDisableVertexAttribArrayARB");
	glextProgramStringARB = (PFNPROGRAMSTRINGARBPROC)GLExtInit_GetProcAddress("glProgramStringARB");
	glextBindProgramARB = (PFNBINDPROGRAMARBPROC)GLExtInit_GetProcAddress("glBindProgramARB");
	glextDeleteProgramsARB = (PFNDELETEPROGRAMSARBPROC)GLExtInit_GetProcAddress("glDeleteProgramsARB");
	glextGenProgramsARB = (PFNGENPROGRAMSARBPROC)GLExtInit_GetProcAddress("glGenProgramsARB");
	glextProgramEnvParameter4dARB = (PFNPROGRAMENVPARAMETER4DARBPROC)GLExtInit_GetProcAddress("glProgramEnvParameter4dARB");
	glextProgramEnvParameter4dvARB = (PFNPROGRAMENVPARAMETER4DVARBPROC)GLExtInit_GetProcAddress("glProgramEnvParameter4dvARB");
	glextProgramEnvParameter4fARB = (PFNPROGRAMENVPARAMETER4FARBPROC)GLExtInit_GetProcAddress("glProgramEnvParameter4fARB");
	glextProgramEnvParameter4fvARB = (PFNPROGRAMENVPARAMETER4FVARBPROC)GLExtInit_GetProcAddress("glProgramEnvParameter4fvARB");
	glextProgramLocalParameter4dARB = (PFNPROGRAMLOCALPARAMETER4DARBPROC)GLExtInit_GetProcAddress("glProgramLocalParameter4dARB");
	glextProgramLocalParameter4dvARB = (PFNPROGRAMLOCALPARAMETER4DVARBPROC)GLExtInit_GetProcAddress("glProgramLocalParameter4dvARB");
	glextProgramLocalParameter4fARB = (PFNPROGRAMLOCALPARAMETER4FARBPROC)GLExtInit_GetProcAddress("glProgramLocalParameter4fARB");
	glextProgramLocalParameter4fvARB = (PFNPROGRAMLOCALPARAMETER4FVARBPROC)GLExtInit_GetProcAddress("glProgramLocalParameter4fvARB");
	glextGetProgramEnvParameterdvARB = (PFNGETPROGRAMENVPARAMETERDVARBPROC)GLExtInit_GetProcAddress("glGetProgramEnvParameterdvARB");
	glextGetProgramEnvParameterfvARB = (PFNGETPROGRAMENVPARAMETERFVARBPROC)GLExtInit_GetProcAddress("glGetProgramEnvParameterfvARB");
	glextGetProgramLocalParameterdvARB = (PFNGETPROGRAMLOCALPARAMETERDVARBPROC)GLExtInit_GetProcAddress("glGetProgramLocalParameterdvARB");
	glextGetProgramLocalParameterfvARB = (PFNGETPROGRAMLOCALPARAMETERFVARBPROC)GLExtInit_GetProcAddress("glGetProgramLocalParameterfvARB");
	glextGetProgramivARB = (PFNGETPROGRAMIVARBPROC)GLExtInit_GetProcAddress("glGetProgramivARB");
	glextGetProgramStringARB = (PFNGETPROGRAMSTRINGARBPROC)GLExtInit_GetProcAddress("glGetProgramStringARB");
	glextGetVertexAttribdvARB = (PFNGETVERTEXATTRIBDVARBPROC)GLExtInit_GetProcAddress("glGetVertexAttribdvARB");
	glextGetVertexAttribfvARB = (PFNGETVERTEXATTRIBFVARBPROC)GLExtInit_GetProcAddress("glGetVertexAttribfvARB");
	glextGetVertexAttribivARB = (PFNGETVERTEXATTRIBIVARBPROC)GLExtInit_GetProcAddress("glGetVertexAttribivARB");
	glextGetVertexAttribPointervARB = (PFNGETVERTEXATTRIBPOINTERVARBPROC)GLExtInit_GetProcAddress("glGetVertexAttribPointervARB");
	glextIsProgramARB = (PFNISPROGRAMARBPROC)GLExtInit_GetProcAddress("glIsProgramARB");


	if(
		glextVertexAttrib1sARB &&
		glextVertexAttrib1fARB &&
		glextVertexAttrib1dARB &&
		glextVertexAttrib2sARB &&
		glextVertexAttrib2fARB &&
		glextVertexAttrib2dARB &&
		glextVertexAttrib3sARB &&
		glextVertexAttrib3fARB &&
		glextVertexAttrib3dARB &&
		glextVertexAttrib4sARB &&
		glextVertexAttrib4fARB &&
		glextVertexAttrib4dARB &&
		glextVertexAttrib4NubARB &&
		glextVertexAttrib1svARB &&
		glextVertexAttrib1fvARB &&
		glextVertexAttrib1dvARB &&
		glextVertexAttrib2svARB &&
		glextVertexAttrib2fvARB &&
		glextVertexAttrib2dvARB &&
		glextVertexAttrib3svARB &&
		glextVertexAttrib3fvARB &&
		glextVertexAttrib3dvARB &&
		glextVertexAttrib4bvARB &&
		glextVertexAttrib4svARB &&
		glextVertexAttrib4ivARB &&
		glextVertexAttrib4ubvARB &&
		glextVertexAttrib4usvARB &&
		glextVertexAttrib4uivARB &&
		glextVertexAttrib4fvARB &&
		glextVertexAttrib4dvARB &&
		glextVertexAttrib4NbvARB &&
		glextVertexAttrib4NsvARB &&
		glextVertexAttrib4NivARB &&
		glextVertexAttrib4NubvARB &&
		glextVertexAttrib4NusvARB &&
		glextVertexAttrib4NuivARB &&
		glextVertexAttribPointerARB &&
		glextEnableVertexAttribArrayARB &&
		glextDisableVertexAttribArrayARB &&
		glextProgramStringARB &&
		glextBindProgramARB &&
		glextDeleteProgramsARB &&
		glextGenProgramsARB &&
		glextProgramEnvParameter4dARB &&
		glextProgramEnvParameter4dvARB &&
		glextProgramEnvParameter4fARB &&
		glextProgramEnvParameter4fvARB &&
		glextProgramLocalParameter4dARB &&
		glextProgramLocalParameter4dvARB &&
		glextProgramLocalParameter4fARB &&
		glextProgramLocalParameter4fvARB &&
		glextGetProgramEnvParameterdvARB &&
		glextGetProgramEnvParameterfvARB &&
		glextGetProgramLocalParameterdvARB &&
		glextGetProgramLocalParameterfvARB &&
		glextGetProgramivARB &&
		glextGetProgramStringARB &&
		glextGetVertexAttribdvARB &&
		glextGetVertexAttribfvARB &&
		glextGetVertexAttribivARB &&
		glextGetVertexAttribPointervARB &&
		glextIsProgramARB &&
		1
	)
	{
		return true;
	}

	return false;
}

