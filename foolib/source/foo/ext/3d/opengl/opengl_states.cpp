#include <foo/ext/3d/3d.h>
#include <foo/base/log.h>
#include <foo/base/string.h>
#include "opengl.h"

#include "ext_arb_multitexture.h"
#include "ext_arb_vertex_program.h"

using namespace NFoo;
using namespace NFoo::N3D;
using namespace NFoo::N3D::NOpenGL;

//
// Convert tables
//
const intr g_aFormatTable[] = {GL_BYTE, GL_SHORT, GL_INT, GL_FLOAT, GL_DOUBLE};
const intr g_aTestTable[] = {GL_NEVER, GL_LESS, GL_LEQUAL, GL_GREATER, GL_GEQUAL, GL_EQUAL, GL_NOTEQUAL, GL_ALWAYS};
const intr g_aStencilOpTable[] = {GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_DECR, GL_INVERT};
const intr g_aCullTable[] = {GL_CW, GL_CCW};


const intr g_aBlendTable[] = {	GL_ZERO, GL_ONE,
								GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
								GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA};

//
// help functions
//
inline void SetOpenGLState(GLenum state, bool enable)
{
	if(enable) glEnable(state);
	else glDisable(state);
}

//
// flags
//
void COpenGLDevice::Flag_Set(int32_t _Flag, bool _On)
{
	switch(_Flag)
	{
	case EFlag_Blend: SetOpenGLState(GL_BLEND, _On); break;
	case EFlag_Cull: SetOpenGLState(GL_CULL_FACE, _On); break;

	//case EFlag_ColorWrite: glColorMask(_On,_On,_On,_On); break;
	
	case EFlag_DepthTest: SetOpenGLState(GL_DEPTH_TEST, _On); break;
	case EFlag_DepthWrite: glDepthMask(_On); break;

	case EFlag_StencilTest: SetOpenGLState(GL_STENCIL_TEST, _On); break;
	case EFlag_StencilWrite: glStencilMask((_On?1:0)*0xFF); break;

	case EFlag_ScissorTest: SetOpenGLState(GL_SCISSOR_TEST, _On); break;
	};

	CState::Flag_Set(_Flag, _On);
}

//
// modes
//
void COpenGLDevice::Mode_BlendFunc(intr _Src, intr _Dst)
{
	glBlendFunc(g_aBlendTable[_Src], g_aBlendTable[_Dst]);
	CState::Mode_BlendFunc(_Src, _Dst);
}

void COpenGLDevice::Mode_DepthTest(intr _Test)
{
	glDepthFunc(g_aTestTable[_Test]);
	CState::Mode_DepthTest(_Test);
}

void COpenGLDevice::Mode_Cull(intr _CullMode)
{
	glFrontFace(g_aCullTable[_CullMode]);
	CState::Mode_Cull(_CullMode);
}

void COpenGLDevice::Mode_StencilFunc(intr t, intr r, intr m)
{
	glStencilFunc(g_aTestTable[t], r, m);
	CState::Mode_StencilFunc(t, r, m);
}

void COpenGLDevice::Mode_StencilOp(intr sf, intr f, intr p)
{
	glStencilOp(g_aStencilOpTable[sf], g_aStencilOpTable[f], g_aStencilOpTable[p]);
	CState::Mode_StencilOp(sf, f, p);
}

void COpenGLDevice::Mode_PrimaryColor(fp32 r, fp32 g, fp32 b, fp32 a)
{
	glColor4f(r, g, b, a);
	CDevice::Mode_PrimaryColor(r, g, b, a);
}

void COpenGLDevice::Mode_WriteMask(bool r, bool g, bool b, bool a)
{
	glColorMask(r,g,b,a);
	CDevice::Mode_WriteMask(r, g, b, a);
}


//
// pointers
//
void COpenGLDevice::Stream_Vertex(int32_t _iVB)
{
	COpenGLVertexBuffer *pVB = m_IndexHandler.GetObject<COpenGLVertexBuffer*>(_iVB, EID_VertexBuffer);
	if(pVB && pVB->m_LockInfo.m_pVertexfp32)
	{
		if(m_Exts.ARB_vertex_buffer_object)
			pVB->ARB_VBO_Bind();

		glVertexPointer(3, /*g_aFormatTable[_Format]*/ GL_FLOAT, 0, pVB->m_LockInfo.m_pVertexfp32);
		glEnableClientState(GL_VERTEX_ARRAY);
	}
	else
		glDisableClientState(GL_VERTEX_ARRAY);

	CState::Stream_Vertex(_iVB);
}

void COpenGLDevice::Stream_Normal(int32_t _iVB)
{
	COpenGLVertexBuffer *pVB = m_IndexHandler.GetObject<COpenGLVertexBuffer*>(_iVB, EID_VertexBuffer);
	if(pVB && pVB->m_LockInfo.m_pNormalfp32)
	{
		if(m_Exts.ARB_vertex_buffer_object)
			pVB->ARB_VBO_Bind();
		glNormalPointer(/*g_aFormatTable[_Format]*/ GL_FLOAT, 0, pVB->m_LockInfo.m_pNormalfp32);
		glEnableClientState(GL_NORMAL_ARRAY);
	}
	else
		glDisableClientState(GL_NORMAL_ARRAY);

	CState::Stream_Normal(_iVB);
}

void COpenGLDevice::Stream_Color(int32_t _iVB)
{
	COpenGLVertexBuffer *pVB = m_IndexHandler.GetObject<COpenGLVertexBuffer*>(_iVB, EID_VertexBuffer);
	if(pVB && pVB->m_LockInfo.m_pColorfp32)
	{
		if(m_Exts.ARB_vertex_buffer_object)
			pVB->ARB_VBO_Bind();
		glColorPointer(4, /*g_aFormatTable[_Format]*/ GL_FLOAT, 0, pVB->m_LockInfo.m_pColorfp32);
		glEnableClientState(GL_COLOR_ARRAY);
	}
	else
		glDisableClientState(GL_COLOR_ARRAY);

	CState::Stream_Color(_iVB);
}

void COpenGLDevice::Stream_Texture(int32_t _iUnit, int32_t _iVB, int32_t _Index) // FIX ME! MULTITEXTURE
{
	if(m_Exts.ARB_multitexture)
	{
		glextActiveTextureARB(GL_TEXTURE0_ARB+_iUnit); // set texture unit
		glextClientActiveTextureARB(GL_TEXTURE0_ARB+_iUnit); // set texture unit
	}
	else if(_iUnit != 0)
		return;

	COpenGLVertexBuffer *pVB = m_IndexHandler.GetObject<COpenGLVertexBuffer*>(_iVB, EID_VertexBuffer);
	if(pVB && pVB->m_LockInfo.m_pTexturefp32[_Index])
	{
		if(m_Exts.ARB_vertex_buffer_object)
			pVB->ARB_VBO_Bind();
		glTexCoordPointer(2, /*g_aFormatTable[_Format]*/ GL_FLOAT, sizeof(fp32)*2, pVB->m_LockInfo.m_pTexturefp32[_Index]);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	CState::Stream_Texture(_iUnit, _iVB, _Index);
}


void COpenGLDevice::Stream_Index(int32_t _iIB)
{
	COpenGLIndexBuffer *pIB = m_IndexHandler.GetObject<COpenGLIndexBuffer*>(_iIB, EID_IndexBuffer);
	if(pIB && pIB->m_LockInfo.m_pIndex16)
		m_pIndexPointer = pIB->m_LockInfo.m_pIndex16;
	else
		m_pIndexPointer = NULL;

	CState::Stream_Index(_iIB);
}
//
// shaders
//
void COpenGLDevice::Shader_Vertex(int32_t _iShader)
{
	if(_iShader == -1)
		glDisable(GL_VERTEX_PROGRAM_ARB);
	else
	{
		COpenGLHandle *pHandle = m_IndexHandler.GetObject<COpenGLHandle*>(_iShader, EID_VertexShader);
		if(pHandle)
		{
			glextBindProgramARB(GL_VERTEX_PROGRAM_ARB, pHandle->m_iHandle);
			glEnable(GL_VERTEX_PROGRAM_ARB);
		}
		else
			glDisable(GL_VERTEX_PROGRAM_ARB);
	}

	CState::Shader_Vertex(_iShader);
}

void COpenGLDevice::Shader_Pixel(int32_t _iShader) // FIX ME!
{
	// SHADER FIX
	CState::Shader_Pixel(_iShader);
}

void COpenGLDevice::Shader_VertexParamFloat(int32_t _iStartReg, const TVector4<fp32>* _pFloats, int32_t _Num)
{
	for(int32_t i = 0; i < _Num; i++)
		glextProgramEnvParameter4fvARB(GL_VERTEX_PROGRAM_ARB, _iStartReg+i, (fp32*)(_pFloats+i));

	CState::Shader_VertexParamFloat(_iStartReg, _pFloats, _Num);
}

//
// textures
//
void COpenGLDevice::Texture_Set(intr _iUnit, int32_t _iTexture) // FIX ME!
{
	if(!m_Exts.ARB_multitexture && _iUnit != 0)
		return;

	if(_iTexture < 0)
	{
		if(m_Exts.ARB_multitexture)
			glextActiveTextureARB(GL_TEXTURE0_ARB+_iUnit);
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		// MULTITEXTURE FIX
		COpenGLTexture *pTexture = m_IndexHandler.GetObject<COpenGLTexture*>(_iTexture, EID_Texture);
		
		if(pTexture)
		{
			if(m_Exts.ARB_multitexture)
				glextActiveTextureARB(GL_TEXTURE0_ARB+_iUnit);
			glBindTexture(GL_TEXTURE_2D, pTexture->m_iGLTexture);
			glEnable(GL_TEXTURE_2D);
		}
		else
		{
			glextActiveTextureARB(GL_TEXTURE0_ARB+_iUnit);
			glDisable(GL_TEXTURE_2D);
		}
	}

	CState::Texture_Set(_iUnit, _iTexture);
}

//
// matrices
//
void COpenGLDevice::Matrix_Model(TMatrix4<fp32> _Matrix)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat*)_Matrix.k);
	CState::Matrix_Model(_Matrix);
}

void COpenGLDevice::Matrix_Projection(TMatrix4<fp32> _Matrix)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat*)_Matrix.k);
	CState::Matrix_Projection(_Matrix);
}

