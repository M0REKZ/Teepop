#include "drawer.h"

using namespace NFoo;
using namespace NFoo::NGUI;

//
//
//
void CDrawer::CPart::Set(int32_t _x, int32_t _y, int32_t _w, int32_t _h)
{
	x = _x;
	y = _y;
	w = _w;
	h = _h;
}

//
//
//
void CDrawer::CRectParts::Init(int32_t _x, int32_t _y)
{
	m_aParts[ECornerTL].Set(0,0,4,4);
	m_aParts[ECornerTR].Set(20,0,4,4);
	m_aParts[ECornerBL].Set(0,20,4,4);
	m_aParts[ECornerBR].Set(20,20,4,4);

	m_aParts[EBorderT].Set( 5, 0,14, 4);
	m_aParts[EBorderB].Set( 5,20,14, 4);
	m_aParts[EBorderL].Set( 0, 5, 4,14);
	m_aParts[EBorderR].Set(20, 5, 4,14);

	m_aParts[EFill].Set(5,5,14,14);

	for(int32_t i = 0; i < ENum; i++)
		m_aParts[i].x += _x, m_aParts[i].y += _y;
		
}

//
//
//
void CDrawer::Init()
{
	m_aRectTypes[0].Init(0,0);
	m_aRectTypes[1].Init(0,16+8);

	//m_ScrollbarButtons.Set(48, 0, 14, 24);
	m_ScrollbarButtons.Set(48, 0, 0, 0);
	m_ScrollbarHandle.Set(54, 24, 10, 17);

	m_aCheckBoxStates[0].Set(0,48,8,8);
	m_aCheckBoxStates[1].Set(0,56,8,8);

	m_WindowCloseButton.Set(39,24,14,13);

	m_aSeparatorParts[0].Set(8,48,3,4);
	m_aSeparatorParts[0].Set(8,52,3,8);
	m_aSeparatorParts[2].Set(8,60,3,4);

	m_Cursor.Set(24,38,15,14);
	m_TextCursor.Set(11, 48, 3, 11);
}

//
//
//
void CDrawer::SetUV(int32_t _u, int32_t _v, int32_t _w, int32_t _h)
{
	m_pPainter->SetUV(_u, _v, _w, _h);
}

//
//
//
void CDrawer::DrawPart(CPart &_rPart, int32_t _Origo, int32_t _x, int32_t _y, int32_t _w, int32_t _h)
{
	SetUV(_rPart.x, _rPart.y, _rPart.w, _rPart.h);

	if(_w < 0)
		_w = _rPart.w;
	if(_h < 0)
		_h = _rPart.h;

	if(_Origo == EOrigo_TL)
		Draw(_x, _y, _w, _h);
	else if(_Origo == EOrigo_TR)
		Draw(_x-_w, _y, _w, _h);
	else if(_Origo == EOrigo_BL)
		Draw(_x, _y-_h, _w, _h);
	else if(_Origo == EOrigo_BR)
		Draw(_x-_w, _y-_h, _w, _h);
}

//
//
//
void CDrawer::DrawRect(CRectParts &_rRect, int32_t _x, int32_t _y, int32_t _w, int32_t _h)
{
	DrawPart(_rRect.m_aParts[CRectParts::EFill], EOrigo_TL,
				_x+_rRect.m_aParts[CRectParts::ECornerTL].w,
				_y+_rRect.m_aParts[CRectParts::ECornerTL].h,
				_w-_rRect.m_aParts[CRectParts::ECornerTL].w-_rRect.m_aParts[CRectParts::ECornerBR].w,
				_h-_rRect.m_aParts[CRectParts::ECornerTL].h-_rRect.m_aParts[CRectParts::ECornerBR].h);

	DrawPart(_rRect.m_aParts[CRectParts::EBorderT], EOrigo_TL, _x+4, _y, _w-8);
	DrawPart(_rRect.m_aParts[CRectParts::EBorderB], EOrigo_BL, _x+4, _y+_h, _w-8);

	DrawPart(_rRect.m_aParts[CRectParts::EBorderL], EOrigo_TL, _x, _y+4, -1, _h-8);
	DrawPart(_rRect.m_aParts[CRectParts::EBorderR], EOrigo_TR, _x+_w, _y+4, -1, _h-8);

	DrawPart(_rRect.m_aParts[CRectParts::ECornerTL], EOrigo_TL, _x, _y);
	DrawPart(_rRect.m_aParts[CRectParts::ECornerTR], EOrigo_TR, _x+_w, _y);
	DrawPart(_rRect.m_aParts[CRectParts::ECornerBL], EOrigo_BL, _x, _y+_h);
	DrawPart(_rRect.m_aParts[CRectParts::ECornerBR], EOrigo_BR, _x+_w, _y+_h);
}

//
//
//
void CDrawer::Draw(int32_t _x, int32_t _y, int32_t _w, int32_t _h)
{
	m_pPainter->DrawRect(_x, _y, _w, _h);
}

//
//
//
/*
void CDrawer::End()
{
	//m_pUtils->Quad_End();
}*/

//
//
//
void CDrawer::DrawText(int32_t _x, int32_t _y, const char *_pText, fp32 _r, fp32 _g, fp32 _b)
{
	m_pPainter->DrawText(_x, _y, _pText, _r, _g, _b);
	/*
	m_pUtils->SetDefaults();
	m_pUtils->SetBlend(N3D::C2DUtils::EBlend_Alpha);
	m_pUtils->SetColor(121/255.0f, 108/255.0f, 92/255.0f, 1);
	
	m_pUtils->Text_Begin(&m_Font);
	m_pUtils->Text_Draw((fp32)_x, (fp32)_y, N3D::C2DUtils::ETextDraw_TL, "%s", _pText);
	m_pUtils->Text_End();
	*/
}

void CDrawer::SetGlobalTranslate(int32_t _x, int32_t _y)
{
	m_pPainter->SetGlobalTranslate(_x, _y);
}

void CDrawer::SetClipping(bool _Use, int32_t _x, int32_t _y, int32_t _w, int32_t _h)
{
	m_pPainter->SetClipping(_Use, _x, _y, _w, _h);
}

void CPainter_2DUtils::Init()
{
	m_Font.BuildNormalAscii();
	//m_Font.m_iTexture = -1;
	//m_Font.m_iTexture = m_pInterface->GetTexture("data/font1");
	m_Font.m_XCells = 16;
	m_Font.m_YCells = 16;
	m_Font.m_Width = 16;
	m_Font.m_Height = 16;
	m_Font.m_Spacing = 6;
}

int32_t CPainter_2DUtils::CreateImage(CImage &_Image)
{
	return 1;
}

void CPainter_2DUtils::SetImage(int32_t _iImage)
{
}

void CPainter_2DUtils::Begin()
{
}

void CPainter_2DUtils::End()
{
}



void CPainter_2DUtils::SetGlobalTranslate(int32_t _x, int32_t _y)
{
	m_pUtils->SetGlobalTranslate(_x, _y);
}

void CPainter_2DUtils::SetClipping(bool _Use, int32_t _x, int32_t _y, int32_t _w, int32_t _h)
{
	m_pUtils->SetClipping(_Use, _x, _y, _w, _h);
}


void CPainter_2DUtils::SetUV(int32_t _u, int32_t _v, int32_t _w, int32_t _h)
{
	m_pUtils->SetTexCoord(  (fp32)_u/(fp32)m_TextureWidth,
							(fp32)_v/(fp32)m_TextureHeight,
							(fp32)_u/(fp32)m_TextureWidth+(fp32)_w/(fp32)m_TextureWidth,
							(fp32)_v/(fp32)m_TextureHeight+(fp32)_h/(fp32)m_TextureHeight);
}

void CPainter_2DUtils::DrawRect(int32_t _x, int32_t _y, int32_t _w, int32_t _h)
{
	m_pUtils->SetTexture(m_iTexture);
	m_pUtils->SetBlend(N3D::C2DUtils::EBlend_Alpha);
	m_pUtils->SetColor(1, 1, 1, 1);
	m_pUtils->Quad_Begin();
	m_pUtils->Quad_DrawTL(_x, _y, _w, _h);
	m_pUtils->Quad_End();
}

void CPainter_2DUtils::DrawText(int32_t _x, int32_t _y, const char *_pText, fp32 _r, fp32 _g, fp32 _b)
{
	m_pUtils->SetDefaults();
	m_pUtils->SetBlend(N3D::C2DUtils::EBlend_Alpha);
	//m_pUtils->SetColor(121/255.0f, 108/255.0f, 92/255.0f, 1);
	m_pUtils->SetColor(_r, _g, _b, 1);
	
	m_pUtils->Text_Begin(&m_Font);
	m_pUtils->Text_Draw((fp32)_x, (fp32)_y, N3D::C2DUtils::ETextDraw_TL, "%s", _pText);
	m_pUtils->Text_End();
}
