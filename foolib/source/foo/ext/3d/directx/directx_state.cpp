
#include <base/base.h>
#include <base/log.h>
#include <func/string.h>

#include "directx.h"

using namespace NFoo;
using namespace NFoo::N3D;
using namespace NFoo::N3D::NDirectX;

const intr g_aBlendTable[] = 
{
	D3DBLEND_ZERO,
	D3DBLEND_ONE,
	D3DBLEND_SRCCOLOR,
	D3DBLEND_INVSRCCOLOR,
	D3DBLEND_SRCALPHA,
	D3DBLEND_INVSRCALPHA,
	D3DBLEND_DESTCOLOR,
	D3DBLEND_INVDESTCOLOR,
	D3DBLEND_DESTALPHA,
	D3DBLEND_INVDESTALPHA
};

const intr g_aTestTable[] =
{
	D3DCMP_NEVER,
	D3DCMP_LESS,
	D3DCMP_LESSEQUAL,
	D3DCMP_GREATER,
	D3DCMP_GREATEREQUAL,
	D3DCMP_EQUAL,
	D3DCMP_NOTEQUAL,
	D3DCMP_ALWAYS
};

const intr g_aCullTable[] =
{
	D3DCULL_CW,
	D3DCULL_CCW
};

const intr g_aStencilOpTable[] =
{
	D3DSTENCILOP_KEEP,
	D3DSTENCILOP_ZERO,
	D3DSTENCILOP_REPLACE,
	D3DSTENCILOP_INCR,
	D3DSTENCILOP_DECR,
	D3DSTENCILOP_INVERT
};

void CDirectXDevice::Flag_Set(int32_t _Flag, bool _On)
{
	if (m_p3DDevice)
	{
		switch(_Flag)
		{
		case EFlag_Blend:
			{
				MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,_On),"Error: CDirectXDevice::Flag_Set\n");
				break;
			}
		case EFlag_Cull:
			{
				MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_CULLMODE,(_On ? g_aCullTable[m_Cull] : D3DCULL_NONE)),"Error: CDirectXDevice::Flag_Set\n");
				break;
			}
		case EFlag_ColorWrite:
			{
				int32_t Mask = (_On ? (D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED): 0);
				MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE,Mask),"Error: CDirectXDevice::Flag_Set\n");
				break;
			}
		case EFlag_DepthTest:
			{
				MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_ZENABLE,_On),"Error: CDirectXDevice::Flag_Set\n");
				break;
			}
		case EFlag_DepthWrite:
			{
				MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,_On),"Error: CDirectXDevice::Flag_Set\n");
				break;
			}
		case EFlag_StencilTest:
			{
				MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_STENCILENABLE,_On),"Error: CDirectXDevice::Flag_Set\n");
				break;
			}
		case EFlag_StencilWrite:
			{
				MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_STENCILWRITEMASK,(_On ? 0xFFFFFFFF : 0)),"Error: CDirectXDevice::Flag_Set\n");
				break;
			}
		};
	}

	CState::Flag_Set(_Flag, _On);
}

// modes
void CDirectXDevice::Mode_BlendFunc(intr _Src, intr _Dst)
{
	if (m_p3DDevice)
	{
		MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_SRCBLEND,g_aBlendTable[_Src]),"Error: CDirectXDevice::Mode_BlendFunc\n");
		MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_DESTBLEND,g_aBlendTable[_Dst]),"Error: CDirectXDevice::Mode_BlendFunc\n");
	}
	CState::Mode_BlendFunc(_Src, _Dst);
}

void CDirectXDevice::Mode_DepthTest(intr _Test)
{
	if (m_p3DDevice)
	{
		MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_ZFUNC,g_aTestTable[_Test]),"Error: CDirectXDevice::Mode_DepthTest\n");
	}
	CState::Mode_DepthTest(_Test);
}

void CDirectXDevice::Mode_Cull(intr _CullMode)
{
	if (m_p3DDevice)
	{
		if (m_aFlags[EFlag_Cull])
		{
			MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_CULLMODE,g_aCullTable[_CullMode]),"Error: CDirectXDevice::Mode_Cull\n");
		}
		else
		{
			MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE),"Error: CDirectXDevice::Mode_Cull\n");
		}
	}
	CState::Mode_Cull(_CullMode);
}

void CDirectXDevice::Mode_StencilFunc(intr _Test, intr _Ref, intr _Mask)
{
	if (m_p3DDevice)
	{
		MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_STENCILFUNC,g_aTestTable[_Test]),"Error: CDirectXDevice::Mode_StencilFunc\n");
		MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_STENCILREF,_Ref),"Error: CDirectXDevice::Mode_StencilFunc\n");
		MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_STENCILMASK,_Mask),"Error: CDirectXDevice::Mode_StencilFunc\n");
	}
	CState::Mode_StencilFunc(_Test, _Ref, _Mask);
}

void CDirectXDevice::Mode_StencilOp(intr _StencilFail, intr _ZFail, intr _Pass)
{
	if (m_p3DDevice)
	{
		MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_STENCILFAIL,g_aStencilOpTable[_StencilFail]),"Error: CDirectXDevice::Mode_StencilOp\n");
		MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_STENCILZFAIL,g_aStencilOpTable[_ZFail]),"Error: CDirectXDevice::Mode_StencilOp\n");
		MACRO_DXCHECKERROR(m_p3DDevice->SetRenderState(D3DRS_STENCILPASS,g_aStencilOpTable[_Pass]),"Error: CDirectXDevice::Mode_StencilOp\n");
	}
	CState::Mode_StencilOp(_StencilFail, _ZFail, _Pass);
}
// Vertexbuffer operations, hmm, do we need to accumulate declarations along the way...
void CDirectXDevice::Stream_Vertex(int32_t _iVB)
{
	// Set vertexbuffer
	if (_iVB != -1)
	{
		CDirectXVertexBuffer *pVertexBuffer = m_IndexHandler.GetObject<CDirectXVertexBuffer*>(_iVB, EID_VertexBuffer);
		if (m_p3DDevice && pVertexBuffer && pVertexBuffer->m_pD3DPositionBuffer)
		{
			int32_t ElementSize = GetBufferElementSize(EFormat_fp32) * ED3D_ELEMENTSPERPOSITION;
			MACRO_DXCHECKERROR(m_p3DDevice->SetStreamSource(ED3D_STREAMSOURCEINDEX_POSITION,pVertexBuffer->m_pD3DPositionBuffer,0,ElementSize),"Error: CDirectXDevice::Stream_Position\n");
			m_StreamFlags |= ED3D_STREAMFLAG_POSITION;
		}
	}
	else
	{
		if (m_p3DDevice)
		{
			MACRO_DXCHECKERROR(m_p3DDevice->SetStreamSource(ED3D_STREAMSOURCEINDEX_POSITION,NULL,0,0),"Error: CDirectXDevice::Stream_Position\n");
			m_StreamFlags &= ~ED3D_STREAMFLAG_POSITION;
		}
	}

	CState::Stream_Vertex(_iVB);
}

void CDirectXDevice::Stream_Index(int32_t _iIB)
{
	// Set indexbuffer
	if (_iIB != -1)
	{
		CDirectXIndexBuffer *pIndexBuffer = m_IndexHandler.GetObject<CDirectXIndexBuffer*>(_iIB, EID_IndexBuffer);
		if (m_p3DDevice && pIndexBuffer && pIndexBuffer->m_pD3DIndexBuffer)
		{
			MACRO_DXCHECKERROR(m_p3DDevice->SetIndices(pIndexBuffer->m_pD3DIndexBuffer),"Error: CDirectXDevice::Stream_Index\n");
		}
	}
	else
	{
		if (m_p3DDevice)
		{
			MACRO_DXCHECKERROR(m_p3DDevice->SetIndices(NULL),"Error: CDirectXDevice::Stream_Index\n");
		}
	}

	CState::Stream_Index(_iIB);
}

void CDirectXDevice::Stream_Normal(int32_t _iVB)
{
	// Set normalbuffer
	if (_iVB != -1)
	{
		CDirectXVertexBuffer *pVertexBuffer = m_IndexHandler.GetObject<CDirectXVertexBuffer*>(_iVB, EID_VertexBuffer);
		if (m_p3DDevice && pVertexBuffer && pVertexBuffer->m_pD3DNormalBuffer)
		{
			int32_t ElementSize = GetBufferElementSize(EFormat_fp32) * ED3D_ELEMENTSPERNORMAL;
			MACRO_DXCHECKERROR(m_p3DDevice->SetStreamSource(ED3D_STREAMSOURCEINDEX_NORMAL,pVertexBuffer->m_pD3DNormalBuffer,0,ElementSize),"Error: CDirectXDevice::Stream_Normal\n");
			m_StreamFlags |= ED3D_STREAMFLAG_NORMAL;
		}
	}
	else
	{
		if (m_p3DDevice)
		{
			MACRO_DXCHECKERROR(m_p3DDevice->SetStreamSource(ED3D_STREAMSOURCEINDEX_NORMAL,NULL,0,0),"Error: CDirectXDevice::Stream_Normal\n");
			m_StreamFlags &= ~ED3D_STREAMFLAG_NORMAL;
		}
	}

	CState::Stream_Normal(_iVB);
}

void CDirectXDevice::Stream_Color(int32_t _iVB)
{
	// Set color buffer
	if (_iVB != -1)
	{
		CDirectXVertexBuffer *pVertexBuffer = m_IndexHandler.GetObject<CDirectXVertexBuffer*>(_iVB, EID_VertexBuffer);
		if (m_p3DDevice && pVertexBuffer && pVertexBuffer->m_pD3DColorBuffer)
		{
			int32_t ElementSize = GetBufferElementSize(EFormat_int32) * ED3D_ELEMENTSPERCOLOR;
			MACRO_DXCHECKERROR(m_p3DDevice->SetStreamSource(ED3D_STREAMSOURCEINDEX_COLOR,pVertexBuffer->m_pD3DColorBuffer,0,ElementSize),"Error: CDirectXDevice::Stream_Color\n");
			m_StreamFlags |= ED3D_STREAMFLAG_COLOR;
		}
	}
	else
	{
		if (m_p3DDevice)
		{
			MACRO_DXCHECKERROR(m_p3DDevice->SetStreamSource(ED3D_STREAMSOURCEINDEX_COLOR,NULL,0,0),"Error: CDirectXDevice::Stream_Color\n");
			m_StreamFlags &= ~ED3D_STREAMFLAG_COLOR;
		}
	}

	CState::Stream_Color(_iVB);
}

void CDirectXDevice::Stream_Texture(int32_t _iTex, int32_t _iVB)
{
	// Set texture coordinate index
	if (_iVB != -1)
	{
		CDirectXVertexBuffer *pVertexBuffer = m_IndexHandler.GetObject<CDirectXVertexBuffer*>(_iVB, EID_VertexBuffer);
		if (m_p3DDevice && pVertexBuffer && pVertexBuffer->m_lpD3DTexCoordbuffer[_iTex])
		{
			int32_t ElementSize = GetBufferElementSize(EFormat_fp32) * ED3D_ELEMENTSPERTEXCOORD;
			MACRO_DXCHECKERROR(m_p3DDevice->SetStreamSource(ED3D_STREAMSOURCEINDEX_TEXCOORD_BASE+_iTex,pVertexBuffer->m_lpD3DTexCoordbuffer[_iTex],0,ElementSize),"Error: CDirectXDevice::Stream_Texture\n");
			m_StreamFlags |= (ED3D_STREAMFLAG_TEXCOORD_BASE << _iTex);
		}
	}
	else
	{
		if (m_p3DDevice)
		{
			MACRO_DXCHECKERROR(m_p3DDevice->SetStreamSource(ED3D_STREAMSOURCEINDEX_TEXCOORD_BASE+_iTex,NULL,0,0),"Error: CDirectXDevice::Stream_Texture\n");
			m_StreamFlags &= ~(ED3D_STREAMFLAG_TEXCOORD_BASE << _iTex);
		}
	}

	CState::Stream_Texture(_iTex, _iVB);
}

// shaders
void CDirectXDevice::Shader_Vertex(int32_t _iShader)
{
	if (m_p3DDevice)
	{
		if(_iShader == -1)
		{
			MACRO_DXCHECKERROR(m_p3DDevice->SetVertexShader(NULL),"Error: CDirectXDevice::Shader_Vertex\n");
		}
		else
		{
			CDirectXVertexShader *pShader = m_IndexHandler.GetObject<CDirectXVertexShader*>(_iShader, EID_VertexShader);
			if(pShader)
			{
				MACRO_DXCHECKERROR(m_p3DDevice->SetVertexShader(pShader->m_pVertexShader),"Error: CDirectXDevice::Shader_Vertex\n");
			}
			else
			{
				MACRO_DXCHECKERROR(m_p3DDevice->SetVertexShader(NULL),"Error: CDirectXDevice::Shader_Vertex\n");
			}
		}
	}

	CState::Shader_Vertex(_iShader);
}

void CDirectXDevice::Shader_Pixel(int32_t _iShader)
{
	if (m_p3DDevice)
	{
		if(_iShader == -1)
		{
			MACRO_DXCHECKERROR(m_p3DDevice->SetPixelShader(NULL),"Error: CDirectXDevice::Shader_Pixel\n");
		}
		else
		{
			CDirectXPixelShader *pShader = m_IndexHandler.GetObject<CDirectXPixelShader*>(_iShader, EID_PixelShader);
			if(pShader)
			{
				MACRO_DXCHECKERROR(m_p3DDevice->SetPixelShader(pShader->m_pPixelShader),"Error: CDirectXDevice::Shader_Pixel\n");
			}
			else
			{
				MACRO_DXCHECKERROR(m_p3DDevice->SetPixelShader(NULL),"Error: CDirectXDevice::Shader_Pixel\n");
			}
		}
	}

	CState::Shader_Pixel(_iShader);
}

void CDirectXDevice::Shader_VertexParamBool(int32_t _iStartReg, const bool* _pBools, int32_t _Num)
{
	if (m_p3DDevice)
		MACRO_DXCHECKERROR(m_p3DDevice->SetVertexShaderConstantB(_iStartReg,(const BOOL*)_pBools,_Num),"Error: CDirectXDevice::Shader_VertexParamBool\n");
	CState::Shader_VertexParamBool(_iStartReg, _pBools, _Num);
}

void CDirectXDevice::Shader_VertexParamFloat(int32_t _iStartReg, const TVector4<fp32>* _pFloats, int32_t _Num)
{
	if (m_p3DDevice)
		MACRO_DXCHECKERROR(m_p3DDevice->SetVertexShaderConstantF(_iStartReg,(const float*)_pFloats,_Num),"Error: CDirectXDevice::Shader_VertexParamFloat\n");
	CState::Shader_VertexParamFloat(_iStartReg, _pFloats, _Num);
}

void CDirectXDevice::Shader_VertexParamInt(int32_t _iStartReg, const TVector4<int32_t>* _pInts, int32_t _Num)
{
	if (m_p3DDevice)
		MACRO_DXCHECKERROR(m_p3DDevice->SetVertexShaderConstantI(_iStartReg,(const int*)_pInts,_Num),"Error: CDirectXDevice::Shader_VertexParamInt\n");
	CState::Shader_VertexParamInt(_iStartReg, _pInts, _Num);
}

// _Num == Number of boolean values in the array of constants
void CDirectXDevice::Shader_PixelParamBool(int32_t _iStartReg, const bool* _pBools, int32_t _Num)
{
	if (m_p3DDevice)
		MACRO_DXCHECKERROR(m_p3DDevice->SetPixelShaderConstantB(_iStartReg,(const BOOL*)_pBools,_Num),"Error: CDirectXDevice::Shader_PixelParamBool\n");
	CState::Shader_PixelParamBool(_iStartReg, _pBools, _Num);
}
// _Num == Number of four float vectors in the array of constants
void CDirectXDevice::Shader_PixelParamFloat(int32_t _iStartReg, const TVector4<fp32>* _pFloats, int32_t _Num)
{
	if (m_p3DDevice)
		MACRO_DXCHECKERROR(m_p3DDevice->SetPixelShaderConstantF(_iStartReg,(const float*)_pFloats,_Num),"Error: CDirectXDevice::Shader_PixelParamFloat\n");
	CState::Shader_PixelParamFloat(_iStartReg, _pFloats, _Num);
}

// _Num == Number of four integer vectors in the array of constants
void CDirectXDevice::Shader_PixelParamInt(int32_t _iStartReg, const TVector4<int32_t>* _pInts, int32_t _Num)
{
	if (m_p3DDevice)
		MACRO_DXCHECKERROR(m_p3DDevice->SetPixelShaderConstantI(_iStartReg,(const int*)_pInts,_Num),"Error: CDirectXDevice::Shader_PixelParamInt\n");
	CState::Shader_PixelParamInt(_iStartReg, _pInts, _Num);
}

// textures
// -1 disable
void CDirectXDevice::Texture_Set(intr _iUnit, int32_t _iTexture)
{
	if (m_p3DDevice)
	{
		CDirectXTexture *pTexture = m_IndexHandler.GetObject<CDirectXTexture*>(_iTexture, EID_Texture);

		if (pTexture)
		{
			MACRO_DXCHECKERROR(m_p3DDevice->SetTexture(_iUnit,pTexture->m_pTexture),"Error: CDirectXDevice::Texture_Set\n");
		}
	}

	CState::Texture_Set(_iUnit, _iTexture);
}

// matrices
void CDirectXDevice::Matrix_Model(TMatrix4<fp32> _Matrix)
{
	if (m_p3DDevice)
	{
		MACRO_DXCHECKERROR(m_p3DDevice->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&_Matrix),"Error: CDirectXDevice::Matrix_Model\n");
	}
	CState::Matrix_Model(_Matrix);
}

void CDirectXDevice::Matrix_Projection(TMatrix4<fp32> _Matrix)
{
	if (m_p3DDevice)
	{
		MACRO_DXCHECKERROR(m_p3DDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&_Matrix),"Error: CDirectXDevice::Matrix_Projection\n");
	}
	CState::Matrix_Projection(_Matrix);
}
