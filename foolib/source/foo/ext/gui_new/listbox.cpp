
#include <foo/base/string.h>
#include "gui.h"
#include "listbox.h"

using namespace NFoo;
using namespace NFoo::NGUI;

//
//
//
void CListItem::SetWidth(int32_t _w) { m_Width = _w; OnWidthChange(); }
int32_t CListItem::GetHeight() { return m_Height; }

//
//
//
CStringItem::CStringItem()
{
	m_Multiline = false;
}

CStringItem::CStringItem(const int8 *_pString, bool _Multiline)
{
	m_Text = _pString;
	m_Multiline = _Multiline;
}


int32_t GetLines(int32_t width, int32_t w, const int8 *pText, TDynamicArray<CString> *pLines=NULL)
{
	//enum { ESeek, EFlush }
	const int8 *pLineStart = pText;
	const int8 *pLineEnd = pText;
	int32_t CurrentWidth = width;
	int32_t NumLines = 0;
	//int32_t State = ESeek;

	while(true)
	{
		bool Flush = false;
		CurrentWidth -= w;
		if(CurrentWidth > w)
		{
			pText++;
			if(!*pText)
			{
				pLineEnd = pText;
				Flush = true;
			}
			else if(IsSpace(*pText))
				pLineEnd = pText;
		}
		else 
			Flush = true;
		
		if(Flush)
		{
			NumLines++;

			if(pLines)
			{
				// flush
				if(pLineEnd == pLineStart)
					pLineEnd = pText;

				CString Line;
				Line.CopyData(pLineStart, pLineEnd-pLineStart);
				pLines->Add(Line);
			}

			pLineStart = pLineEnd;
			CurrentWidth = width;
			if(!*pText)
				break;
		};

	}
	return NumLines;
}

//
//
//
void CStringItem::OnWidthChange()
{
	if(m_Multiline)
		m_Height = GetLines(m_Width,6,m_Text,NULL)*12;
	else
		m_Height = 12;
}

//
//
//
void CStringItem::OnDraw(CDrawer &rDrawer, bool Selected)
{
	fp32 r=1, g=1, b=1;
	if(Selected)
		r=1, g=0.7f, b=0.7f;

	if(m_Multiline)
	{
		TDynamicArray<CString> lLines;
		GetLines(m_Width,6,m_Text,&lLines);

		for(int32_t i = 0; i < lLines.Len(); i++)
			rDrawer.DrawText(-2, -3+i*12, lLines[i], r, g, b);
	}
	else
		rDrawer.DrawText(-2, -3, m_Text.GetData(), r, g, b);
}


//
//
//
CListBox::CListBox(CView *pParent, const int8 *pName, CGUI *pGUI)
: CView(pParent, pName, pGUI), m_ScrollBar(NULL, NULL, pGUI)
{
	SetSize(100, 100);
	m_ScrollBar.SetFlags(EFlag_Indestructable);
	m_ScrollBar.SetGeometry(100-m_ScrollBar.GetSize().x, 0, m_ScrollBar.GetSize().x, 100);
	m_ScrollBar.SetResizeMode(EFollow_Right|EFollow_Top_Bottom);
	AddChild(&m_ScrollBar);
}


CListBox::~CListBox()
{
	Clear();
}

//
//
//
int32_t CListBox::GetStartY()
{
	int32_t iHeight = m_ItemsHeight;
	//if(m_lItems.Len())
	//	iHeight += m_lItems[m_lItems.Len()-1]->GetHeight();
		
	int32_t y = (int32_t)((iHeight-GetSize().y)*m_ScrollBar.GetAmount());
	if(y < 0)
		y = 0;
	return y;
}

//
//
//
int32_t CListBox::GetItemsHeight()
{
	int32_t h = 0;
	for(int32_t i = 0; i < m_lItems.Len(); i++)
		h += m_lItems[i]->GetHeight();
	return h;
}

//
//
//
void CListBox::AddItem(CListItem *_pItem)
{
	m_lItems.Add(_pItem);
	OnSize();
}


//
//
//
void CListBox::Clear()
{
	m_lItems.DeleteAll();
	OnSize();
}

//
//
//
void CListBox::OnMouseDown(CPoint Pos, int32_t Button)
{
	CPoint WorldPos = GetWorldPosition();
	int32_t y = -GetStartY()-(Pos.y-WorldPos.y);

	Signal(EEvent_Select);

	for(int32_t i = 0; i < m_lItems.Len(); i++)
	{   
		int32_t h = m_lItems[i]->GetHeight();
		y += h;
		if(y > 0)
		{
			m_SelectedItem = i;
			return;
		}
	}
	
	m_SelectedItem = -1;
}

void CListBox::OnDoubleClick(CPoint _Pos, int32_t _Button)
{
	Signal(EEvent_DoubleClicked);
}

//
//
//
void CListBox::Set(CString &rSetting, CString &rValue)
{
	if(rSetting == "amount")
		m_ScrollBar.SetAmount(rValue.GetFloat());
	else if(rSetting == "ondoubleclick")
		Bind(EEvent_DoubleClicked, m_pGUI->GetMapping(rValue));
	else if(rSetting == "onselect")
		Bind(EEvent_Select, m_pGUI->GetMapping(rValue));
	else
		CView::Set(rSetting, rValue);
}

//
//
//
void CListBox::OnLostFocus()
{
	m_SelectedItem = -1;
}

//
//
//
void CListBox::OnSize()
{
	int32_t w = GetSize().x-m_ScrollBar.GetSize().x;
	for(int32_t i = 0; i < m_lItems.Len(); i++)
		m_lItems[i]->SetWidth(w);
	m_ItemsHeight = GetItemsHeight();
}

CListItem *CListBox::GetSelectedItem()
{
	if(m_SelectedItem >= 0 && m_SelectedItem < m_lItems.Len())
		return m_lItems[m_SelectedItem];
	return NULL;
}

//
//
//
void CListBox::OnDraw(CDrawer &_rDrawer)
{
	_rDrawer.DrawRect(_rDrawer.m_aRectTypes[1], 0, 0, GetSize().x-m_ScrollBar.GetSize().x-1, GetSize().y);
	
	int32_t y = -GetStartY();
	
	CPoint Pos = GetWorldPosition();
	for(int32_t i = 0; i < m_lItems.Len(); i++)
	{   
		int32_t h = m_lItems[i]->GetHeight();
		if(y > GetSize().y)
			break;
			
		if(y+h > 0)
		{
			_rDrawer.SetGlobalTranslate(Pos.x, Pos.y+y);
			//_rDrawer.m_pUtils->SetClipping(true, Pos.x+2, Pos.y, GetSize().x-4, m_lItems[i]->GetSize().y);
			//_rDrawer.DrawRect(_rDrawer.m_aRectTypes[1], 0, 0, GetSize().x-m_pScrollBar->GetSize().x-1, m_lItems[i]->GetHeight());
			m_lItems[i]->OnDraw(_rDrawer, i == m_SelectedItem);
		}
		y += m_lItems[i]->GetHeight();
	}

	CView::OnDraw(_rDrawer);
}
