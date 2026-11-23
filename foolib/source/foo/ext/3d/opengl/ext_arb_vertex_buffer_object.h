#define GL_ARRAY_BUFFER_ARB                             0x8892
#define GL_ELEMENT_ARRAY_BUFFER_ARB                     0x8893
#define GL_ARRAY_BUFFER_BINDING_ARB                     0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB             0x8895
#define GL_VERTEX_ARRAY_BUFFER_BINDING_ARB              0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING_ARB              0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING_ARB               0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING_ARB               0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB       0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB           0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB     0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB      0x889D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB              0x889E
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB       0x889F
#define GL_STREAM_DRAW_ARB                              0x88E0
#define GL_STREAM_READ_ARB                              0x88E1
#define GL_STREAM_COPY_ARB                              0x88E2
#define GL_STATIC_DRAW_ARB                              0x88E4
#define GL_STATIC_READ_ARB                              0x88E5
#define GL_STATIC_COPY_ARB                              0x88E6
#define GL_DYNAMIC_DRAW_ARB                             0x88E8
#define GL_DYNAMIC_READ_ARB                             0x88E9
#define GL_DYNAMIC_COPY_ARB                             0x88EA
#define GL_READ_ONLY_ARB                                0x88B8
#define GL_WRITE_ONLY_ARB                               0x88B9
#define GL_READ_WRITE_ARB                               0x88BA
#define GL_BUFFER_MAP_POINTER_ARB                       0x88BD
typedef void (GLFUNC *PFNBINDBUFFERARBPROC)( GLenum target, GLuint buffer);
typedef void (GLFUNC *PFNDELETEBUFFERSARBPROC)( GLuint n, const GLuint *buffers);
typedef void (GLFUNC *PFNGENBUFFERSARBPROC)( GLuint n, GLuint *buffers);
typedef GLboolean (GLFUNC *PFNISBUFFERARBPROC)( GLuint buffer);
typedef void (GLFUNC *PFNBUFFERDATAARBPROC)( GLenum target, GLuint size, const void *data, GLenum usage);
typedef void (GLFUNC *PFNBUFFERSUBDATAARBPROC)( GLenum target, GLuint offset, GLuint size, const void *data);
typedef void (GLFUNC *PFNGETBUFFERSUBDATAARBPROC)( GLenum target, GLuint offset, GLuint size, void *data);
typedef void* (GLFUNC *PFNMAPBUFFERARBPROC)( GLenum target, GLenum access);
typedef GLboolean (GLFUNC *PFNUNMAPBUFFERARBPROC)( GLenum target);
typedef void (GLFUNC *PFNGETBUFFERPARAMETERIVARBPROC)( GLenum target, GLenum pname, GLint *params);
typedef void (GLFUNC *PFNGETBUFFERPOINTERVARBPROC)( GLenum target, GLenum pname, void **params);extern PFNBINDBUFFERARBPROC glextBindBufferARB;
extern PFNDELETEBUFFERSARBPROC glextDeleteBuffersARB;
extern PFNGENBUFFERSARBPROC glextGenBuffersARB;
extern PFNISBUFFERARBPROC glextIsBufferARB;
extern PFNBUFFERDATAARBPROC glextBufferDataARB;
extern PFNBUFFERSUBDATAARBPROC glextBufferSubDataARB;
extern PFNGETBUFFERSUBDATAARBPROC glextGetBufferSubDataARB;
extern PFNMAPBUFFERARBPROC glextMapBufferARB;
extern PFNUNMAPBUFFERARBPROC glextUnmapBufferARB;
extern PFNGETBUFFERPARAMETERIVARBPROC glextGetBufferParameterivARB;
extern PFNGETBUFFERPOINTERVARBPROC glextGetBufferPointervARB;


bool GLExtInit_arb_vertex_buffer_object();

