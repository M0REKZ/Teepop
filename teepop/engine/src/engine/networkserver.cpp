#include "networkserver.h"

using namespace NFoo;
using namespace NEngine;

MACRO_REGISTER_CLASS(CNetworkServer, "EngineModule")

//# Function: Start
//# Description: Starts the server
//# Parameters:
//#		Port, Network port to listen to
//# Declaration:
void CNetworkServer::Start(int32_t Port)
{
	m_SlotServer.Start(Port, m_VersionString); 
}

//# Function: SendMessage
//# Description: Sends a message to a specific or all clients
//# Parameters:
//#		iClient, Client id which to send the message to. Pass -1 to send to all clients.
//#		rMsg, Message to send
//# Declaration:
/*
int32_t CNetworkServer::SendMessage(int32_t iClient, CNetMessage &rMsg)
{
	if(iClient == -1) // Send to all clients?
	{
		for(int32_t i = 0; i < m_SlotServer.GetNumClients(); i++)
			m_SlotServer.GetClient(i)->QueueChunk(rMsg.CreateChunk());
		return m_SlotServer.GetNumClients();
	}
	else
	{
		CConnection *pConnection = m_SlotServer.FindClient(iClient);
		if(pConnection)
		{
			pConnection->QueueChunk(rMsg.CreateChunk());
			return 1;
		}
	}
	return 0;
}*/

//
//
//
void CNetworkServer::Update()
{
	m_SlotServer.Update();

	// handle connections
	for(int32_t i = 0; i < m_SlotServer.GetNumClients(); i++)
	{
		int32_t id = m_SlotServer.GetClientID(i);
		if(!m_mConnections.Exist(id))
		{
			CNetworkConnection *pConnection = m_pfnClientCreator(m_pCreatorData);
			pConnection->m_pConnection = m_SlotServer.GetClient(i);
			pConnection->m_iClient = id;
			m_mConnections[id] = pConnection; 
			pConnection->OnConnect();
		}
	}

	// handle disconnections
	for(int32_t i = 0; i < m_mConnections.Len(); i++)
	{
		int32_t id = m_mConnections.GetKey(i);
		if(!m_SlotServer.FindClient(id))
			m_mConnections[id]->m_pConnection = NULL;
	}
	for(int32_t i = 0; i < m_mConnections.Len(); i++)
	{
		int32_t id = m_mConnections.GetKey(i);
		if(!m_SlotServer.FindClient(id))
		{
			m_mConnections[id]->OnDisconnect();
			m_mConnections[id]->DetachAllRecvivers();
			delete m_mConnections[id];
			m_mConnections.Remove(id);
		}
	}
	
	//
	for(int32_t i = 0; i < m_SlotServer.GetNumClients(); i++)
	{
		CConnection *pConnection = m_SlotServer.GetClient(i);
		int32_t iClient = m_SlotServer.GetClientID(i);

		CChunk *pChunk = NULL;
		while(pChunk = pConnection->GetChunk())
		{
			if(pChunk->m_Type == EChunk_Message)
			{
				#ifdef HASHED_MESSAGE_ID
					NetMsgID iMsg = pChunk->m_Reader.ReadBE<int32_t>();
				#else
					NetMsgID iMsg = pChunk->m_Reader.Read_p16();
				#endif
				
				m_mConnections[iClient]->OnNetMessage(iMsg, pChunk->m_Reader);
				//Assert(m_mMessageMap.Exist(iMsg));
				//Assert(m_mMessageMap[iMsg]);
				//m_mMessageMap[iMsg]->OnMessageServer(iMsg, iClient, pChunk->m_Reader);
			}
			else
				Assert(0);
			delete pChunk;
		}
	}
}

/*
bool CNetworkServer::HotswapWrite(COutputStream &rStream)
{
	m_SlotServer.HotswapWrite(rStream);
	rStream.WriteNE<int32_t>(m_mConnections.Len());
	for(int32_t i = 0; i < m_mConnections.Len(); i++)
	{
		rStream.WriteNE<int32_t>(m_mConnections.GetKey(i));
		//m_mConnections.GetValue(i)->
	}


	return true;
}

bool CNetworkServer::HotswapRead(CInputStream &rStream)
{
	m_SlotServer.HotswapRead(rStream);

	return true;
}*/


const char *CNetworkServer::GetStatusString()
{
	static CString Status;
	
	Status = "";

	if(m_SlotServer.Running())
		Status.Set("Running with %d clients connected", m_SlotServer.GetNumClients());
	else
		Status.Set("Stopped");
	
	return Status.GetData();
}

