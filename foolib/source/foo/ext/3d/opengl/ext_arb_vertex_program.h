#define GL_VERTEX_PROGRAM_ARB                              0x8620
#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB                   0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE_ARB                     0x8643
#define GL_COLOR_SUM_ARB                                   0x8458
#define GL_PROGRAM_FORMAT_ASCII_ARB                        0x8875
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB                 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB                    0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB                  0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB                    0x8625
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB              0x886A
#define GL_CURRENT_VERTEX_ATTRIB_ARB                       0x8626
#define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB                 0x8645
#define GL_PROGRAM_LENGTH_ARB                              0x8627
#define GL_PROGRAM_FORMAT_ARB                              0x8876
#define GL_PROGRAM_BINDING_ARB                             0x8677
#define GL_PROGRAM_INSTRUCTIONS_ARB                        0x88A0
#define GL_MAX_PROGRAM_INSTRUCTIONS_ARB                    0x88A1
#define GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB                 0x88A2
#define GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB             0x88A3
#define GL_PROGRAM_TEMPORARIES_ARB                         0x88A4
#define GL_MAX_PROGRAM_TEMPORARIES_ARB                     0x88A5
#define GL_PROGRAM_NATIVE_TEMPORARIES_ARB                  0x88A6
#define GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB              0x88A7
#define GL_PROGRAM_PARAMETERS_ARB                          0x88A8
#define GL_MAX_PROGRAM_PARAMETERS_ARB                      0x88A9
#define GL_PROGRAM_NATIVE_PARAMETERS_ARB                   0x88AA
#define GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB               0x88AB
#define GL_PROGRAM_ATTRIBS_ARB                             0x88AC
#define GL_MAX_PROGRAM_ATTRIBS_ARB                         0x88AD
#define GL_PROGRAM_NATIVE_ATTRIBS_ARB                      0x88AE
#define GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB                  0x88AF
#define GL_PROGRAM_ADDRESS_REGISTERS_ARB                   0x88B0
#define GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB               0x88B1
#define GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB            0x88B2
#define GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB        0x88B3
#define GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB                0x88B4
#define GL_MAX_PROGRAM_ENV_PARAMETERS_ARB                  0x88B5
#define GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB                 0x88B6
#define GL_PROGRAM_STRING_ARB                              0x8628
#define GL_PROGRAM_ERROR_POSITION_ARB                      0x864B
#define GL_CURRENT_MATRIX_ARB                              0x8641
#define GL_TRANSPOSE_CURRENT_MATRIX_ARB                    0x88B7
#define GL_CURRENT_MATRIX_STACK_DEPTH_ARB                  0x8640
#define GL_MAX_VERTEX_ATTRIBS_ARB                          0x8869
#define GL_MAX_PROGRAM_MATRICES_ARB                        0x862F
#define GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB              0x862E
#define GL_PROGRAM_ERROR_STRING_ARB                        0x8874
#define GL_MATRIX0_ARB                                     0x88C0
#define GL_MATRIX1_ARB                                     0x88C1
#define GL_MATRIX2_ARB                                     0x88C2
#define GL_MATRIX3_ARB                                     0x88C3
#define GL_MATRIX4_ARB                                     0x88C4
#define GL_MATRIX5_ARB                                     0x88C5
#define GL_MATRIX6_ARB                                     0x88C6
#define GL_MATRIX7_ARB                                     0x88C7
#define GL_MATRIX8_ARB                                     0x88C8
#define GL_MATRIX9_ARB                                     0x88C9
#define GL_MATRIX10_ARB                                    0x88CA
#define GL_MATRIX11_ARB                                    0x88CB
#define GL_MATRIX12_ARB                                    0x88CC
#define GL_MATRIX13_ARB                                    0x88CD
#define GL_MATRIX14_ARB                                    0x88CE
#define GL_MATRIX15_ARB                                    0x88CF
#define GL_MATRIX16_ARB                                    0x88D0
#define GL_MATRIX17_ARB                                    0x88D1
#define GL_MATRIX18_ARB                                    0x88D2
#define GL_MATRIX19_ARB                                    0x88D3
#define GL_MATRIX20_ARB                                    0x88D4
#define GL_MATRIX21_ARB                                    0x88D5
#define GL_MATRIX22_ARB                                    0x88D6
#define GL_MATRIX23_ARB                                    0x88D7
#define GL_MATRIX24_ARB                                    0x88D8
#define GL_MATRIX25_ARB                                    0x88D9
#define GL_MATRIX26_ARB                                    0x88DA
#define GL_MATRIX27_ARB                                    0x88DB
#define GL_MATRIX28_ARB                                    0x88DC
#define GL_MATRIX29_ARB                                    0x88DD
#define GL_MATRIX30_ARB                                    0x88DE
#define GL_MATRIX31_ARB                                    0x88DF
typedef void (GLFUNC *PFNVERTEXATTRIB1SARBPROC)( GLuint index, GLshort x);
typedef void (GLFUNC *PFNVERTEXATTRIB1FARBPROC)( GLuint index, GLfloat x);
typedef void (GLFUNC *PFNVERTEXATTRIB1DARBPROC)( GLuint index, GLdouble x);
typedef void (GLFUNC *PFNVERTEXATTRIB2SARBPROC)( GLuint index, GLshort x, GLshort y);
typedef void (GLFUNC *PFNVERTEXATTRIB2FARBPROC)( GLuint index, GLfloat x, GLfloat y);
typedef void (GLFUNC *PFNVERTEXATTRIB2DARBPROC)( GLuint index, GLdouble x, GLdouble y);
typedef void (GLFUNC *PFNVERTEXATTRIB3SARBPROC)( GLuint index, GLshort x, GLshort y, GLshort z);
typedef void (GLFUNC *PFNVERTEXATTRIB3FARBPROC)( GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (GLFUNC *PFNVERTEXATTRIB3DARBPROC)( GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (GLFUNC *PFNVERTEXATTRIB4SARBPROC)( GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (GLFUNC *PFNVERTEXATTRIB4FARBPROC)( GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (GLFUNC *PFNVERTEXATTRIB4DARBPROC)( GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (GLFUNC *PFNVERTEXATTRIB4NUBARBPROC)( GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef void (GLFUNC *PFNVERTEXATTRIB1SVARBPROC)( GLuint index, const GLshort *v);
typedef void (GLFUNC *PFNVERTEXATTRIB1FVARBPROC)( GLuint index, const GLfloat *v);
typedef void (GLFUNC *PFNVERTEXATTRIB1DVARBPROC)( GLuint index, const GLdouble *v);
typedef void (GLFUNC *PFNVERTEXATTRIB2SVARBPROC)( GLuint index, const GLshort *v);
typedef void (GLFUNC *PFNVERTEXATTRIB2FVARBPROC)( GLuint index, const GLfloat *v);
typedef void (GLFUNC *PFNVERTEXATTRIB2DVARBPROC)( GLuint index, const GLdouble *v);
typedef void (GLFUNC *PFNVERTEXATTRIB3SVARBPROC)( GLuint index, const GLshort *v);
typedef void (GLFUNC *PFNVERTEXATTRIB3FVARBPROC)( GLuint index, const GLfloat *v);
typedef void (GLFUNC *PFNVERTEXATTRIB3DVARBPROC)( GLuint index, const GLdouble *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4BVARBPROC)( GLuint index, const GLbyte *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4SVARBPROC)( GLuint index, const GLshort *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4IVARBPROC)( GLuint index, const GLint *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4UBVARBPROC)( GLuint index, const GLubyte *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4USVARBPROC)( GLuint index, const GLushort *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4UIVARBPROC)( GLuint index, const GLuint *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4FVARBPROC)( GLuint index, const GLfloat *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4DVARBPROC)( GLuint index, const GLdouble *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4NBVARBPROC)( GLuint index, const GLbyte *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4NSVARBPROC)( GLuint index, const GLshort *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4NIVARBPROC)( GLuint index, const GLint *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4NUBVARBPROC)( GLuint index, const GLubyte *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4NUSVARBPROC)( GLuint index, const GLushort *v);
typedef void (GLFUNC *PFNVERTEXATTRIB4NUIVARBPROC)( GLuint index, const GLuint *v);
typedef void (GLFUNC *PFNVERTEXATTRIBPOINTERARBPROC)( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (GLFUNC *PFNENABLEVERTEXATTRIBARRAYARBPROC)( GLuint index);
typedef void (GLFUNC *PFNDISABLEVERTEXATTRIBARRAYARBPROC)( GLuint index);
typedef void (GLFUNC *PFNPROGRAMSTRINGARBPROC)( GLenum target, GLenum format, GLsizei len, const void *string); 
typedef void (GLFUNC *PFNBINDPROGRAMARBPROC)( GLenum target, GLuint program);
typedef void (GLFUNC *PFNDELETEPROGRAMSARBPROC)( GLsizei n, const GLuint *programs);
typedef void (GLFUNC *PFNGENPROGRAMSARBPROC)( GLsizei n, GLuint *programs);
typedef void (GLFUNC *PFNPROGRAMENVPARAMETER4DARBPROC)( GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (GLFUNC *PFNPROGRAMENVPARAMETER4DVARBPROC)( GLenum target, GLuint index, const GLdouble *params);
typedef void (GLFUNC *PFNPROGRAMENVPARAMETER4FARBPROC)( GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (GLFUNC *PFNPROGRAMENVPARAMETER4FVARBPROC)( GLenum target, GLuint index, const GLfloat *params);
typedef void (GLFUNC *PFNPROGRAMLOCALPARAMETER4DARBPROC)( GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (GLFUNC *PFNPROGRAMLOCALPARAMETER4DVARBPROC)( GLenum target, GLuint index, const GLdouble *params);
typedef void (GLFUNC *PFNPROGRAMLOCALPARAMETER4FARBPROC)( GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (GLFUNC *PFNPROGRAMLOCALPARAMETER4FVARBPROC)( GLenum target, GLuint index, const GLfloat *params);
typedef void (GLFUNC *PFNGETPROGRAMENVPARAMETERDVARBPROC)( GLenum target, GLuint index, GLdouble *params);
typedef void (GLFUNC *PFNGETPROGRAMENVPARAMETERFVARBPROC)( GLenum target, GLuint index, GLfloat *params);
typedef void (GLFUNC *PFNGETPROGRAMLOCALPARAMETERDVARBPROC)( GLenum target, GLuint index, GLdouble *params);
typedef void (GLFUNC *PFNGETPROGRAMLOCALPARAMETERFVARBPROC)( GLenum target, GLuint index, GLfloat *params);
typedef void (GLFUNC *PFNGETPROGRAMIVARBPROC)( GLenum target, GLenum pname, GLint *params);
typedef void (GLFUNC *PFNGETPROGRAMSTRINGARBPROC)( GLenum target, GLenum pname, void *string);
typedef void (GLFUNC *PFNGETVERTEXATTRIBDVARBPROC)( GLuint index, GLenum pname, GLdouble *params);
typedef void (GLFUNC *PFNGETVERTEXATTRIBFVARBPROC)( GLuint index, GLenum pname, GLfloat *params);
typedef void (GLFUNC *PFNGETVERTEXATTRIBIVARBPROC)( GLuint index, GLenum pname, GLint *params);
typedef void (GLFUNC *PFNGETVERTEXATTRIBPOINTERVARBPROC)( GLuint index, GLenum pname, void **pointer);
typedef GLboolean (GLFUNC *PFNISPROGRAMARBPROC)( GLuint program);extern PFNVERTEXATTRIB1SARBPROC glextVertexAttrib1sARB;
extern PFNVERTEXATTRIB1FARBPROC glextVertexAttrib1fARB;
extern PFNVERTEXATTRIB1DARBPROC glextVertexAttrib1dARB;
extern PFNVERTEXATTRIB2SARBPROC glextVertexAttrib2sARB;
extern PFNVERTEXATTRIB2FARBPROC glextVertexAttrib2fARB;
extern PFNVERTEXATTRIB2DARBPROC glextVertexAttrib2dARB;
extern PFNVERTEXATTRIB3SARBPROC glextVertexAttrib3sARB;
extern PFNVERTEXATTRIB3FARBPROC glextVertexAttrib3fARB;
extern PFNVERTEXATTRIB3DARBPROC glextVertexAttrib3dARB;
extern PFNVERTEXATTRIB4SARBPROC glextVertexAttrib4sARB;
extern PFNVERTEXATTRIB4FARBPROC glextVertexAttrib4fARB;
extern PFNVERTEXATTRIB4DARBPROC glextVertexAttrib4dARB;
extern PFNVERTEXATTRIB4NUBARBPROC glextVertexAttrib4NubARB;
extern PFNVERTEXATTRIB1SVARBPROC glextVertexAttrib1svARB;
extern PFNVERTEXATTRIB1FVARBPROC glextVertexAttrib1fvARB;
extern PFNVERTEXATTRIB1DVARBPROC glextVertexAttrib1dvARB;
extern PFNVERTEXATTRIB2SVARBPROC glextVertexAttrib2svARB;
extern PFNVERTEXATTRIB2FVARBPROC glextVertexAttrib2fvARB;
extern PFNVERTEXATTRIB2DVARBPROC glextVertexAttrib2dvARB;
extern PFNVERTEXATTRIB3SVARBPROC glextVertexAttrib3svARB;
extern PFNVERTEXATTRIB3FVARBPROC glextVertexAttrib3fvARB;
extern PFNVERTEXATTRIB3DVARBPROC glextVertexAttrib3dvARB;
extern PFNVERTEXATTRIB4BVARBPROC glextVertexAttrib4bvARB;
extern PFNVERTEXATTRIB4SVARBPROC glextVertexAttrib4svARB;
extern PFNVERTEXATTRIB4IVARBPROC glextVertexAttrib4ivARB;
extern PFNVERTEXATTRIB4UBVARBPROC glextVertexAttrib4ubvARB;
extern PFNVERTEXATTRIB4USVARBPROC glextVertexAttrib4usvARB;
extern PFNVERTEXATTRIB4UIVARBPROC glextVertexAttrib4uivARB;
extern PFNVERTEXATTRIB4FVARBPROC glextVertexAttrib4fvARB;
extern PFNVERTEXATTRIB4DVARBPROC glextVertexAttrib4dvARB;
extern PFNVERTEXATTRIB4NBVARBPROC glextVertexAttrib4NbvARB;
extern PFNVERTEXATTRIB4NSVARBPROC glextVertexAttrib4NsvARB;
extern PFNVERTEXATTRIB4NIVARBPROC glextVertexAttrib4NivARB;
extern PFNVERTEXATTRIB4NUBVARBPROC glextVertexAttrib4NubvARB;
extern PFNVERTEXATTRIB4NUSVARBPROC glextVertexAttrib4NusvARB;
extern PFNVERTEXATTRIB4NUIVARBPROC glextVertexAttrib4NuivARB;
extern PFNVERTEXATTRIBPOINTERARBPROC glextVertexAttribPointerARB;
extern PFNENABLEVERTEXATTRIBARRAYARBPROC glextEnableVertexAttribArrayARB;
extern PFNDISABLEVERTEXATTRIBARRAYARBPROC glextDisableVertexAttribArrayARB;
extern PFNPROGRAMSTRINGARBPROC glextProgramStringARB;
extern PFNBINDPROGRAMARBPROC glextBindProgramARB;
extern PFNDELETEPROGRAMSARBPROC glextDeleteProgramsARB;
extern PFNGENPROGRAMSARBPROC glextGenProgramsARB;
extern PFNPROGRAMENVPARAMETER4DARBPROC glextProgramEnvParameter4dARB;
extern PFNPROGRAMENVPARAMETER4DVARBPROC glextProgramEnvParameter4dvARB;
extern PFNPROGRAMENVPARAMETER4FARBPROC glextProgramEnvParameter4fARB;
extern PFNPROGRAMENVPARAMETER4FVARBPROC glextProgramEnvParameter4fvARB;
extern PFNPROGRAMLOCALPARAMETER4DARBPROC glextProgramLocalParameter4dARB;
extern PFNPROGRAMLOCALPARAMETER4DVARBPROC glextProgramLocalParameter4dvARB;
extern PFNPROGRAMLOCALPARAMETER4FARBPROC glextProgramLocalParameter4fARB;
extern PFNPROGRAMLOCALPARAMETER4FVARBPROC glextProgramLocalParameter4fvARB;
extern PFNGETPROGRAMENVPARAMETERDVARBPROC glextGetProgramEnvParameterdvARB;
extern PFNGETPROGRAMENVPARAMETERFVARBPROC glextGetProgramEnvParameterfvARB;
extern PFNGETPROGRAMLOCALPARAMETERDVARBPROC glextGetProgramLocalParameterdvARB;
extern PFNGETPROGRAMLOCALPARAMETERFVARBPROC glextGetProgramLocalParameterfvARB;
extern PFNGETPROGRAMIVARBPROC glextGetProgramivARB;
extern PFNGETPROGRAMSTRINGARBPROC glextGetProgramStringARB;
extern PFNGETVERTEXATTRIBDVARBPROC glextGetVertexAttribdvARB;
extern PFNGETVERTEXATTRIBFVARBPROC glextGetVertexAttribfvARB;
extern PFNGETVERTEXATTRIBIVARBPROC glextGetVertexAttribivARB;
extern PFNGETVERTEXATTRIBPOINTERVARBPROC glextGetVertexAttribPointervARB;
extern PFNISPROGRAMARBPROC glextIsProgramARB;


bool GLExtInit_arb_vertex_program();


