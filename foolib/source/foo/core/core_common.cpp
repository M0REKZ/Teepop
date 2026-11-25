//==========================================================================================
// Copyright (c), Clas M. Auvinen 2002
//==========================================================================================

#include <foo/base/log.h>
#include <foo/base/memory.h>
#include "core.h"

using namespace NFoo;
using namespace NFoo::NCore;

CCore *NFoo::NCore::g_pCore;

//
//
//
CSubCore::CSubCore()
{
	NLog::WriteF("SubCore: Initing\n");
	GetCore()->m_lSubCore.Add(this);
	m_iCore = GetCore()->m_iCurrent++;
}

//
//
//
CSubCore::~CSubCore()
{
	GetCore()->m_lSubCore.Remove(this);
}


//
//
//
void NCore::InitCore(CCore *_pCore)
{
	NLog::WriteF("Core: Initing\n");
	if(!_pCore)
		NLog::WriteF("Core: no core supplied to the init function");
	g_pCore = _pCore;
	g_pCore->Init();
}

//
//
//
bool NCore::GotMessages()
{
	return g_pCore->m_lMessageQueue.Len() > 0;
}

//
//
//
CMessage NCore::GetMessage()
{
	if(GotMessages())
	{   
		CMessage Msg = g_pCore->m_lMessageQueue[0];
		g_pCore->m_lMessageQueue.RemoveByIndex(0);
		return Msg;
	}
	
	return CMessage();
}

//
//
//
void NCore::AddMessage(CMessage _msg)
{
	g_pCore->m_lMessageQueue.Add(_msg);
}

//
//
//
CCore::~CCore()
{
	while(m_lSubCore.Len())
		delete m_lSubCore[0];
}

//
//
//
void NCore::CCore::Init()
{
	g_pCore->m_iCurrent = 1;
}

//
//
//
bool NCore::CCore::Update()
{
	// Update All
	for(int32_t i = m_lSubCore.Len()-1; i >= 0; i--)
	{
		if(!m_lSubCore[i]->Update())
		{
			if(m_lSubCore[i]->m_pClassHandle)
				NLog::WriteF("Core: %s subcore failed update\n", m_lSubCore[i]->m_pClassHandle->m_aName);
			else
				NLog::WriteF("Core: Unknown subcore failed update\n");
			return false;
		}
	}

	// Return success
	return true;
}

//
//
//
bool NCore::Update()
{
	return GetCore()->Update();
}

bool NCore::UpdateAndPump()
{
	if(!Update()) 
		return false;

	while(GotMessages()) 
		if(GetMessage().m_MessageID == EMsg_Quit)
		{
			NLog::WriteF("Core: Got quit message\n");
			return false;
		}

	return true;
}

/*
CString CKeyboard::KeySymToString(int32_t sym) 
{
	struct TransTable
	{
		int32_t sym; // keysym
		int8 str[14]; // key string
	} Tabl[] =
	{
	{EBtn_Key_Delete, "Delete"},
	{EBtn_Key_Insert, "Insert"},
	{EBtn_Key_Page_Up, "Page Up"},
	{EBtn_Key_Page_Down, "Page Down"},
	{EBtn_Key_Home, "Home"},
	{EBtn_Key_End, "End"},
	
	{EBtn_Key_Backspace, "Backspace"},
	{EBtn_Key_Tab, "Tab"},
	{EBtn_Key_Enter, "Enter"},
	{EBtn_Key_Shift, "Shift"},
	{EBtn_Key_Control, "Control"},
	{EBtn_Key_Escape, "Escape"},
	{EBtn_Key_Space, "Space"},

	{EBtn_Key_LShift, "Left Shift"},
	{EBtn_Key_RShift, "Right Shift"},
	{EBtn_Key_LControl, "Left Control"},
	{EBtn_Key_RControl, "Right Control"},
	{EBtn_Key_LMenu, "Left Menu"},
	{EBtn_Key_RMenu, "Right Menu"},

	{EBtn_Key_LSpecial, "Left Special"},
	{EBtn_Key_RSpecial, "Right Special"},

	{EBtn_Key_Left, "Left"},
	{EBtn_Key_Up, "Up"},
	{EBtn_Key_Right, "Right"},
	{EBtn_Key_Down, "Down"},

	{EBtn_Key_F1, "F1"},
	{EBtn_Key_F2, "F2"},
	{EBtn_Key_F3, "F3"},
	{EBtn_Key_F4, "F4"},
	{EBtn_Key_F5, "F5"},
	{EBtn_Key_F6, "F6"},
	{EBtn_Key_F7, "F7"},
	{EBtn_Key_F8, "F8"},
	{EBtn_Key_F9, "F9"},
	{EBtn_Key_F10, "F10"},
	{EBtn_Key_F11, "F11"},
	{EBtn_Key_F12, "F12"},
	{0, ""}
	};
	
	for (int32_t i = 0; Tabl[i].sym != 0; ++i) 
	{
		if (Tabl[i].sym == sym)
			return Tabl[i].str;
	}		
	char buff[16];	
	sprintf(buff, "%c", sym);
	return buff;
}
*/
/*
//
//
//
CCore::CCore()
{
	Init();
	NCore::m_pCore = this;
}

//
//
//
CCore::~CCore()
{
	Shutdown();
	m_lMessageQueue.DeleteAll();
	NClassRegistry::Clear();
}

//
//
//
bool CCore::Update()
{
	// OS Pre Update
	if(!OSPreUpdate())
		return false;

	// Update All
	for(int32_t i = m_lSubSystems.Len()-1; i >= 0; i--)
	{
		if(!m_lSubSystems[i]->Update())
			return false;
	}

	// OS Post Update
	if(!OSPostUpdate())
		return false;

	// Return success
	return true;
}

//
//
//
bool CCore::UpdateAndPump()
{
	if(!Update())
		return false;

	while(GotMessages())
	{
		CMessage *pMsg = GetCurrentMessage();
		if(pMsg->m_MessageID == EMsg_Quit)
			return false;
		DiscardCurrentMessage();
	}

	return true;
}

//
//
//
void CCore::AddMessage(CMessage *_pMsg)
{
	m_lMessageQueue.Add(_pMsg);
}

//
//
//
bool CCore::GotMessages()
{
	if(m_lMessageQueue.Len() > 0)
		return true;

	return false;
}

//
//
//
CMessage *CCore::GetCurrentMessage()
{
	if(!m_lMessageQueue.Len())
		return NULL;

	return m_lMessageQueue[0];
}

//
//
//
void CCore::DiscardCurrentMessage()
{
	if(!m_lMessageQueue.Len())
		return;
	
	delete m_lMessageQueue[0];
	m_lMessageQueue.RemoveByIndex(0);
}
*/
