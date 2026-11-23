#ifndef __FOO_3D_H__
#define __FOO_3D_H__

#include <foo/func/vector2.h>
#include <foo/func/vector3.h>
#include <foo/func/vector4.h>
#include <foo/func/matrix4.h>
#include <foo/func/image.h>
//#include <foo/func/image.h>
#include <foo/func/string.h>
#include <foo/core/core.h>
#include <foo/func/stream.h>


namespace NFoo { namespace N3D {

	const int8 aDeviceClassTag[] = "3D Device";

	enum
	{
		EMaxTextureUnits=8,
		
		// tests
		ETest_Never=0,
		ETest_Less,
		ETest_LessEqual,
		ETest_Greater,
		ETest_GreaterEqual,
		ETest_Equal,
		ETest_NotEqual,
		ETest_Always,

		//
		ECull_CW=0,
		ECull_CCW,

		//
		EStencilOp_Keep=0,
		EStencilOp_Zero,
		EStencilOp_Repace,
		EStencilOp_Increase,
		EStencilOp_Decrease,
		EStencilOp_Invert,

		// draw mode
		EDraw_Points=0,
		EDraw_Lines=1,
		EDraw_LineStrip=2,
		EDraw_Triangles=3,
		EDraw_TriangleStrip=4,
		EDraw_TriangleFan=5,
		EDraw_Quads=6,

		// blending
		EBlend_Zero=0,
		EBlend_One,

		EBlend_SrcColor,
		EBlend_InvSrcColor,
		EBlend_SrcAlpha,
		EBlend_InvSrcAlpha,

		EBlend_DstColor,
		EBlend_InvDstColor,
		EBlend_DstAlpha,
		EBlend_InvDstAlpha,

		// flags
		EFlag_Blend=0,
		EFlag_Cull,
		
		//EFlag_ColorWrite,

		EFlag_DepthTest,
		EFlag_DepthWrite,

		EFlag_StencilTest,
		EFlag_StencilWrite,

		EFlag_ScissorTest,

		ENumFlags,

		// formats
		EFormat_int8=0,
		EFormat_int16,
		EFormat_int32,
		EFormat_fp32,
		EFormat_fp64,

		//
		EBuffer_Color=1,
		EBuffer_Depth=2,
		EBuffer_Stencil=4,

		//
		/*
		ETexFormat_RGB24=0,
		ETexFormat_RGBA32=1,
		ETexFormat_Depth=2,
		ETexFormat_BW=3,
		ENumTexFormats,

		ETexFormatMask=0xff,
		ETexFlagShift=8,
		*/
		ETexFlag_NoMipMap=1,

		//

		// vertex buffers
		EVB_Vertex=0x01,
		EVB_Normal=0x02,
		EVB_Color=0x04,
		
		EVB_TextureBase=0x10,
		EVB_Texture0=0x10,
		EVB_Texture1=0x20,
		EVB_Texture2=0x40,
		EVB_Texture3=0x80,
		EVB_Texture4=0x100,
		EVB_Texture5=0x200,

		EVB_TextureMask = (EVB_Texture0 | EVB_Texture1 | EVB_Texture2 | EVB_Texture3 | EVB_Texture4 | EVB_Texture5),
		EVB_All = (EVB_Vertex | EVB_Color | EVB_Normal | EVB_TextureMask)
	};

	//
	//
	//
	class CMode
	{
	public:
		enum
		{
			EType_Unspecified = -1,
			EType_Enable = 1,
			EType_Specified = 2,

			// Adapter default is 0 (primary display)
			EAdapter_Default = 0,
			
			EWidth=0,
			EHeight,
			ERefreshRate,
			EFullscreen,

			EColorBits,
			EStencilBits,
			EDepthBits,
			EAlphaBits,
			EAdapter,

			ENumParams
		};

		struct SParameter
		{
			int32 m_Value;
			int32 m_Type;
			uint32 m_DeviceParams[4];
			void Set(int32 _Type, int32 _Value=0, uint32 _DeviceParam0 = 0, 
				uint32 _DeviceParam1 = 0, uint32 _DeviceParam2 = 0, uint32 _DeviceParam3 = 0) 
			{ 
				m_Type = _Type; 
				m_Value = _Value; 
				m_DeviceParams[0] = _DeviceParam0; 
				m_DeviceParams[1] = _DeviceParam1; 
				m_DeviceParams[2] = _DeviceParam2; 
				m_DeviceParams[3] = _DeviceParam3;
			}
			bool Accaptable(SParameter _Param) const;
		};

		SParameter m_aParams[ENumParams];

		CMode();
		CMode(int32 _Width, int32 _Height, bool _Fullscreen, int32 _Flags);

		void Clear(); // Sets all to unspecified
		void Enable(int32 _Param) { m_aParams[_Param].Set(EType_Enable); }
		void Specify(int32 _Param, int32 _Value=0) { m_aParams[_Param].Set(EType_Specified, _Value); }
		void Set(int32 _Param, int32 _Type, int32 _Value=0, uint32 _DeviceParam0 = 0, 
			uint32 _DeviceParam1 = 0, uint32 _DeviceParam2 = 0, uint32 _DeviceParam3 = 0) 
		{ m_aParams[_Param].Set(_Type, _Value, _DeviceParam0, _DeviceParam1, _DeviceParam2, _DeviceParam3); }
		int32 Get(int32 _Param) { return m_aParams[_Param].m_Value; }

		bool Compare(CMode _Mode) const; // returns true if they are the same
		bool Acceptable(CMode _Mode) const; // compares this mode to the other. if mode fits this one this will return true
	};

	//
	//
	//
	class CState
	{
	protected:
		// flags
		uint8 m_aFlags[ENumFlags];

		// modes
		uint8 m_BlendSrc:4;
		uint8 m_BlendDst:4;
		uint8 m_DepthTest:4;
		uint8 m_Cull:2;
		bool m_WriteMask[4];
		
		// stencil
		uint8 m_StencilOpFail:4;
		uint8 m_StencilOpZFail:4;
		uint8 m_StencilOpZPass:4;
		uint8 m_StencilTest:4;
		uint8 m_StencilRef;
		uint8 m_StencilMask;

		TVector4<fp32> m_PrimaryColor;

		// streams
		int32 m_iVertexStream;
		int32 m_iIndexStream;
		int32 m_iNormalStream;
		int32 m_iColorStream;
		int32 m_aiTextureStreamVB[EMaxTextureUnits];
		int32 m_aiTextureStreamVBIndex[EMaxTextureUnits];

		// shaders
		int32 m_iVertexShader;
		int32 m_iPixelShader;
		
		// textures
		int32 m_iaTexture[EMaxTextureUnits];

		// matrices
		TMatrix4<fp32> m_ModelMatrix;
		TMatrix4<fp32> m_ProjectionMatrix;

	public:
		// flags
		virtual void Flag_Set(int32 _Flag, bool _On)		{ m_aFlags[_Flag] = _On; }
		
		// modes
		virtual void Mode_BlendFunc(intr _Src, intr _Dst)	{ m_BlendSrc = _Src; m_BlendDst = _Dst; }
		virtual void Mode_DepthTest(intr _Test)				{ m_DepthTest = _Test; }
		virtual void Mode_Cull(intr _CullMode)				{ m_Cull = _CullMode; }
		virtual void Mode_StencilFunc(intr t, intr r, intr m){ m_StencilTest = t; m_StencilRef = r; m_StencilMask = m;}
		virtual void Mode_StencilOp(intr sf, intr f, intr p){ m_StencilOpFail = sf; m_StencilOpZFail = f; m_StencilOpZPass = p; }
		virtual void Mode_PrimaryColor(fp32 r, fp32 g, fp32 b, fp32 a) { m_PrimaryColor.Set(r,g,b,a); }
		virtual void Mode_WriteMask(bool r, bool g, bool b, bool a) {	m_WriteMask[0] = r; m_WriteMask[1] = g;
																		m_WriteMask[2] = b; m_WriteMask[3] = a;}
		
		// streams/pointers
		void Stream_Clear();
		void Stream_Set(int32 _iVB, int32 _What=EVB_All);
		virtual void Stream_Vertex(int32 _iVB)				{ m_iVertexStream = _iVB; }
		virtual void Stream_Index(int32 _iIB)				{ m_iIndexStream = _iIB; }
		virtual void Stream_Normal(int32 _iVB)				{ m_iNormalStream = _iVB; }
		virtual void Stream_Color(int32 _iVB)				{ m_iColorStream = _iVB; };
		virtual void Stream_Texture(int32 _iUnit, int32 _iVB, int32 _Index)	{ m_aiTextureStreamVB[_iUnit] = _iVB;
																				m_aiTextureStreamVBIndex[_iUnit] = _Index; }

		// shaders
		virtual void Shader_Vertex(int32 _iShader)	{ m_iVertexShader = _iShader; }
		virtual void Shader_Pixel(int32 _iShader)	{ m_iPixelShader = _iShader; }
		
		// Maybe these should be moved to the device? Or keep system copy of variables and set with the rest of the state?
		virtual void Shader_GlobalParam() {}; // ?
		// _Num == Number of boolean values in the array of constants
		virtual void Shader_VertexParamBool(int32 _iStartReg, const bool* _pBools, int32 _Num) {}; // ?
		// _Num == Number of four float vectors in the array of constants
		virtual void Shader_VertexParamFloat(int32 _iStartReg, const TVector4<fp32>* _pFloats, int32 _Num){}; //?
		// _Num == Number of four integer vectors in the array of constants
		virtual void Shader_VertexParamInt(int32 _iStartReg, const TVector4<int32>* _pInts, int32 _Num) {}; // ?

		// _Num == Number of boolean values in the array of constants
		virtual void Shader_PixelParamBool(int32 _iStartReg, const bool* _pBools, int32 _Num) {}; // ?
		// _Num == Number of four float vectors in the array of constants
		virtual void Shader_PixelParamFloat(int32 _iStartReg, const TVector4<fp32>* _pFloats, int32 _Num){}; //?
		// _Num == Number of four integer vectors in the array of constants
		virtual void Shader_PixelParamInt(int32 _iStartReg, const TVector4<int32>* _pInts, int32 _Num) {}; // ?
		virtual void Shader_PixelParam() {}; // ?

		// textures
		virtual void Texture_Set(intr _iUnit, int32 _iTexture) { m_iaTexture[_iUnit] = _iTexture; }; // -1 disable
		
		// matrices
		virtual void Matrix_Model(TMatrix4<fp32> _Matrix)		{ m_ModelMatrix = _Matrix; }
		virtual void Matrix_Projection(TMatrix4<fp32> _Matrix)	{ m_ProjectionMatrix = _Matrix; }
		
		// state
		virtual void State_Set(const CState &_rState, bool _Force=false);
		virtual void State_SetDefaults(bool _Force=false);
	};

	//
	//
	//
	class CTextureInfo
	{
	public:
		int32 m_iIndex;
		TVector2<int32> m_Size;
		int32 m_Format;
	};

	//
	// Never touch these pointers, the render device can use then internaly.
	// Also, no touchie on the m_Count
	//
	class CVertexBuffer
	{
	public:
		virtual ~CVertexBuffer() {};
		int32 m_Count;

		int32 What();

		union
		{
			TVector3<int16> *m_pVertexint16;
			TVector3<int32> *m_pVertexint32;
			TVector3<fp32> *m_pVertexfp32;
			TVector3<fp64> *m_pVertexfp64;
		};

		union
		{
			TVector3<int8> *m_pNormalint8;
			TVector3<int16> *m_pNormalint16;
			TVector3<int32> *m_pNormalint32;
			TVector3<fp32> *m_pNormalfp32;
			TVector3<fp64> *m_pNormalfp64;
		};

		union
		{
			TVector4<int8> *m_pColorint8;
			TVector4<int16> *m_pColorint16;
			TVector4<int32> *m_pColorint32;
			TVector4<fp32> *m_pColorfp32;
			TVector4<fp64> *m_pColorfp64;
		};

		union
		{
			TVector2<int8> *m_pTextureint8[EMaxTextureUnits];
			TVector2<int16> *m_pTextureint16[EMaxTextureUnits];
			TVector2<int32> *m_pTextureint32[EMaxTextureUnits];
			TVector2<fp32> *m_pTexturefp32[EMaxTextureUnits];
			TVector2<fp64> *m_pTexturefp64[EMaxTextureUnits];
		};


		void *GetPointer(int32 i);
		int32 GetPointerElementSize(int32 i);
		int32 GetNumPointers() { return 11; }
	};

	//
	// Never touch these pointers, the render device can use then internaly.
	// Also, no touchie on the m_Count
	//
	class CIndexBuffer
	{
	public:
		virtual ~CIndexBuffer() {};
		int32 m_Count;

		union
		{
			uint16 *m_pIndex16;
			uint32 *m_pIndex32;
		};
	};

	//
	//
	//
	class CIndexHandler
	{
		friend class CDevice;
		struct SID
		{
			int32 m_Type;
			void *m_pData;
		};

		TDynamicArray<SID> m_lIDNumbers;
		TDynamicArray<int32> m_lFreeNumbers; // index in m_lIDNumbers
	public:
		int32 GetFree(int32 _Type);
		void Remove(int32 _ID);

		void SetData(int32 _Index, void *_pData) { m_lIDNumbers[_Index].m_pData = _pData; }
		template<class T> T GetObject(int32 _Index, int32 _Type)
		{
			if(_Index < 0 || _Index >= m_lIDNumbers.Len())
				return NULL;
			if(m_lIDNumbers[_Index].m_Type != _Type)
				return NULL;

			return (T)m_lIDNumbers[_Index].m_pData;
		}

		int32 Len() { return m_lIDNumbers.Len(); }
		void* GetSIDData(int32 _Index) { return (m_lIDNumbers.Len() > _Index ? &m_lIDNumbers[_Index].m_pData : NULL); }
	};

	//
	//
	//
	class CDevice : public CState, public NCore::CSubCore 
	{
	protected:
		uint32 m_CapabilityFlags;

		// shader versions
		TDynamicArray<CString> m_lShaderVersion;

		// clear attributes
		fp32 m_ClearDepth;
		TVector4<fp32> m_ClearColor;
		uint8 m_ClearStencil;
		
		// ID Handling
		enum
		{
			EID_Texture, EID_VertexShader, EID_PixelShader, EID_VertexBuffer, EID_IndexBuffer
		};

		CIndexHandler m_IndexHandler;

		void ClearAllAllocatedIndexes();
	public:
		// Capability flags
		enum
		{
			ECap_MultiTexture=1,
			ECap_RenderToTexture=2,
			ECap_VertexShaders=4,
			ECap_PixelShaders=8
		};

		virtual ~CDevice() { }

		// before init operations
		bool GetBestMode(CMode _Mode, CMode &_rBestMode);	// returns the displaymode that is the best, returns true if found one
		int32 CheckMode(CMode _Mode);						// returns how many diffrent modes that matches
		virtual int32 EnumDisplayModes(TDynamicArray<CMode> &_rModes, CMode _Mode) pure;

		// main operation
		virtual bool Init(CMode _Mode) pure;	// 
		virtual bool SwapBuffers() pure;		//
		virtual bool Update() pure;				// Called from the core
		virtual bool Shutdown() pure;			//

		//
		uint32 GetCapabilities() { return m_CapabilityFlags; };
		virtual uint32 GetVertexShaderCapabilities() { return 0; };
		virtual uint32 GetPixelShaderCapabilities() { return 0; };
		virtual void GetShaderBaseDir(CString& _DirName) { return; };

		// texture handling
		virtual int32 CreateTexture(CImage::CFormat Format, int32 Flags=0, void *pData=NULL) pure;
		virtual int32 CreateTexture(CImage &_Image, int32 Flags=0) pure;
		virtual bool DestroyTexture(int32 _iTexture) pure;
		virtual CTextureInfo GetTextureInfo(int32 _iTexture) pure;

		// Create/destroy vertex buffer
		virtual int32 CreateVertexBuffer(int32 _Count, int32 _What, int32 _Flags) pure;
		virtual bool DestroyVertexBuffer(int32 _iBuffer) pure; 
		// Get vertexbuffer from id, Return control of vertexbuffer to device
		virtual CVertexBuffer* LockVertexBuffer(int32 _iBuffer, int32 _What = EVB_All /*, int32 _Start = 0, int32 _Count = 0*/) pure;
		virtual void UnlockVertexBuffer(int32 _iBuffer, int32 _What = EVB_All) pure;

		virtual int32 CreateIndexBuffer(int32 _Count, int32 _Flags) pure;
		virtual bool DestroyIndexBuffer(int32 _iBuffer) pure; 
		// Get indexbuffer from id, Return control of indexbuffer to device
		virtual CIndexBuffer* LockIndexBuffer(int32 _iBuffer) pure;
		virtual void UnlockIndexBuffer(int32 _iBuffer) pure;

		// shaders
		virtual int32 CreateVertexShader(const int8 *_pString) pure;
		virtual int32 CreatePixelShader(const int8 *_pString) pure;
		virtual bool DestroyVertexShader(int32 _iShader) pure;
		virtual bool DestroyPixelShader(int32 _iShader) pure;

		// clearing
		void Clear_Color(fp32 _r=0, fp32 _g=0, fp32 _b=0, fp32 _a=0) { m_ClearColor = TVector4<fp32>(_r,_g,_b,_a); }
		void Clear_Stencil(uint8 _Value) { m_ClearStencil = _Value; }
		void Clear_Depth(fp32 _Depth=1) { m_ClearDepth = _Depth; }
		virtual void Clear(intr _What) pure;
		
		// draw operation
		virtual void Draw(int32 _Mode, int32 _Start, int32 _PrimCount) pure;
		virtual void DrawIndexed(int32 _Mode, int32 _Start, int32 _PrimCount) pure;
		//virtual void Draw(CIndexBuffer *_pBuffer, int32 _Mode, int32 _Count, int32 _Offset=0) pure;

		// grab
		virtual bool Grab(int32 _iTexture, int32 _What) pure;
		virtual bool Grab(NFoo::CImage &_rImage, int32 _What=EBuffer_Color) pure;
		
		// gamma
		virtual bool SetGammaRamp(uint16 *_pRamp) { return false; }

		//
		virtual void SetViewport(int32 _x, int32 _y, int32 _w, int32 _h) pure;
		virtual void SetScissors(int32 _x, int32 _y, int32 _w, int32 _h) pure;

		// 
		//
		CString LoadShader(CInputStream &_rStream);

		// creates a new vertexbuffer and copies the content to it
		int32 CreateVertexBuffer(CVertexBuffer *_pVB, int32 _Count=-1, int32 _Mask=EVB_All);
		int32 CreateIndexBuffer(CIndexBuffer *_pIB, int32 _Count=-1);

		bool m_Active;
	};

	// creates software buffers
	CVertexBuffer *CreateVertexBufferSW(int32 _Count, int32 _What, int32 _Flags);
	CIndexBuffer *CreateIndexBufferSW(int32 _Count, int32 _Flags);

	// Tools
	namespace NMatrix
	{
		TMatrix4<fp32> Unit();
		TMatrix4<fp32> Scale(fp32 x, fp32 y, fp32 z);
		TMatrix4<fp32> Translation(fp32 x, fp32 y, fp32 z);
		TMatrix4<fp32> Rotation(fp32 x, fp32 y, fp32 z);
		TMatrix4<fp32> Ortho(fp32 _Left, fp32 _Right, fp32 _Top, fp32 _Bottom, fp32 _Far, fp32 _Near);
		TMatrix4<fp32> Projection(fp32 _Near = 1, fp32 _Far = 20, fp32 _Right = 1.3333, fp32 _Left = -1.333, fp32 _Top = 1.0, fp32 _Bottom = -1.0);
		TMatrix4<fp32> LookAt(TVector3<fp32> _At, TVector3<fp32> _Eye, TVector3<fp32> _Up);
	}
} } // end name spaces

#endif // __FOO_3D_H__


