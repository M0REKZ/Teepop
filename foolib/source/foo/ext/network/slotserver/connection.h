#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <foo/base/base.h>
#include "chunk.h"

namespace NFoo { namespace NSlotServer {

	//
	// Connection
	//
	class CConnection
	{
		friend class CSlotServer;
		friend class CSlotClient;

		CString m_ErrorMsg;

		// state of the connection
		int32_t m_State;
		int32_t m_InternalState; //
		NFoo::NNetwork::CAddr m_RemoteAddress;

		// used for timeout
		int64_t m_LastReceiveTime;
		int64_t m_LastSendTime;

		//
		int32_t m_SequenceNumber;
		int32_t m_VitalSequenceNumber;

		int32_t m_LastVitalNumber;
		
		// remote location
		CChunkSocket *m_pSocket;
		bool m_HandlingSocket;
		
		NFoo::TDynamicArray<CChunk*> m_lReceivedChunks; // chunks that we have recived
		NFoo::TDynamicArray<CChunk*> m_lPendingChunks; // chunks that are going to be sent
		
		class CVitalHolder
		{
		public:
			CChunk *m_pChunk;
			int64_t m_LastSendTime;
			int32_t m_ResendCount;
		};
		
		NFoo::TDynamicArray<CVitalHolder> m_lVitalChunks; // are going to be resent if they don't get an ack
		
		virtual void Close(const char *pReason="");

	protected:
		CLock m_Lock;

		void SetState(int32_t _State) { m_InternalState = _State; }
		void SetSocket(CChunkSocket *pSocket, bool handle);
		void Process(CChunk *_pChunk); // resposible for deleting the chunk if needed

		// state handling
		void Init(CChunkSocket *pSocket, NFoo::NNetwork::CAddr &rRemoteAddress, int32_t state);
		void Update();
		void Reset();
		
	public:
		
		enum
		{
			EState_Closed=0,
			EState_Connecting,
			EState_Connected
		};

		CConnection();
		virtual ~CConnection();

		const char *GetErrorMsg() { return m_ErrorMsg; }

		// state retrival
		int32_t GetState() { return m_State; }
		bool IsConnected() { return m_State==EState_Connected; }
		bool IsConnecting() { return m_State==EState_Connecting; }
		bool IsClosed() { return m_State==EState_Closed; }
		
		NFoo::NNetwork::CAddr GetRemoteAddress() { return m_RemoteAddress; }

		//	
		void QueueChunk(CChunk *_pChunk); // (LOCKS)
		void QueueChunk(uint8_t _Type, uint8_t _Flags=0, char *_pData=NULL, uint16_t _Size=0);

		CChunk *GetChunk();  // (LOCKS)
		void Flush(); // (LOCKS) sends all pending chunks
		
		// time handling
		int32_t GetTimeoutCounter() { return 0; }; // in milliseconds
		int32_t GetLatency() { return 0; }; // in milliseconds
	};
} }

#endif // __CONNECTION_H__

