
#include <foo/base/string.h>
#include <foo/base/memory.h>
#include <foo/base/log.h>
#include "server.h"

using namespace NFoo;
using namespace NFoo::NNetwork;
using namespace NFoo::NSlotServer;

//# Req_Namespace: NFoo::NSlotServer
//# Req_Header: ext/network/slotserver/slotserver.h
//# Req_Library: 
//
//# Context: Ext/Network/SlotServer/CSlotServer



///////////////////////////////////////////////////////////////////////////////////////
// Server
//
CSlotServer::CSlot::CSlot()
{
	//m_SwitchState = 0;
	m_Name.Set("Unknown");
}

CSlotServer::CSlotServer()
{
	m_CurrentClientID = 1000;
	//m_NumSlots = 32;

	//NNetwork::CAddr NullAddr(0);
	//for(int32_t i = 0; i < m_NumSlots; i++)
	//	m_aSlot[i].Init(m_Socket, NullAddr, false);
}

CSlotServer::~CSlotServer()
{
	m_lSlot.DeleteAll();
}

int32_t CSlotServer::GetNumClients()
{
	return m_lSlot.Len();
}

int32_t CSlotServer::GetClientID(int32_t Index)
{
	Assert(Index >= 0 && Index < m_lSlot.Len());
	return m_lSlot[Index]->m_iClient;
}

CConnection *CSlotServer::GetClient(int32_t Index)
{
	Assert(Index >= 0 && Index < m_lSlot.Len());
	return m_lSlot[Index];
}

CConnection *CSlotServer::FindClient(int32_t iClient)
{
	for(int32_t i = 0; i < m_lSlot.Len(); i++)
		if(m_lSlot[i]->m_iClient == iClient)
			return m_lSlot[i];
	return NULL;
}

CSlotServer::CSlot *CSlotServer::NewSlot()
{
	CSlot *pSlot = new CSlot(); // create new slut!
	m_lSlot.Add(pSlot);
	return pSlot;
}


//
//
//
void StrAdd(int8 *&_Str, const int8 *_pFormat, ...)
{
    int8 Buffer[4092];

    va_list Args;
    va_start(Args, _pFormat);
    StringCFormatArgList(Buffer, sizeof(Buffer), _pFormat, Args);
    va_end(Args);

    int32_t len = StringLength(Buffer);
    MemoryCopy(_Str, Buffer, len);
    _Str += len;
}

//
//
//
void CSlotServer::Update()
{
	// remove dead slots
	for(int32_t i = 0; i < m_lSlot.Len(); i++)
	{
		if(m_lSlot[i]->IsClosed())
		{
			SafeDelete(m_lSlot[i]);
			m_lSlot.RemoveByIndexFast(i);
			i--;
		}
	}

	// Fetch chunks
	NNetwork::CAddr Addr;
	NFoo::TDynamicArray<CChunk*> lChunks;

	while(m_Socket.RecvChunks(lChunks, Addr) > 0)
	{
		// find existing connection
		bool Processed = false;
		for(int32_t i = 0; i < m_lSlot.Len(); i++)
		{
			if(m_lSlot[i]->IsClosed())
				continue;
			
			if(m_lSlot[i]->GetRemoteAddress() == Addr)
			{
				// the slot will be responible fore deleting the chunks
				for(int32_t c = 0; c < lChunks.Len(); c++)
					m_lSlot[i]->Process(lChunks[c]);
				Processed = true;
				break;
			}
		}
		
		if(Processed == false)
		{
			if(lChunks.Len() == 1)
			{
				if(lChunks[0]->m_Type == EChunk_RequestSlot)
				{
					CSlot *pSlot = NULL;
					CString Version = lChunks[0]->m_Reader.Read_p8();
					
					// a new connection request
					if(m_VersionString == Version)
					{
						pSlot = NewSlot();
						
						if(pSlot)
						{
							//CSlot &rSlot = m_lSlot[iSlot];
							pSlot->SetState(CConnection::EState_Connected);
							pSlot->Init(&m_Socket, Addr, CConnection::EState_Connected);
							pSlot->m_iClient = m_CurrentClientID++;
							NLog::WriteF("SlotServer: Client connected, %s\n", Addr.Str());
							
							CChunk *pChunk = new CChunk(EChunk_Accepted, EFlag_Vital);
							pChunk->m_Writer.WriteBE<int32_t>(pSlot->m_iClient);
							pSlot->QueueChunk(pChunk);

							pSlot->Flush();
						}
						else
						{
							// TODO: send Reject Packet
							NLog::Write("SlotServer: Client connection rejected\n");
						}
					}
					else
					{
						NLog::WriteF("SlotServer: Client rejected, version error, ip=%s clientversion=%s serverversion=%s\n", Addr.Str(), Version.GetData(), m_VersionString.GetData());
						CString ErrorMsg("Version error. Client is running %s. Server is running %s.", Version.GetData(), m_VersionString.GetData());
						CChunk Chunk(EChunk_Rejected, EFlag_Data);
						Chunk.m_Writer.Write_p8(ErrorMsg);
						Chunk.m_DataSize = Chunk.m_Writer.GetSize();

						NFoo::TDynamicArray<CChunk*> lChunks;
						lChunks.Add(&Chunk);
						m_Socket.SendChunks(lChunks, Addr);
					}
				}
				else
					NLog::WriteF("SlotServer: Unknown chunk, ip=%s type=%x\n", Addr.Str(), lChunks[0]->m_Type);
			}
			else
				NLog::WriteF("SlotServer: Strange package from unknown client, ip=%s\n", Addr.Str());
				
			// delete the chunks
			lChunks.DeleteAll();
		}
	
	}
	
	// Update all slots
	for(int32_t i = 0; i < m_lSlot.Len(); i++)
		m_lSlot[i]->Update();

	{
		
		/*
		if(m_lSlot[i].IsReady() && m_lSlot[i].m_SwitchedToConnected)
		{
			//NLog::Write("SlotServer: Got connection Ack\n");
			OnClientConnect(i);
			m_lSlot[i].m_SwitchedToConnected = false;
		}*/
		/*
		if(m_lSlot[i].m_SwitchState != 0)
		{
			if(m_lSlot[i].m_SwitchState == 1)
				OnClientConnect(i);
			else if(m_lSlot[i].m_SwitchState == 2)
				OnClientDisconnect(i);
			
			m_lSlot[i].m_SwitchState = 0;
		}*/
	}
		
	// Update stats socket
	NNetwork::CSocket *pSocket;
	
	while(pSocket = m_StatsSocket.Accept())
	{
		int8 aString[1024*32];
		int8 *pCurrent = aString;
		StrAdd(pCurrent, "SlotServer Statistics\n\n");
		for(int32_t i = 0; i < m_lSlot.Len(); i++)
		{
			StrAdd(pCurrent, "Slot %2d: ", i);
			
			if(m_lSlot[i]->IsClosed())
				StrAdd(pCurrent, "Free");
			else
				StrAdd(pCurrent, "%s (%s)", m_lSlot[i]->m_Name.GetData(), m_lSlot[i]->GetRemoteAddress().Str());

			StrAdd(pCurrent, "\n");
		}

		pSocket->Send(aString, StringLength(aString));
		delete pSocket;
	}
}

bool CSlotServer::Running()
{
	return m_Socket.Valid();
}

void CSlotServer::Stop()
{
	//m_WebSocket.Close();
	m_Socket.Close();
}



bool CSlotServer::Start(uint16 _Port, const int8 *pVersionString)
{
	m_StatsSocket.Create(NNetwork::CSocket::EType_TCP);
	m_StatsSocket.SetBlocking(false);
	m_StatsSocket.Bind(NNetwork::CAddr(_Port+1));
	m_StatsSocket.Listen();

	m_VersionString = pVersionString;
	
	m_Socket.Create(NNetwork::CSocket::EType_UDP);
	m_Socket.SetBlocking(false);
	return m_Socket.Bind(NNetwork::CAddr(_Port));
}

