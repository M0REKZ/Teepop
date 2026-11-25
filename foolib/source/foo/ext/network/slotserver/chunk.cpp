#include <foo/base/string.h>
#include <foo/base/memory.h>

#include "chunk.h"

using namespace NFoo;
using namespace NFoo::NNetwork;
using namespace NFoo::NSlotServer;

//# Req_Namespace: NFoo::NSlotServer
//# Req_Header: ext/network/slotserver/slotserver.h
//# Req_Library: 
//
//# Context: Ext/Network/SlotServer/CChunk
//

int32_t g_NumChunks = 0;



//
// constructor
//
CChunk::CChunk(uint8_t _Type, uint8_t _Flags)
{
	Clear();
	m_Type = _Type;
	m_Flags = _Flags;

	ResetStreams();
	
//	g_NumChunks++;
}


/*
NFoo::CMemoryWriter &CChunk::BeginWrite()
{
	m_Writer.Set(m_aData);
	return m_Writer;
}

int32_t CChunk::EndWrite()
{
	m_DataSize = m_Writer.GetSize();
	if(m_DataSize)
		m_Flags |= EFlag_Data;
	else
		m_Flags &= ~EFlag_Data;
	return m_DataSize;
}*/

void CChunk::ResetStreams()
{
	m_Writer.Set(m_aData);
	m_Reader.Set(m_aData, EMaxPacketSize);
}

CChunk::~CChunk()
{
//	g_NumChunks--;
}


CChunk *CChunk::Duplicate()
{
	CChunk *pChunk = new CChunk();
	pChunk->m_Type = m_Type;
	pChunk->m_Flags = m_Flags;
	pChunk->m_SequenceNumber = m_SequenceNumber;
	pChunk->m_VitalNumber = m_VitalNumber;
	pChunk->m_DataSize = m_DataSize;
	MemoryCopy(pChunk->m_aData, m_aData, EMaxPacketSize);
	return pChunk;
}






//
// Chunk read and write
//
void CChunk::Write(COutputStream &_rStream)
{
	_rStream.WriteBE(m_Type);
	_rStream.WriteBE(m_Flags);
	_rStream.WriteBE(m_SequenceNumber);
	
	if(m_Flags&EFlag_Vital)
		_rStream.WriteBE(m_VitalNumber);
		
	if(m_Flags&EFlag_Data)
	{
		_rStream.WriteBE(m_DataSize);
		_rStream.Write(m_aData, m_DataSize);
	}
}

int32_t CChunk::GetSize()
{
	int32_t Size = sizeof(m_Type)+sizeof(m_Flags)+sizeof(m_SequenceNumber);
	
	if(m_Flags&EFlag_Vital)
		Size += sizeof(m_VitalNumber);
		
	if(m_Flags&EFlag_Data)
		Size += sizeof(m_DataSize)+m_DataSize;

	return Size;
}

void CChunk::Read(CInputStream &_rStream)
{
	_rStream.ReadBE(m_Type);
	_rStream.ReadBE(m_Flags);
	_rStream.ReadBE(m_SequenceNumber);
	
	if(m_Flags&EFlag_Vital)
		_rStream.ReadBE(m_VitalNumber);
	
	if(m_Flags&EFlag_Data)
	{
		_rStream.ReadBE(m_DataSize);
		_rStream.Read(m_aData, m_DataSize);
	}
}

void CChunk::Clear()
{
	m_Type = 0;
	m_Flags = 0;
	m_SequenceNumber = 0;
	m_VitalNumber = 0;
	m_DataSize = 0;
}

void CChunk::SetData(const char *_pData, uint16_t _Size)
{
	if(_pData && _Size > 0)
	{
		m_Flags |= EFlag_Data;
		m_DataSize = _Size;
		MemoryCopy(m_aData, _pData, _Size);
	}
	else
	{
		m_Flags &= ~EFlag_Data;
		m_DataSize = 0;
	}
}

const char *CChunk::String()
{
	static char aStr[512];
	if(m_Flags&EFlag_Vital)
		StringCFormat(aStr, sizeof(aStr), "(type=%x, flags=0x%x, datasize=0x%d, vitalnumber=%d)", m_Type, m_Flags, m_DataSize, m_VitalNumber);
	else
		StringCFormat(aStr, sizeof(aStr), "(type=%x, flags=0x%x, datasize=0x%d, vn=)", m_Type, m_Flags, m_DataSize);
	return aStr;
}

//
//# Context: Ext/Network/SlotServer/CChunkSocket
//

//
//# Function: RecvChunks
//# Description: Recivces chunks.
//# Parameters:
//#		rlChunks, List to be filled with the incomming chunks.
//#		rSource, Source will be filled with the source address of the chunks.
//# Returns:
//#		Returns the number of chunks recvied.
//# Remarks:
//#		The applications is reponsible for deleting the chunk. You should call RecvChunks until it
//#		returns 0.
//# Declaration:
int32_t CChunkSocket::RecvChunks(NFoo::TDynamicArray<CChunk*> &rlChunks, NFoo::NNetwork::CAddr &rSource)
{
	// clear list
	rlChunks.Clear();

	// Fetch Packet
	char Buffer[EMaxPacketSize];
	NNetwork::CAddr Addr;

	int32_t RecvBytes = RecvFrom(Buffer, EMaxPacketSize, Addr);
	if(RecvBytes > 0)
	{
		rSource = Addr;
		
		CMemoryReader Packet(Buffer, RecvBytes);
		int32_t NumChunks = Packet.ReadBE<uint8_t>();
		
		for(int32_t i = 0; i < NumChunks; i++)
		{
			CChunk *pChunk = new CChunk;
			rlChunks.Add(pChunk);
			pChunk->Read(Packet);
		}
		
		//RecvBytes = RecvFrom(Buffer, EMaxPacketSize, Addr);
	}
	
	return rlChunks.Len();
}

//# Function: SendChunks
//# Description: Adds a chunk to the send queue
//# Parameters:
//#		pChunk, Chunk to be queued
//#		rTarget, Target for the chunk
//# Returns:
//#		Returns size of the final package.
//# Remarks:
//#		Application is in charge of deletion of the chunks.
//# Declaration:
int32_t CChunkSocket::SendChunks(NFoo::TDynamicArray<CChunk*> &rlChunks, NFoo::NNetwork::CAddr &rTarget)
{
	//NFoo::TDynamicArray<CChunk*> lChunks;
	//lChunks = rlChunks;
	int32_t TotalSize = 0;
	int32_t Base = 0;
	
	while(Base < rlChunks.Len())
	{
		// see how many chunks we can fit into this packet
		int32_t Size = 1;
		int32_t NumChunks = 0;
		for(int32_t i = Base; i < rlChunks.Len(); i++)
		{
			int32_t ChunkSize = rlChunks[i]->GetSize();
			if(ChunkSize < EMaxPacketSize-Size)
			{
				Size += ChunkSize;
				NumChunks++;
				if(NumChunks == 255)
					break;
			}
			else
				break;
		}

		// construct and send packet
		{
			char aBuffer[EMaxPacketSize];
			CMemoryWriter Packet(aBuffer);
			Packet.WriteBE((uint8_t)NumChunks);
			for(int32_t i = Base; i < NumChunks; i++)
				rlChunks[i]->Write(Packet);

			SendTo(aBuffer, Packet.GetSize(), rTarget);

			TotalSize += Size;
			Base += NumChunks;
		}
	}

	return TotalSize;
}

