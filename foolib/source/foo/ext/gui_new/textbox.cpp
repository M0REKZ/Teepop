#include "textbox.h"
#include "gui.h"

using namespace NFoo;
using namespace NFoo::NGUI;

//
// a textbox
//
CTextBox::CTextBox(CView *pParent, const int8 *pName, CGUI *pGUI)
: CView(pParent, pName, pGUI)
{
	m_ViewStart = 0;
	m_CursorPos = 0;
	SetFlags(EFlag_TabStop);
	m_HideType = 0;
}

//
//
//
void CTextBox::CursorPosCheck()
{
	if(m_CursorPos < 0)
		m_CursorPos = 0;
	if(m_CursorPos > m_Text.Len())
		m_CursorPos = m_Text.Len();
	
	if(m_CursorPos < m_ViewStart)
		m_ViewStart = m_CursorPos;

	if(m_CursorPos+1 > m_ViewStart+GetSize().x/6)
		m_ViewStart = (m_CursorPos+1)-GetSize().x/6;
	
	if(m_ViewStart < 0)
		m_ViewStart = 0;
}

void CTextBox::SetHideType(bool hide)
{
	m_HideType = hide;
}

//
// 
void CTextBox::OnMouseDown(CPoint _Pos, int32 _Button)
{
	m_CursorPos = (_Pos-GetWorldPosition()).x/6+m_ViewStart;
	CursorPosCheck();
}

//
// 
//
void CTextBox::OnKeyPress(int32 _Key, int8 _Char, bool Repeat)
{
	int8 aBuf[2] = {_Char, 0};

	if(_Key == NCore::EBtn_Key_Left)
		m_CursorPos--;
	else if(_Key == NCore::EBtn_Key_Right)
		m_CursorPos++;
	else if(_Key == NCore::EBtn_Key_Backspace)
	{
		if(m_CursorPos > 0)
		{
			m_Text = m_Text.Delete(m_CursorPos-1, 1);
			m_CursorPos--;
		}
	}
	else if(_Key == NCore::EBtn_Key_Delete)
	{
		if(m_Text.Len() > m_CursorPos)
			m_Text = m_Text.Delete(m_CursorPos, 1);
	}
	else if(_Key == NCore::EBtn_Key_Tab)
	{
		CView::OnKeyPress(_Key, _Char, Repeat);
		return;
	}
	else if(_Key == NCore::EBtn_Key_Enter)
		Signal(EEvent_Enter);
	else if(_Char != 0)
	{
		if(m_Text.Len() == m_CursorPos)
		{
			m_Text += aBuf;
			m_CursorPos++;
		}
		else
		{
			CString After = m_Text.GetData() + m_CursorPos;
			m_Text = m_Text.Delete(m_CursorPos, m_Text.Len()-m_CursorPos);
			m_Text += aBuf;
			m_Text += After;
			m_CursorPos++;
		}
	}

	CursorPosCheck();
}

//
//
//
CString CTextBox::GetText() { return m_Text; }
void CTextBox::SetText(CString _s)  { m_Text = _s; CursorPosCheck(); }

//
// 
//
void CTextBox::OnDraw(CDrawer &_rDrawer)
{
	_rDrawer.DrawRect(_rDrawer.m_aRectTypes[1], 0, 0, GetSize().x, GetSize().y);
	
	if(m_ViewStart > m_Text.Len())
		m_ViewStart = m_Text.Len()-1;

	if(m_HideType)
	{
		CString Text;
		for(int32 i = 0; i < m_Text.Len(); i++)
			Text += "*";
		_rDrawer.DrawText(0, 0, Text.GetData()+m_ViewStart);
	}
	else
		_rDrawer.DrawText(0, 0, m_Text.GetData()+m_ViewStart);

	if(IsFocused())
		_rDrawer.DrawPart(_rDrawer.m_TextCursor, CDrawer::EOrigo_TL, m_CursorPos*6+3-m_ViewStart*6, 3);
	
	CView::OnDraw(_rDrawer);
}

//
//
//
void CTextBox::Set(CString &rSetting, CString &rValue)
{
	if(rSetting == "text")
		SetText(rValue);
	else if(rSetting == "onenter")
		Bind(EEvent_Enter, m_pGUI->GetMapping(rValue));
	else if(rSetting == "hidetype")
		SetHideType(true);
	else
		CView::Set(rSetting, rValue);
}

//
//
//
CLabel::CLabel(CView *pParent, const int8 *pName, CGUI *pGUI)
: CView(pParent, pName, pGUI)
{

}

CString CLabel::GetText() { return m_Text; }
void CLabel::SetText(CString _s)  { m_Text = _s; }

void CLabel::OnDraw(CDrawer &_rDrawer)
{
	//_rDrawer.DrawRect(_rDrawer.m_aRectTypes[1], 0, 0, GetSize().x, GetSize().y);
	_rDrawer.DrawText(0, 0, m_Text.GetData());

	//if(IsFocused())
	//	_rDrawer.DrawPart(_rDrawer.m_TextCursor, CDrawer::EOrigo_TL, m_CursorPos*6+3, 3);
	
	CView::OnDraw(_rDrawer);
}


//
//
//
void CLabel::Set(CString &rSetting, CString &rValue)
{
	if(rSetting == "text")
		SetText(rValue);
	else
		CView::Set(rSetting, rValue);
}

/*
	class CLabel : public CView
	{
		CString m_Text;
	protected:
		virtual void OnDraw(CDrawer &_rDrawer);
	public:
		CLabel(CView *pParent=NULL, CGUI *pGUI=NULL);
		CString GetText();
		void SetText(CString _s);
	};
*/
