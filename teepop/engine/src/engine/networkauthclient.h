#include "network.h"

namespace NEngine
{
	//
	//
	//
	class CNetworkAuthClient : public CNetworkMsgRecviver, public CConsoleClient, public CMessageSender, public CModule
	{
	protected:
		CNetworkConnection *m_pConnection;
		bool Init();
		virtual void OnAttach(CNetworkConnection *pConnection);
		virtual void OnDetach(CNetworkConnection *pConnection);
		virtual void OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream);
		virtual void OnCommand(CString &rCmd, CString &rArgs);

		// client
		CString m_Username;
		CString m_Password;
		
		int32_t m_LoggedOn;
		CString m_ErrorMessage;

	public:
		enum
		{
			EEvent_Success,
			EEvent_Failed
		};

		void Logon(const char *pUsername, const char *pPassword);
		CString GetErrorMessage() { return m_ErrorMessage; }
	};
 }