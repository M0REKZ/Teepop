#ifndef FOO_EXT_3D_OPENGL_H
#define FOO_EXT_3D_OPENGL_H

#include "opengl_include.h"
#include <foo/ext/3d/3d.h>

// extentions
//#include "ext_arb_vertex_program.h"

//
//
//
namespace NFoo { namespace N3D { namespace NOpenGL {

	//
	//
	//
	class COpenGLTexture
	{
	public:
		GLuint m_iGLTexture;
		GLuint m_GLFormat;
		CTextureInfo m_Info;
	};

	//
	//
	//
	class COpenGLHandle
	{
	public:
		GLuint m_iHandle;
	};


	//
	//
	//
	class COpenGLBuffer
	{
	public:
		enum
		{
			ELocked=1
		};

		COpenGLBuffer();
		bool Lock();
		void Unlock();
		int32 m_Flags;
	};

	//
	//
	//
	class COpenGLIndexBuffer : public COpenGLBuffer
	{
	public:
		COpenGLIndexBuffer();
		~COpenGLIndexBuffer();
		CIndexBuffer m_LockInfo;
	};

	//
	//
	//
	class COpenGLVertexBuffer : public COpenGLBuffer
	{
	public:
		COpenGLVertexBuffer();
		~COpenGLVertexBuffer();
		void ARB_VBO_Bind();
		CVertexBuffer m_LockInfo;
		CVertexBuffer m_LockInfo2;
		int32 m_iBufferID; // when using VBO
		int32 m_What;
	};

	//
	//
	//
	class COpenGLDevice : public CDevice
	{
		//int32 Internal_CreateTexture(int32 _w, int32 _h, int32 _Format, uint8 *m_pData);

		struct SExt
		{
			bool ARB_vertex_buffer_object;
			bool ARB_vertex_program;
			bool ARB_multitexture;
			bool EXT_swap_control;
			bool EXT_bgra;
		} m_Exts;

		void *m_pIndexPointer;
		int32 m_ScreenWidth;
		int32 m_ScreenHeight;

	public:
		COpenGLDevice();
		virtual ~COpenGLDevice() { }

		// before init operations
		virtual int32 EnumDisplayModes(TDynamicArray<CMode> &_rModes, CMode _Mode) pure;

		// main operation
		virtual bool Init(CMode _Mode);
		virtual bool SwapBuffers() pure;
		virtual bool Shutdown() pure;

		// texture handling
		virtual int32 CreateTexture(CImage::CFormat Format, int32 Flags=0, void *pData=NULL);
		virtual int32 CreateTexture(CImage &_Image, int32 Flags=0);
		virtual bool DestroyTexture(int32 _iTexture);
		virtual CTextureInfo GetTextureInfo(int32 _iTexture);

		// index buffers (UNIMPLEMENTED)
		virtual int32 CreateIndexBuffer(int32 _Count, int32 _Flags);
		virtual bool DestroyIndexBuffer(int32 _iBuffer); 
		// Get indexbuffer from id, Return control of indexbuffer to device
		virtual CIndexBuffer* LockIndexBuffer(int32 _iBuffer);
		virtual void UnlockIndexBuffer(int32 _iBuffer);

		// Create/destroy vertex buffer
		virtual int32 CreateVertexBuffer(int32 _Count, int32 _What, int32 _Flags);
		virtual bool DestroyVertexBuffer(int32 _iBuffer);
		// Get vertexbuffer from id, Return control of vertexbuffer to device
		virtual CVertexBuffer* LockVertexBuffer(int32 _iBuffer, int32 _What = -1);
		virtual void UnlockVertexBuffer(int32 _iBuffer, int32 _What = -1);

		// shaders
		virtual int32 CreateVertexShader(const int8 *_pString);
		virtual int32 CreatePixelShader(const int8 *_pString);
		virtual bool DestroyVertexShader(int32 _iShader);
		virtual bool DestroyPixelShader(int32 _iShader);

		// clearing
		virtual void Clear(intr _What);

		// draw operation
		virtual void Draw(int32 _Mode, int32 _Start, int32 _Count);
		virtual void DrawIndexed(int32 _Mode, int32 _Start, int32 _Count); // (UNIMPLEMENTED)

		// grab
		virtual bool Grab(int32 _iTexture, int32 _What);
		virtual bool Grab(NFoo::CImage &_rImage, int32 _What);

		// viewport
		virtual void SetViewport(int32 _x, int32 _y, int32 _w, int32 _h);
		virtual void SetScissors(int32 _x, int32 _y, int32 _w, int32 _h);

		//
		// STATES
		//

		// flags
		virtual void Flag_Set(int32 _Flag, bool _On);
		
		// modes
		virtual void Mode_BlendFunc(intr _Src, intr _Dst);
		virtual void Mode_DepthTest(intr _Test);
		virtual void Mode_Cull(intr _CullMode);
		virtual void Mode_StencilFunc(intr t, intr r, intr m);
		virtual void Mode_StencilOp(intr sf, intr f, intr p);
		virtual void Mode_PrimaryColor(fp32 r, fp32 g, fp32 b, fp32 a);
		virtual void Mode_WriteMask(bool r, bool g, bool b, bool a);

		// pointers
		virtual void Stream_Vertex(int32 _iVB);
		virtual void Stream_Normal(int32 _iVB);
		virtual void Stream_Color(int32 _iVB);
		virtual void Stream_Texture(int32 _iUnit, int32 _iVB, int32 _Index);
		virtual void Stream_Index(int32 _iIB);

		// shaders
		virtual void Shader_Vertex(int32 _iShader);
		virtual void Shader_Pixel(int32 _iShader);
		
		virtual void Shader_GlobalParam() {}; // ?
		virtual void Shader_VertexParamFloat(int32 _iStartReg, const TVector4<fp32>* _pFloats, int32 _Num); // ?
		virtual void Shader_PixelParam() {}; // ?

		// textures
		virtual void Texture_Set(intr _iUnit, int32 _iTexture); // -1 disable
		
		// matrices
		virtual void Matrix_Model(TMatrix4<fp32> _Matrix);
		virtual void Matrix_Projection(TMatrix4<fp32> _Matrix);
	};

} } } // end namespaces

#endif // FOO_EXT_3D_OPENGL_H
