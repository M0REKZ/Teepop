#include "scrollbar.h"
#include "gui.h"

using namespace NFoo;
using namespace NFoo::NGUI;

//
//
//
CScrollBar::CHandle::CHandle(CView *pParent, CGUI *pGUI)
: CView(pParent, NULL, pGUI)
{
	m_Scrubbing = false;
	m_Amount = 0;

	SetGeometry(2,2, m_pGUI->m_Drawer.m_ScrollbarHandle.w, m_pGUI->m_Drawer.m_ScrollbarHandle.h);
}

//
//
//
void CScrollBar::CHandle::OnMouseDown(CPoint _Pos, int32_t _iButton)
{
	m_Offset = _Pos.y-GetWorldPosition().y;
	m_pGUI->CaptureMouse(this);
	m_Scrubbing = true;
}

//
//
//
void CScrollBar::CHandle::OnMouseUp(CPoint _Pos, int32_t _iButton)
{
	m_pGUI->ReleaseMouse();
	m_Scrubbing = false;
}

//
//
//
void CScrollBar::CHandle::OnMouseMove(CPoint _Pos)
{
	if(!m_Scrubbing)
		return;

	int32_t y = _Pos.y-GetParent()->GetWorldPosition().y-m_Offset;
	if(y < GetMin()) y = GetMin();
	if(y > GetMax()) y = GetMax();

	SetPosition(GetPosition().x, y);
}

//
//
//
void CScrollBar::CHandle::SetPos(int32_t p) { SetPosition(GetPosition().x, p + GetMin()); }
int32_t CScrollBar::CHandle::GetPos() { return GetPosition().y - GetMin(); }
int32_t CScrollBar::CHandle::GetMin() { return 2; }
int32_t CScrollBar::CHandle::GetMax(int32_t py)
{
	if(py == -1)
		py = GetParent()->GetSize().y;
		
	return py - m_pGUI->m_Drawer.m_ScrollbarButtons.h-m_pGUI->m_Drawer.m_ScrollbarHandle.h-2;
}

//
//
//
void CScrollBar::CHandle::OnDraw(CDrawer &_rDrawer)
{
	//_rDrawer.Begin();
	_rDrawer.DrawPart(_rDrawer.m_ScrollbarHandle, CDrawer::EOrigo_TL, 0, m_Amount);
	//_rDrawer.End();

	if(IsMouseOver())
	{
		//_rDrawer.Begin(N3D::C2DUtils::EBlend_Add, 1,1,1,0.1f);
		_rDrawer.DrawPart(_rDrawer.m_ScrollbarHandle, CDrawer::EOrigo_TL, 0, m_Amount);
		//_rDrawer.End();
	}

}

//
//
//
int32_t CScrollBar::ScrollArea(int32_t h)
{
	return m_Handle.GetMax(h)-m_Handle.GetMin();
}

//
//
//
CScrollBar::CScrollBar(CView *pParent, const int8 *pName, CGUI *pGUI)
: CView(pParent, pName, pGUI), m_Handle(NULL, pGUI ? pGUI : (pParent != NULL ? pParent->GetGUI(): NULL))
{
	AddChild(&m_Handle);
	m_Handle.SetFlags(EFlag_Indestructable);
	SetSize(14, 100);
	SetAmount(0);
}

//
//
//
void CScrollBar::OnSize(CPoint _OldSize)
{
	int32_t c = m_Handle.GetPos();
	int32_t m = ScrollArea(_OldSize.y);
	SetAmount(c/(fp32)m);
}

//
//
//
void CScrollBar::SetAmount(fp32 _a)
{
	if(_a < 0) _a = 0;
	if(_a > 1) _a = 1;
	int32_t m = ScrollArea(GetSize().y);
	int32_t c = (int32_t)(m*_a);
	m_Handle.SetPos(c);
}

//
//
//
fp32 CScrollBar::GetAmount()
{
	int32_t c = m_Handle.GetPos();
	int32_t m = ScrollArea(GetSize().y);
	return c/(fp32)m;
}

//
//
//
void CScrollBar::OnDraw(CDrawer &_rDrawer)
{
	_rDrawer.DrawRect(_rDrawer.m_aRectTypes[1], 0, 0, GetSize().x, GetSize().y);
	_rDrawer.DrawPart(_rDrawer.m_ScrollbarButtons, CDrawer::EOrigo_BL, 0, 0+GetSize().y);

	CView::OnDraw(_rDrawer);
}
