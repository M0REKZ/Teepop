
#include <foo/base/log.h>
#include <foo/base/string.h>
#include "client.h"

using namespace NFoo;
using namespace NFoo::NNetwork;
using namespace NFoo::NSlotServer;

//# Req_Namespace: NFoo::NSlotServer
//# Req_Header: ext/network/slotserver/slotserver.h
//# Req_Library: 
//
//# Context: Ext/Network/SlotServer/CSlotClient

///////////////////////////////////////////////////////////////////////////////////////
// Client
//
CSlotClient::CSlotClient()
{
	m_Threaded = false;
}

CSlotClient::~CSlotClient()
{
	if(m_Threaded)
	{
		SendTermination();
		WaitForTermination();
	}

	Close();
	Reset();
}

int32_t CSlotClient::ThreadMain()
{
	while(!AwaitingTermination())
	{
		PumpSocket();
		Sleep(1);
	}

	CConnection::Close();
	return 0;
}


//# Function: Connect
//# Description: Attempts to connect to a server.
//# Parameters:
//#		Addr, Address to server
//# Declaration:
void CSlotClient::Connect(NNetwork::CAddr Addr, const char *pVersionString, bool Threaded)
{
	if(!IsClosed())
		Close();

	m_ErrorMsg = "";
	NLog::Write("SlotClient: Connecting...\n");

	m_Threaded = false;

	//m_State = EState_Connecting;
	//m_RemoteAddress = Addr;

	m_Socket.Create(NNetwork::CSocket::EType_UDP);
	m_Socket.SetBlocking(false);
	m_Socket.Bind(NNetwork::CAddr());
	
	Init(&m_Socket, Addr, EState_Connecting);
	CConnection::Update();

	// send connect chunk
	CChunk *pChunk = new CChunk(EChunk_RequestSlot);
	pChunk->m_Writer.Write_p8(pVersionString);
	//pChunk->m_Writer.Write("Unknown lamer", StringLength("Unknown lamer")+1); // name
	//pChunk->m_Writer.Write("", 1); // passwd
	QueueChunk(pChunk);
	Flush(); // send it direct

	if(Threaded)
	{
		m_Threaded = Threaded;
		StartThread();
	}
		//CreateThread(ThreadMain, this);
}

void CSlotClient::Close()
{
	if(m_Threaded)
		SendTermination();
	else
		CConnection::Close();
}

//# Function: Disconnect
//# Description: Disconnects.
//# Declaration:
void CSlotClient::Disconnect()
{
	Close();
}



//# Function: Update
//# Description: Updates the connection and retrives all chunks.
//# Remarks:
//#		Call GetChunk to retrive the chunks.
//# Declaration:
void CSlotClient::Update()
{
	if(!m_Threaded)
		PumpSocket();
}

void CSlotClient::PumpSocket()
{
	if(IsConnecting())
	{
		// Fetch chunks
		NNetwork::CAddr Addr;
		NFoo::TDynamicArray<CChunk*> lChunks;
		
		if(m_Socket.RecvChunks(lChunks, Addr))
		{
			if(lChunks[0]->m_Type == EChunk_Accepted)
			{
				// server accepted the connection
				NLog::Write("SlotClient: Server accepted connection\n");
				SetState(EState_Connected);
				m_iClient = lChunks[0]->m_Reader.ReadBE<int32_t>();
				//Init(&m_Socket, Addr, EState_Connected);
			}
			else if(lChunks[0]->m_Type == EChunk_Rejected)
			{
				m_ErrorMsg = lChunks[0]->m_Reader.Read_p8();
				SetState(EState_Closed);
				// server activly rejected the connection
				NLog::Write("SlotClient: Server activly rejected connection\n");
				Reset();
			}
		}
		
		lChunks.DeleteAll();

		// online
		CConnection::Update();
	}
	else if(IsConnected())
	{
		NNetwork::CAddr Addr;
		NFoo::TDynamicArray<CChunk*> lChunks;
		while(m_Socket.RecvChunks(lChunks, Addr))
		{
			for(int32_t i = 0; i < lChunks.Len(); i++)
				Process(lChunks[i]);
		}
		
		// online
		CConnection::Update();
	}
}


