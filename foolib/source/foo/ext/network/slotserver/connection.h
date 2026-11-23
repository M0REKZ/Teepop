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
		int32 m_State;
		int32 m_InternalState; //
		NFoo::NNetwork::CAddr m_RemoteAddress;

		// used for timeout
		int64 m_LastReceiveTime;
		int64 m_LastSendTime;

		//
		int32 m_SequenceNumber;
		int32 m_VitalSequenceNumber;

		int32 m_LastVitalNumber;
		
		// remote location
		CChunkSocket *m_pSocket;
		bool m_HandlingSocket;
		
		NFoo::TDynamicArray<CChunk*> m_lReceivedChunks; // chunks that we have recived
		NFoo::TDynamicArray<CChunk*> m_lPendingChunks; // chunks that are going to be sent
		
		class CVitalHolder
		{
		public:
			CChunk *m_pChunk;
			int64 m_LastSendTime;
			int32 m_ResendCount;
		};
		
		NFoo::TDynamicArray<CVitalHolder> m_lVitalChunks; // are going to be resent if they don't get an ack
		
		virtual void Close(const int8 *pReason="");

	protected:
		CLock m_Lock;

		void SetState(int32 _State) { m_InternalState = _State; }
		void SetSocket(CChunkSocket *pSocket, bool handle);
		void Process(CChunk *_pChunk); // resposible for deleting the chunk if needed

		// state handling
		void Init(CChunkSocket *pSocket, NFoo::NNetwork::CAddr &rRemoteAddress, int32 state);
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

		const int8 *GetErrorMsg() { return m_ErrorMsg; }

		// state retrival
		int32 GetState() { return m_State; }
		bool IsConnected() { return m_State==EState_Connected; }
		bool IsConnecting() { return m_State==EState_Connecting; }
		bool IsClosed() { return m_State==EState_Closed; }
		
		NFoo::NNetwork::CAddr GetRemoteAddress() { return m_RemoteAddress; }

		//	
		void QueueChunk(CChunk *_pChunk); // (LOCKS)
		void QueueChunk(uint8 _Type, uint8 _Flags=0, int8 *_pData=NULL, uint16 _Size=0);

		CChunk *GetChunk();  // (LOCKS)
		void Flush(); // (LOCKS) sends all pending chunks
		
		// time handling
		int32 GetTimeoutCounter() { return 0; }; // in milliseconds
		int32 GetLatency() { return 0; }; // in milliseconds
	};
} }

#endif // __CONNECTION_H__

