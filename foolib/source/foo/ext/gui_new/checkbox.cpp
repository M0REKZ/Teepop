#include "checkbox.h"
#include "gui.h"

using namespace NFoo;
using namespace NFoo::NGUI;

//
//
//
CCheckBox::CCheckBox(CView *pParent, const char *pName, CGUI *pGUI)
: CView(pParent, pName, pGUI)
{
	m_Checked = false;
	SetSize(m_pGUI->m_Drawer.m_aCheckBoxStates[0].w, m_pGUI->m_Drawer.m_aCheckBoxStates[0].h);
}

//
//
//
void CCheckBox::OnMouseDown(CPoint _Pos, int32_t _iButton)
{
	m_Checked = !m_Checked;
}

//
//
//
void CCheckBox::OnDraw(CDrawer &_rDrawer)
{
	if(m_Checked)
		_rDrawer.DrawPart(_rDrawer.m_aCheckBoxStates[1], CDrawer::EOrigo_TL, 0, 0);
	else
		_rDrawer.DrawPart(_rDrawer.m_aCheckBoxStates[0], CDrawer::EOrigo_TL, 0, 0);
	CView::OnDraw(_rDrawer);
}
