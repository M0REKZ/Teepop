#include "network.h"

namespace NEngine
{
	//
	//
	//
	class CNetworkServer : public CModule // : public CNetworkBase
	{
		CSlotServer m_SlotServer;
		TMap<int32, CNetworkConnection*> m_mConnections;
		CString m_VersionString;
	public:

		//
		void Start(int32 Port);
		void GetClients(TDynamicArray<int32> &lClients);
		void SetVersion(const int8 *pVersion) { m_VersionString = pVersion; }
		//int32 SendMessage(int32 iClient, CNetMessage &rMsg);

		CNetworkConnection *GetClient(int32 iClient) { return m_mConnections.Find(iClient, NULL); }
		virtual const int8 *GetStatusString();
		virtual void Update();

		typedef CNetworkConnection *(*PFNClientCreator)(void*);
		void SetClientCreator(PFNClientCreator pfnFunc, void *pCreatorData) { m_pfnClientCreator = pfnFunc; m_pCreatorData = pCreatorData; }
	private:
		PFNClientCreator m_pfnClientCreator;
		void *m_pCreatorData;
		static CNetworkConnection *DefaultCreator(void*) { return new CNetworkConnection(); };

		bool HotswapWrite(COutputStream &rStream);
		bool HotswapRead(CInputStream &rStream);
	};
}