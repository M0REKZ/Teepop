#ifndef __FOO_EXT_3D_DIRECTX__
#define __FOO_EXT_3D_DIRECTX__
/*#ifndef _WINDOWS_
	#error Can only have directx on windows platforms
#endif*/

#include <d3d9.h>

#include <ext/3d/3d.h>

//
//
//


namespace NFoo { namespace N3D { namespace NDirectX {
#define _DX_DEVICE_DEBUG
	#ifdef _DX_DEVICE_DEBUG
		#define MACRO_DXCHECKERROR(Val,Text)\
			if (!SUCCEEDED(##Val))\
			NLog::Write(##Text)
	#else
		#define MACRO_DXCHECKERROR(Val,Text)\
			(##Val);
	#endif
	class CDirectXDevice : public CDevice
	{
		enum
		{
			EMODE_DEVICEPARAMINDEX_COLOR = 0,
			EMODE_DEVICEPARAMINDEX_DEPTHSTENCIL = 0,

			ED3D_ELEMENTSPERPOSITION = 3,
			ED3D_ELEMENTSPERNORMAL = 3,
			ED3D_ELEMENTSPERCOLOR  = 1,
			ED3D_ELEMENTSPERTEXCOORD = 2,
			ED3D_ELEMENTSPERINDEX = 1,

			ED3D_STREAMFLAG_POSITION		= 1 << 0,
			ED3D_STREAMFLAG_NORMAL			= 1 << 1,
			ED3D_STREAMFLAG_COLOR			= 1 << 2,
			ED3D_STREAMFLAG_TEXCOORD_BASE	= 1 << 3,
			ED3D_STREAMFLAG_TEXCOORD0		= ED3D_STREAMFLAG_TEXCOORD_BASE << 0,
			ED3D_STREAMFLAG_TEXCOORD1		= ED3D_STREAMFLAG_TEXCOORD_BASE << 1,
			ED3D_STREAMFLAG_TEXCOORD2		= ED3D_STREAMFLAG_TEXCOORD_BASE << 2,
			ED3D_STREAMFLAG_TEXCOORD3		= ED3D_STREAMFLAG_TEXCOORD_BASE << 3,
			ED3D_STREAMFLAG_TEXCOORD4		= ED3D_STREAMFLAG_TEXCOORD_BASE << 4,

			ED3D_STREAMSOURCEINDEX_POSITION		= 0,
			ED3D_STREAMSOURCEINDEX_NORMAL,
			ED3D_STREAMSOURCEINDEX_COLOR,
			ED3D_STREAMSOURCEINDEX_TEXCOORD_BASE,
			ED3D_STREAMSOURCEINDEX_TEXCOORD0 = ED3D_STREAMSOURCEINDEX_TEXCOORD_BASE,
			ED3D_STREAMSOURCEINDEX_TEXCOORD1,
			ED3D_STREAMSOURCEINDEX_TEXCOORD2,
			ED3D_STREAMSOURCEINDEX_TEXCOORD3,
			ED3D_STREAMSOURCEINDEX_TEXCOORD4,
			ED3D_STREAMSOURCE_NUMSTREAMS,
		};
		struct SBBufferMode
		{
			D3DFORMAT m_Format;
			bool m_bWindowed;
		};
		class CDirectXTexture
		{
		public:
			IDirect3DTexture9* m_pTexture;
			CTextureInfo m_Info;
		};

		class CDirectXVertexBuffer : public CVertexBuffer
		{
		public:
			IDirect3DVertexBuffer9* m_pD3DPositionBuffer;
			IDirect3DVertexBuffer9* m_pD3DNormalBuffer;
			IDirect3DVertexBuffer9* m_pD3DColorBuffer;
			IDirect3DVertexBuffer9* m_lpD3DTexCoordbuffer[EMaxTextureUnits];
			CDirectXVertexBuffer();
			virtual ~CDirectXVertexBuffer();

			void ClearBasePointers();
		};

		class CDirectXIndexBuffer : public CIndexBuffer
		{
		public:
			IDirect3DIndexBuffer9* m_pD3DIndexBuffer;
			
			CDirectXIndexBuffer();
			virtual ~CDirectXIndexBuffer();
		};

		class CDirectXVertexShader
		{
		public:
			IDirect3DVertexShader9* m_pVertexShader;
			CDirectXVertexShader()
			{
				m_pVertexShader = NULL;
			}
			virtual ~CDirectXVertexShader()
			{
				SafeRelease(m_pVertexShader);
			}
		};

		class CDirectXPixelShader
		{
		public:
			IDirect3DPixelShader9* m_pPixelShader;
			CDirectXPixelShader()
			{
				m_pPixelShader = NULL;
			}
			virtual ~CDirectXPixelShader()
			{
				SafeRelease(m_pPixelShader);
			}
		};

		HWND m_hWindow;

		// Should get caps
		D3DCAPS9 m_DeviceCaps;
		IDirect3D9* m_pD3D;
		IDirect3DDevice9* m_p3DDevice;
		IDirect3DVertexDeclaration9* m_pVertexDeclaration;

		int32 m_StreamFlags;
		int32 m_StreamDeclSet;

		static int32 ColorBits(D3DFORMAT _Format);
		static int32 AlphaBits(D3DFORMAT _Format);
		static int32 DepthBits(D3DFORMAT _Format);
		static int32 StencilBits(D3DFORMAT _Format);
		static int32 GetTexelSize(D3DFORMAT _Format);
		static int32 GetBufferElementSize(int32 _What);

		static D3DFORMAT VerifyColorFormat(uint32 _Format);
		static D3DFORMAT VerifyDepthStencil(uint32 _Format);
		// Give CImage::Enumformat
		static D3DFORMAT GetTextureFormat(int32 _Format);

		static bool AddDeviceMode(TDynamicArray<CMode>& _rModes, const CMode& _AddMode, const CMode& _ModeRef);

		// Check how many stencil modes we can have in current displaymode
		void GetDepthModes(uint32 _iAdapter, D3DFORMAT _DisplayFormat, D3DFORMAT _BBufferFormat,
			TDynamicArray<D3DFORMAT>& _DepthModes, const CMode& _Mode);
		void GetBackBufferFormats(uint32 _iAdapter, D3DFORMAT _DisplayFormat, 
			TDynamicArray<SBBufferMode>& _BBModes, bool _bWindowed, const CMode& _Mode);

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		bool CopyTextureData(int32 _Width, int32 _Height, const D3DLOCKED_RECT& _LockedRect, D3DFORMAT _TexFormat, uint8* _pData, int32 _Format);


		void ZeroCaps(D3DCAPS9& _Caps);

		void SetStreamDeclaration();

	public:
		CDirectXDevice();
		virtual ~CDirectXDevice();

		// TEMP REMOVE
		inline IDirect3DDevice9* GetDXDevice() { return m_p3DDevice; }

		virtual int32 EnumDisplayModes(TDynamicArray<CMode> &_rModes, CMode _Mode);

		// Main operation
		virtual bool Init(CMode _Mode);
		virtual bool SwapBuffers();
		virtual bool Update();
		virtual bool Shutdown();

		// Texture handling
		virtual int32 CreateTexture(int32 _Width, int32 _Height, int32 _Format);
		virtual int32 CreateTexture(CImage &_Image);
		virtual bool DestroyTexture(int32 _iTexture);
		virtual CTextureInfo GetTextureInfo(int32 _iTexture);

		// _Format, format of indata
		virtual int32 Internal_CreateTexture(int32 _Width, int32 _Height, int32 _Format, uint8 *_pData);

		// vertex buffers
		virtual int32 CreateVertexBuffer(int32 _Count, int32 _What, int32 _Flags);
		// Get vertexbuffer from id
		virtual CVertexBuffer* LockVertexBuffer(int32 _iBuffer, int32 _What = EVB_All);
		// Return control of vertexbuffer to device
		virtual void UnlockVertexBuffer(int32 _iBuffer, int32 _What = EVB_All);
		virtual bool DestroyVertexBuffer(int32 _iBuffer);

		virtual int32 CreateIndexBuffer(int32 _Count, int32 _Flags);
		virtual bool DestroyIndexBuffer(int32 _iBuffer);
		// Get indexbuffer from id, Return control of indexbuffer to device
		virtual CIndexBuffer* LockIndexBuffer(int32 _iBuffer);
		virtual void UnlockIndexBuffer(int32 _iBuffer);

		// Shaders
		virtual int32 CreateVertexShader(const int8 *_pString);
		virtual int32 CreatePixelShader(const int8 *_pString);
		virtual bool DestroyVertexShader(int32 _iShader);
		virtual bool DestroyPixelShader(int32 _iShader);

		virtual void GetShaderBaseDir(CString& _DirName) { _DirName += "DirectX/"; }

		// Clearing
		virtual void Clear(intr _What);

		virtual void Draw(int32 _Mode, int32 _Start, int32 _Count);
		virtual void DrawIndexed(int32 _Mode, int32 _Start, int32 _Count);

		// Caps
		virtual uint32 GetVertexShaderCapabilities() { return m_DeviceCaps.VertexShaderVersion; };
		virtual uint32 GetPixelShaderCapabilities() { return m_DeviceCaps.PixelShaderVersion; };


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

		// streams/pointers
		virtual void Stream_Vertex(int32 _iVB);
		virtual void Stream_Index(int32 _iIB);
		virtual void Stream_Normal(int32 _iVB);
		virtual void Stream_Color(int32 _iVB);
		virtual void Stream_Texture(int32 _iTex, int32 _iVB);

		// shaders
		virtual void Shader_Vertex(int32 _iShader);
		virtual void Shader_Pixel(int32 _iShader);

		// _Num == Number of boolean values in the array of constants
		virtual void Shader_VertexParamBool(int32 _iStartReg, const bool* _pBools, int32 _Num);
		// _Num == Number of four float vectors in the array of constants
		virtual void Shader_VertexParamFloat(int32 _iStartReg, const TVector4<fp32>* _pFloats, int32 _Num);
		// _Num == Number of four integer vectors in the array of constants
		virtual void Shader_VertexParamInt(int32 _iStartReg, const TVector4<int32>* _pInts, int32 _Num);

		// _Num == Number of boolean values in the array of constants
		virtual void Shader_PixelParamBool(int32 _iStartReg, const bool* _pBools, int32 _Num);
		// _Num == Number of four float vectors in the array of constants
		virtual void Shader_PixelParamFloat(int32 _iStartReg, const TVector4<fp32>* _pFloats, int32 _Num);
		// _Num == Number of four integer vectors in the array of constants
		virtual void Shader_PixelParamInt(int32 _iStartReg, const TVector4<int32>* _pInts, int32 _Num);

		// textures
		virtual void Texture_Set(intr _iUnit, int32 _iTexture); // -1 disable

		// matrices
		virtual void Matrix_Model(TMatrix4<fp32> _Matrix);
		virtual void Matrix_Projection(TMatrix4<fp32> _Matrix);

		// grab
		virtual bool Grab(int32 _iTexture, int32 _What){ return false;}
		//
		virtual void SetViewport(int32 _X, int32 _Y, int32 _Width, int32 _Height);
	};

} } } // end namespaces

#endif
