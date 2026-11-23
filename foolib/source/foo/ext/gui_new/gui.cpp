#include <foo/base/string.h>
#include <foo/func/registry.h>
#include "gui.h"
#include "view.h"

#include "textbox.h"
#include "window.h"
#include "button.h"
#include "scrollbar.h"
#include "listbox.h"

using namespace NFoo;
using namespace NFoo::NGUI;


//
//
//
CView *CGUI::CreateFromRegistry(CRegistry *pReg)
{
	CView *pView;

	// create view
	if(pReg->m_Name == "view") pView = new CView(0,0,this);
	else if(pReg->m_Name == "frame") pView = new CFrame(0,0,this);
	else if(pReg->m_Name == "button") pView = new CButton(0,0,this);
	else if(pReg->m_Name == "textbox") pView = new CTextBox(0,0,this);
	else if(pReg->m_Name == "listbox") pView = new CListBox(0,0,this);
	else if(pReg->m_Name == "label") pView = new CLabel(0,0,this);
	
	if(pView == NULL)
		return NULL;

	// parse value
	int8 aValue[1024*16];
	int8 *pPos = aValue;
	int8 *pNameStart = NULL;
	int32 NameLen = 0;
	int8 *pValueStart = NULL;
	int32 ValueLen = 0;
	StringCopy(aValue, pReg->m_Value);

	CString Name, Value;

	enum { ESkip, EName, ESearchForEqual, ESearchForString, EValue, EProcess };
	int32 State = ESkip;

	while(1)
	{
		if(!*pPos)
		{
			if(State == ESkip)
				break;
			State = EProcess;
		}

		switch(State)
		{
		case ESkip:
			if(IsSpace(*pPos))
			{
				pPos++;
				break; // continue;
			}
			pNameStart = pPos;
			State = EName;
		case EName:
			if(IsAlphaNumeric(*pPos))
			{
				NameLen++;
				pPos++;
				break; // continue;
			}
			if(!NameLen)
				Assert(0);
			State = ESearchForEqual;
		case ESearchForEqual:
			if(IsSpace(*pPos))
				pPos++;
			else if(*pPos == '=')
			{
				pPos++;
				State = ESearchForString;
			}
			else
				State = EProcess;
			break;
		case ESearchForString:
			if(IsSpace(*pPos))
				pPos++;
			else if(*pPos == '"')
			{
				pPos++;
				pValueStart = pPos;
				State = EValue;
			}
			else
				State = EProcess;
			break;
		case EValue:
			if(*pPos == '"')
				State = EProcess;
			else
				ValueLen++;
			pPos++;
			break;
		case EProcess:
			if(NameLen)
			{
				Name.CopyData(pNameStart, NameLen);
				if(pValueStart && ValueLen)
					Value.CopyData(pValueStart, ValueLen);
				else
					Value = "";

				pView->Set(Name, Value);
			}
			pNameStart = NULL;
			pValueStart = NULL;
			NameLen = 0;
			ValueLen = 0;
			State = ESkip;
			break;
		}
	}

	// create children
	for(int32 i = 0; i < pReg->m_lValues.Len(); i++)
	{
		CView *pChild = CreateFromRegistry(pReg->m_lValues[i]);
		if(pChild != NULL)
			pView->AddChild(pChild);
	}

	return pView;
};

//
//
//
void CGUI::MouseBoundCheck()
{
	if(m_MousePosition.x < 0) m_MousePosition.x = 0;
	if(m_MousePosition.y < 0) m_MousePosition.y = 0;
	
	if(m_pRoot != NULL)
	{
		if(m_MousePosition.x >= m_pRoot->GetSize().x) m_MousePosition.x = m_pRoot->GetSize().x-1;
		if(m_MousePosition.y >= m_pRoot->GetSize().y) m_MousePosition.y = m_pRoot->GetSize().y-1;
	}
}
		
CGUI::CGUI()
{
	m_pMouseOverView = NULL;
	m_pMouseCaptureView = NULL;
	m_pFocusedView = NULL;
	m_pRoot = NULL;
	m_MouseLastClickTime = GetTickCount();

	m_KeyPressed = false;
	m_KeyStroke = 0;
	m_KeyAsciiCode = 0;
	m_KeyRepeatTime = GetTickCount();
}

CGUI::~CGUI()
{
	m_lMappings.DeleteAll();
	if(m_pRoot != NULL)
		m_pRoot->DestroyTree();
}

CDrawer m_Drawer;

//
//
//
void CGUI::RenderCursor()
{
	m_Drawer.SetGlobalTranslate(0,0);
	m_Drawer.SetClipping(false);
	m_Drawer.DrawPart(m_Drawer.m_Cursor, CDrawer::EOrigo_TL, m_MousePosition.x, m_MousePosition.y);
}

//
//
//
void CGUI::Render()
{
	if(m_pRoot != NULL)
		m_pRoot->RenderTree(m_Drawer, CPoint(0,0));
	m_Drawer.SetGlobalTranslate(0,0);
}

//
//
//
CView *CGUI::GetRoot() { return m_pRoot; }
void CGUI::SetRoot(CView *_pView) { m_pRoot = _pView; }
void CGUI::Select(CView *pView)
{
	if(m_pFocusedView != pView)
	{
		if(m_pFocusedView != NULL)
			m_pFocusedView->OnLostFocus();

		if(pView != NULL)
			pView->OnFocus();

		m_pFocusedView = pView;
	}
}

//
//
//
void CGUI::Refresh()
{
	if(m_MouseMoved)
	{
		MouseBoundCheck();
		m_MouseMoved = false;

		//
		if(m_pMouseCaptureView != NULL)
			m_pMouseCaptureView->OnMouseMove(m_MousePosition);
		else
		{
			//CView *pLast = m_pRoot->GetChildByPoint(m_MouseLastPosition);
			CView *pCurrent = m_pRoot->GetChildByPoint(m_MousePosition, false);

			if(m_pMouseOverView != pCurrent)
			{
				if(m_pMouseOverView != NULL)
					m_pMouseOverView->OnMouseExit();
				if(pCurrent != NULL)
					pCurrent->OnMouseEnter();
				m_pMouseOverView = pCurrent;
			}
			if(pCurrent != NULL)
				pCurrent->OnMouseMove(m_MousePosition);
		}

		m_MouseLastPosition = m_MousePosition;
	}

	// detect key repeat
	if(m_KeyPressed && GetTickCount() > m_KeyRepeatTime)
		KeyDown(m_KeyStroke, m_KeyAsciiCode, true);
}

//
//
//
void CGUI::AddMapping(const int8 *pName, CMessageReceiver *pReceiver, CMessage &rMsg)
{
	CMapping *pMap = new CMapping;
	pMap->m_Name = pName;
	pMap->m_Target.m_pReceiver = pReceiver;
	pMap->m_Target.m_Msg = rMsg;
	m_lMappings.Add(pMap);
}

void CGUI::AddMapping(const int8 *pName, CMessageReceiver *pReceiver, int32 iMsg)
{
	CMessage Msg(iMsg);
	AddMapping(pName, pReceiver, Msg);
}

CMessageTarget CGUI::GetMapping(const int8 *pName)
{
	for(int32 i = 0; i < m_lMappings.Len(); i++)
		if(m_lMappings[i]->m_Name == pName)
			return m_lMappings[i]->m_Target;
	return CMessageTarget(NULL,0);
}

//
// Mouse functions
//
void CGUI::CaptureMouse(CView *_pView) { m_pMouseCaptureView = _pView; }
void CGUI::ReleaseMouse() { m_pMouseCaptureView = NULL; }

//
// Mouse
//
void CGUI::MouseMoveAbs(int32 x, int32 y) { m_MousePosition.Set(x, y); m_MouseMoved = true; }
void CGUI::MouseMoveRel(int32 x, int32 y)
{
	m_MousePosition.x += x;
	m_MousePosition.y += y;
	m_MouseMoved = true;
}

//
//
//
void CGUI::MouseDown(int32 _Button)
{
	CView *pCurrent = NULL;
	if(m_pMouseCaptureView != NULL)
		pCurrent = m_pMouseCaptureView;
	else
		pCurrent = m_pRoot->GetChildByPoint(m_MousePosition);

	if(pCurrent != NULL)
		pCurrent->OnMouseDown(m_MousePosition, _Button);

	Select(pCurrent);

	// detect dubble click
	if(pCurrent != NULL)
	{
		if(GetTickCount()-m_MouseLastClickTime < GetTickFrequency()/4)
		{
			if((m_MouseLastClickPosition-m_MousePosition).Length() < 4)
			{
				pCurrent->OnDoubleClick(m_MousePosition, _Button);
			}
		}
	}

	m_MouseLastClickPosition = m_MousePosition;
	m_MouseLastClickTime = GetTickCount();
}

//
//
//
void CGUI::MouseUp(int32 _Button)
{
	CView *pCurrent = NULL;
	if(m_pMouseCaptureView != NULL)
		pCurrent = m_pMouseCaptureView;
	else
		pCurrent = m_pRoot->GetChildByPoint(m_MousePosition);

	if(pCurrent != NULL)
		pCurrent->OnMouseUp(m_MousePosition, _Button);
		//pCurrent->OnEvent(CEvent(CEvent::EMouseButtonUp, m_MousePosition.x, m_MousePosition.y));
}

// Keyboard
void CGUI::KeyDown(int32 _Stroke, uint8 _AsciiCode, bool Repeat)
{
	if(m_pFocusedView != NULL)
		m_pFocusedView->OnKeyPress(_Stroke, _AsciiCode, false);
	
	if(Repeat)
		m_KeyRepeatTime = GetTickCount()+GetTickFrequency()/30;
	else
	{
		m_KeyPressed = true;
		m_KeyStroke = _Stroke;
		m_KeyAsciiCode = _AsciiCode;
		m_KeyRepeatTime = GetTickCount()+GetTickFrequency()/5;
	}
}

void CGUI::KeyUp(int32 _Stroke, uint8 _AsciiCode)
{
	m_KeyPressed = false;
	//if(m_pFocusedView)
}
