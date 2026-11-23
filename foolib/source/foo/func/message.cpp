
#include <foo/base/string.h>
#include "message.h"

using namespace NFoo;

CMessage::CMessage(int32 what, CMessageQueue *pTarget)
{
	m_OrgMsg = what;
	m_What = what;
	m_pTarget = pTarget;
}

CMessage::CMessage(const CMessage &rMessage)
{
	m_lIntValues = rMessage.m_lIntValues;
	m_lFloatValues = rMessage.m_lFloatValues;
	m_lStringValues = rMessage.m_lStringValues;
	m_What = rMessage.m_What;
	m_OrgMsg = rMessage.m_OrgMsg;
}

void CMessage::SetInt(const int8 *_pName, int32 _Value) { m_lIntValues[StringGetHash(_pName)] = _Value; }
void CMessage::SetFloat(const int8 *_pName, fp32 _Value) { m_lFloatValues[StringGetHash(_pName)] = _Value; }
void CMessage::SetString(const int8 *_pName, const int8 *_pValue) { m_lStringValues[StringGetHash(_pName)] = _pValue; }

int32 CMessage::GetInt(const int8 *_pName, int32 _Default) { return m_lIntValues[StringGetHash(_pName)]; }
fp32 CMessage::GetFloat(const int8 *_pName, fp32 _Default) { return m_lFloatValues[StringGetHash(_pName)]; }
CString CMessage::GetString(const int8 *_pName, const int8 *_pDefault) { return m_lStringValues[StringGetHash(_pName)]; }



CMessageQueue::~CMessageQueue()
{
	m_lMessages.DeleteAll();
}

void CMessageQueue::PostMessage(CMessage &rMsg) // copies the message
{
	m_lMessages.Add(new CMessage(rMsg));
}

bool CMessageQueue::GotMessage()
{
	return m_lMessages.Len() > 0;
}

CMessage &CMessageQueue::GetMessage()
{
	return *m_lMessages[0];
}

void CMessageQueue::DispatchMessage()
{
	delete m_lMessages[0];
	m_lMessages.RemoveByIndex(0);
}

	
//
//
//
void CMessageSender::Signal(int32 iMsg)
{
	int32 index = m_MessageMap.Find(iMsg);
	if(index < 0)
		return;

	CMessageTarget &rTarget = m_MessageMap.GetValue(index);
	Assert(rTarget.m_pReceiver);
	rTarget.m_pReceiver->PostMessage(rTarget.m_Msg);
}

void CMessageSender::Signal(CMessage &rMsg)
{
	int32 index = m_MessageMap.Find(rMsg.m_What);
	if(index < 0)
		return;

	// design problem, how do we merge the two messages
	CMessageTarget &rTarget = m_MessageMap.GetValue(index);
	Assert(rTarget.m_pReceiver);
	CMessage Msg(rMsg);
	Msg.m_What = rTarget.m_Msg.m_What;
	rTarget.m_pReceiver->PostMessage(Msg);
}

//
//
//
bool CMessageSender::Bind(int32 iMsg, CMessageTarget Target)
{
	if(Target.m_pReceiver)
	{
		Target.m_Msg.m_OrgMsg = iMsg;
		m_MessageMap[iMsg] = Target;
	}
	return true;
}

//
//
//
bool CMessageSender::Bind(int32 iMsg, CMessageReceiver *pReceiver, int32 iTargetMsg)
{
	m_MessageMap[iMsg] = CMessageTarget(pReceiver, iTargetMsg);
	m_MessageMap[iMsg].m_Msg.m_OrgMsg = iMsg;
	return true;
}

//
//
//
bool CMessageSender::Bind(int32 iMsg, CMessageReceiver *pReceiver, CMessage &rMsg)
{
	rMsg.m_OrgMsg = iMsg;
	m_MessageMap[iMsg] = CMessageTarget(pReceiver, rMsg);
	return true;
}
