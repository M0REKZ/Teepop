#include "network.h"

namespace NEngine
{
	//
	//
	//
	class CNetworkClient : public CNetworkConnection, public CConsoleClient, public CMessageSender, public CModule
	{
		int32_t m_LastState;
		CSlotClient m_SlotClient;
		CString m_VersionString;
	protected:
		virtual bool Init();
		virtual void OnCommand(CString &rCmd, CString &rArgs);
		void UpdateState();
	public:
		enum
		{
			EEvent_Connecting,
			EEvent_Disconnected,
			EEvent_Connected
		};

		void SetVersion(const int8 *pVersion) { m_VersionString = pVersion; }

		bool IsConnecting() { return m_SlotClient.IsConnecting(); }
		bool IsConnected() { return m_SlotClient.IsConnected(); }
		bool IsClosed() { return m_SlotClient.IsClosed(); }

		void Connect(NNetwork::CAddr Addr) { m_SlotClient.Connect(Addr, m_VersionString); UpdateState(); }
		void Disconnect() { m_SlotClient.Disconnect(); UpdateState(); }

		virtual const int8 *GetStatusString();
		virtual void Update();
	};
};