#include "window.h"
#include "gui.h"

using namespace NFoo;
using namespace NFoo::NGUI;

CMenu::CMenu(CView *pParent, const char *pName, CGUI *pGUI)
: CView(pParent, pName, pGUI)
{

}


CPoint CMenu::FindDimentions()
{
	CPoint Size(0,0);
	for(int32_t i = 0; i < m_lItems.Len(); i++)
	{
		if(m_lItems[i]->GetSize().x > Size.x)
			Size.x = m_lItems[i]->GetSize().x;
		Size.y += m_lItems[i]->GetSize().y;
	}
	return Size;
}

void CMenu::Activate(CPoint _Pos)
{
	CPoint Dims = FindDimentions();
	SetSize(Dims.x, Dims.y);
	m_pGUI->CaptureMouse(this);
}

void CMenu::Deactivate()
{
	m_pGUI->ReleaseMouse();
}

void CMenu::AddItem(CMenuItem *_pItem)
{

}

//
// frame
//
CFrame::CFrame(CView *pParent, const char *pName, CGUI *pGUI)
: CView(pParent, pName, pGUI)
{
}

void CFrame::OnDraw(CDrawer &rDrawer)
{
	rDrawer.DrawRect(rDrawer.m_aRectTypes[0], 0, 0, GetSize().x, GetSize().y);
}

//
//
//
CWindow::CWindow(CView *pParent, const char *pName, CGUI *pGUI)
: CView(pParent, pName, pGUI)
{
	m_Draging = false;
	m_Sizing = false;
	m_Caption = "Window";
}

//
//
//
void CWindow::SetCaption(const char *_pCap)
{
	m_Caption = _pCap;
}

//
//
//
void CWindow::OnMouseDown(CPoint _Pos, int32_t _iButton)
{
	MoveToFront();
	
	m_DragStart = _Pos-GetWorldPosition();

	if(m_DragStart.y < EHeaderSize)
	{
		m_pGUI->CaptureMouse(this);
		m_Draging = true;
	}
	else if(m_DragStart.y > GetSize().y-14 && m_DragStart.x > GetSize().x-14)
	{
		m_DragStart = GetSize() - (_Pos-GetWorldPosition());
		m_pGUI->CaptureMouse(this);
		m_Sizing = true;
	}
}

//
//
//
void CWindow::OnMouseUp(CPoint _Pos, int32_t _iButton)
{
	m_pGUI->ReleaseMouse();
	m_Draging = false;
	m_Sizing = false;
}

//
//
//
void CWindow::OnMouseMove(CPoint _Pos)
{
	if(m_Draging)
		SetPosition(_Pos-GetParent()->GetWorldPosition()-m_DragStart);
	else if(m_Sizing)
		SetSize((_Pos-GetWorldPosition())+m_DragStart);
}

//
//
//
void CWindow::OnDraw(CDrawer &_rDrawer)
{
	_rDrawer.DrawRect(_rDrawer.m_aRectTypes[0], 0, 0, GetSize().x, EHeaderSize);
	_rDrawer.DrawRect(_rDrawer.m_aRectTypes[0], 0, EHeaderSize-1, GetSize().x, GetSize().y-EHeaderSize);
	_rDrawer.DrawPart(_rDrawer.m_WindowCloseButton, CDrawer::EOrigo_TR, GetSize().x, 0);
	_rDrawer.DrawText(0, -2, m_Caption.GetData());

	CView::OnDraw(_rDrawer);
}
