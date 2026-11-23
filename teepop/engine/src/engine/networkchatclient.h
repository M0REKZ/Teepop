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
		TMap<int32, CString> m_lUsers;

		enum
		{
			EEvent_ChannelSwitched,
			EEvent_ChatMessage
		};

		const int8 *GetUserName(int32 id);
		void SetChannel(const int8 *pChannel);
		void Chat(const int8 *pMsg);
	};
}