#include "../engine/network.h"
#include "../shared/board.h"

using namespace NEngine;

//
// Require: NetworkRoomServer
//
class CNetworkGameServer : public CNetworkMsgRecviver, public CConsoleClient, public CMessageSender, public CModule
{
protected:
	enum
	{
		EWaiting,
		EPlaying,
		ELost,
		EWon
	};

	class CInfo
	{
	public:
		int32_t m_iRoom;
		int32_t m_State;
		CBoard m_Board;
		int64 m_StartTime;
		int32_t m_Level;
	};

	class CNetworkRoomServer *m_pRoom;

	CInfo *GetInfo(CNetworkConnection *pConnection) { return pConnection->GetData<CInfo>(this); }

	//
	//
	void CheckWinConditions(int32_t roomid, int32_t iClientIgnore=-1);

	//
	//
	void SendNewBlocks(CNetworkConnection *pConnection);
	void SendGameStateChange(CNetworkConnection *pConnection, int32_t State);

	//
	//
	virtual bool Init();
	virtual void Update();

	//
	//
	virtual void OnAttach(CNetworkConnection *pConnection);
	virtual void OnDetach(CNetworkConnection *pConnection);
	virtual void OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream);

	//
	//
	virtual void OnCommand(CString &rCmd, CString &rArgs);
public:

	// hotswapping
	bool HotswapWrite(COutputStream &rStream);
	bool HotswapRead(CInputStream &rStream);
};

