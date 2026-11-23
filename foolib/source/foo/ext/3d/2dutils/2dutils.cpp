
#include <foo/base/string.h>
#include <foo/base/log.h>
#include <foo/base/math.h>
#include "2dutils.h"

using namespace NFoo;
using namespace NFoo::N3D;

/*
C2DClip::C2DClip(bool _bClip, bool _bClipAny, const TVector2<fp32>& _ClipTL, const TVector2<fp32>& _ClipBR)
{
	m_bClip = _bClip;
	m_bClipAny = _bClipAny;
	m_ClipTL = _ClipTL;
	m_ClipBR = _ClipBR;
}

bool C2DClip::Clip(const TVector2<fp32>& _P1, const TVector2<fp32>& _P2, const TVector2<fp32>& _P3, const TVector2<fp32>& _P4)
{
	if (!m_bClip)
		return false;

	int32 Clipped = 0;

	if (m_bClipAny)
	{
		if (Clip(_P1))
			return true;
		if (Clip(_P2))
			return true;
		if (Clip(_P3))
			return true;
		if (Clip(_P4))
			return true;
	}
	else
	{
		if (Clip(_P1))
			Clipped |= E2DCLIP_MASK_1;
		if (Clip(_P2))
			Clipped |= E2DCLIP_MASK_2;
		if (Clip(_P3))
			Clipped |= E2DCLIP_MASK_3;
		if (Clip(_P4))
			Clipped |= E2DCLIP_MASK_4;

		return Clipped = (E2DCLIP_MASK_1 | E2DCLIP_MASK_2 | E2DCLIP_MASK_3 | E2DCLIP_MASK_4);
	}
	return false;
}

// Simple clipper, clips the entire triangle if any part is outside
bool C2DClip::Clip(const TVector2<fp32>& _P1, const TVector2<fp32>& _P2, const TVector2<fp32>& _P3)
{
	if (!m_bClip)
		return false;
	
	int32 Clipped = 0;

	if (m_bClipAny)
	{
		if (Clip(_P1))
			return true;
		if (Clip(_P2))
			return true;
		if (Clip(_P3))
			return true;
	}
	else
	{
		if (Clip(_P1))
			Clipped |= E2DCLIP_MASK_1;
		if (Clip(_P2))
			Clipped |= E2DCLIP_MASK_2;
		if (Clip(_P3))
			Clipped |= E2DCLIP_MASK_3;

		return Clipped = (E2DCLIP_MASK_1 | E2DCLIP_MASK_2 | E2DCLIP_MASK_3);
	}

	return false;
}

bool C2DClip::Clip(const TVector2<fp32>& _P1, const TVector2<fp32>& _P2)
{
	if (!m_bClip)
		return false;

	int32 Clipped = 0;

	if (m_bClipAny)
	{
		if (Clip(_P1))
			return true;
		if (Clip(_P2))
			return true;
	}
	else
	{
		if (Clip(_P1))
			Clipped |= E2DCLIP_MASK_1;
		if (Clip(_P2))
			Clipped |= E2DCLIP_MASK_2;

		return Clipped = (E2DCLIP_MASK_1 | E2DCLIP_MASK_2);
	}

	return false;
}

bool C2DClip::Clip(const TVector2<fp32>& _P1)
{
	if (!m_bClip)
		return false;

	if (m_bClipAny)
	{
		if (_P1.x < m_ClipTL.x || _P1.x > m_ClipBR.x)
			return true;
		if (_P1.y < m_ClipTL.y || _P1.y > m_ClipBR.y)
			return true;
	}
	else
	{
		if ((_P1.x < m_ClipTL.x || _P1.x > m_ClipBR.x) &&
			(_P1.y < m_ClipTL.y || _P1.y > m_ClipBR.y))
			return true;
	}

	return false;
}
*/

void C2DUtils::CFont::BuildNormalAscii()
{
	for(int32 i = 0; i < 256; i++)
		m_aTranslationTable[i] = i;
}

void C2DUtils::CFont::BuildTranslationTable(const uint8 *_pString)
{
	MemorySet(m_aTranslationTable, 0, 256);

	int32 i = 0;
	while(*_pString)
	{
		m_aTranslationTable[*_pString] = i;
		_pString++;
		i++;
	}
}

//
//
//
C2DUtils::C2DUtils()
{
	m_pDevice = NULL;
	m_iVertexBuffer = -1;
	m_pVertexBuffer = NULL;
	m_pFont = NULL;
	m_NudgeX = 0;
	m_NudgeY = 0;
	m_Mode = EMode_NotInited;
	m_ScreenWidth = 640;
	m_ScreenHeight = 480;
	m_UseClip = false;
	m_GlobalTranslate.Set(0,0);
	SetDefaults();
}

//
//
//
C2DUtils::~C2DUtils()
{
	Shutdown();
}

//
//
//
bool C2DUtils::Init(NFoo::N3D::CDevice *_pDevice, int32 _BufferSize)
{
	while(true)
	{
		if(m_Mode != EMode_NotInited)
			break;
	
		m_pDevice = _pDevice;
		if(!m_pDevice)
			break;
		
		m_iVertexBuffer = m_pDevice->CreateVertexBuffer(_BufferSize, EVB_Vertex|EVB_Color|EVB_Texture0, 0);
		if(m_iVertexBuffer < 0)
			break;

		m_ScreenWidth = 640;
		m_ScreenHeight = 480;

		m_Mode = EMode_Idle;
		return true;
	}
	
	return false;
}

//
//
//
void C2DUtils::Shutdown()
{
	if(m_pDevice && m_iVertexBuffer != -1)
		m_pDevice->DestroyVertexBuffer(m_iVertexBuffer);
	m_pDevice = NULL;
	m_iVertexBuffer = -1;
}

//
//
//
bool C2DUtils::LockVB(int32 _iMode)
{
	if(m_Mode != EMode_Idle)
	{
		NLog::Write("C2DUtils::LockVB: Errors mode\n");
		return false;
	}

	m_pVertexBuffer = m_pDevice->LockVertexBuffer(m_iVertexBuffer);
	
	if(!m_pVertexBuffer)
		return false;

	m_VertexCount = 0;
	m_Mode = _iMode;
	return true;
}

//
//
//
void C2DUtils::UnlockVB()
{
	if(m_Mode == EMode_Idle || m_Mode == EMode_NotInited)
		return;
		
	m_Mode = EMode_Idle;
	m_pDevice->UnlockVertexBuffer(m_iVertexBuffer);
	m_pVertexBuffer = NULL;
}


//
//
//
void C2DUtils::SetDefaults()
{
	m_aWriteMask[0] = m_aWriteMask[1] = m_aWriteMask[2] = m_aWriteMask[3] = true;
	m_aColors[0][0] = m_aColors[0][1] = m_aColors[0][2] = m_aColors[0][3] = 1;
	m_aColors[1][0] = m_aColors[1][1] = m_aColors[1][2] = m_aColors[1][3] = 1;
	m_aColors[2][0] = m_aColors[2][1] = m_aColors[2][2] = m_aColors[2][3] = 1;
	m_aColors[3][0] = m_aColors[3][1] = m_aColors[3][2] = m_aColors[3][3] = 1;

	m_aTexCoords[0][0] = 0;
	m_aTexCoords[0][1] = 0;

	m_aTexCoords[1][0] = 1;
	m_aTexCoords[1][1] = 0;

	m_aTexCoords[2][0] = 0;
	m_aTexCoords[2][1] = 1;

	m_aTexCoords[3][0] = 1;
	m_aTexCoords[3][1] = 1;
	
	m_Blend = EBlend_None;
	m_QuadRotation = 0;
	m_iTexture = -1;
}

void C2DUtils::SetWriteMask(bool r, bool g, bool b, bool a)
{
	m_aWriteMask[0] = r;
	m_aWriteMask[1] = g;
	m_aWriteMask[2] = b;
	m_aWriteMask[3] = a;
}

void C2DUtils::SetScreenSize(int32 width, int32 height)
{
	m_ScreenWidth = width;
	m_ScreenHeight = height;
}

void C2DUtils::SetClipping(bool _Use, fp32 _x, fp32 _y, fp32 _w, fp32 _h)
{
	m_UseClip = _Use;
	if(m_UseClip)
	{
		//m_Translate = _Translate;
		m_ClipRect.m_Position.x = _x;
		m_ClipRect.m_Position.y = _y;
		m_ClipRect.m_Size.x = _w;
		m_ClipRect.m_Size.y = _h;
	}
}

void C2DUtils::SetGlobalTranslate(fp32 _x, fp32 _y)
{
	m_GlobalTranslate.x = _x;
	m_GlobalTranslate.y = _y;
}



/*
void C2DUtils::PushClip(C2DClip _Clip)
{
	m_lClipStack.Add(_Clip);
}

void C2DUtils::PopClip()
{
	int32 Len = m_lClipStack.Len();
	if (Len > 0)
		m_lClipStack.RemoveByIndex(Len - 1);
}

C2DClip C2DUtils::GetClip() const
{
	int32 Len = m_lClipStack.Len();
	if (Len > 0)
		return m_lClipStack[Len - 1];

	return C2DClip();
}*/

void C2DUtils::Quad_SetRotation(fp32 _r)
{
	m_QuadRotation = _r;
}



//
//
//
void C2DUtils::SetTexture(int32 _iTex)
{
	m_iTexture = _iTex;
}


void C2DUtils::SetBlend(int32 _iBlend)
{
	m_Blend = _iBlend;
}


//
//
//
void C2DUtils::SetTexCoord(int32 _Index, fp32 _u, fp32 _v)
{
	m_aTexCoords[_Index][0] = _u;
	m_aTexCoords[_Index][1] = _v;
}

//
//
//
void C2DUtils::SetTexCoord(fp32 _u0, fp32 _v0, fp32 _u1, fp32 _v1)
{
	SetTexCoord(0, _u0, _v0);
	SetTexCoord(1, _u1, _v0);
	SetTexCoord(2, _u0, _v1);
	SetTexCoord(3, _u1, _v1);
}

//
//
//
void C2DUtils::SetColor(int32 _Index, fp32 _r, fp32 _g, fp32 _b, fp32 _a)
{
	m_aColors[_Index][0] = _r;
	m_aColors[_Index][1] = _g;
	m_aColors[_Index][2] = _b;
	m_aColors[_Index][3] = _a;
}

//
//
//
void C2DUtils::SetColor(fp32 _r, fp32 _g, fp32 _b, fp32 _a)
{
	SetColor(0, _r, _g, _b, _a);
	SetColor(1, _r, _g, _b, _a);
	SetColor(2, _r, _g, _b, _a);
	SetColor(3, _r, _g, _b, _a);
}

//
//
//
void C2DUtils::setColor(TVector4<fp32> color)
{
	SetColor(0, color[0], color[1], color[2], color[3]);
	SetColor(1, color[0], color[1], color[2], color[3]);
	SetColor(2, color[0], color[1], color[2], color[3]);
	SetColor(3, color[0], color[1], color[2], color[3]);
}

//
//
//
bool C2DUtils::Line_Begin()
{
	if(m_Mode != EMode_Idle)
		return false;

	if(!LockVB(EMode_Lines))
		return false;
	return true;
}

//
//
//
void C2DUtils::Line_Draw(fp32 _x0, fp32 _y0, fp32 _x1, fp32 _y1)
{
	if(m_Mode != EMode_Lines)
		return;
		
	if(m_VertexCount >= m_pVertexBuffer->m_Count-2)
	{
		Line_End();
		Line_Begin();
	}
	/*
	C2DClip Clip = GetClip();
	// Clip line
	if (Clip.Clip(TVector2<fp32>(_x0,_y0),TVector2<fp32>(_x1,_y1)))
		return;
		*/

	// add stuff to the VB
	m_pVertexBuffer->m_pVertexfp32[m_VertexCount].Set(_x0,_y0,-1);
	m_pVertexBuffer->m_pColorfp32[m_VertexCount].Set(m_aColors[0][0],m_aColors[0][1],m_aColors[0][2],m_aColors[0][3]);
	m_VertexCount++;
	m_pVertexBuffer->m_pVertexfp32[m_VertexCount].Set(_x1,_y1,-1);
	m_pVertexBuffer->m_pColorfp32[m_VertexCount].Set(m_aColors[1][0],m_aColors[1][1],m_aColors[1][2],m_aColors[1][3]);
	m_VertexCount++;
}

void C2DUtils::Line_End()
{
	if(m_Mode != EMode_Lines)
		return;
		
	UnlockVB();

	// 
	SetState();
	m_pDevice->Draw(EDraw_Lines, 0, m_VertexCount/2);
}

// triangles
bool C2DUtils::Triangle_Start()
{
	if(m_Mode != EMode_Idle)
		return false;

	if(!LockVB(EMode_Triangles))
		return false;
	return true;
}

void C2DUtils::Triangle_DrawTriangle(fp32 _X0, fp32 _Y0, fp32 _X1, fp32 _Y1, fp32 _X2, fp32 _Y2)
{
	if(m_Mode != EMode_Triangles)
		return;

	/*
	C2DClip Clip = GetClip();
	// Clip triangle vertex
	if (Clip.Clip(TVector2<fp32>(_X0,_Y0),TVector2<fp32>(_X1,_Y1), TVector2<fp32>(_X2,_Y2)))
		return;*/

	// add stuff to the VB
	m_pVertexBuffer->m_pVertexfp32[m_VertexCount].Set(_X0,_Y0,-1);
	m_pVertexBuffer->m_pColorfp32[m_VertexCount].Set(m_aColors[0][0],m_aColors[0][1],m_aColors[0][2],m_aColors[0][3]);
	m_VertexCount++;
	m_pVertexBuffer->m_pVertexfp32[m_VertexCount].Set(_X1,_Y1,-1);
	m_pVertexBuffer->m_pColorfp32[m_VertexCount].Set(m_aColors[0][0],m_aColors[0][1],m_aColors[0][2],m_aColors[0][3]);
	m_VertexCount++;
	m_pVertexBuffer->m_pVertexfp32[m_VertexCount].Set(_X2,_Y2,-1);
	m_pVertexBuffer->m_pColorfp32[m_VertexCount].Set(m_aColors[0][0],m_aColors[0][1],m_aColors[0][2],m_aColors[0][3]);
	m_VertexCount++;
}

void C2DUtils::Triangle_End()
{
	if(m_Mode != EMode_Triangles)
		return;

	UnlockVB();

	// 
	SetState();
	m_pDevice->Draw(EDraw_Triangles, 0, m_VertexCount/3);
}


//
//
//
bool C2DUtils::Quad_Begin()
{
	if(m_Mode != EMode_Idle)
		return false;

	if(!LockVB(EMode_Quads))
		return false;

	return true;
}

//
//
//
void C2DUtils::Quad_Draw(fp32 _x, fp32 _y, fp32 _w, fp32 _h)
{
	Quad_DrawTL(_x-_w/2, _y-_h/2, _w, _h);
}

// Top left version
void C2DUtils::Quad_DrawTL(fp32 _x, fp32 _y, fp32 _w, fp32 _h)
{
	if(m_Mode != EMode_Quads)
		return;

	TVector2<fp32> Center(_x + _w/2, _y + _h/2);

	TVector2<fp32> aVectors[4];
	aVectors[0].Set(_x, _y);
	aVectors[1].Set(_x+_w, _y);
	aVectors[2].Set(_x, _y+_h);
	aVectors[3].Set(_x+_w, _y+_h);

	for(int32 i = 0; i < 4; i++)
	{
		TVector2<fp32> Temp = aVectors[i] - Center;
		Temp.Rotate(m_QuadRotation);
		aVectors[i] = Temp + Center;
	}

	//if(m_UseClip)
	for(int32 i = 0; i < 4; i++)
		aVectors[i] += m_GlobalTranslate;

	/*
	C2DClip Clip = GetClip();
	// Clip quad
	if (Clip.Clip(aVectors[0],aVectors[1],aVectors[2],aVectors[3]))
		return;
		*/

	// add stuff to the VB
	m_pVertexBuffer->m_pVertexfp32[m_VertexCount].Set(aVectors[0].x,aVectors[0].y,-1);
	m_pVertexBuffer->m_pTexturefp32[0][m_VertexCount].Set(m_aTexCoords[0][0], m_aTexCoords[0][1]);
	m_pVertexBuffer->m_pColorfp32[m_VertexCount].Set(m_aColors[0][0],m_aColors[0][1],m_aColors[0][2],m_aColors[0][3]);
	m_VertexCount++;

	m_pVertexBuffer->m_pVertexfp32[m_VertexCount].Set(aVectors[1].x,aVectors[1].y,-1);
	m_pVertexBuffer->m_pTexturefp32[0][m_VertexCount].Set(m_aTexCoords[1][0], m_aTexCoords[1][1]);
	m_pVertexBuffer->m_pColorfp32[m_VertexCount].Set(m_aColors[1][0],m_aColors[1][1],m_aColors[1][2],m_aColors[1][3]);
	m_VertexCount++;

	m_pVertexBuffer->m_pVertexfp32[m_VertexCount].Set(aVectors[2].x,aVectors[2].y,-1);
	m_pVertexBuffer->m_pTexturefp32[0][m_VertexCount].Set(m_aTexCoords[2][0], m_aTexCoords[2][1]);
	m_pVertexBuffer->m_pColorfp32[m_VertexCount].Set(m_aColors[2][0],m_aColors[2][1],m_aColors[2][2],m_aColors[2][3]);
	m_VertexCount++;



	m_pVertexBuffer->m_pVertexfp32[m_VertexCount].Set(aVectors[1].x,aVectors[1].y,-1);
	m_pVertexBuffer->m_pTexturefp32[0][m_VertexCount].Set(m_aTexCoords[1][0], m_aTexCoords[1][1]);
	m_pVertexBuffer->m_pColorfp32[m_VertexCount].Set(m_aColors[1][0],m_aColors[1][1],m_aColors[1][2],m_aColors[1][3]);
	m_VertexCount++;

	m_pVertexBuffer->m_pVertexfp32[m_VertexCount].Set(aVectors[3].x,aVectors[3].y,-1);
	m_pVertexBuffer->m_pTexturefp32[0][m_VertexCount].Set(m_aTexCoords[3][0], m_aTexCoords[3][1]);
	m_pVertexBuffer->m_pColorfp32[m_VertexCount].Set(m_aColors[3][0],m_aColors[3][1],m_aColors[3][2],m_aColors[3][3]);
	m_VertexCount++;

	m_pVertexBuffer->m_pVertexfp32[m_VertexCount].Set(aVectors[2].x,aVectors[2].y,-1);
	m_pVertexBuffer->m_pTexturefp32[0][m_VertexCount].Set(m_aTexCoords[2][0], m_aTexCoords[2][1]);
	m_pVertexBuffer->m_pColorfp32[m_VertexCount].Set(m_aColors[2][0],m_aColors[2][1],m_aColors[2][2],m_aColors[2][3]);
	m_VertexCount++;
}

//
//
//
void C2DUtils::Quad_End()
{
	if(m_Mode != EMode_Quads)
		return;
		
	UnlockVB();

	SetState();

	//
	if(m_UseClip)
	{
		m_pDevice->SetScissors((int32)m_ClipRect.m_Position.x, (int32)m_ClipRect.m_Position.y,
								(int32)m_ClipRect.m_Size.x, (int32)m_ClipRect.m_Size.y);
		m_pDevice->Flag_Set(EFlag_ScissorTest, true);
	}
	
	m_pDevice->Draw(EDraw_Triangles, 0, m_VertexCount/3);
	if(m_UseClip)
	{
		m_pDevice->SetScissors(0, 0, m_ScreenWidth, m_ScreenHeight);
		m_pDevice->Flag_Set(EFlag_ScissorTest, false);
		//m_AreaX = 640;
		//m_AreaY = 480;
	}
}

//
//
//
void C2DUtils::Text_Begin(CFont *_pFont)
{
	m_pFont = _pFont;
	//SetDefaults();
	SetTexture(m_pFont->m_iTexture);
	SetBlend(EBlend_Alpha);
	Quad_Begin();
}

//
//
//
void C2DUtils::Text_Draw(fp32 _x, fp32 _y, int32 _Flags, const int8 *_pFormat, ...)
{
	int8 aBuffer[1024*4];
	StringCFormatArgList(aBuffer, &_pFormat);

	fp32 w = 1.0f/m_pFont->m_XCells;
	fp32 h = 1.0f/m_pFont->m_YCells;

	uint8 *pCurrent = (uint8 *)aBuffer;
	while(*pCurrent)
	{
		uint8 c = m_pFont->m_aTranslationTable[*pCurrent];
		
		if(c == '\a')
		{
			pCurrent++;
			fp32 colors[4] = {1};
			for(int32 i = 0; i < 4 && *pCurrent; i++)
			{
				c = *pCurrent;
				pCurrent++;
				if(c >= '0' && c <= '9')
					colors[i] = (c-'0')/(fp32)0xf;
				else if(c >= 'a' && c <= 'f')
					colors[i] = (c-'a'+10)/(fp32)0xf;
			}
			SetColor(colors[0], colors[1], colors[2], colors[3]);
			continue;
		}

		fp32 ix = (c%m_pFont->m_XCells)*w;
		fp32 iy = (c/m_pFont->m_XCells)*h;
		SetTexCoord(ix, iy, ix+w, iy+h);
		if (_Flags & ETextDraw_TL)
			Quad_DrawTL(_x, _y, m_pFont->m_Width, m_pFont->m_Height);
		else
			Quad_Draw(_x, _y, m_pFont->m_Width, m_pFont->m_Height);
		_x += m_pFont->m_Spacing;
		pCurrent++;
	}
}

//
//
//
void C2DUtils::Text_End()
{
	Quad_End();
}



void C2DUtils::SetState()
{
	m_pDevice->State_SetDefaults(false);
	m_pDevice->Flag_Set(EFlag_DepthTest, false);
	m_pDevice->Flag_Set(EFlag_Cull, false);
	
	m_pDevice->Stream_Set(m_iVertexBuffer);
	m_pDevice->Matrix_Projection(NMatrix::Ortho(0, m_ScreenWidth, 0, m_ScreenHeight, 2, 0));
	m_pDevice->Texture_Set(0, m_iTexture);
	m_pDevice->Matrix_Model(NMatrix::Translation(m_NudgeX, m_NudgeY, 0));
	m_pDevice->Mode_WriteMask(m_aWriteMask[0], m_aWriteMask[1], m_aWriteMask[2], m_aWriteMask[3]);
	
	if(m_Blend == EBlend_None)
		m_pDevice->Flag_Set(EFlag_Blend, false);
	else
		m_pDevice->Flag_Set(EFlag_Blend, true);

	if(m_Blend == EBlend_Alpha)
		m_pDevice->Mode_BlendFunc(N3D::EBlend_SrcAlpha, N3D::EBlend_InvSrcAlpha);

	if(m_Blend == EBlend_Add)
		m_pDevice->Mode_BlendFunc(N3D::EBlend_SrcAlpha, N3D::EBlend_One);

	if(m_Blend == EBlend_DstAlpha)
		m_pDevice->Mode_BlendFunc(N3D::EBlend_InvDstAlpha, N3D::EBlend_DstAlpha);
}



