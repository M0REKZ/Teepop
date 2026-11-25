#include <foo/base/memory.h>
#include <foo/func/filestream.h>
#include <foo/func/math.h>
#include <foo/core/core.h>
#include <foo/base/log.h>

#include "../engine/engine.h"
#include "../engine/networkserver.h"
#include "../engine/networkchatserver.h"
#include "../engine/networkauthserver.h"

#include "gameserver.h"
#include "roomserver.h"

using namespace NFoo;
using namespace NEngine;

#include "../shared/board.h"
#include "../shared/version.h"

//#define Require(b) if(!(b)) return 
//#define RequireRet(b,r) if(!(b)) return r

//
//
//
class CPuyoServer // : public CNetworkMsgRecviver
{
public:
	CCore m_Core;

	// modules
	CNetworkServer *m_pServer;
	CNetworkAuthServer *m_pAuth;
	CNetworkChatServer *m_pChat;
	CNetworkRoomServer *m_pRoom;
	CNetworkGameServer *m_pGame;

	//
	//
	//
	class CPlayer : public CNetworkConnection
	{
	public:
		CPuyoServer *m_pPuyoServer;

		//
		//
		//
		CPlayer(CPuyoServer *pPuyoServer)
		: m_pPuyoServer(pPuyoServer)
		{
		}

		//
		//
		//
		CPlayer()
		{
			m_iClient = 0;
		}

		//
		//
		//
		~CPlayer()
		{
		}

		virtual void OnConnect()
		{
			AttachRecviver(m_pPuyoServer->m_pAuth);
		}

		virtual void OnDisconnect()
		{
		}
	};

	static CNetworkConnection *ClientCreator(void *p) { return new CPlayer((CPuyoServer*)p); }

	// events
	enum
	{
		EAuthEvent_Success,
		EEvent_JoinedRoom,
		EEvent_LeftRoom,
		ENumEvents
	};

	CMessageQueue m_MsgQueue;

	//
	//
	//
	void Init()
	{
		// load modules
		Assert(m_Core.LoadModule(m_pServer, "NetworkServer"));
		Assert(m_Core.LoadModule(m_pAuth, "NetworkAuthServer"));
		Assert(m_Core.LoadModule(m_pChat, "NetworkChatServer"));
		Assert(m_Core.LoadModule(m_pRoom, "NetworkRoomServer"));
		Assert(m_Core.LoadModule(m_pGame, "NetworkGameServer"));
	
		//
		m_pServer->SetVersion(TEEPOP_NETWORK_VERSION);

		// bind events
		m_pAuth->Bind(CNetworkAuthServer::EEvent_AuthSuccess, &m_MsgQueue, EAuthEvent_Success);
		m_pRoom->Bind(CNetworkRoomServer::EEvent_Joined, &m_MsgQueue, EEvent_JoinedRoom);
		m_pRoom->Bind(CNetworkRoomServer::EEvent_Left, &m_MsgQueue, EEvent_LeftRoom);

		// load users
		CFileReader CfgFile("users.cfg");
		if(CfgFile.IsReady())
		{
			while(!CfgFile.EndOfStream())
			{
				CString Line = CfgFile.ReadLine();
				CString Name = Line.GetStrSep(" ");
				CString Password = Line.GetStrSep(" ");
				int32_t Level = Line.GetStrSep(" ").GetInt();
				if(Name.Len() && Password.Len())
					m_pAuth->NewUser(Name, Password, Level, false);
			}
		}

		// init and start server
		m_pServer->SetClientCreator(ClientCreator, this);
		m_pServer->Start(8303);
	}

	//
	//
	//
	void Update()
	{
		m_Core.Update();

		while(m_MsgQueue.GotMessage())
		{
			CMessage &rMsg = m_MsgQueue.GetMessage();
			switch(rMsg.m_What)
			{
			case EAuthEvent_Success:
				{
					int32_t iClient = rMsg.GetInt("client", -1);
					if(iClient < 0)
						break;
					
					CPlayer *pPlayer = (CPlayer*)m_pServer->GetClient(iClient);
					if(!pPlayer)
						break;

					CString Name = rMsg.GetString("username", "");
					int32_t Guest = rMsg.GetInt("guest", 0);
					int32_t Level = rMsg.GetInt("level", 0);

					pPlayer->AttachRecviver(m_pChat);
					pPlayer->AttachRecviver(m_pRoom);

					if(Guest)
						m_pChat->SetUserName(pPlayer, CString("%s (guest)", Name.GetData()));
					else if(Level >= 100)
						m_pChat->SetUserName(pPlayer, CString("%s (admin)", Name.GetData()));
					else
						m_pChat->SetUserName(pPlayer, Name);

					m_pChat->SetChannel(pPlayer, "");
				} break;
			case EEvent_LeftRoom:
			case EEvent_JoinedRoom:
				{
					int32_t iClient = rMsg.GetInt("client", -1);
					if(iClient < 0)
						break;
					
					CPlayer *pPlayer = (CPlayer*)m_pServer->GetClient(iClient);
					if(!pPlayer)
						break;

					/*
					int32_t roomid = -1;
					CRoom *pRoom = m_pRoom->GetRoom(pPlayer);

					if(pRoom)
						roomid = pRoom->m_ID;*/

					if(rMsg.m_What == EEvent_LeftRoom)
					{
						pPlayer->DetachRecviver(m_pGame);
						m_pChat->SetChannel(pPlayer, "");
					}
					else
					{
						pPlayer->AttachRecviver(m_pGame);
						m_pChat->SetChannel(pPlayer, rMsg.GetString("roomname"));
					}
				} break;
			}

			m_MsgQueue.DispatchMessage();

		}
	}
};

//
//
//
class CBadConnectionSimulator
{
public:
	NNetwork::CSocket m_aSocket;
	
	~CBadConnectionSimulator()
	{
		m_lPackets.DeleteAll();
	}
	
	class CPacket
	{
	public:
		int32_t m_Size;
		char m_aData[1500];
		
		int64_t m_SendTime;
		NNetwork::CAddr m_Target;
	};
	
	TDynamicArray<CPacket*> m_lPackets;

	int32_t m_OtherPort;
	
	void Init()
	{
		m_aSocket.Create(NNetwork::CSocket::EType_UDP);
		m_aSocket.Bind(NNetwork::CAddr(8303));
		m_aSocket.SetBlocking(false);
	}
	
	void Update()
	{
		// send packets
		for(int32_t i = 0; i < m_lPackets.Len(); i++)
		{
			if(m_lPackets[i]->m_SendTime < GetTickCount())
			{
				// get packet
				CPacket *pPacket = m_lPackets[i];
				m_lPackets.RemoveByIndexFast(i);
				i--;
				
				// send it
				m_aSocket.SendTo(pPacket->m_aData, pPacket->m_Size, pPacket->m_Target);
				
				// remove it
				delete pPacket;
			}
		}
	
		// Get new packets
		NNetwork::CAddr SrcAddr;
		NNetwork::CAddr DstAddr;
		char aData[1500];
		int32_t ReadBytes;
		
		if((ReadBytes = m_aSocket.RecvFrom(aData, 1500, SrcAddr)) > 0)
		{

			if(SrcAddr.m_Port == 8300)
			{
				DstAddr = SrcAddr;
				DstAddr.m_Port = m_OtherPort;
			}
			else
			{
				m_OtherPort = SrcAddr.m_Port;
				DstAddr = SrcAddr;
				DstAddr.m_Port = 8300;
			}
			
			CPacket *pPacket = new CPacket;
			m_lPackets.Add(pPacket);
			MemoryCopy(pPacket->m_aData, aData, 1500);
			pPacket->m_Target = DstAddr;
			pPacket->m_Size = ReadBytes;
			int32_t Ping = 250+(int32_t)(Random()*100); // 150 - 400
			pPacket->m_SendTime = GetTickCount()+(int64_t)(GetTickFrequency()*(Ping/1000.0f)); // 250ms
		}
	}
}; 

//
//
class CStdOutLogger : public NLog::CLogger
{
public:
	CStdOutLogger() { NLog::Attach(this); NLog::Write("CStdOutLogger attached\n"); }
	virtual ~CStdOutLogger() { NLog::Write("CStdOutLogger deattching\n"); NLog::Detach(this); }
	virtual void Write(const char *pBuffer)
	{
		g_Stdout.Write(pBuffer, StringLength(pBuffer));
	}
};

//
//
int32_t FooMain(char *cmdline)
{
	CStdOutLogger stdlogger;
	//CBadConnectionSimulator Simulator;
	//Simulator.Init();

	CPuyoServer Server;
	Server.Init();

	while(NCore::UpdateAndPump())
	{
		Server.Update();
		//Simulator.Update();
		Sleep(1);
	}

	return 0;
}

