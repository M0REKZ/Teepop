
#include <foo/base/log.h>
#include <foo/base/sys.h>
#include "connection.h"

using namespace NFoo;
using namespace NFoo::NNetwork;
using namespace NFoo::NSlotServer;

//# Req_Namespace: NFoo::NSlotServer
//# Req_Header: ext/network/slotserver/slotserver.h
//# Req_Library: 
//
//# Context: Ext/Network/SlotServer/CConnection
//

const bool g_ExtraLogging = false;


//
//
//
CConnection::CConnection()
{
	m_State = EState_Closed;
	m_InternalState = EState_Closed;
	m_HandlingSocket = false;
	m_pSocket = NULL;
	Reset();
}

//
//
//
CConnection::~CConnection()
{
	Close();
	Reset();
}

//
//
//
void CConnection::SetSocket(CChunkSocket *pSocket, bool handle)
{
	if(m_HandlingSocket && m_pSocket)
		SafeDelete(m_pSocket);
	m_HandlingSocket = handle;
	m_pSocket = pSocket;
}

//
//
//
void CConnection::Reset()
{
	m_lVitalChunks.Clear();
	m_lPendingChunks.DeleteAll();
	m_lReceivedChunks.DeleteAll();
	m_SequenceNumber = 0;
	m_VitalSequenceNumber = 0;
	m_LastVitalNumber = 0;
	
	SetSocket(NULL, false);
//	SetState(EState_Closed);
	m_State = EState_Closed;
	m_InternalState = EState_Closed;
}


void CConnection::Process(CChunk *pChunk)
{
	m_LastReceiveTime = GetTickCount();

	if(pChunk->m_Flags&EFlag_Vital)
	{
		if(pChunk->m_VitalNumber <= m_LastVitalNumber)
		{
			// send ack
			//NLog::Write("Connection: Acking packet, vn=%d (%d)\n", _pChunk->m_VitalNumber, m_LastVitalNumber);
			CChunk *pAckChunk = new CChunk(EChunk_Ack);
//			pChunk->m_Storage.WriteBE_int32(_pChunk->m_VitalNumber);
			pAckChunk->m_Writer.WriteBE(pChunk->m_VitalNumber);
			QueueChunk(pAckChunk);

			if(g_ExtraLogging)
				NLog::WriteF("Connection: Sending ACK vn=%d, %s\n", pChunk->m_VitalNumber, pChunk->String());

			if(pChunk->m_VitalNumber == m_LastVitalNumber)
				m_LastVitalNumber++;
			else
			{
				if(g_ExtraLogging)
					NLog::WriteF("Connection: Dropped unimportant vital chunk, lvn=%d, %s\n", m_LastVitalNumber, pChunk->String());
				delete pChunk;
				return;
			}
		}
		else
		{
			if(g_ExtraLogging)
				NLog::WriteF("Connection: Vital chunk came to fast, waiting, lvn=%d, %s\n", m_LastVitalNumber, pChunk->String());
			delete pChunk;
			return;
		}
	}

	if(pChunk->m_Type >= EChunk_Base)
	{
		if(pChunk->m_Type == EChunk_Ack)
		{
			//int32_t Num = _pChunk->m_VitalNumber; //_pChunk->m_Storage.ReadBE_int32();
			int32_t Num = pChunk->m_Reader.ReadBE<uint16>();
			//NLog::WriteF("Connection: Got ack for packet, vn=%d\n", Num);
			
			// find vital chunk and remove it
			for(int32_t i = 0; i < m_lVitalChunks.Len(); i++)
			{
				if(m_lVitalChunks[i].m_pChunk->m_VitalNumber == Num)
				{
					// remove the chunk if it's pending for sending
					/*
					for(int32_t j = 0; j < m_lPendingChunks.Len(); j++)
					{
						if(m_lPendingChunks[j] == m_lVitalChunks[i].m_pChunk)
						{
							m_lPendingChunks.RemoveByIndexFast(j);
							j--;
						}
					}*/

					// remove chunk
					delete m_lVitalChunks[i].m_pChunk;
					m_lVitalChunks.RemoveByIndexFast(i);
					
					if(m_lVitalChunks.Len() == 0 && IsConnecting())
					{
						//OnConnection();
						//SetState(EState_Connected);
					}
					
					break;
				}
			}
		}
		else if(pChunk->m_Type == EChunk_Close)
		{
			//OnDisconnection();
			Reset();
		}
		
		// remove this
		delete pChunk;
	}
	else
	{
		// application packet
		m_lReceivedChunks.Add(pChunk);
	}
}

//# Function: Init
//# Description: Inits a connection from a socket, address and state
//# Parameters:
//#		rSocket, Socket to bind
//#		rRemoteAddress, Remote address of connection.
//#		State, State of the connection.
//# Declaration:
void CConnection::Init(CChunkSocket *pSocket, NNetwork::CAddr &rRemoteAddress, int32_t State)
{
	// reset counters and store connection information
	Reset();
	m_RemoteAddress = rRemoteAddress;
	SetSocket(pSocket, false);
	SetState(State);
	m_LastReceiveTime = GetTickCount();
	m_LastSendTime = GetTickCount();
	
//	if(m_Open)
//		NLog::Write("Connection: opened\n");
}

//# Function: Close
//# Description: Closes the connection resulting in an disconnection.
//# Remarks:
//#		It sends close commands to the remote end to ensure a graceful termination.
//# Declaration:
void CConnection::Close(const int8 *pReason)
{
	if(!IsClosed())
	{
		NLog::WriteF("Connection: Closing connection, ip=%s reason=%s\n", m_RemoteAddress.Str(), pReason);
		m_ErrorMsg = pReason;
		
		// triple send so we are sure atleast something arrives
		Flush();
		QueueChunk(EChunk_Close); Flush();
		QueueChunk(EChunk_Close); Flush();
		QueueChunk(EChunk_Close); Flush();
	}

	Reset();
}

//# Function: QueueChunk (1)
//# Description: Queues a chunk for sending.
//# Parameters:
//#		pChunk, Chunk to queue.
//# Remarks:
//#		You shouldn't touch the chunk after it has been queued.
//#		CConnection will take care of deletion of the chunk.
//# Declaration:
void CConnection::QueueChunk(CChunk *pChunk)
{
	//CAutoLock Locker(m_Lock);

	if(pChunk->m_Writer.GetSize())
	{
		pChunk->m_Flags |= EFlag_Data;
		pChunk->m_DataSize = (uint16)pChunk->m_Writer.GetSize();
	}
	
	pChunk->m_SequenceNumber = (uint16)m_SequenceNumber++;
	
	if(pChunk->m_Flags&EFlag_Vital)
	{
		pChunk->m_VitalNumber = (uint16)m_VitalSequenceNumber++;

		// add this chunk to the list of vital stuff
		CVitalHolder Holder;
		Holder.m_pChunk = pChunk;
		Holder.m_LastSendTime = GetTickCount();
		Holder.m_ResendCount = 0;
		m_lVitalChunks.Add(Holder);
		m_lPendingChunks.Add(pChunk->Duplicate()); // queue chunk
	}
	else
		m_lPendingChunks.Add(pChunk); // queue chunk
}

//# Function: QueueChunk (2)
//# Description: Creates a chunk and queues it.
//# Parameters:
//#		Type, Type of the chunk.
//#		Flags, Flags.
//#		pData, Data, can be NULL.
//#		Size, Size of the data. Can be set to zero.
//# Declaration:
void CConnection::QueueChunk(uint8 Type, uint8 Flags, int8 *pData, uint16 Size)
{
	CChunk *pChunk = new CChunk;
	pChunk->Clear();
	pChunk->SetData(pData, Size);
	pChunk->m_Flags |= Flags;
	pChunk->m_Type = Type;
	
	QueueChunk(pChunk);
}


//# Function: GetChunk
//# Description: Returns a retrived chunk.
//# Returns:
//#		Returns a chunk. Returns NULL when no more chunks are available.
//# Remarks:
//#		Call update to fetch the incomming chunks, then call this function.
//#		The user is reponsible for deletion of the chunk. 
//# Declaration:
CChunk *CConnection::GetChunk()
{
	//CAutoLock Locker(m_Lock);
	if(!m_lReceivedChunks.Len())
		return NULL;
		
	CChunk *pChunk = m_lReceivedChunks[0];
	m_lReceivedChunks.RemoveByIndex(0);
	return pChunk;
}

//# Function: Flush
//# Description: Sends all queued chunks.
//# Declaration:
void CConnection::Flush()
{
	//CAutoLock Locker(m_Lock);

	// check if we should send any packets
	if(m_lPendingChunks.Len() > 0 && m_pSocket)
	{
		// send them
		m_pSocket->SendChunks(m_lPendingChunks, m_RemoteAddress);
		m_lPendingChunks.DeleteAll();
		m_LastSendTime = GetTickCount();
	}
}

//# Function: Update (protected)
//# Description:
//#		Handles resends of vital chunks. Flushes the connection ensuring that the
//#		the chunks are being sent. Updates state	
//# Declaration:
void CConnection::Update()
{
	//CAutoLock Locker(m_Lock);
	m_State = m_InternalState;

	if(IsClosed())
		return;

	// check if we need to send keep-alive
	if(GetTickCount()-m_LastSendTime > GetTickFrequency()*15)
	{
		CChunk *pChunk = new CChunk(EChunk_KeepAlive);
		QueueChunk(pChunk);
	}

	// check timeout
	if(GetTickCount()-m_LastReceiveTime > GetTickFrequency()*300)
	{
		Close("Timeout");
	}

	// check for vital chunk resends
	for(int32_t i = 0; i < m_lVitalChunks.Len(); i++)
	{
		if(m_lVitalChunks[i].m_LastSendTime+GetTickFrequency()/3 < GetTickCount())
		{
			m_lVitalChunks[i].m_LastSendTime = GetTickCount();
			m_lVitalChunks[i].m_ResendCount++;
			if(m_lVitalChunks[i].m_ResendCount > 60)
			{
				Close("Too many resends");
				break;
			}
			else
			{
				if(g_ExtraLogging)
				{
					NLog::WriteF("Connection: Resending vital chunk (vn=%d,t=%x)\n",
								m_lVitalChunks[i].m_pChunk->m_VitalNumber,
								m_lVitalChunks[i].m_pChunk->m_Type);
				}
				m_lPendingChunks.Add(m_lVitalChunks[i].m_pChunk->Duplicate());
			}
		}
	}

	//	
	Flush();
}

