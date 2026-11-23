#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <foo/func/thread.h>
#include "connection.h"

namespace NFoo { namespace NSlotServer {

	//
	// Client
	//
	class CSlotClient : public CConnection, private CThread
	{
		CChunkSocket m_Socket;
		

		int32 m_iClient;
		bool m_Threaded;
		virtual int32 ThreadMain();

		void PumpSocket();

		virtual void Close();
	public:
		CSlotClient();
		~CSlotClient();

		int32 GetID() { return m_iClient; }
		
		void Connect(NFoo::NNetwork::CAddr _Addr, const int8 *pVersionString, bool Threaded=false);
		void Disconnect();
		
		void Update();
	};
} }
#endif // __CLIENT_H__

