
#include "roomserver.h"

MACRO_REGISTER_CLASS(CNetworkRoomServer, "EngineModule")

void CRoom::PackAll(COutputStream &rStream)
{
	rStream.WriteBE<int32_t>(m_ID); // id
	rStream.Write_p8(m_Name); // name
	//rStream.WriteBE<uint8_t>(0); // password protected (should be baked into state)
	rStream.WriteBE<uint8_t>(m_MaxPlayers); // max players
	PackPartial(rStream, false);
}

void CRoom::PackPartial(COutputStream &rStream, bool DoId)
{
	if(DoId)
		rStream.WriteBE<int32_t>(m_ID); // id
	rStream.WriteBE<uint8_t>(m_State|(m_lPlayers.Len()?EState_PasswordProtected:0)); // state
	rStream.WriteBE<uint8_t>(m_lPlayers.Len()); // num players
	for(int32_t i = 0; i < m_lPlayers.Len(); i++) // player ids
		rStream.WriteBE<int32_t>(m_lPlayers[i]->ClientID());
}

//
//
bool CNetworkRoomServer::Init()
{
	m_LastUpdate = GetTickCount();
	m_CurrentRoomID = 100;
	return CModule::Init();
}

//
//
void CNetworkRoomServer::SendCompleateUpdate(CNetworkConnection *pConnection)
{
	CNetMessage Msg(NETMSG("clRoomUpdate"), true);

	Msg.WriteBE<uint8_t>(0); // deletions
	Msg.WriteBE<uint8_t>(m_lRooms.Len()); // creations
	for(int32_t i = 0; i < m_lRooms.Len(); i++)
		m_lRooms.GetValue(i)->PackAll(Msg);
	Msg.WriteBE<uint8_t>(0); // updates
	pConnection->SendMessage(Msg);
}


//
//
void CNetworkRoomServer::SendGlobalRoomUpdate()
{
	if(!m_lRoomDeletions.Len() && !m_lRoomCreations.Len() && !m_lRoomUpdates.Len())
		return; // nothing to do

	CNetMessage Msg(NETMSG("clRoomUpdate"), true);

	// add deletions
	Msg.WriteBE<uint8_t>(m_lRoomDeletions.Len());
	for(int32_t i = 0; i < m_lRoomDeletions.Len(); i++)
	{
		Msg.WriteBE<int32_t>(m_lRoomDeletions[i]);
		m_lRoomCreations.RemoveFast(m_lRoomDeletions[i]);
		m_lRoomUpdates.RemoveFast(m_lRoomDeletions[i]);
	}

	// add creations
	Msg.WriteBE<uint8_t>(m_lRoomCreations.Len());
	for(int32_t i = 0; i < m_lRoomCreations.Len(); i++)
	{
		CRoom *pRoom = m_lRooms.Find(m_lRoomCreations[i], NULL);
		if(pRoom)
			pRoom->PackAll(Msg);
		m_lRoomUpdates.RemoveFast(m_lRoomCreations[i]);
	}

	// add updates
	Msg.WriteBE<uint8_t>(m_lRoomUpdates.Len());
	for(int32_t i = 0; i < m_lRoomUpdates.Len(); i++)
	{
		CRoom *pRoom = m_lRooms.Find(m_lRoomUpdates[i], NULL);
		if(pRoom)
			pRoom->PackPartial(Msg);
	}

	// clear lists
	m_lRoomDeletions.Clear();
	m_lRoomCreations.Clear();
	m_lRoomUpdates.Clear();

	// Send it to all players
	for(int32_t i = 0; i < m_lPlayers.Len(); i++)
		m_lPlayers[i]->SendMessage(Msg);
}

//
//
void CNetworkRoomServer::Update()
{
	if(GetTickCount()-m_LastUpdate > GetTickFrequency())
	{
		SendGlobalRoomUpdate();
		m_LastUpdate = GetTickCount();
	}

	CModule::Update();
}

//
//
void CNetworkRoomServer::JoinRoom(CNetworkConnection *pConnection, CRoom *pRoom)
{
	if(!pConnection || !pRoom)
		return;

	if(pRoom == GetRoom(pConnection))
		return;

	if(pRoom->m_lPlayers.Len() >= pRoom->m_MaxPlayers)
		return;

	LeaveRoom(pConnection, false);
	pRoom->m_lPlayers.Add(pConnection);
	pConnection->SetData(this, pRoom);
	m_lRoomUpdates.Add(pRoom->m_ID);
	
	// Send network notification
	{
		CNetMessage Msg(NETMSG("clRoomChanged"), true);
		Msg.WriteBE<int32_t>(pRoom->m_ID);
		pConnection->SendMessage(Msg);
	}

	// send local message
	{
		CMessage Msg(EEvent_Joined);
		Msg.SetString("roomname", pRoom->m_Name);
		Msg.SetInt("roomid", pRoom->m_ID);
		Msg.SetInt("client", pConnection->ClientID());
		Signal(Msg);
	}
}

//
//
const char *CNetworkRoomServer::CreateRoom(CNetworkConnection *pConnection, const char *pName, const char *pPassword, int32_t MaxPlayers)
{
	if(!pConnection)
		return "You don't exist!";

	if(StringLength(pName) <= 0)
		return "Name to short";

	// duplication check
	for(int32_t i = 0; i < m_lRooms.Len(); i++)
		if(m_lRooms.GetValue(i)->m_Name == pName)
			return "Room with the same name already exists";

	// create new room and set params
	CRoom *pRoom = new CRoom;
	pRoom->m_ID = m_CurrentRoomID++;
	pRoom->m_SinglePlayer = 0;
	pRoom->m_Name = pName;
	pRoom->m_Password = pPassword;
	pRoom->m_MaxPlayers = MaxPlayers;
	pRoom->m_State = CRoom::EState_Stopped;
	pRoom->m_SinglePlayer = 0;

	m_lRooms[pRoom->m_ID] = pRoom;
	m_lRoomCreations.Add(pRoom->m_ID);

	// join the room
	JoinRoom(pConnection, pRoom);
	return NULL;
}

//
//
void CNetworkRoomServer::LeaveRoom(CNetworkConnection *pConnection, bool SendNetMessage)
{
	if(!pConnection)
		return;

	CRoom *pCurrentRoom = GetRoom(pConnection);
	if(!pCurrentRoom)
		return;

	pConnection->SetData(this, NULL);
	pCurrentRoom->m_lPlayers.Remove(pConnection);
	
	// Send network notification
	if(SendNetMessage)
	{
		CNetMessage Msg(NETMSG("clRoomChanged"), true);
		Msg.WriteBE<int32_t>(-1);
		pConnection->SendMessage(Msg);
	}

	if(!pCurrentRoom->m_lPlayers.Len())
	{
		// no more players, destroy room
		m_lRoomDeletions.Add(pCurrentRoom->m_ID);
		m_lRooms.Remove(pCurrentRoom->m_ID);
		delete pCurrentRoom;
	}
	else
		m_lRoomUpdates.Add(pCurrentRoom->m_ID);

	// send message
	CMessage Msg(EEvent_Left);
	Msg.SetInt("client", pConnection->ClientID());
	Signal(Msg);
}


//
//
void CNetworkRoomServer::OnAttach(CNetworkConnection *pConnection)
{
	pConnection->RegisterNetMessage(NETMSG("svCreateRoom"), this);
	pConnection->RegisterNetMessage(NETMSG("svJoinRoom"), this);
	pConnection->RegisterNetMessage(NETMSG("svLeaveRoom"), this);
	m_lPlayers.Add(pConnection);
	SendCompleateUpdate(pConnection);
}

//
//
void CNetworkRoomServer::OnDetach(CNetworkConnection *pConnection)
{
	LeaveRoom(pConnection);
	m_lPlayers.Remove(pConnection);
}

//
//
void CNetworkRoomServer::OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream)
{
	if(!pConnection)
		return;

	CRoom *pCurrentRoom = GetRoom(pConnection);

	if(iMsg == NETMSG("svCreateRoom"))
	{
		CString name = rStream.Read_p8();
		CString password = rStream.Read_p8();
		int32_t max = Clamp(rStream.ReadBE<uint8_t>(), 1, 4);
		const char *pError = CreateRoom(pConnection, name, password, max);
		if(pError)
		{
			CNetMessage Msg(NETMSG("clCreateRoomError"), true);
			Msg.Write_p8(pError);
			pConnection->SendMessage(Msg);
		}
	}
	else if(iMsg == NETMSG("svJoinRoom"))
	{
		int32_t id = rStream.ReadBE<int32_t>();
		JoinRoom(pConnection, m_lRooms.Find(id, NULL));
	}
	else if(iMsg == NETMSG("svLeaveRoom"))
	{
		LeaveRoom(pConnection);
	}
}

//
//
void CNetworkRoomServer::OnCommand(CString &rCmd, CString &rArgs)
{
}

//
//
void CNetworkRoomServer::SetRoomState(int32_t iRoom, int32_t State)
{
	CRoom *pRoom = GetRoom(iRoom);
	if(pRoom)
	{
		pRoom->m_State = State;
		m_lRoomUpdates.Add(pRoom->m_ID);
	}

}

//
//
void CNetworkRoomServer::SetRoomState(CNetworkConnection *pConnection, int32_t State)
{
	CRoom *pRoom = GetRoom(pConnection);
	if(pRoom)
		SetRoomState(pRoom->m_ID, State);
}


