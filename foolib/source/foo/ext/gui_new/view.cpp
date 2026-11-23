#include <foo/core/messages.h>
#include <foo/base/string.h>
#include "view.h"
#include "gui.h"

using namespace NFoo;
using namespace NFoo::NGUI;

CView::CView(CView *pParent, const int8 *pName, CGUI *pGUI)
{
	m_Name = pName;
	m_pGUI = pGUI;
	m_pParent = pParent;
	if(!m_pGUI && m_pParent != NULL)
		m_pGUI = m_pParent->m_pGUI;

	if(!m_pGUI)
		throw "Couldn't get the hold of the GUI handler";

	if(m_pParent != NULL)
		m_pParent->AddChild(this);


	m_Flags = 0;
	m_Position.Set(0,0);
	m_Size.Set(0,0);
	m_MinSize.Set(-1,-1);
	m_MaxSize.Set(-1,-1);
	
	m_Follow = EFollow_Left|EFollow_Top;
}

CView::~CView()
{
	if(m_pGUI)
	{
		if(m_pGUI->m_pFocusedView == this)
			m_pGUI->Select(NULL);
		if(m_pGUI->m_pMouseCaptureView == this)
			m_pGUI->ReleaseMouse();
		if(m_pGUI->m_pMouseOverView == this)
			m_pGUI->m_pMouseOverView = NULL;
	}

	while(GetNumChildren())
		RemoveChild((int32)0);

	if(m_pParent)
		m_pParent->RemoveChild(this);
}

//
//
//
void CView::Set(CString &rKey, CString &rValue)
{
	if(rKey == "position" || rKey == "pos")
	{
		int32 x = rValue.GetStrSep(",").GetInt();
		int32 y = rValue.GetInt();
		SetPosition(x, y);
	}
	else if(rKey == "size")
	{
		int32 x = rValue.GetStrSep(",").GetInt();
		int32 y = rValue.GetInt();
		SetSize(x, y);
	}
	else if(rKey == "geometry" || rKey == "geom")
	{
		int32 px = rValue.GetStrSep(",").GetInt();
		int32 py = rValue.GetStrSep(",").GetInt();
		int32 sx = rValue.GetStrSep(",").GetInt();
		int32 sy = rValue.GetInt();
		SetGeometry(px, py, sx, sy);
	}
	else if(rKey == "name")
	{
		m_Name = rValue;
	}
}

//
//
//
void CView::DestroyTree()
{
	while(GetNumChildren())
	{
		CView *pChild = GetChild((int32)0);
		pChild->DestroyTree();
		RemoveChild((int32)0);
		if(!(pChild->Flags()&EFlag_Indestructable))
			delete pChild;
	}
}

void CView::DumpTree()
{
}

void CView::CheckTree(void *p)
{
	if(p == this)
		DebugBreak();

	for(int32 i = 0; i < GetNumChildren(); i++)
	{
		if(p)
			GetChild(i)->CheckTree(p);
		GetChild(i)->CheckTree(this);
	}
}



void CView::SetResizeMode(int32 s) { m_Follow = s; }
int32 CView::GetResizeMode() { return m_Follow; }

// Child, CView * handling (is it only layered containers that can have children?)
CView *CView::GetParent() { return m_pParent; } // returns the CView *
int32 CView::AddChild(CView *pChild) // Adds a child. returns the local index to the child
{
	if(!pChild)
		return -1;
	if(pChild->m_pParent != NULL)
		pChild->m_pParent->RemoveChild(pChild);
	int32 i = m_lChildren.Find(pChild);
	if(i != -1)
		return i;
	pChild->m_pParent = this;
	return m_lChildren.Add(pChild);
}
	
bool CView::RemoveChild(CView *spChild) // Removes a child by pointer. Returns true of removed
{
	if(spChild == NULL)
		return false;
	if(spChild->m_pParent != this)
		return false;

	m_lChildren.RemoveFast(spChild);
	spChild->m_pParent = NULL;
	return true;
}


bool CView::RemoveChild(int32 _iLocal) // Removes a child by local index. Returns true of removed
{
	m_lChildren.RemoveByIndexFast(_iLocal);
	return true;
}

int32 CView::GetNumChildren() { return m_lChildren.Len(); } // Returns the number of children
CView *CView::GetChild(int32 _iLocal) { return m_lChildren[_iLocal]; } // returns the child by local index
bool CView::IsChild(CView *_pContainer) { return m_pParent == _pContainer; } // returns true of _pContainer is a child of this CView
CView *CView::GetChildByPoint(CPoint _Point, bool HiddensAsWell)
{
	for(int32 i = 0; i < GetNumChildren(); i++)
	{
		CView *pChild = GetChild(i);
		if(!HiddensAsWell && pChild->Flags()&EFlag_Hidden)
			continue;

		if(_Point.x > pChild->GetPosition().x && _Point.y > pChild->GetPosition().y &&
			_Point.x < pChild->GetPosition().x + pChild->GetSize().x &&
			_Point.y < pChild->GetPosition().y + pChild->GetSize().y)
		{
			return pChild->GetChildByPoint(_Point-pChild->GetPosition());
		}
	}
	return this;
}

CView *CView::GetChild(const int8 *_pName)
{
	for(int32 i = 0; i < GetNumChildren(); i++)
	{
		CView *pChild = GetChild(i);
		if(pChild  != NULL && pChild->m_Name == _pName)
			return pChild;
	}

	for(int32 i = 0; i < GetNumChildren(); i++)
	{
		CView *pChild = GetChild(i);
		if(pChild != NULL)
		{
			CView *pFound = pChild->GetChild(_pName);		
			if(pFound != NULL)
				return pFound;
		}
	}

	return NULL;
}

//
// Geometry handling
//
void CView::SetPosition(int32 x, int32 y) { m_Position.Set(x,y); }
void CView::SetSize(int32 w, int32 h)
{
	CPoint OldSize = m_Size;
	m_Size.Set(w, h);

	// apply size limits
	if(m_MinSize.x != -1 && m_Size.x < m_MinSize.x) m_Size.x = m_MinSize.x;
	if(m_MinSize.y != -1 && m_Size.y < m_MinSize.y) m_Size.y = m_MinSize.y;
	if(m_MaxSize.x != -1 && m_Size.x > m_MaxSize.x) m_Size.x = m_MaxSize.x;
	if(m_MaxSize.y != -1 && m_Size.y > m_MaxSize.y) m_Size.y = m_MaxSize.y;

	for(int32 i = 0; i < GetNumChildren(); i++)
		GetChild(i)->ParentResized(OldSize, m_Size);
		
	OnSize(OldSize);
}

//
//
//
void CView::ParentResized(CPoint _OldSize, CPoint _NewSize)
{
	// resizes and repositions acording to strings
	CPoint NewSize = GetSize();
	CPoint NewPos = GetPosition();
	if(m_Follow&EFollow_Right)
	{
		if(m_Follow&EFollow_Left)
			NewSize.x += _NewSize.x - _OldSize.x;
		else
			NewPos.x += _NewSize.x - _OldSize.x;
	}

	if(m_Follow&EFollow_Bottom)
	{
		if(m_Follow&EFollow_Top)
			NewSize.y += _NewSize.y - _OldSize.y;
		else
			NewPos.y += _NewSize.y - _OldSize.y;
	}
	
	SetSize(NewSize.x, NewSize.y);
	SetPosition(NewPos.x, NewPos.y);
}

//
//
//
void CView::SetSizeLimits(CPoint _MinSize, CPoint _MaxSize)
{
	m_MaxSize = _MinSize;
	m_MinSize = _MaxSize;
	SetSize(m_Size.x, m_Size.y);
}
	
//
//
//
void CView::SetMaxSize(CPoint _Size)
{
	m_MaxSize = _Size;
	SetSize(m_Size.x, m_Size.y);
}

//
//
//
void CView::SetMinSize(CPoint _Size)
{
	m_MinSize = _Size;
	SetSize(m_Size.x, m_Size.y);
}

//
//
//
void CView::SetGeometry(int32 x, int32 y, int32 w, int32 h) { SetPosition(x,y), SetSize(w,h); } // guess again

//
//
//
CPoint CView::GetPosition() { return m_Position; } // guess
CPoint CView::GetSize() { return m_Size; } // guess again

//
//
//
CPoint CView::GetWorldPosition() // returns the position relative to the root container ''(name suggestions?)''
{
	if(m_pParent != NULL)
		return m_pParent->GetWorldPosition()+m_Position;
	return m_Position;
}

//
//
//
void CView::MoveToFront()
{
	if(m_pParent != NULL)
		return;

	TDynamicArray<CView *> lChildren;
	for(int32 i = 0; i < m_pParent->m_lChildren.Len(); i++)
		lChildren.Add(m_pParent->m_lChildren[i]);
	
	m_pParent->m_lChildren.Clear();
	m_pParent->AddChild(this);
	
	for(int32 i = 0; i < lChildren.Len(); i++)
		if(lChildren[i] != this)
			m_pParent->AddChild(lChildren[i]);
}

//
//
//
void CView::RenderTree(CDrawer &_rRender, CPoint _Pos)
{
	// do clipping
	CPoint Size = GetSize();
	CPoint Position = GetPosition();
	if(m_pParent != NULL)
	{
		CPoint ParentSize = m_pParent->GetSize();

		if(Position.x > ParentSize.x) return;
		if(Position.y > ParentSize.y) return;

		if(Position.x+Size.x > ParentSize.x)
			Size.x = ParentSize.x-Position.x;
		if(Position.y+Size.y > ParentSize.y)
			Size.y = ParentSize.y-Position.y;
		if(Position.x < 0)
			Size.x = Size.x+Position.x, Position.x = 0;
		if(Position.y < 0)
			Size.y = Size.y+Position.y, Position.y = 0;

		if(Size.x < 0)
			return;
		if(Size.y < 0)
			return;
	}

	// set clipping and translate
	_rRender.SetGlobalTranslate(_Pos.x+GetPosition().x, _Pos.y+GetPosition().y);
	_rRender.SetClipping(true, _Pos.x+Position.x, _Pos.y+Position.y, Size.x, Size.y);
	OnDraw(_rRender); // call this view first
	int32 n = GetNumChildren();
	for(int32 i = n-1; i >= 0; i--) // reverse order
		if(!(GetChild(i)->Flags()&EFlag_Hidden))
			GetChild(i)->RenderTree(_rRender, _Pos+GetPosition());
}


void CView::OnKeyPress(int32 Key, int8 Char, bool Repeat)
{
	if(Key == NCore::EBtn_Key_Tab)
	{
		if(m_pParent != NULL)
		{
			TDynamicArray<CView *> lViews;
			
			for(int32 i = 0; i < m_pParent->GetNumChildren(); i++)
			{
				CView *pChild = m_pParent->GetChild(i);
				if(this == &*pChild || !(pChild->Flags()&EFlag_TabStop))
					continue;

				lViews.Add(pChild);
			}

			if(lViews.Len())
			{
				m_pGUI->Select(lViews[0]);
			}
		}
	}
}



//
//
//
/*
void CView::Signal(const int8 *_pName)
{
	int32 Hash = StringGetHash(_pName);
	if(m_lTargets.Find(Hash) < 0)
		return;

	for(int32 i = 0; i < m_lTargets[Hash].Len(); i++)
		m_lTargets[Hash][i]->m_Pulses++;
}

//
//
//
bool CView::Connect(const int8 *_pName, CSlot *_pSlot)
{
	m_lTargets[StringGetHash(_pName)].Add(_pSlot);
	return true;
}
*/

//
//
//
bool CView::IsFocused()
{
	if(m_pGUI->m_pFocusedView == this)
		return true;
	return false;
}

//
//
//
bool CView::IsMouseOver()
{
	if(m_pGUI->m_pMouseOverView == this)
		return true;
	return false;
}
