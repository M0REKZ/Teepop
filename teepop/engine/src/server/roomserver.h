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

	int32_t m_SinglePlayer;
	int32_t m_UpdateNeeds;
	int32_t m_State;

	int32_t m_ID;
	int32_t m_MaxPlayers;
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
	int64_t m_LastUpdate;
	
	int32_t m_CurrentRoomID;
	TMap<int32_t, CRoom*> m_lRooms;
	TDynamicArray<CNetworkConnection*> m_lPlayers;
	
	TDynamicArray<int32_t> m_lRoomCreations;
	TDynamicArray<int32_t> m_lRoomDeletions;
	TDynamicArray<int32_t> m_lRoomUpdates;
	
	//
	virtual bool Init();
	virtual void Update();

	//
	void SendCompleateUpdate(CNetworkConnection *pConnection);
	void SendGlobalRoomUpdate();

	//
	void JoinRoom(CNetworkConnection *pConnection, CRoom *pRoom);
	const char *CreateRoom(CNetworkConnection *pConnection, const char *pName, const char *pPassword=NULL, int32_t MaxPlayers=4);
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
	void SetRoomState(CNetworkConnection *pConnection, int32_t State);
	void SetRoomState(int32_t iRoom, int32_t State);
	CRoom *GetRoom(CNetworkConnection *pConnection) { return pConnection->GetData<CRoom>(this); }
	CRoom *GetRoom(int32_t iRoom) { return m_lRooms.Find(iRoom, NULL); }

	// howswapping
	bool HotswapWrite(COutputStream &rStream);
	bool HotswapRead(CInputStream &rStream);
};
