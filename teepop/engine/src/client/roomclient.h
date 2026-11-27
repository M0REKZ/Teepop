#include "../engine/network.h"

using namespace NEngine;

//
//
class CRoom
{
public:
	enum
	{
		EState_Stopped=0,
		EState_Playing=1,
		EState_Mask=0xf,
		EState_PasswordProtected=0x80
	};

	int32_t m_ID;
	int32_t m_State;
	int32_t m_MaxPlayers;
	bool m_PasswordProtected;
	CString m_Name;
	TDynamicArray<int32_t> m_lPlayers;

	CString GetString()
	{
		const char *aStates[2] = {"Waiting", "Started"};
		return CString("%-20s %d/%d  %s", m_Name.GetData(), m_lPlayers.Len(), m_MaxPlayers, aStates[m_State&EState_Mask]);
	}
};

//
//
class CNetworkRoomClient : public CNetworkMsgRecviver, public CConsoleClient, public CMessageSender, public CModule
{
protected:
	CNetworkConnection *m_pConnection;

	//
	virtual bool Init();

	// network
	virtual void OnAttach(CNetworkConnection *pConnection);
	virtual void OnDetach(CNetworkConnection *pConnection);
	virtual void OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream);

	// console
	virtual void OnCommand(CString &rCmd, CString &rArgs);

public:
	enum
	{
		EEvent_RoomChange,
		EEvent_RoomCreateError,
		EEvent_RoomsUpdated
	};

	int32_t m_iCurrentRoom;
	TMap<int32_t, CRoom> m_lRooms;

	// room commands
	void JoinRoom(int32_t RoomID);
	void JoinRoom(const char *pName);
	void LeaveRoom();
	void CreateRoom(const char *pName, const char *pPassword, int32_t Max);
};
