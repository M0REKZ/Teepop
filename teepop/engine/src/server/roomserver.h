#include "../engine/network.h"

using namespace NEngine;

//
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

	int32 m_SinglePlayer;
	int32 m_UpdateNeeds;
	int32 m_State;

	int32 m_ID;
	int32 m_MaxPlayers;
	CString m_Name;
	CString m_Password;
	TDynamicArray<CNetworkConnection*> m_lPlayers;

	void PackAll(COutputStream &rStream);
	void PackPartial(COutputStream &rStream, bool DoId = true);
};

//
//
//
class CNetworkRoomServer : public CNetworkMsgRecviver, public CConsoleClient, public CMessageSender, public CModule
{
protected:
	int64 m_LastUpdate;
	
	int32 m_CurrentRoomID;
	TMap<int32, CRoom*> m_lRooms;
	TDynamicArray<CNetworkConnection*> m_lPlayers;
	
	TDynamicArray<int32> m_lRoomCreations;
	TDynamicArray<int32> m_lRoomDeletions;
	TDynamicArray<int32> m_lRoomUpdates;
	
	//
	virtual bool Init();
	virtual void Update();

	//
	void SendCompleateUpdate(CNetworkConnection *pConnection);
	void SendGlobalRoomUpdate();

	//
	void JoinRoom(CNetworkConnection *pConnection, CRoom *pRoom);
	const int8 *CreateRoom(CNetworkConnection *pConnection, const int8 *pName, const int8 *pPassword=NULL, int32 MaxPlayers=4);
	void LeaveRoom(CNetworkConnection *pConnection, bool SendNetMessage = true);

	//
	virtual void OnAttach(CNetworkConnection *pConnection);
	virtual void OnDetach(CNetworkConnection *pConnection);
	virtual void OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream);

	//
	//
	virtual void OnCommand(CString &rCmd, CString &rArgs);

public:

	//
	//
	enum
	{
		EEvent_Created,
		EEvent_Destroyed,
		EEvent_Joined,
		EEvent_Left
	};

	//
	//
	void SetRoomState(CNetworkConnection *pConnection, int32 State);
	void SetRoomState(int32 iRoom, int32 State);
	CRoom *GetRoom(CNetworkConnection *pConnection) { return pConnection->GetData<CRoom>(this); }
	CRoom *GetRoom(int32 iRoom) { return m_lRooms.Find(iRoom, NULL); }

	// howswapping
	bool HotswapWrite(COutputStream &rStream);
	bool HotswapRead(CInputStream &rStream);
};
