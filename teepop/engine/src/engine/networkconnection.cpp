#include <foo/base/log.h>
#include "network.h"

using namespace NEngine;

//
//
//
CNetworkConnection::CNetworkConnection()
{
	m_pConnection = NULL;
}

CNetworkConnection::~CNetworkConnection()
{
}

//
//
//
void CNetworkConnection::OnNetMessage(NetMsgID iMsg, CInputStream &rStream)
{
	if(m_mMessageMap.Exist(iMsg))
		m_mMessageMap[iMsg]->OnNetMessage(this, iMsg, rStream);
	else
	{
		#ifdef HASHED_MESSAGE_ID	
			NLog::WriteF("NetworkConnection: Unknown message, %d\n", iMsg);
		#else
			NLog::WriteF("NetworkConnection: Unknown message, %s\n", iMsg.GetData());
		#endif
	}
}

//
//
//
void CNetworkConnection::RegisterNetMessage(NetMsgID iMsg, CNetworkMsgRecviver *pRecviver)
{
	m_mMessageMap[iMsg] = pRecviver;
}

//
//
//
void CNetworkConnection::AttachRecviver(CNetworkMsgRecviver *pRecviver)
{
	Assert(pRecviver);
	m_mReciverData[pRecviver] = NULL;
	pRecviver->OnAttach(this);
}

//
//
//
void CNetworkConnection::DetachRecviver(CNetworkMsgRecviver *pRecviver)
{
	Assert(pRecviver);
	// remove all messages
	for(int32_t i = 0; i < m_mMessageMap.Len(); i++)
	{
		if(m_mMessageMap.GetValue(i) == pRecviver)
		{
			m_mMessageMap.Remove(m_mMessageMap.GetKey(i));
			i--;
		}
	}
	pRecviver->OnDetach(this);
	m_mReciverData.Remove(pRecviver);
}

//
//
//
void CNetworkConnection::DetachAllRecvivers()
{
	// Dettach all
	while(m_mMessageMap.Len())
		DetachRecviver(m_mMessageMap.GetValue(0));
}

//
//
//
void CNetworkConnection::SetData(CNetworkMsgRecviver *pRecviver, void *pData)
{
	m_mReciverData[pRecviver] = pData;
}

//
//
//
void *CNetworkConnection::GetData(CNetworkMsgRecviver *pRecviver)
{
	return m_mReciverData.Find(pRecviver, NULL);
}

//
//
//
bool CNetworkConnection::IsActive()
{
	return m_pConnection->IsConnected();
}

//
//
//
void CNetworkConnection::DispatchNetMessage(int32_t iMsg, CInputStream &rStream)
{
	
}

//
//
//
void CNetworkConnection::SendMessage(CNetMessage &rMsg)
{
	if(m_pConnection)
		m_pConnection->QueueChunk(rMsg.CreateChunk());
}








