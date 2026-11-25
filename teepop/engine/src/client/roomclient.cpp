#include "roomclient.h"

MACRO_REGISTER_CLASS(CNetworkRoomClient, "EngineModule")

//
//
bool CNetworkRoomClient::Init()
{
	m_pCore->RegisterCommand("join", this);
	m_pCore->RegisterCommand("create", this);
	m_pCore->RegisterCommand("leave", this);
	m_pConnection = NULL;
	return CModule::Init();
}

//
//
void CNetworkRoomClient::OnAttach(CNetworkConnection *pConnection)
{
	m_pConnection = pConnection;
	m_pConnection->RegisterNetMessage(NETMSG("clRoomUpdate"), this);
	m_pConnection->RegisterNetMessage(NETMSG("clRoomChanged"), this);
	m_pConnection->RegisterNetMessage(NETMSG("clCreateRoomError"), this);
	m_lRooms.Clear();
	m_iCurrentRoom = -1;
}

//
//
void CNetworkRoomClient::OnDetach(CNetworkConnection *pConnection)
{
	m_pConnection = NULL;
	m_lRooms.Clear();
}

//
//
void CNetworkRoomClient::OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream)
{
	if(iMsg == NETMSG("clRoomUpdate"))
	{
		int32_t Count = 0;

		// deletions
		Count = rStream.ReadBE<uint8>();
		for(int32_t i = 0; i < Count; i++)
			m_lRooms.Remove(rStream.ReadBE<int32_t>());

		// creations
		Count = rStream.ReadBE<uint8>();
		for(int32_t i = 0; i < Count; i++)
		{
			int32_t RoomID = rStream.ReadBE<int32_t>();
			CRoom &rRoom = m_lRooms[RoomID];
			rRoom.m_ID = RoomID;
			rRoom.m_Name = rStream.Read_p8();
			rRoom.m_MaxPlayers = rStream.ReadBE<uint8>();
			rRoom.m_State = rStream.ReadBE<uint8>();
			int32_t NumPlayers = rStream.ReadBE<uint8>();
			rRoom.m_lPlayers.Clear();
			for(int32_t i = 0; i < NumPlayers; i++)
				rRoom.m_lPlayers.Add(rStream.ReadBE<int32_t>());
		}

		// updates
		Count = rStream.ReadBE<uint8>();
		for(int32_t i = 0; i < Count; i++)
		{
			int32_t RoomID = rStream.ReadBE<int32_t>();
			int32_t State = rStream.ReadBE<uint8>();
			int32_t NumPlayers = rStream.ReadBE<uint8>();
			if(m_lRooms.Exist(RoomID))
			{
				CRoom &rRoom = m_lRooms[RoomID];
				rRoom.m_State = State;
				rRoom.m_lPlayers.Clear();
				for(int32_t i = 0; i < NumPlayers; i++)
					rRoom.m_lPlayers.Add(rStream.ReadBE<int32_t>());
			}
			else
			{
				for(int32_t i = 0; i < NumPlayers; i++)
					rStream.ReadBE<int32_t>();
			}
		}

		Signal(EEvent_RoomsUpdated);
	}
	else if(iMsg == NETMSG("clCreateRoomError"))
	{
		CString Error = rStream.Read_p8();
		CMessage Msg(EEvent_RoomCreateError);
		Msg.SetString("errormsg", Error);
		Signal(Msg);
	}
	else if(iMsg == NETMSG("clRoomChanged"))
	{
		m_iCurrentRoom = rStream.ReadBE<int32_t>();
		CMessage Msg(EEvent_RoomChange);
		Msg.SetInt("roomid", m_iCurrentRoom);
		Signal(Msg);
	}
}

//
//
void CNetworkRoomClient::OnCommand(CString &rCmd, CString &rArgs)
{
	if(rCmd == "create")
	{
		CString name = rArgs.GetStrSep(" ");
		int32_t max = rArgs.GetStrSep(" ").GetInt();
		CString password = rArgs.GetStrSep(" ");
		CreateRoom(name, password, max);
	}
	else if(rCmd == "join")
		JoinRoom(rArgs);
	else if(rCmd == "leave")
		LeaveRoom();
}


//
//
void CNetworkRoomClient::JoinRoom(int32_t RoomID)
{
	if(!m_pConnection)
		return;

	CNetMessage Msg(NETMSG("svJoinRoom"), true);
	Msg.WriteBE<int32_t>(RoomID);
	m_pConnection->SendMessage(Msg);
}

//
//
void CNetworkRoomClient::JoinRoom(const int8 *pName)
{
	if(!m_pConnection)
		return;

	for(int32_t i = 0; i < m_lRooms.Len(); i++)
		if(m_lRooms.GetValue(i).m_Name == pName)
		{
			JoinRoom(m_lRooms.GetValue(i).m_ID);
			return;
		}
}

//
//
void CNetworkRoomClient::LeaveRoom()
{
	CNetMessage Msg(NETMSG("svLeaveRoom"), true);
	m_pConnection->SendMessage(Msg);
}

//
//
void CNetworkRoomClient::CreateRoom(const int8 *pName, const int8 *pPassword, int32_t Max)
{
	CNetMessage Msg(NETMSG("svCreateRoom"), true);
	Msg.Write_p8(pName); // name
	Msg.Write_p8(pPassword); // password
	Msg.WriteBE<uint8>(Max);
	m_pConnection->SendMessage(Msg);
}
