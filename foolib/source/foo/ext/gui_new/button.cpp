#include "button.h"
#include "gui.h"

using namespace NFoo;
using namespace NFoo::NGUI;

//
//
//
CButton::CButton(CView *pParent, const char *pName, CGUI *pGUI)
: CView(pParent, pName, pGUI)
{
	m_Pressed = false;
}

//
//
//
void CButton::OnMouseDown(CPoint _Pos, int32_t _iButton)
{
	m_pGUI->CaptureMouse(this);
	m_Pressed = true;
}

//
//
//
void CButton::OnMouseUp(CPoint _Pos, int32_t _iButton)
{
	m_pGUI->ReleaseMouse();
	m_Pressed = false;
	Signal(EEvent_Clicked);
}

//
//
//
void CButton::SetCaption(const char *_pCap)
{
	m_Caption = _pCap;
}

//
//
//
void CButton::OnDraw(CDrawer &_rDrawer)
{
	if(m_Pressed)
		_rDrawer.DrawRect(_rDrawer.m_aRectTypes[1], 0, 0, GetSize().x, GetSize().y);
	else
		_rDrawer.DrawRect(_rDrawer.m_aRectTypes[0], 0, 0, GetSize().x, GetSize().y);
	_rDrawer.DrawText((int32_t)(GetSize().x/2-(m_Caption.Len()/2.0f)*8), GetSize().y/2-8, m_Caption.GetData());

	CView::OnDraw(_rDrawer);
}

//
//
//
void CButton::Set(CString &rSetting, CString &rValue)
{
	if(rSetting == "caption")
		SetCaption(rValue);
	else if(rSetting == "onclicked")
		Bind(EEvent_Clicked, m_pGUI->GetMapping(rValue));
	else
		CView::Set(rSetting, rValue);
}