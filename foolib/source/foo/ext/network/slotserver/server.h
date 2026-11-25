#ifndef __SERVER_H__
#define __SERVER_H__

#include <foo/func/string.h>
#include "connection.h"

namespace NFoo { namespace NSlotServer {

	//
	// Server Object
	//
	class CSlotServer
	{
	public:
		class CSlot : public CConnection
		{
		public:
			CSlot();
			NFoo::CString m_Name;
			uint32_t m_UserData;
			int32_t m_iClient;		// Unique client id

			//int32_t m_SwitchState; // 0 = normal, 1 = turned online, 2 = turned offline

			CSlot &operator = (const CSlot &rSlot)
			{
				m_Name = rSlot.m_Name;
				m_UserData = rSlot.m_UserData;
				m_iClient = rSlot.m_iClient;
				return *this;
			}
		};
	private:
		NFoo::NNetwork::CSocket m_StatsSocket;

		CString m_VersionString;

		CChunkSocket m_Socket;
		TDynamicArray<CSlot*> m_lSlot;
		CSlot *NewSlot();
		int32_t m_CurrentClientID;
	public:
		int32_t GetNumClients();
		int32_t GetClientID(int32_t Index);
		CConnection *GetClient(int32_t Index);
		CConnection *FindClient(int32_t iClient);
		
		CSlotServer();
		~CSlotServer();

		bool Running();
		bool Start(uint16 _Port, const int8 *pVersionString);
		void Stop();

		void Update();
	};
} }
#endif // __NETWORKPART_H__

