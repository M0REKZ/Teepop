#include <foo/base/log.h>
#include "network.h"
#include "networkclient.h"

using namespace NFoo;
using namespace NEngine;

MACRO_REGISTER_CLASS(CNetworkClient, "EngineModule")

bool CNetworkClient::Init()
{
	Assert(CModule::Init());
	m_LastState = m_SlotClient.GetState();
	m_pConnection = &m_SlotClient;
	m_pCore->RegisterCommand("connect", this);
	m_pCore->RegisterCommand("disconnect", this);
	return true;
}

void CNetworkClient::OnCommand(CString &rCmd, CString &rArgs)
{
	if(rCmd == "connect")
		Connect(AddrFromHostname(rArgs, 8303));
	else if(rCmd == "disconnect")
		Disconnect();
}

//# Function: SendMessage
//# Description: Sends a message to the server
//# Parameters:
//#		rMsg, Message to send
//# Declaration:
//void CNetworkClient::SendMessage(CNetMessage &rMsg)
//{ m_SlotClient.QueueChunk(rMsg.CreateChunk()); }
void CNetworkClient::UpdateState()
{
	// detect state changes
	int32_t State = m_SlotClient.GetState();
	if(State != m_LastState)
	{
		if(State == CSlotClient::EState_Connected)
			Signal(EEvent_Connected);
		else if(State == CSlotClient::EState_Connecting)
			Signal(EEvent_Connecting);
		else if(State == CSlotClient::EState_Closed)
		{
			CMessage Msg(EEvent_Disconnected);
			Msg.SetString("errormsg", m_SlotClient.GetErrorMsg());
			Signal(Msg);
			DetachAllRecvivers();
		}
	}
	m_LastState = m_SlotClient.GetState();
}

void CNetworkClient::Update()
{
	m_SlotClient.Update();
	UpdateState();



	m_iClient = m_SlotClient.GetID();

	//
	CChunk *pChunk = NULL;
	while(pChunk = m_SlotClient.GetChunk())
	{
		if(pChunk->m_Type == EChunk_Message)
		{
			#ifdef HASHED_MESSAGE_ID
				NetMsgID iMsg = pChunk->m_Reader.ReadBE<int32_t>();
			#else
				NetMsgID iMsg = pChunk->m_Reader.Read_p16();
			#endif
				//			int32_t iMsg = pChunk->m_Reader.ReadBE<int32_t>();
			OnNetMessage(iMsg, pChunk->m_Reader);
			//Assert(m_mMessageMap.Exist(iMsg));
			//Assert(m_mMessageMap[iMsg]);
			//m_mMessageMap[iMsg]->OnMessageClient(iMsg, pChunk->m_Reader);
		}
		else
			Assert(0);
		delete pChunk;
	}

	//PumpConnection(&m_SlotClient);
}

const char *CNetworkClient::GetStatusString()
{
	static CString Status;
	
	Status = "";

	if(IsConnected())
		Status.Set("Connected to %s", m_SlotClient.GetRemoteAddress().Str());
	else if(IsConnecting())
		Status.Set("Connecting to %s", m_SlotClient.GetRemoteAddress().Str());
	else if(IsClosed())
		Status.Set("Closed");
	else
		Status = "Unknown";

	return Status.GetData();
}