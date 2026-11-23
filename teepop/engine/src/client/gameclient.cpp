#include <foo/base/log.h>
#include <foo/base/math.h>

#include "gameclient.h"

MACRO_REGISTER_CLASS(CNetworkGameClient, "EngineModule")

void CClientBoard::ResetTimers()
{
	m_LastUpdate = GetTickCount();
	m_IntertickTime = 0;
	m_TickLength = GetTickFrequency();
}

void CClientBoard::Refresh()
{
	m_Stable = false;
	m_Updated = false;
	m_Punished = false;
	if(GetTickCount()-m_LastUpdate > m_TickLength)
	{
		int32 s = m_IttrState;
		if(Update()) // is stable?
		{
			/*
			fp32 Coverage = m_LocalBoard.Coverage();
			if(Coverage > 0.6f && m_LastCoverage <= 0.6f)
				Signal(EEvent_GoneHigh);
			else if(Coverage < 0.55f && m_LastCoverage >= 0.55f)
				Signal(EEvent_GoneLow);
			m_LastCoverage = Coverage;
			*/


			m_Stable = true;
			//m_BoardState = EState_Waiting;
		}

		if(s != m_IttrState && m_NumStones > 0)
		{
			//CMessage Msg(EEvent_Punish);
			//Msg.SetInt("numstones", m_LocalBoard.m_NumStones);
			//Signal(Msg);
			m_Punished = true;
		}

		if(m_NumZaps)
			m_TickLength = GetTickFrequency()*0.35f;
		else
			m_TickLength = GetTickFrequency()/30;

		m_Updated = true;
		//m_BoardUpdated = true;
		m_LastUpdate = GetTickCount();
		m_IntertickTime = 0;
	}
	else
	{
		m_IntertickTime = (GetTickCount()-m_LastUpdate)/(fp32)m_TickLength;
		if(m_IntertickTime > 1)
			m_IntertickTime = 1;
	}
}

//
//
void CNetworkGameClient::DeleteAllBoards()
{
	for(int32 i = 0; i < m_mOpponentBoards.Len(); i++)
		delete m_mOpponentBoards.GetValue(i);
	m_mOpponentBoards.Clear();
}

//
//
bool CNetworkGameClient::Init()
{
	m_pCore->RegisterCommand("start", this);
	m_pCore->RegisterCommand("pause", this);
	m_pCore->RegisterCommand("stop", this);
	
	m_pConnection = NULL;
	return CModule::Init();
}

//
//
void CNetworkGameClient::OnAttach(CNetworkConnection *pConnection)
{
	m_pConnection = pConnection;
	m_pConnection->RegisterNetMessage(NETMSG("clNewBlocks"), this);
	m_pConnection->RegisterNetMessage(NETMSG("clBoardUpdate"), this);
	m_pConnection->RegisterNetMessage(NETMSG("clGameStateChange"), this);
	m_pConnection->RegisterNetMessage(NETMSG("clStonesSent"), this);
	m_pConnection->RegisterNetMessage(NETMSG("clEvadedStones"), this);
	m_pConnection->RegisterNetMessage(NETMSG("clLevelIncrease"), this);
	

	m_XPos = 2;
	m_YPos = 0;
	m_Rotation = 0;

	//m_TickLength = GetTickFrequency()/30;
	m_GameState = EGameState_Waiting;
	m_LocalBoard.m_SetNewBlocks = false;
	m_QueuedBoard.m_SetNewBlocks = false;
	m_LocalBoard.Reset();
	m_LocalBoard.ResetTimers();
	m_BoardState = EState_Waiting;
	m_BoardQueued = false;
	DeleteAllBoards();
}

//
//
void CNetworkGameClient::OnDetach(CNetworkConnection *pConnection)
{
	if(m_GameState == EGameState_Playing)
		Signal(EEvent_GameEnd);

	m_pConnection = NULL;
	DeleteAllBoards();
}

//
//
void CNetworkGameClient::OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream)
{
	if(!m_pConnection || !pConnection)
		return;

	if(iMsg == NETMSG("clNewBlocks"))
	{
		if(m_BoardState == EState_Placing)
			NLog::WriteF("!!!!! Got new blocks under placement face\n");

		m_QueuedBoard.Unpack(rStream);
		uint8 NewBlock = rStream.ReadBE<uint8>();
		uint8 NewNextBlock = rStream.ReadBE<uint8>();
		m_QueuedBoard.m_aCurrentBlocks[0] = NewBlock&0xf;
		m_QueuedBoard.m_aCurrentBlocks[1] = NewBlock>>4;
		m_QueuedBoard.m_aNextBlocks[0] = NewNextBlock&0xf;
		m_QueuedBoard.m_aNextBlocks[1] = NewNextBlock>>4;
		m_BoardQueued = true;
		NLog::WriteF("GameClient: New blocks\n");
	}
	else if(iMsg == NETMSG("clLevelIncrease"))
	{
		m_Level = rStream.ReadBE<uint8>();
	
		CMessage Msg(EEvent_LevelIncrease);
		Msg.SetInt("level", m_Level);
		Signal(Msg);
	}
	else if(iMsg == NETMSG("clBoardUpdate"))
	{
		int32 iClient = rStream.ReadBE<int32>();
		CClientBoard *pBoard = m_mOpponentBoards.Find(iClient, NULL);
		if(!pBoard)
		{
			pBoard = new CClientBoard;
			m_mOpponentBoards[iClient] = pBoard;
		}
		pBoard->Unpack(rStream);
		pBoard->m_IttrState = CBoard::EIttrState1;
	}
	else if(iMsg == NETMSG("clStonesSent"))
	{
		CMessage Msg(EEvent_SentStones);
		Msg.SetInt("clientid", rStream.ReadBE<int32>());
		Msg.SetInt("numstones", rStream.ReadBE<uint8>());
		Signal(Msg);
	}
	else if(iMsg == NETMSG("clEvadedStones"))
	{
		CMessage Msg(EEvent_EvadedStones);
		Msg.SetInt("clientid", rStream.ReadBE<int32>());
		Msg.SetInt("numstones", rStream.ReadBE<uint8>());
		Signal(Msg);
	}
	else if(iMsg == NETMSG("clGameStateChange"))
	{
		int32 State = rStream.ReadBE<uint8>();

		if(State == 0) // wait
		{
			m_GameState = EGameState_Waiting;
			m_BoardState = EState_Itterating;
		}
		else if(State == 1) // start
		{
			m_LocalBoard.Reset();
			DeleteAllBoards();
			m_LastCoverage = 0;
			m_Level = 0;
			m_BoardState = EState_Waiting;
			m_GameState = EGameState_Playing;
			Signal(EEvent_GameStart);
		}
		else if(State == 2) // win
		{
			if(m_BoardState == EState_Placing)
				m_BoardState = EState_Waiting;
			m_GameState = EGameState_Win;
			Signal(EEvent_GameEnd);
		}
		else if(State == 3) // lose
		{
			m_BoardState = EState_Itterating;
			m_GameState = EGameState_Lose;
			Signal(EEvent_GameEnd);
		}
	}
}

//
//
void CNetworkGameClient::OnCommand(CString &rCmd, CString &rArgs)
{
	if(rCmd == "start")
		StartGame();
}

//
//
//
void CNetworkGameClient::Update()
{
	//if(m_GameState != EGameState_Playing)
	//	return;


	// update opponent boards
	for(int32 i = 0; i < m_mOpponentBoards.Len(); i++)
		m_mOpponentBoards.GetValue(i)->Refresh();

	if(m_BoardState == EState_Waiting)
	{
		if(m_BoardQueued)
		{
			m_LocalBoard = m_QueuedBoard;
			m_LocalBoard.ResetTimers();
			m_BoardQueued = false;
			m_BoardState = EState_Placing;

			m_XPos = 2;
			m_YPos = 0;
			m_Rotation = 0;
			//m_IntertickTime = 0;
			//m_TickLength = GetTickFrequency()/30;
		}
	}
	else if(m_BoardState == EState_Placing)
	{
		if(m_GameState != EGameState_Playing)
			m_BoardState = EState_Waiting;

		m_LocalBoard.m_TickLength = GetTickFrequency()/pow(1.3f+m_Level*0.25f, 1.6f);
		if(GetTickCount()-m_LocalBoard.m_LastUpdate > m_LocalBoard.m_TickLength)
		{
			if(m_LocalBoard.ValidatePos(m_XPos, m_YPos+1, m_Rotation))
				m_YPos++;
			else
				CtlDropBlocks();
			m_LocalBoard.m_LastUpdate = GetTickCount();
		}

		m_LocalBoard.m_IntertickTime = (GetTickCount()-m_LocalBoard.m_LastUpdate)/(fp32)m_LocalBoard.m_TickLength;
		if(m_LocalBoard.m_IntertickTime > 1)
			m_LocalBoard.m_IntertickTime = 1;
		m_LocalBoard.m_Updated = false;
	}
	else if(m_BoardState == EState_Itterating)
	{
		m_LocalBoard.Refresh();
		if(m_LocalBoard.m_Stable)
		{
			fp32 Coverage = m_LocalBoard.Coverage();
			if(Coverage > 0.6f && m_LastCoverage <= 0.6f)
				Signal(EEvent_GoneHigh);
			else if(Coverage < 0.55f && m_LastCoverage >= 0.55f)
				Signal(EEvent_GoneLow);
			m_LastCoverage = Coverage;

			m_BoardState = EState_Waiting;
		}
		else if(m_LocalBoard.m_Updated && m_LocalBoard.m_Punished)
		{
			CMessage Msg(EEvent_Punish);
			Msg.SetInt("numstones", m_LocalBoard.m_NumStones);
			Signal(Msg);
		}

	}
}

//
//
void CNetworkGameClient::SendDropBlocks()
{
	if(!m_pConnection || m_GameState != EGameState_Playing)
		return;
	CNetMessage Msg(NETMSG("svDropBlocks"), true);

	Msg.WriteBE<uint8>(m_XPos|(m_Rotation<<4));
	m_pConnection->SendMessage(Msg);
}

// 
//
void CNetworkGameClient::StartGame()
{
	if(!m_pConnection)
		return;
	CNetMessage Msg(NETMSG("svStartGame"), true);
	m_pConnection->SendMessage(Msg);
}

// controls
void CNetworkGameClient::CtlDropBlocks()
{
	if(m_BoardState != EState_Placing || m_GameState != EGameState_Playing)
		return;
	SendDropBlocks();
	if(!m_LocalBoard.Drop(m_XPos, m_YPos, m_Rotation))
		NLog::WriteF("!!!!! DropError\n");

	m_BoardState = EState_Itterating;
	m_LocalBoard.m_IntertickTime = 1;
	m_LocalBoard.m_TickLength = 0;
	
	// THIS CAN GO HORRIBLE WRONG!!! DAMN IT
	//for(int32 i = 0; i < m_YPos; i++)
	//	m_LocalBoard.Update();

	// fix fall flag
	//m_LocalBoard.ClearExtra();
}

//
void CNetworkGameClient::CtlMoveLeft()
{
	if(m_LocalBoard.ValidatePos(m_XPos-1, m_YPos, m_Rotation))
		m_XPos--;
	else
		Signal(EEvent_BadMove);
}

void CNetworkGameClient::CtlMoveRight()
{
	if(m_LocalBoard.ValidatePos(m_XPos+1, m_YPos, m_Rotation))
		m_XPos++;
	else
		Signal(EEvent_BadMove);
}

void CNetworkGameClient::CtlRotate(int32 RotDir)
{
	int32 newr = m_Rotation+RotDir;
	while(newr >= 4) newr-=4;
	while(newr < 0) newr+=4;

	if(m_LocalBoard.ValidatePos(m_XPos, m_YPos, newr))
	{
		m_Rotation = newr;
		Signal(EEvent_Rotate);
	}
	else if(m_LocalBoard.ValidatePos(m_XPos+1, m_YPos, newr))
	{
		m_XPos++;
		m_Rotation = newr;
		Signal(EEvent_Rotate);
	}
	else if(m_LocalBoard.ValidatePos(m_XPos-1, m_YPos, newr))
	{
		m_XPos--;
		m_Rotation = newr;
		Signal(EEvent_Rotate);
	}
	else
		Signal(EEvent_BadMove);


}

void CNetworkGameClient::CtlRotateCW() { CtlRotate(1); }
void CNetworkGameClient::CtlRotateCCW() { CtlRotate(-1); }

bool CNetworkGameClient::IsActive()
{
	return m_pConnection != NULL;
}
