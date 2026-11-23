#include "network.h"

namespace NEngine
{
	//
	//
	//
	class CNetworkAuthServer : public CNetworkMsgRecviver, public CConsoleClient, public CMessageSender, public CModule
	{
	protected:
		virtual ~CNetworkAuthServer();
		bool Init();
		virtual void OnAttach(CNetworkConnection *pConnection);
		virtual void OnDetach(CNetworkConnection *pConnection);
		virtual void OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream);
		virtual void OnCommand(CString &rCmd, CString &rArgs);

		class CUser
		{
		public:
			CUser()
			{
				m_UserLevel = -1;
			}

			CUser(CString name, CString password, int32 level, bool Guest)
			{
				m_Name = name;
				m_Password = password;
				m_UserLevel = level;
				m_GuestAccount = Guest;
				m_LoggedOns = 0;
			}

			CString m_Name;
			CString m_Password;
			int32 m_UserLevel;
			int32 m_LoggedOns;
			bool m_GuestAccount;
		};

		// server
		bool m_AllowGuests;
		bool m_AllowMultipleLogons;

		TDynamicArray<CUser*> m_lUsers;
		//TMap<CNetworkConnection*, CUser*> m_lAuths;

		CUser *GetUser(const int8 *pName);
		int32 GetAuthLevel(CNetworkConnection *pConnection);

	public:
		enum
		{
			EEvent_AuthSuccess,
			EEvent_AuthFailed
		};


		bool NewUser(CString Name, CString Password, int32 Level, bool Guest);
		bool IsAuthed(CNetworkConnection *pConn) { return pConn->GetData(this) != NULL; }

		bool HotswapWrite(COutputStream &rStream);
		bool HotswapRead(CInputStream &rStream);
	};
}