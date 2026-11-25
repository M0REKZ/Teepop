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
		int32_t m_Flags;
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
		int32_t m_iBufferID; // when using VBO
		int32_t m_What;
	};

	//
	//
	//
	class COpenGLDevice : public CDevice
	{
		//int32_t Internal_CreateTexture(int32_t _w, int32_t _h, int32_t _Format, uint8_t *m_pData);

		struct SExt
		{
			bool ARB_vertex_buffer_object;
			bool ARB_vertex_program;
			bool ARB_multitexture;
			bool EXT_swap_control;
			bool EXT_bgra;
		} m_Exts;

		void *m_pIndexPointer;
		int32_t m_ScreenWidth;
		int32_t m_ScreenHeight;

	public:
		COpenGLDevice();
		virtual ~COpenGLDevice() { }

		// before init operations
		virtual int32_t EnumDisplayModes(TDynamicArray<CMode> &_rModes, CMode _Mode) pure;

		// main operation
		virtual bool Init(CMode _Mode);
		virtual bool SwapBuffers() pure;
		virtual bool Shutdown() pure;

		// texture handling
		virtual int32_t CreateTexture(CImage::CFormat Format, int32_t Flags=0, void *pData=NULL);
		virtual int32_t CreateTexture(CImage &_Image, int32_t Flags=0);
		virtual bool DestroyTexture(int32_t _iTexture);
		virtual CTextureInfo GetTextureInfo(int32_t _iTexture);

		// index buffers (UNIMPLEMENTED)
		virtual int32_t CreateIndexBuffer(int32_t _Count, int32_t _Flags);
		virtual bool DestroyIndexBuffer(int32_t _iBuffer); 
		// Get indexbuffer from id, Return control of indexbuffer to device
		virtual CIndexBuffer* LockIndexBuffer(int32_t _iBuffer);
		virtual void UnlockIndexBuffer(int32_t _iBuffer);

		// Create/destroy vertex buffer
		virtual int32_t CreateVertexBuffer(int32_t _Count, int32_t _What, int32_t _Flags);
		virtual bool DestroyVertexBuffer(int32_t _iBuffer);
		// Get vertexbuffer from id, Return control of vertexbuffer to device
		virtual CVertexBuffer* LockVertexBuffer(int32_t _iBuffer, int32_t _What = -1);
		virtual void UnlockVertexBuffer(int32_t _iBuffer, int32_t _What = -1);

		// shaders
		virtual int32_t CreateVertexShader(const char *_pString);
		virtual int32_t CreatePixelShader(const char *_pString);
		virtual bool DestroyVertexShader(int32_t _iShader);
		virtual bool DestroyPixelShader(int32_t _iShader);

		// clearing
		virtual void Clear(intr _What);

		// draw operation
		virtual void Draw(int32_t _Mode, int32_t _Start, int32_t _Count);
		virtual void DrawIndexed(int32_t _Mode, int32_t _Start, int32_t _Count); // (UNIMPLEMENTED)

		// grab
		virtual bool Grab(int32_t _iTexture, int32_t _What);
		virtual bool Grab(NFoo::CImage &_rImage, int32_t _What);

		// viewport
		virtual void SetViewport(int32_t _x, int32_t _y, int32_t _w, int32_t _h);
		virtual void SetScissors(int32_t _x, int32_t _y, int32_t _w, int32_t _h);

		//
		// STATES
		//

		// flags
		virtual void Flag_Set(int32_t _Flag, bool _On);
		
		// modes
		virtual void Mode_BlendFunc(intr _Src, intr _Dst);
		virtual void Mode_DepthTest(intr _Test);
		virtual void Mode_Cull(intr _CullMode);
		virtual void Mode_StencilFunc(intr t, intr r, intr m);
		virtual void Mode_StencilOp(intr sf, intr f, intr p);
		virtual void Mode_PrimaryColor(fp32 r, fp32 g, fp32 b, fp32 a);
		virtual void Mode_WriteMask(bool r, bool g, bool b, bool a);

		// pointers
		virtual void Stream_Vertex(int32_t _iVB);
		virtual void Stream_Normal(int32_t _iVB);
		virtual void Stream_Color(int32_t _iVB);
		virtual void Stream_Texture(int32_t _iUnit, int32_t _iVB, int32_t _Index);
		virtual void Stream_Index(int32_t _iIB);

		// shaders
		virtual void Shader_Vertex(int32_t _iShader);
		virtual void Shader_Pixel(int32_t _iShader);
		
		virtual void Shader_GlobalParam() {}; // ?
		virtual void Shader_VertexParamFloat(int32_t _iStartReg, const TVector4<fp32>* _pFloats, int32_t _Num); // ?
		virtual void Shader_PixelParam() {}; // ?

		// textures
		virtual void Texture_Set(intr _iUnit, int32_t _iTexture); // -1 disable
		
		// matrices
		virtual void Matrix_Model(TMatrix4<fp32> _Matrix);
		virtual void Matrix_Projection(TMatrix4<fp32> _Matrix);
	};

} } } // end namespaces

#endif // FOO_EXT_3D_OPENGL_H
