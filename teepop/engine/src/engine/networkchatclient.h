#include "network.h"

namespace NEngine
{
	class CNetworkChatClient : public CNetworkMsgRecviver, public CConsoleClient, public CMessageSender, public CModule
	{
	protected:
		CNetworkConnection *m_pConnection;
		bool Init();
		virtual void OnAttach(CNetworkConnection *pConnection);
		virtual void OnDetach(CNetworkConnection *pConnection);
		virtual void OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream);
		virtual void OnCommand(CString &rCmd, CString &rArgs);

		CString m_Channel;
	public:
		TMap<int32_t, CString> m_lUsers;

		enum
		{
			EEvent_ChannelSwitched,
			EEvent_ChatMessage
		};

		const char *GetUserName(int32_t id);
		void SetChannel(const char *pChannel);
		void Chat(const char *pMsg);
	};
}