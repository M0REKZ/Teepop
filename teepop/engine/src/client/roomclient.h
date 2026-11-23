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

	int32 m_ID;
	int32 m_State;
	int32 m_MaxPlayers;
	bool m_PasswordProtected;
	CString m_Name;
	TDynamicArray<int32> m_lPlayers;

	CString GetString()
	{
		int8 *aStates[2] = {"Waiting", "Started"};
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

	int32 m_iCurrentRoom;
	TMap<int32, CRoom> m_lRooms;

	// room commands
	void JoinRoom(int32 RoomID);
	void JoinRoom(const int8 *pName);
	void LeaveRoom();
	void CreateRoom(const int8 *pName, const int8 *pPassword, int32 Max);
};
