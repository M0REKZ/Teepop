#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <foo/func/dynamicarray.h>
#include <foo/func/stream.h>
#include <foo/func/memorystream.h>
#include <foo/ext/network/network.h>

namespace NFoo { namespace NSlotServer {

	enum
	{
		EChunk_Base=0xF0,
		EChunk_RequestSlot,	// Client  -> Server
		EChunk_Accepted,	// Client <-  Server
		EChunk_Rejected,	// Client <-  Server
		EChunk_KeepAlive,	// Client <-> Server
		EChunk_Ack,			// Client <-> Server
		EChunk_Close,		// Client <-> Server
		
		EMaxPacketSize=1024,
		
		EFlag_Vital=1,
		EFlag_Data=2
	};


	//
	// Chunk
	//
	// Could optimize abit.
	// Make the flags 6 bit and the datasize 12bit
	// Perhaps Sequence and Vital number could share 24bit instead of 32bit
	//
	class CChunk
	{
		char m_aData[EMaxPacketSize];
	public:
		uint8_t m_Type;
		uint8_t m_Flags;
		uint16_t m_SequenceNumber;
		uint16_t m_VitalNumber;
		uint16_t m_DataSize;
		
		const char *String();

		int32_t GetSize();

		NFoo::CMemoryWriter m_Writer; // used for costruction purposes
		NFoo::CMemoryReader m_Reader; // used for costruction purposes
		void ResetStreams();

		//NFoo::CMemoryWriter &BeginWrite();
		//int32_t EndWrite();

		//NFoo::NNetwork::CAddr m_Address; // Target / Source address

		CChunk(uint8_t Type=0, uint8_t Flags=0);
		~CChunk();

		CChunk *Duplicate();
		void Clear();
		void SetData(const char *pData, uint16_t Size);
		void Write(NFoo::COutputStream &rStream);
		void Read(NFoo::CInputStream &rStream);
	};

	//
	//
	//
	class CChunkSocket : public NFoo::NNetwork::CSocket
	{
	public:
		int32_t RecvChunks(NFoo::TDynamicArray<CChunk*> &rlChunks, NFoo::NNetwork::CAddr &rSource);
		int32_t SendChunks(NFoo::TDynamicArray<CChunk*> &rlChunks, NFoo::NNetwork::CAddr &rTarget);
	};
} }
#endif // __CHUNK_H__

