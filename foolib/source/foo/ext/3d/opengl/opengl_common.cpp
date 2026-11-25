#include <foo/ext/3d/3d.h>
#include <foo/base/log.h>
#include <foo/base/string.h>
#include <foo/base/memory.h>
//#include <foo/ext/loader/tga/tga.h>
#include "opengl.h"

#include "ext_arb_multitexture.h"
#include "ext_arb_vertex_program.h"
#include "ext_arb_vertex_buffer_object.h"
#include "ext_ext_swap_control.h"

#define GL_BGR_EXT	0x80E0
#define GL_BGRA_EXT	0x80E1

using namespace NFoo;
using namespace NFoo::N3D;
using namespace NFoo::N3D::NOpenGL;


//
//
//
COpenGLVertexBuffer::COpenGLVertexBuffer()
{
	MemorySet(&m_LockInfo, 0, sizeof(m_LockInfo));
}

COpenGLVertexBuffer::~COpenGLVertexBuffer()
{
}

void COpenGLVertexBuffer::ARB_VBO_Bind()
{
	glextBindBufferARB(GL_ARRAY_BUFFER_ARB, m_iBufferID);
}

COpenGLIndexBuffer::COpenGLIndexBuffer()
{
	MemorySet(&m_LockInfo, 0, sizeof(m_LockInfo));
}

COpenGLIndexBuffer::~COpenGLIndexBuffer()
{
	SafeArrayDelete(m_LockInfo.m_pIndex16);
	SafeArrayDelete(m_LockInfo.m_pIndex32);
}

//
//
//
COpenGLDevice::COpenGLDevice()
{
	MemorySet(&m_Exts, 0, sizeof(m_Exts));
}

//
//
//
bool COpenGLDevice::Init(CMode _Mode)
{
	m_Exts.ARB_vertex_program = GLExtInit_arb_vertex_program();
	m_Exts.ARB_multitexture = false;
	m_Exts.ARB_multitexture = GLExtInit_arb_multitexture();
	m_Exts.EXT_swap_control = GLExtInit_ext_swap_control();
	//m_Exts.ARB_vertex_buffer_object = GLExtInit_arb_vertex_buffer_object();
	
	//m_Exts.EXT_bgra = false;
	m_Exts.ARB_vertex_buffer_object = false; // this can slow down horribly (must fix)

	if(m_Exts.ARB_vertex_program)
		m_lShaderVersion.Add("ARBvp1_0");

	if(m_Exts.EXT_swap_control)
		glextSwapIntervalEXT(0);
	
	if(!m_Exts.ARB_multitexture)
		NLog::WriteF("opengl: ERROR! ARB_multitexture extentions isn't present. The device will not work without it.\n");

	CString Exts = (const int8*)glGetString(GL_EXTENSIONS);
	if(Exts.Find("GL_EXT_bgra"))
		m_Exts.EXT_bgra = true;

	m_ScreenWidth = _Mode.Get(CMode::EWidth);
	m_ScreenHeight = _Mode.Get(CMode::EHeight);


	NLog::WriteF("Extentions: %s\n", glGetString(GL_EXTENSIONS));

	State_SetDefaults(true);
	return true;
}

//
//
//
COpenGLBuffer::COpenGLBuffer()
{
	m_Flags = 0;
}

//
//
//
bool COpenGLBuffer::Lock()
{
	if(m_Flags&ELocked)
	{
		NLog::Write("opengl: Lock() called on a buffer that is already locked\n");
		return false;
	}
	m_Flags|=ELocked;
	return true;
}

//
//
//
void COpenGLBuffer::Unlock()
{
	if(!(m_Flags&ELocked))
	{
		NLog::Write("opengl: Unlock() called on a buffer that isn't locked\n");
		return;
	}
	m_Flags&=~ELocked;
}


int32_t COpenGLDevice::CreateTexture(CImage::CFormat Format, int32_t Flags, void *pData)
{
	if(!pData || Format.Width() < 0 || Format.Height() < 0)
		DebugBreak();
	CImage Image(Format, (uint8 *)pData);
	int32_t i = CreateTexture(Image, Flags);
	Image.RemoveData();
	return i;
}

int32_t COpenGLDevice::CreateTexture(CImage &rImage, int32_t Flags)
{
	if(!rImage.Data() || rImage.Format().Width() < 0 || rImage.Format().Height() < 0)
		DebugBreak();
	//bool Convert = true;
	CImage TempImage;
	CImage *pFinalImage = &rImage;
	int32_t InternalFormat = 0;
	int32_t Format = 0;
	int32_t Components = 0;
	int32_t Type = GL_UNSIGNED_BYTE;
	int32_t Width = rImage.Format().Width();
	int32_t Height = rImage.Format().Height();
	if(rImage.Format().Type() == CImage::EType_Channels)
	{
		int32_t ColorSpace = rImage.Format().ColorSpace();
		uint32_t Channels = rImage.Format().Channels();
		uint32_t Depths = rImage.Format().ChannelDepths();
		
		if(ColorSpace == CImage::EColorSpace_RGB)
		{
			if(Depths == 0x08080800)
			{
				InternalFormat = GL_RGB8;
				Components = 3;
				if(Channels == 0x000102FF) // RGB8
					Format = GL_RGB;
				else if (Channels == 0x020100FF) // BGR8
				{
					if(m_Exts.EXT_bgra)
						Format = GL_BGR_EXT;
				}
			}
			else if(Depths == 0x08080808)
			{
				InternalFormat = GL_RGBA8;
				Components = 4;
				if(Channels == 0x00010203) // RGBA8
					Format = GL_RGBA;
				else if (Channels == 0x02010003) // BGRA8
				{
					if(m_Exts.EXT_bgra)
						Format = GL_BGRA_EXT;
				}
			}

		}
		else if(ColorSpace == CImage::EColorSpace_YUV)
		{

		}
		else if(ColorSpace == CImage::EColorSpace_GrayScale && Depths == 0x08000000 && Channels == 0x00FFFFFF)
		{
			Components = 1;
			Format = GL_LUMINANCE;
			InternalFormat = GL_LUMINANCE8;
		}
		else if(ColorSpace == CImage::EColorSpace_GrayScale && Depths == 0x08080000 && Channels == 0x0003FFFF)
		{
			Components = 2;
			Format = GL_LUMINANCE_ALPHA;
			InternalFormat = GL_LUMINANCE8_ALPHA8;
		}
	}

	//Format = 0;
	if(!Format) // no format found, convert to RGBA8
	{
		Type = GL_UNSIGNED_BYTE;
		Components = 4;
		InternalFormat = GL_RGBA8;
		Format = GL_RGBA;
		pFinalImage = &TempImage;
		rImage.Convert(CImage::CFormat(CImage::EQuick_RGBAi8), TempImage);
		NLog::WriteF("Performance Warning: Convertion during texture creation\n");
	}

	GLuint gltexture;
	glGenTextures(1, (GLuint *)&gltexture);
	glBindTexture(GL_TEXTURE_2D, gltexture);
	glEnable(GL_TEXTURE_2D);
	
	while(glGetError() != GL_NO_ERROR); // pump error message???? this solves som strange driver problems

	// transfer data
	if(Flags&ETexFlag_NoMipMap)
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format, Type, pFinalImage->Data());
	else
		gluBuild2DMipmaps(GL_TEXTURE_2D, Components, Width, Height, Format, Type, pFinalImage->Data());

	if(glGetError() != GL_NO_ERROR)
	{
		NLog::WriteF("OpenGL: Couldn't handle the texture format. Reverting to RGBAi8\n");

		CImage TempImage;
		pFinalImage->Convert(CImage::CFormat(CImage::EQuick_RGBAi8), TempImage);

		if(Flags&ETexFlag_NoMipMap)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, TempImage.Data());
		else
			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, TempImage.Data());

		if(glGetError() != GL_NO_ERROR)
		{
			NLog::WriteF("OpenGL: Well, that didn't work. Bailing\n");
			glDeleteTextures(1, (GLuint *)&gltexture);
			return -1;
		}
	}
	

	//
	COpenGLTexture *pTexture = new COpenGLTexture();

	// add it to the index
	int32_t iTexture = m_IndexHandler.GetFree(EID_Texture);
	m_IndexHandler.SetData(iTexture, pTexture);

	//pTexture->m_GLFormat = g_aTexFormatTableFormat[_Format];
	pTexture->m_iGLTexture = gltexture;

	if(Flags&ETexFlag_NoMipMap)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}

	// set info
	//pTexture->m_Info.m_Format = _Format;
	pTexture->m_Info.m_iIndex = iTexture;
	pTexture->m_Info.m_Size.Set(Width, Height);
	
	// all done
	return iTexture;

}


//
//
//
/*
									// ETexFormat_RGB24, ETexFormat_RGBA32, ETexFormat_Depth
const intr g_aTexFormatTableComponents[] =	{3, 4, 1, 1};
const intr g_aTexFormatTableInt[] =		{GL_RGB8			, GL_RGBA8			, GL_ALPHA, GL_LUMINANCE8};
const intr g_aTexFormatTableFormat[] =	{GL_RGB				, GL_RGBA			, GL_ALPHA, GL_LUMINANCE};
const intr g_aTexFormatTableType[] =	{GL_UNSIGNED_BYTE	, GL_UNSIGNED_BYTE	, GL_FLOAT, GL_UNSIGNED_BYTE};

int32_t COpenGLDevice::Internal_CreateTexture(int32_t _w, int32_t _h, int32_t _Format, uint8 *_pData)
{
	glGetError();
	// gen and bind

	int32_t Format = _Format&ETexFormatMask;
	int32_t Flags = _Format;

	GLuint gltexture;
	glGenTextures(1, (GLuint *)&gltexture);
	glBindTexture(GL_TEXTURE_2D, gltexture);
	glEnable(GL_TEXTURE_2D);
	
	// transfer data
	if(Flags&ETexFlag_NoMipMap)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, g_aTexFormatTableInt[Format], _w, _h, 0,
						g_aTexFormatTableFormat[Format], g_aTexFormatTableType[Format], _pData);
	}
	else
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, g_aTexFormatTableComponents[Format], _w, _h, g_aTexFormatTableFormat[Format], 
							g_aTexFormatTableType[Format], _pData);
	}
	
	// check for errors
	if(glGetError() != GL_NO_ERROR)
	{
		glDeleteTextures(1, (GLuint *)&gltexture);
		return -1;
	}

	//
	COpenGLTexture *pTexture = new COpenGLTexture();

	// add it to the index
	int32_t iTexture = m_IndexHandler.GetFree(EID_Texture);
	m_IndexHandler.SetData(iTexture, pTexture);

	pTexture->m_GLFormat = g_aTexFormatTableFormat[_Format];
	pTexture->m_iGLTexture = gltexture;

	if(Flags&ETexFlag_NoMipMap)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}

	// set info
	pTexture->m_Info.m_Format = _Format;
	pTexture->m_Info.m_iIndex = iTexture;
	pTexture->m_Info.m_Size.Set(_w, _h);
	
	// all done
	return iTexture;
}

//
//
//
int32_t COpenGLDevice::CreateTexture(int32_t _w, int32_t _h, int32_t _Format, void *_pData)
{
	if(_pData)
		return Internal_CreateTexture(_w, _h, _Format, (uint8 *)_pData);

	uint8 *pData = new uint8 [_w*_h*4]; // worst case, temporary data
	int32_t iTex = Internal_CreateTexture(_w, _h, _Format, pData);
	SafeArrayDelete(pData);
	return iTex;
}

//
//
//
int32_t COpenGLDevice::CreateTexture(CImage &rImage)
{
	CImage Temp;
	CImage &FinalImage = rImage;
	int32_t Format = 0;
	if(rImage.Format().Type() == CImage::EType_Channels && rImage.Format().ChannelType() == CImage::EChannelType_Int)
	{
		int32_t nc = rImage.Format().NumChannels();
		int32_t c = rImage.Format().Channels();
		int32_t d = rImage.Format().ChannelDepths();

		if(rImage.Format().ColorSpace() == CImage::EColorSpace_RGB)
		{
			if(nc == 4 && d == 0x08080808)
			{
				if(c != 0x01020304)
				{
					Format = ETexFormat_RGBA32;
					rImage.Convert(CImage::CFormat(CImage::EQuick_RGBAi8), Temp);
					FinalImage = Temp;
				}
				Format = ETexFormat_RGBA32;
			}
			else if(nc == 3 && rImage.Format().BPP() == 24)
				Format = ETexFormat_RGB24;
		}
		else if(nc == 1 && rImage.Format().BPP() == 8)
			Format = ETexFormat_BW;
		else
		{
			Format = ETexFormat_RGBA32;
			rImage.Convert(CImage::CFormat(CImage::EQuick_RGBAi8), Temp);
			FinalImage = Temp;
			//return -1;
		}
	}
	else
		return -1;
	/*
		Format = ETexFormat_RGB24;
	else if(_Image.m_Desc.m_Format == CImage::EFormat_RGBA32)
		Format = ETexFormat_RGBA32;
	else if(_Image.m_Desc.m_Format == CImage::EFormat_BW)
		Format = ETexFormat_BW;
	else 
		return -1;

	return CreateTexture(FinalImage.Format().Width(), FinalImage.Format().Height(), Format, FinalImage.Data());
}
*/
//
//
//
bool COpenGLDevice::DestroyTexture(int32_t _iTexture)
{
	COpenGLTexture *pTexture = m_IndexHandler.GetObject<COpenGLTexture*>(_iTexture, EID_Texture);
	if(!pTexture)
		return false;
	
	glDeleteTextures(1, &pTexture->m_iGLTexture);

	delete pTexture;
	m_IndexHandler.Remove(_iTexture);

	return true;
}

//
//
//
CTextureInfo COpenGLDevice::GetTextureInfo(int32_t _iTexture)
{
	CTextureInfo Info;

	COpenGLTexture *pTexture = m_IndexHandler.GetObject<COpenGLTexture*>(_iTexture, EID_Texture);
	if(!pTexture)
		return Info;

	return pTexture->m_Info;
}


// index buffers
int32_t COpenGLDevice::CreateIndexBuffer(int32_t _Count, int32_t _Flags)
{
	COpenGLIndexBuffer *pBuffer = new COpenGLIndexBuffer;
	int32_t iIB = m_IndexHandler.GetFree(EID_IndexBuffer);
	m_IndexHandler.SetData(iIB, pBuffer);
	pBuffer->m_LockInfo.m_Count = _Count;
	pBuffer->m_LockInfo.m_pIndex16 = new uint16[_Count];
	return iIB;
}

bool COpenGLDevice::DestroyIndexBuffer(int32_t _iBuffer)
{
	COpenGLIndexBuffer *pBuffer = m_IndexHandler.GetObject<COpenGLIndexBuffer*>(_iBuffer, EID_IndexBuffer);
	if(!pBuffer)
		return false;

	delete pBuffer;
	m_IndexHandler.Remove(_iBuffer);
	return true;
}

// Get indexbuffer from id, Return control of indexbuffer to device
CIndexBuffer* COpenGLDevice::LockIndexBuffer(int32_t _iBuffer)
{
	COpenGLIndexBuffer *pBuffer = m_IndexHandler.GetObject<COpenGLIndexBuffer*>(_iBuffer, EID_IndexBuffer);
	if(!pBuffer)
		return NULL;
	
	if(!pBuffer->Lock())
		return NULL;
	
	return &pBuffer->m_LockInfo;
}

void COpenGLDevice::UnlockIndexBuffer(int32_t _iBuffer)
{
	COpenGLIndexBuffer *pBuffer = m_IndexHandler.GetObject<COpenGLIndexBuffer*>(_iBuffer, EID_IndexBuffer);
	if(!pBuffer)
		return;
	
	pBuffer->Unlock();
}

//
//
//
int32_t COpenGLDevice::CreateVertexShader(const int8 *_pString)
{
	if(!m_Exts.ARB_vertex_program)
		return -1;
	
	glGetError();
	GLuint Program;
	glextGenProgramsARB(1, &Program);
	if(glGetError() != GL_NO_ERROR) return -1;

	glextBindProgramARB(GL_VERTEX_PROGRAM_ARB, Program);
	if(glGetError() != GL_NO_ERROR) return -1;

	glextProgramStringARB(GL_VERTEX_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB, StringLength(_pString),_pString); 
	if(glGetError() != GL_NO_ERROR)
	{
		GLint pos;
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &pos);
		NLog::WriteF("opengl: Error creating vertex shader, %d: %s\n", pos, glGetString(GL_PROGRAM_ERROR_STRING_ARB));
		return -1;
	}

	// add it to the index
	COpenGLHandle *pHandle = new COpenGLHandle;
	pHandle->m_iHandle = Program;
	int32_t iShader = m_IndexHandler.GetFree(EID_VertexShader);
	m_IndexHandler.SetData(iShader, pHandle);

	/*
	NLog::WriteF("gen GLerr=%d\n", glGetError());
	NLog::WriteF("bind GLerr=%d\n", glGetError());
	NLog::WriteF("prg %d GLerr=%d\n", StringLength(_pString)-1, glGetError());

	int8 aProgram[1024*4];
	aProgram[0] = 0;
	GLExt_glGetProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, aProgram);
	NLog::WriteF("prg %s\n", aProgram);
	*/


	/*
	COpenGLVertexProgram *pProgram = new COpenGLVertexProgram();
	pProgram->m_Handle = Program;
	
	return pProgram;
	*/


	return iShader;
}

//
//
//
bool COpenGLDevice::DestroyVertexShader(int32_t _iShader)
{
	COpenGLHandle *pHandle = m_IndexHandler.GetObject<COpenGLHandle*>(_iShader, EID_VertexShader);
	if(!pHandle)
		return false;
	
	glextDeleteProgramsARB(1, &pHandle->m_iHandle);
	m_IndexHandler.Remove(_iShader);
	delete pHandle;
	return true;
}

//
//
//
int32_t COpenGLDevice::CreatePixelShader(const int8 *_pString)
{
	return -1;
}


//
//
//
bool COpenGLDevice::DestroyPixelShader(int32_t _iShader)
{
	return false;
}

//
//
//
int32_t COpenGLDevice::CreateVertexBuffer(int32_t _Count, int32_t _What, int32_t _Flags)
{
	COpenGLVertexBuffer *pBuffer = new COpenGLVertexBuffer;
	int32_t iVB = m_IndexHandler.GetFree(EID_VertexBuffer);
	m_IndexHandler.SetData(iVB, pBuffer);
	pBuffer->m_LockInfo.m_Count = _Count;

	if(m_Exts.ARB_vertex_buffer_object)
	{
		glextGenBuffersARB(1, (GLuint*)&pBuffer->m_iBufferID);
		glextBindBufferARB(GL_ARRAY_BUFFER_ARB, pBuffer->m_iBufferID);

		uint8 *pData = ((uint8 *)NULL)+4; // we must waste 4 bytes.. 0 = NULL

		for(int32_t i = 0; i < EMaxTextureUnits; i++)
		{
			if(_What&(EVB_TextureBase<<i))
			{
				pBuffer->m_LockInfo.m_pTexturefp32[i] = (TVector2<fp32>*)pData;
				pData += _Count*sizeof(TVector2<fp32>);
			}
		}

		pBuffer->m_LockInfo.m_pVertexfp32 = NULL;
		pBuffer->m_LockInfo.m_pNormalfp32 = NULL;
		pBuffer->m_LockInfo.m_pColorfp32 = NULL;
		if(_What&EVB_Vertex)
		{
			pBuffer->m_LockInfo.m_pVertexfp32 = (TVector3<fp32>*)pData;
			pData += sizeof(TVector3<fp32>)*_Count;
		}

		if(_What&EVB_Normal)
		{
			pBuffer->m_LockInfo.m_pNormalfp32 = (TVector3<fp32>*)pData;
			pData += sizeof(TVector3<fp32>)*_Count;
		}

		if(_What&EVB_Color)
		{
			pBuffer->m_LockInfo.m_pColorfp32 = (TVector4<fp32>*)pData;
			pData += sizeof(TVector4<fp32>)*_Count;
		}

		int8 *pTemp = new int8[(intptr_t)pData];
		glextBufferDataARB(GL_ARRAY_BUFFER_ARB, (intptr_t)pData, pTemp, GL_DYNAMIC_DRAW_ARB); //GL_STATIC_DRAW_ARB);
		delete [] pTemp;

	}
	else
	{
		for(int32_t i = 0; i < EMaxTextureUnits; i++)
		{
			if(_What&(EVB_TextureBase<<i))
				pBuffer->m_LockInfo.m_pTexturefp32[i] = new TVector2<fp32>[_Count];
			else
				pBuffer->m_LockInfo.m_pTexturefp32[i] = NULL;
		}

		pBuffer->m_LockInfo.m_pVertexfp32 = NULL;
		pBuffer->m_LockInfo.m_pNormalfp32 = NULL;
		pBuffer->m_LockInfo.m_pColorfp32 = NULL;
		if(_What&EVB_Vertex) pBuffer->m_LockInfo.m_pVertexfp32 = new TVector3<fp32>[_Count];
		if(_What&EVB_Normal) pBuffer->m_LockInfo.m_pNormalfp32 = new TVector3<fp32>[_Count];
		if(_What&EVB_Color) pBuffer->m_LockInfo.m_pColorfp32 = new TVector4<fp32>[_Count];
	}

	pBuffer->m_What = _What;

	return iVB;
}

//
//
bool COpenGLDevice::DestroyVertexBuffer(int32_t _iBuffer)
{
	COpenGLVertexBuffer *pVB = m_IndexHandler.GetObject<COpenGLVertexBuffer*>(_iBuffer, EID_VertexBuffer);
	if(!pVB)
		return false;
	
	m_IndexHandler.Remove(_iBuffer);
	if(m_Exts.ARB_vertex_buffer_object)
		glextDeleteBuffersARB(1, (GLuint*)&pVB->m_iBufferID);
	else
	{
		SafeArrayDelete(pVB->m_LockInfo.m_pVertexfp32);
		SafeArrayDelete(pVB->m_LockInfo.m_pNormalfp32);
		SafeArrayDelete(pVB->m_LockInfo.m_pColorfp32);
		
		for(intr i = 0; i < EMaxTextureUnits; i++)
			SafeArrayDelete(pVB->m_LockInfo.m_pTexturefp32[i]);
	}
	delete pVB;
	return true;
}

//
//
CVertexBuffer* COpenGLDevice::LockVertexBuffer(int32_t _iBuffer, int32_t _What)
{
	COpenGLVertexBuffer *pVB = m_IndexHandler.GetObject<COpenGLVertexBuffer*>(_iBuffer, EID_VertexBuffer);
	if(!pVB)
		return NULL;
	
	if(!pVB->Lock())
	{
		NLog::WriteF("opengl: warning, LockVertexBuffer called on a buffer that is locked\n");
		return NULL;
	}

	if(m_Exts.ARB_vertex_buffer_object)
	{
		// bind the buffer
		pVB->ARB_VBO_Bind();

		// map it
		int32_t What = pVB->m_What;
		int32_t Count = pVB->m_LockInfo.m_Count;
		uint8 *pData = (uint8 *)glextMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);

		// fill the struct
		pVB->m_LockInfo2.m_Count = Count;
		for(int32_t i = 0; i < EMaxTextureUnits; i++)
			pVB->m_LockInfo2.m_pTexturefp32[i] = (TVector2<fp32>*)(pData+(intptr_t)pVB->m_LockInfo.m_pTexturefp32[i]);

		pVB->m_LockInfo2.m_pVertexfp32 = (TVector3<fp32>*)(pData+(intptr_t)pVB->m_LockInfo.m_pVertexfp32);
		pVB->m_LockInfo2.m_pNormalfp32 = (TVector3<fp32>*)(pData+(intptr_t)pVB->m_LockInfo.m_pNormalfp32);
		pVB->m_LockInfo2.m_pColorfp32 = (TVector4<fp32>*)(pData+(intptr_t)pVB->m_LockInfo.m_pColorfp32);

		return &pVB->m_LockInfo2;
	}

	return &pVB->m_LockInfo;
}

//
//
void COpenGLDevice::UnlockVertexBuffer(int32_t _iBuffer, int32_t _What)
{
	COpenGLVertexBuffer *pVB = m_IndexHandler.GetObject<COpenGLVertexBuffer*>(_iBuffer, EID_VertexBuffer);
	if(!pVB)
		return;

	pVB->Unlock();

	if(m_Exts.ARB_vertex_buffer_object)
	{
		pVB->ARB_VBO_Bind();
		glextUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
	}
}

//
// clear
//
void COpenGLDevice::Clear(intr _What)
{
	int32_t Flags = 0;
	if(_What&EBuffer_Color) Flags |= GL_COLOR_BUFFER_BIT;
	if(_What&EBuffer_Depth) Flags |= GL_DEPTH_BUFFER_BIT;
	if(_What&EBuffer_Stencil) Flags |= GL_STENCIL_BUFFER_BIT;
	
	glClearColor(m_ClearColor.x,m_ClearColor.y,m_ClearColor.z,m_ClearColor.w);
	glClearStencil(m_ClearStencil);
	glClearDepth(m_ClearDepth);

	glClear(Flags);
}

//
// draw operation
//
void CheckPointer(GLenum _GetFlag, GLenum _EnableFlag, const int8 *_pName)
{
	GLvoid *p;
    glGetPointerv(_GetFlag, &p);
	if(glIsEnabled(_EnableFlag) && p == NULL)
		NLog::WriteF("opengl: warning, %s is enabled and has a null pointer\n", _pName);
}

const intr g_aDrawTable[] = {GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS};
const intr g_aPrimTable[] = {1, 2, 1, 3, 1, 1, 4};
const intr g_aPrimAddTable[] = {0, 0, 1, 0, 2, 2, 0};

int32_t PrimToOGL(int32_t _Mode, int32_t _PrimCount)
{
	return g_aPrimTable[_Mode]*_PrimCount+g_aPrimAddTable[_Mode];
}

void COpenGLDevice::Draw(int32_t _Mode, int32_t _Start, int32_t _PrimCount)
{
	try
	{
		//int32_t i = glIsEnabled(GL_VERTEX_ARRAY);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glPointSize(3);
		int32_t Count = PrimToOGL(_Mode, _PrimCount);
		glDrawArrays(g_aDrawTable[_Mode], _Start, Count);
	}
	catch(...)
	{
		NLog::WriteF("opengl: glDrawArrays generated an error. Checking pointers\n");
		CheckPointer(GL_COLOR_ARRAY_POINTER, GL_COLOR_ARRAY, "color array");
		CheckPointer(GL_INDEX_ARRAY_POINTER, GL_INDEX_ARRAY, "index array");
		CheckPointer(GL_NORMAL_ARRAY_POINTER, GL_NORMAL_ARRAY, "normal array");
		CheckPointer(GL_TEXTURE_COORD_ARRAY_POINTER, GL_TEXTURE_COORD_ARRAY, "texcoord array");
		CheckPointer(GL_VERTEX_ARRAY_POINTER, GL_VERTEX_ARRAY, "vertex array");
	}
}

void COpenGLDevice::DrawIndexed(int32_t _Mode, int32_t _Start, int32_t _PrimCount)
{
	if(!m_pIndexPointer)
		return;

	try
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		/*
		fp32 color[] = {2/255.0f,24/255.0f,34/255.0f,1.0f};
		glFogf(GL_FOG_MODE, GL_LINEAR);
		glFogf(GL_FOG_START, 50);
		glFogf(GL_FOG_END, 75);
		glFogfv(GL_FOG_COLOR, color);
		glEnable(GL_FOG);
		glFogf(GL_FOG_DENSITY, 0.5f);
		*/
		glDrawElements(g_aDrawTable[_Mode], PrimToOGL(_Mode, _PrimCount), GL_UNSIGNED_SHORT, (uint16*)m_pIndexPointer+_Start);
	}
	catch(...)
	{
		NLog::WriteF("opengl: glDrawElements generated an error. Checking pointers\n");
		CheckPointer(GL_COLOR_ARRAY_POINTER, GL_COLOR_ARRAY, "color array");
		CheckPointer(GL_INDEX_ARRAY_POINTER, GL_INDEX_ARRAY, "index array");
		CheckPointer(GL_NORMAL_ARRAY_POINTER, GL_NORMAL_ARRAY, "normal array");
		CheckPointer(GL_TEXTURE_COORD_ARRAY_POINTER, GL_TEXTURE_COORD_ARRAY, "texcoord array");
		CheckPointer(GL_VERTEX_ARRAY_POINTER, GL_VERTEX_ARRAY, "vertex array");
	}
}

//
// grab
//
bool COpenGLDevice::Grab(int32_t _iTexture, int32_t _What)
{
	/*
	if(_iTexture == -10 && _What == EBuffer_Color)
	{
		CImage Image;
		Image.Create(TVector2<int32_t>(640,480), CImage::EFormat_RGBA32);
		glReadPixels(0, 0, 640, 480, GL_RGBA, GL_UNSIGNED_BYTE, Image.m_pData);
		NTGA::Write(CFile("dump.tga", CFile::EWriteBinary), Image);
		return true;
	}*/
	glGetError();

	COpenGLTexture *pTexture = m_IndexHandler.GetObject<COpenGLTexture*>(_iTexture, EID_Texture);
	if(!pTexture)
		return false;

	// bind texture
	glBindTexture(GL_TEXTURE_2D, pTexture->m_iGLTexture);
	glEnable(GL_TEXTURE_2D);

	int32_t w = pTexture->m_Info.m_Size[0];
	int32_t h = pTexture->m_Info.m_Size[1];
	int32_t x = 0;
	int32_t y = 0;
	//y = 480;
	//if(y < 0)
	//	y = 0;
	//if(w > 640)
	//	w = 640;
	//if(h > 480)
	//	h = 480;
	//h = -h;
	if(_What == EBuffer_Depth) // depth grab
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, x, y, w, h, 0);
	else if(_What == EBuffer_Color) // color grab
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, w, h, 0);
	else
		return false;

	GLenum err = glGetError();
	if(err == GL_NO_ERROR)
		return true;
	if(err == GL_INVALID_OPERATION)
		return false;
	if(err == GL_INVALID_VALUE)
		return false;
	if(err == GL_NO_ERROR)
		return false;
	if(err == GL_NO_ERROR)
		return false;

	 

	return false;
}

bool COpenGLDevice::Grab(NFoo::CImage &rImage, int32_t _What)
{
	if(_What == EBuffer_Color)
	{
		//rImage.Create(TVector2<int32_t>(640,480), CImage::EFormat_RGBA32);
		//glReadPixels(0, 0, 640, 480, GL_RGBA, GL_UNSIGNED_BYTE, _rImage.m_pData);
		//NTGA::Write(CFile("dump.tga", CFile::EWriteBinary), Image);
		return true;
	}
	return false;
}

//
// viewport
//
void COpenGLDevice::SetViewport(int32_t _x, int32_t _y, int32_t _w, int32_t _h)
{
	//GLint aViewport[4];
	//glGetIntegerv(GL_VIEWPORT, aViewport);
	glViewport(_x, m_ScreenHeight-_h-_y, _w, _h);
}

void COpenGLDevice::SetScissors(int32_t _x, int32_t _y, int32_t _w, int32_t _h)
{
	//GLint aViewport[4];
	//glGetIntegerv(GL_VIEWPORT, aViewport);
	glScissor(_x, m_ScreenHeight-_h-_y, _w, _h);
}

