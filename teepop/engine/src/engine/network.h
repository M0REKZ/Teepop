#ifndef FOO_FILE_ENGINE_NETWORK_H
#define FOO_FILE_ENGINE_NETWORK_H

#include <foo/base/string.h>
#include <foo/base/log.h>
#include <foo/func/memorystream.h>
#include <foo/func/map.h>
#include <foo/func/message.h>
#include <foo/ext/network/slotserver/slotserver.h>
#include "engine.h"

namespace NEngine
{
	using namespace NFoo;
	using namespace NFoo::NNetwork;
	using namespace NFoo::NSlotServer;

	//
	class CNetworkModule;
	class CNetworkBase;
	class CNetworkClient;
	class CNetworkServer;

	//#define HASHED_MESSAGE_ID

	#ifdef HASHED_MESSAGE_ID	
		#define NETMSG(x) StringGetHash(x)
		typedef int32 NetMsgID;
	#else
		#define NETMSG(x) x
		typedef CString NetMsgID;
	#endif


	enum
	{
		EChunk_Message
	};

	//
	//
	//
	class CNetMessage : public CMemoryWriter
	{
		friend class CNetworkClient;
		friend class CNetworkServer;
	protected:
		NetMsgID m_iMsg;
			
		uint8 m_aData[512];
		bool m_Vital;
	public:
		CNetMessage(NetMsgID iMsg, bool Vital)
		: CMemoryWriter(m_aData), m_iMsg(iMsg), m_Vital(Vital)
		{}

		NetMsgID GetID() { return m_iMsg; }

		void Log()
		{
			#ifdef HASHED_MESSAGE_ID
				NLog::WriteF("%d ", m_iMsg)
			#else
				NLog::WriteF("%s ", m_iMsg.GetData());
			#endif

			NLog::WriteF("size=%d vital=%d ", GetSize(), m_Vital);


			uint8 aTemp[512];
			for(int32 i = 0; i < GetSize() && i < 10; i++)
				NLog::WriteF("%x ", m_aData[i]);
			NLog::WriteF("\n");
		};

		CChunk *CreateChunk()
		{
			CChunk *pChunk = new CChunk(EChunk_Message, m_Vital);
			#ifdef HASHED_MESSAGE_ID
				pChunk->m_Writer.WriteBE<int32>(m_iMsg);
			#else
				pChunk->m_Writer.Write_p16(m_iMsg);
			#endif
			pChunk->m_Writer.Write(m_aData, GetSize());
			return pChunk;
		}
	};

	//
	//
	//
	class CNetworkMsgRecviver;
	class CNetworkConnection;

	//
	//
	//
	class CNetworkMsgRecviver
	{
	public:
		virtual void OnAttach(CNetworkConnection *pConnection) {};
		virtual void OnDetach(CNetworkConnection *pConnection) {};
		virtual void OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream) {};
	};

	//

	//
	//
	//
	class CNetworkConnection
	{
		friend class CNetworkServer;
		friend class CNetworkClient;
		TMap<NetMsgID, CNetworkMsgRecviver*> m_mMessageMap;
		TMap<CNetworkMsgRecviver*, void*> m_mReciverData;

		CConnection *m_pConnection;
	protected:
		int32 m_iClient;

		virtual void OnNetMessage(NetMsgID iMsg, CInputStream &rStream);
		virtual void OnConnect() {};
		virtual void OnDisconnect() {};

	public:

		int32 ClientID() { return m_iClient; }

		template<class T>
		T *GetData(CNetworkMsgRecviver *pRecviver) { return (T*)GetData(pRecviver); }
		void SetData(CNetworkMsgRecviver *pRecviver, void *pData);
		void *GetData(CNetworkMsgRecviver *pRecviver);

		void RegisterNetMessage(NetMsgID iMsg, CNetworkMsgRecviver *pRecviver);
		void AttachRecviver(CNetworkMsgRecviver *pRecviver);
		void DetachRecviver(CNetworkMsgRecviver *pRecviver);
		void DetachAllRecvivers();

		CNetworkConnection();
		virtual ~CNetworkConnection();

		bool IsActive();

		void DispatchNetMessage(int32 iMsg, CInputStream &rStream);
		void SendMessage(CNetMessage &rMsg);
	};
}

#endif // FOO_FILE_ENGINE_NETWORK_H

