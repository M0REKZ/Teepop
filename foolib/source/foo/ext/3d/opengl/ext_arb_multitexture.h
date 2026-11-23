#define GL_TEXTURE0_ARB                   0x84C0
#define GL_TEXTURE1_ARB                   0x84C1
#define GL_TEXTURE2_ARB                   0x84C2
#define GL_TEXTURE3_ARB                   0x84C3
#define GL_TEXTURE4_ARB                   0x84C4
#define GL_TEXTURE5_ARB                   0x84C5
#define GL_TEXTURE6_ARB                   0x84C6
#define GL_TEXTURE7_ARB                   0x84C7
#define GL_TEXTURE8_ARB                   0x84C8
#define GL_TEXTURE9_ARB                   0x84C9
#define GL_TEXTURE10_ARB                  0x84CA
#define GL_TEXTURE11_ARB                  0x84CB
#define GL_TEXTURE12_ARB                  0x84CC
#define GL_TEXTURE13_ARB                  0x84CD
#define GL_TEXTURE14_ARB                  0x84CE
#define GL_TEXTURE15_ARB                  0x84CF
#define GL_TEXTURE16_ARB                  0x84D0
#define GL_TEXTURE17_ARB                  0x84D1
#define GL_TEXTURE18_ARB                  0x84D2
#define GL_TEXTURE19_ARB                  0x84D3
#define GL_TEXTURE20_ARB                  0x84D4
#define GL_TEXTURE21_ARB                  0x84D5
#define GL_TEXTURE22_ARB                  0x84D6
#define GL_TEXTURE23_ARB                  0x84D7
#define GL_TEXTURE24_ARB                  0x84D8
#define GL_TEXTURE25_ARB                  0x84D9
#define GL_TEXTURE26_ARB                  0x84DA
#define GL_TEXTURE27_ARB                  0x84DB
#define GL_TEXTURE28_ARB                  0x84DC
#define GL_TEXTURE29_ARB                  0x84DD
#define GL_TEXTURE30_ARB                  0x84DE
#define GL_TEXTURE31_ARB                  0x84DF
#define GL_ACTIVE_TEXTURE_ARB             0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB      0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB          0x84E2

typedef void (GLFUNC *PFNACTIVETEXTUREARBPROC)( GLenum);
typedef void (GLFUNC *PFNCLIENTACTIVETEXTUREARBPROC)( GLenum);
typedef void (GLFUNC *PFNMULTITEXCOORD1DARBPROC)( GLenum, GLdouble);
typedef void (GLFUNC *PFNMULTITEXCOORD1DVARBPROC)( GLenum, const GLdouble *);
typedef void (GLFUNC *PFNMULTITEXCOORD1FARBPROC)( GLenum, GLfloat);
typedef void (GLFUNC *PFNMULTITEXCOORD1FVARBPROC)( GLenum, const GLfloat *);
typedef void (GLFUNC *PFNMULTITEXCOORD1IARBPROC)( GLenum, GLint);
typedef void (GLFUNC *PFNMULTITEXCOORD1IVARBPROC)( GLenum, const GLint *);
typedef void (GLFUNC *PFNMULTITEXCOORD1SARBPROC)( GLenum, GLshort);
typedef void (GLFUNC *PFNMULTITEXCOORD1SVARBPROC)( GLenum, const GLshort *);
typedef void (GLFUNC *PFNMULTITEXCOORD2DARBPROC)( GLenum, GLdouble, GLdouble);
typedef void (GLFUNC *PFNMULTITEXCOORD2DVARBPROC)( GLenum, const GLdouble *);
typedef void (GLFUNC *PFNMULTITEXCOORD2FARBPROC)( GLenum, GLfloat, GLfloat);
typedef void (GLFUNC *PFNMULTITEXCOORD2FVARBPROC)( GLenum, const GLfloat *);
typedef void (GLFUNC *PFNMULTITEXCOORD2IARBPROC)( GLenum, GLint, GLint);
typedef void (GLFUNC *PFNMULTITEXCOORD2IVARBPROC)( GLenum, const GLint *);
typedef void (GLFUNC *PFNMULTITEXCOORD2SARBPROC)( GLenum, GLshort, GLshort);
typedef void (GLFUNC *PFNMULTITEXCOORD2SVARBPROC)( GLenum, const GLshort *);
typedef void (GLFUNC *PFNMULTITEXCOORD3DARBPROC)( GLenum, GLdouble, GLdouble, GLdouble);
typedef void (GLFUNC *PFNMULTITEXCOORD3DVARBPROC)( GLenum, const GLdouble *);
typedef void (GLFUNC *PFNMULTITEXCOORD3FARBPROC)( GLenum, GLfloat, GLfloat, GLfloat);
typedef void (GLFUNC *PFNMULTITEXCOORD3FVARBPROC)( GLenum, const GLfloat *);
typedef void (GLFUNC *PFNMULTITEXCOORD3IARBPROC)( GLenum, GLint, GLint, GLint);
typedef void (GLFUNC *PFNMULTITEXCOORD3IVARBPROC)( GLenum, const GLint *);
typedef void (GLFUNC *PFNMULTITEXCOORD3SARBPROC)( GLenum, GLshort, GLshort, GLshort);
typedef void (GLFUNC *PFNMULTITEXCOORD3SVARBPROC)( GLenum, const GLshort *);
typedef void (GLFUNC *PFNMULTITEXCOORD4DARBPROC)( GLenum, GLdouble, GLdouble, GLdouble, GLdouble);
typedef void (GLFUNC *PFNMULTITEXCOORD4DVARBPROC)( GLenum, const GLdouble *);
typedef void (GLFUNC *PFNMULTITEXCOORD4FARBPROC)( GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
typedef void (GLFUNC *PFNMULTITEXCOORD4FVARBPROC)( GLenum, const GLfloat *);
typedef void (GLFUNC *PFNMULTITEXCOORD4IARBPROC)( GLenum, GLint, GLint, GLint, GLint);
typedef void (GLFUNC *PFNMULTITEXCOORD4IVARBPROC)( GLenum, const GLint *);
typedef void (GLFUNC *PFNMULTITEXCOORD4SARBPROC)( GLenum, GLshort, GLshort, GLshort, GLshort);
typedef void (GLFUNC *PFNMULTITEXCOORD4SVARBPROC)( GLenum, const GLshort *);

extern PFNACTIVETEXTUREARBPROC glextActiveTextureARB;
extern PFNCLIENTACTIVETEXTUREARBPROC glextClientActiveTextureARB;
extern PFNMULTITEXCOORD1DARBPROC glextMultiTexCoord1dARB;
extern PFNMULTITEXCOORD1DVARBPROC glextMultiTexCoord1dvARB;
extern PFNMULTITEXCOORD1FARBPROC glextMultiTexCoord1fARB;
extern PFNMULTITEXCOORD1FVARBPROC glextMultiTexCoord1fvARB;
extern PFNMULTITEXCOORD1IARBPROC glextMultiTexCoord1iARB;
extern PFNMULTITEXCOORD1IVARBPROC glextMultiTexCoord1ivARB;
extern PFNMULTITEXCOORD1SARBPROC glextMultiTexCoord1sARB;
extern PFNMULTITEXCOORD1SVARBPROC glextMultiTexCoord1svARB;
extern PFNMULTITEXCOORD2DARBPROC glextMultiTexCoord2dARB;
extern PFNMULTITEXCOORD2DVARBPROC glextMultiTexCoord2dvARB;
extern PFNMULTITEXCOORD2FARBPROC glextMultiTexCoord2fARB;
extern PFNMULTITEXCOORD2FVARBPROC glextMultiTexCoord2fvARB;
extern PFNMULTITEXCOORD2IARBPROC glextMultiTexCoord2iARB;
extern PFNMULTITEXCOORD2IVARBPROC glextMultiTexCoord2ivARB;
extern PFNMULTITEXCOORD2SARBPROC glextMultiTexCoord2sARB;
extern PFNMULTITEXCOORD2SVARBPROC glextMultiTexCoord2svARB;
extern PFNMULTITEXCOORD3DARBPROC glextMultiTexCoord3dARB;
extern PFNMULTITEXCOORD3DVARBPROC glextMultiTexCoord3dvARB;
extern PFNMULTITEXCOORD3FARBPROC glextMultiTexCoord3fARB;
extern PFNMULTITEXCOORD3FVARBPROC glextMultiTexCoord3fvARB;
extern PFNMULTITEXCOORD3IARBPROC glextMultiTexCoord3iARB;
extern PFNMULTITEXCOORD3IVARBPROC glextMultiTexCoord3ivARB;
extern PFNMULTITEXCOORD3SARBPROC glextMultiTexCoord3sARB;
extern PFNMULTITEXCOORD3SVARBPROC glextMultiTexCoord3svARB;
extern PFNMULTITEXCOORD4DARBPROC glextMultiTexCoord4dARB;
extern PFNMULTITEXCOORD4DVARBPROC glextMultiTexCoord4dvARB;
extern PFNMULTITEXCOORD4FARBPROC glextMultiTexCoord4fARB;
extern PFNMULTITEXCOORD4FVARBPROC glextMultiTexCoord4fvARB;
extern PFNMULTITEXCOORD4IARBPROC glextMultiTexCoord4iARB;
extern PFNMULTITEXCOORD4IVARBPROC glextMultiTexCoord4ivARB;
extern PFNMULTITEXCOORD4SARBPROC glextMultiTexCoord4sARB;
extern PFNMULTITEXCOORD4SVARBPROC glextMultiTexCoord4svARB;


bool GLExtInit_arb_multitexture();

