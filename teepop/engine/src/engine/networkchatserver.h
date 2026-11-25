#include "network.h"

namespace NEngine
{
	//
	//
	//
	class CNetworkChatServer : public CNetworkMsgRecviver, public CConsoleClient, public CMessageSender, public CModule
	{
	protected:
		CNetworkConnection *m_pConnection;
		bool Init();
		virtual void OnAttach(CNetworkConnection *pConnection);
		virtual void OnDetach(CNetworkConnection *pConnection);
		virtual void OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream);
		virtual void OnCommand(CString &rCmd, CString &rArgs);

		class CUser;
		class CChannel;

		class CChannel
		{
		public:
			CString m_Name;
			TDynamicArray<CUser*> m_lMembers;
		};

		class CUser
		{
		public:
			CString m_Name;
			CNetworkConnection *m_pConnection;
			CChannel *m_pChannel;
		};

		TDynamicArray<CChannel*> m_lChannels;
		//TMap<CNetworkConnection*, CUser*> m_lUsers;

		void DestroyUser(CUser *pUser);
		bool LeaveChannel(CUser *pUser);
		bool SetChannel(CUser *pUser, CChannel *pChannel);
		bool SetChannel(CUser *pUser, const char *pName);
		CChannel *FindChannel(const char *pName);
		void Chat(CUser *pUser, const char *pMessage);
		void Broadcast(const char *pMessage);

		void SendMessageToChannel(CChannel *pChannel, CNetMessage &rMsg);

	public:
		bool SetUserName(CNetworkConnection *pConnection, const char *pName);
		bool SetChannel(CNetworkConnection *pConnection, const char *pName);

		enum
		{
			EEvent_JoinChannel,
			EEvent_LeaveChannel,
			EEvent_UserCreated,
			EEvent_UserDestroyed,
			EEvent_Chat
		};

		bool HotswapWrite(COutputStream &rStream);
		bool HotswapRead(CInputStream &rStream);

	};
}