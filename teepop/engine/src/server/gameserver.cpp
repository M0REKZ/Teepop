#include <foo/base/log.h>

#include "gameserver.h"
#include "roomserver.h"

MACRO_REGISTER_CLASS(CNetworkGameServer, "EngineModule")

//
//
void CNetworkGameServer::SendNewBlocks(CNetworkConnection *pConnection)
{
	if(!pConnection)
		return;
	CInfo *pInfo = GetInfo(pConnection);
	if(!pInfo)
		return;

	CNetMessage Msg(NETMSG("clNewBlocks"), true); // 36+1 bytes
	pInfo->m_Board.Pack(Msg);
	Msg.WriteBE<uint8_t>(pInfo->m_Board.m_aCurrentBlocks[0]|(pInfo->m_Board.m_aCurrentBlocks[1]<<4));
	Msg.WriteBE<uint8_t>(pInfo->m_Board.m_aNextBlocks[0]|(pInfo->m_Board.m_aNextBlocks[1]<<4));
	pConnection->SendMessage(Msg);
}

//
//
void CNetworkGameServer::SendGameStateChange(CNetworkConnection *pConnection, int32_t State)
{
	CNetMessage NetMsg(NETMSG("clGameStateChange"), true);
	NetMsg.WriteBE<uint8_t>(State);
	pConnection->SendMessage(NetMsg);
}

//
//
bool CNetworkGameServer::Init()
{
	m_pRoom = (CNetworkRoomServer *)m_pCore->GetModule("NetworkRoomServer");
	Assert(m_pRoom);
	return CModule::Init();
}

//
//
void CNetworkGameServer::OnAttach(CNetworkConnection *pConnection)
{
	pConnection->RegisterNetMessage(NETMSG("svDropBlocks"), this);
	pConnection->RegisterNetMessage(NETMSG("svStartGame"), this);
	pConnection->RegisterNetMessage(NETMSG("svStopGame"), this);
	CInfo *pInfo = new CInfo;
	pInfo->m_State = EWaiting;
	CRoom *pRoom = m_pRoom->GetRoom(pConnection);
	if(pRoom)
		pInfo->m_iRoom = pRoom->m_ID;
	pConnection->SetData(this, pInfo);
}

//
//
void CNetworkGameServer::OnDetach(CNetworkConnection *pConnection)
{
	if(!pConnection)
		return;
	CInfo *pInfo = GetInfo(pConnection);
	if(!pInfo)
		return;

	//CheckWinConditions(pInfo->m_iRoom, pConnection->ClientID());
	
	delete pInfo;
	pConnection->SetData(this, NULL);
}

//
//
void CNetworkGameServer::CheckWinConditions(int32_t roomid, int32_t iClientIgnore)
{
	CRoom *pRoom = m_pRoom->GetRoom(roomid);
	if(!pRoom)
		return;

	int32_t NumSpectators = 0;
	int32_t NumPlaying = 0;
	int32_t TotalClients = 0;

	for(int32_t i = 0; i < pRoom->m_lPlayers.Len(); i++)
	{
		if(pRoom->m_lPlayers[i]->ClientID() == iClientIgnore)
			continue;
		TotalClients++;

		CInfo *pInfo = GetInfo(pRoom->m_lPlayers[i]);

		if(pInfo)
		{
			if(pInfo->m_State == EPlaying)
				NumPlaying++;
			else
				NumSpectators++;
		}
	}

	if(pRoom->m_State == CRoom::EState_Playing)
	{
		if(NumPlaying == 0)
			m_pRoom->SetRoomState(pRoom->m_ID, CRoom::EState_Stopped);
		else if(NumPlaying == 1 && !pRoom->m_SinglePlayer)
		{
			// we got a winner
			m_pRoom->SetRoomState(pRoom->m_ID, CRoom::EState_Stopped);

			for(int32_t i = 0; i < pRoom->m_lPlayers.Len(); i++)
			{
				if(pRoom->m_lPlayers[i]->ClientID() == iClientIgnore)
					continue;

				CInfo *pInfo = GetInfo(pRoom->m_lPlayers[i]);
				if(pInfo && pInfo->m_State == EPlaying)
				{
					pInfo->m_State = EWon;
					SendGameStateChange(pRoom->m_lPlayers[i], 2);
				}
			}
		}
	}
}

//
//
void CNetworkGameServer::OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream)
{
	if(!pConnection)
		return;

	CInfo *pInfo = GetInfo(pConnection);
	CRoom *pRoom = m_pRoom->GetRoom(pConnection);
	if(!pRoom)
		return;

	if(iMsg == NETMSG("svStartGame"))
	{
		if(pRoom->m_State != CRoom::EState_Stopped)
			return;

		// START GAME
		m_pRoom->SetRoomState(pConnection, CRoom::EState_Playing);
		int32_t NumPlayers = 0;
		for(int32_t i = 0; i < pRoom->m_lPlayers.Len(); i++)
		{
			CInfo *pPlayerInfo = GetInfo(pRoom->m_lPlayers[i]);
			if(pPlayerInfo)// && pPlayerInfo->m_State == EWaiting)
			{
				pPlayerInfo->m_Board.Reset();
				pPlayerInfo->m_State = EPlaying;
				pPlayerInfo->m_Level = 0;
				pPlayerInfo->m_StartTime = GetTickCount();
				NumPlayers++;
				SendGameStateChange(pRoom->m_lPlayers[i], 1); // game start
				SendNewBlocks(pRoom->m_lPlayers[i]);
			}
		}

		if(NumPlayers == 1)
			pRoom->m_SinglePlayer = 1;
		else
			pRoom->m_SinglePlayer = 0;
	}
	else if(iMsg == NETMSG("svStopGame"))
	{
		if(pRoom->m_State != CRoom::EState_Playing)
			return;

		m_pRoom->SetRoomState(pConnection, CRoom::EState_Stopped);

		for(int32_t i = 0; i < pRoom->m_lPlayers.Len(); i++)
		{
			CInfo *pPlayerInfo = GetInfo(pRoom->m_lPlayers[i]);
			if(pPlayerInfo && pPlayerInfo->m_State >= EPlaying)
				pPlayerInfo->m_State = EWaiting;
		}
	}
	else if(iMsg == NETMSG("svDropBlocks"))
	{
		if(pInfo->m_State != EPlaying || pRoom->m_State != CRoom::EState_Playing)
			return;

		// unpack message
		int32_t b = rStream.ReadBE<uint8_t>();

		CBoard &rBoard = pInfo->m_Board;
		rBoard.Drop(b&0xf, b>>4); // drop blocks

		// send board updates
		CNetMessage Msg(NETMSG("clBoardUpdate"), false);
		Msg.WriteBE<int32_t>(pConnection->ClientID());
		pInfo->m_Board.Pack(Msg);
		for(int32_t i = 0; i < pRoom->m_lPlayers.Len(); i++)
		{
			CNetworkConnection *pOpponent = pRoom->m_lPlayers[i];
			if(pOpponent == pConnection)
				continue;
			pOpponent->SendMessage(Msg);
		}

		// update until stable
		bool Stable = false;
		while(!Stable && !rBoard.m_Lost)
			Stable = rBoard.Update();

		//
		if(rBoard.m_Lost || rBoard.m_aField[2][0] != 0 || rBoard.m_aField[3][0] != 0) // lost game
			pInfo->m_State = ELost;

		CheckWinConditions(pRoom->m_ID);

		// transfer queued stones
		rBoard.m_NumStones += rBoard.m_QueuedStones;
		rBoard.m_QueuedStones = 0;
		
		// in single player you will get all the blocks
		if(pRoom->m_SinglePlayer && rBoard.m_SendStones > 0)
			rBoard.m_NumStones += rBoard.m_SendStones;

		// send new board and new blocks
		//if(pInfo->m_State == EPlaying)
		SendNewBlocks(pConnection);

		//
		if(pInfo->m_State == ELost)
			SendGameStateChange(pConnection, 3);

		// do level increase
		if(pInfo->m_State == EPlaying)
		{
			int32_t Level = (GetTickCount()-pInfo->m_StartTime)/GetTickFrequency();
			Level /= 60; // level increase every 60 seconds
			if(Level != pInfo->m_Level)
			{
				pInfo->m_Level = Level;
				CNetMessage Msg(NETMSG("clLevelIncrease"), true);
				Msg.WriteBE<char>(pInfo->m_Level);
				pConnection->SendMessage(Msg);
			}
		}


		// evade
		if(rBoard.m_EvadedStones > 0)
		{
			CNetMessage Msg(NETMSG("clEvadedStones"), true);
			Msg.WriteBE<int32_t>(pConnection->ClientID());
			Msg.WriteBE<uint8_t>(rBoard.m_EvadedStones);
			for(int32_t i = 0; i < pRoom->m_lPlayers.Len(); i++)
				pRoom->m_lPlayers[i]->SendMessage(Msg);
		}

		//
		if(rBoard.m_SendStones > 0)
		{
			CNetMessage Msg(NETMSG("clStonesSent"), true);
			Msg.WriteBE<int32_t>(pConnection->ClientID());
			Msg.WriteBE<uint8_t>(rBoard.m_SendStones);
			for(int32_t i = 0; i < pRoom->m_lPlayers.Len(); i++)
				pRoom->m_lPlayers[i]->SendMessage(Msg);
		}


		// send board updates
		for(int32_t i = 0; i < pRoom->m_lPlayers.Len(); i++)
		{
			CNetworkConnection *pOpponent = pRoom->m_lPlayers[i];
			if(pOpponent == pConnection)
				continue;

			CInfo *pOpponentInfo = GetInfo(pOpponent);
			if(rBoard.m_SendStones > 0 && pOpponentInfo)
				pOpponentInfo->m_Board.m_QueuedStones += rBoard.m_SendStones;
		}
	}
}

//
//
void CNetworkGameServer::OnCommand(CString &rCmd, CString &rArgs)
{
}

//
//
//
void CNetworkGameServer::Update()
{

}

