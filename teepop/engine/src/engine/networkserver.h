#include "network.h"

namespace NEngine
{
	//
	//
	//
	class CNetworkServer : public CModule // : public CNetworkBase
	{
		CSlotServer m_SlotServer;
		TMap<int32_t, CNetworkConnection*> m_mConnections;
		CString m_VersionString;
	public:

		//
		void Start(int32_t Port);
		void GetClients(TDynamicArray<int32_t> &lClients);
		void SetVersion(const char *pVersion) { m_VersionString = pVersion; }
		//int32_t SendMessage(int32_t iClient, CNetMessage &rMsg);

		CNetworkConnection *GetClient(int32_t iClient) { return m_mConnections.Find(iClient, NULL); }
		virtual const char *GetStatusString();
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