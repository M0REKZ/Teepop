#include "../engine/network.h"
#include "../shared/board.h"

using namespace NEngine;

class CClientBoard : public CBoard
{
public:
	CClientBoard() { ResetTimers(); }
	
	int64 m_LastUpdate;
	int64 m_TickLength;
	fp32 m_IntertickTime;

	bool m_Stable;
	bool m_Punished;
	bool m_Updated;

	void ResetTimers();
	void Refresh();
};

//
//
class CNetworkGameClient : public CNetworkMsgRecviver, public CConsoleClient, public CMessageSender, public CModule
{
protected:
	CNetworkConnection *m_pConnection;

	void DeleteAllBoards();

	//
	virtual bool Init();
	virtual void Update();

	// network
	virtual void OnAttach(CNetworkConnection *pConnection);
	virtual void OnDetach(CNetworkConnection *pConnection);
	virtual void OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream);

	//
	virtual void OnCommand(CString &rCmd, CString &rArgs);

	//
	void SendDropBlocks();

public:

	//
	//
	enum
	{
		EState_Waiting, // waiting for new blocks
		EState_Placing, // placing new blocks
		EState_Itterating, // animating board

		EGameState_Waiting,
		EGameState_Playing,
		EGameState_Win,
		EGameState_Lose,

		EIttr_Gravity,
		EIttr_Zap,

		EEvent_BadMove,
		EEvent_Rotate,
		EEvent_Punish,
		EEvent_SentStones,
		EEvent_EvadedStones,
		EEvent_LevelIncrease,

		EEvent_GameStart,
		EEvent_GoneHigh,
		EEvent_GoneLow,
		EEvent_GameEnd
	};

	//
	int32 m_GameState;

	//
	fp32 m_LastCoverage;
	int32 m_BoardState;
	int32 m_Level;
	//int64 m_LastBoardUpdate;

	// positioning
	int32 m_Rotation;
	int32 m_XPos;
	int32 m_YPos;

	// local board
	//bool m_BoardUpdated;
	CClientBoard m_LocalBoard;

	// queued board
	CClientBoard m_QueuedBoard;
	bool m_BoardQueued;

	// opponents
	TMap<int32, CClientBoard*> m_mOpponentBoards;
	//int64 m_LastOpponentBoardUpdate;
	//fp32 m_OpponentIntertickTime;

	// 
	void StartGame();
	bool IsActive();

	// controls
	void CtlDropBlocks();
	void CtlMoveLeft();
	void CtlMoveRight();
	void CtlRotateCW();
	void CtlRotateCCW();
	void CtlRotate(int32 RotDir);

};
