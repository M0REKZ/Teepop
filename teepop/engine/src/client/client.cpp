#include <foo/base/memory.h>
#include <foo/func/filestream.h>
#include <foo/func/math.h>
#include <foo/func/registry.h>
#include <foo/ext/input/input.h>
#include <foo/ext/input/keyboard.h>
#include <foo/ext/input/mouse.h>
#include <foo/ext/3d/3d.h>
#include <foo/ext/3d/2dutils/2dutils.h>

#include <foo/ext/sound/device.h>
#include <foo/ext/sound/sources/sample.h>
#include <foo/ext/sound/sources/sinus.h>

#include <foo/ext/gui_new/gui.h>
#include <foo/ext/gui_new/textbox.h>
#include <foo/ext/gui_new/listbox.h>
#include <foo/ext/gui_new/button.h>
#include <foo/ext/gui_new/window.h>
//#include <foo/ext/gui_new/label.h>

#include "../engine/engine.h"
#include "../engine/network.h"
#include "../engine/networkclient.h"
#include "../engine/networkchatclient.h"
#include "../engine/networkauthclient.h"
#include "../engine/render.h"
#include "../engine/input.h"

#include "../shared/board.h"
#include "../shared/version.h"

#include "roomclient.h"
#include "gameclient.h"

using namespace NFoo;
using namespace NFoo::N3D;
using namespace NFoo::NSound;
using namespace NFoo::NGUI;
using namespace NEngine;

int32 g_ScreenWidth = 800;
int32 g_ScreenHeight = 600;
//int32 g_ScreenWidth = 800;
//int32 g_ScreenHeight = 600;


bool g_HighContrast = false;
bool g_Music = true;
bool g_Fullscreen = false;
int32 g_FrameLimit = 60;

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "mlp.h"

/*
static size_t Callback_Read(void *ptr, size_t size, size_t nmemb, CMSound_Codec_VORB *This)
{
}

static int Callback_Seek(CMSound_Codec_VORB *This, ogg_int64_t offset, int whence)
{
}

static int Callback_Close(CMSound_Codec_VORB *This)
{
}

static long Callback_Tell(CMSound_Codec_VORB *This)
{
}*/


/*
	TODO:
		BUG: You can maximaze when in fullscreen
		BUG: opponent itteration doesn't show falling blocks for some reason
		BUG: Broadcast displays message to many times

		FEAT: Show what players won/lost
		FEAT: Incomming stone count
		FEAT: Roomname in room
		FEAT: Spectate
		FEAT: Gametime clock
		FEAT: Level increase time
		FEAT: Eng game stats

		FEAT: No sound option

		FEAT: Keyrepeat ingame


		INTERNAL FEAT: Evade, Sent etc should be chat messages


		FEAT: Server statistics
		FEAT: Timestamp server log
		FEAT: MOTD
		FEAT: Music when a big clean is done

		FEAT: /me :)




		(FEAT: Find player?)


		FEAT: Lower FPS when window isn't active

		FEAT: Options, FPS Limiter, Save Login, Scrolling BG, Buffer Size, High Contrast

		FEAT: Miniblobs

		FEAT: Special Combo: Clean Sweep

		BUG: Client can crash when server disconnects
		BUG: Client get stuck if server and client doesn't run the same version
		BUG: Warpwrap has problems with lines with no spaces

		FEAT: Hopswap server
		
		FEAT: How-To-Play help screens

		BUG: Remove selection from chat

		FEAT: irc client in server

		FEAT: New gui

		FEAT: Sort rooms
		FEAT: sort players
		FEAT: Stop button when playing in single player

		FEAT: team play

		FEAT: Scrollwheel
		FEAT: Alpha fade animations

		FEAT: Blinking eyes
		FEAT: Remote console

		FEAT: STMP/POP3 reg thingie
			FEAT: Scores
			FEAT: Ranking


		FEAT: Offline gameplay :)
		(FEAT: GDI render?)
		FEAT: CRC check
		FEAT: SafeReader
		FEAT: Bundle

	FIXED (xxx):
		FEAT: More, much more Nice, Great, Uber
		BUG: Flickering tees


	FIXED (v9):
		BUG: Autoscroll scrolls too far on long lines
		FEAT: F5 to F8 control music volume
		FEAT: Volume in mixing
		FEAT: Max players option
		FEAT: Quit button
		FEAT: Next blocks view
		FEAT: Increase a modifier over time to increase hardness
		FEAT: Fullscreen
		BUG: low FPS when running on a 16bit desktop
		FEAT: Auto select chat input while ingame
		FEAT: Change clUpdate command so we can display itteration process <<< GOOOD STUFF


	FIXED (v8.1):
		BUG: Bug that causes crash when typing
		FEAT: Crash log when stuff goes wrong

	FIXED (v8):
		FEAT: Music hi and low
		BALANCE: Fix priorities on stone placement
		FEAT: Broadcast message
		FEAT: Auto pushout when rotate
		

	FIXED (v7.2):
		FEAT: Audio Compression

	FIXED (v7.1):
		BUG: Huge memory leak in the new sound system
		BUG: some times the boarder and name for your field isn't shown

	FIXED (v7):
		BALANCE: Big is too good
		BUG: Client should animate last move so you see how you die
		BUG: You can resize the window
		BUG: Borders is incorrectly displayed when joining a room
		FEAT: High Contrast option
		FEAT: "Game starts" in the chat
		FEAT: DirectSound

	FIXED (Alpha5):
		FEAT: Who sent the blocks?
		FEAT: Fix BIIIG combo thingies
		FEAT: Special Combo: BIG!
		FEAT: Auto room name
		FEAT: Log chat messages on server
		FEAT: Only show fields that got players
		FEAT: Loading screen (NICE TOUCH!)
		BUG: Zapping can be done to early because (of the itteration?)
		BUG: Some error when placing blocks (PRESUMED FIXED)
		BUG: Room update seams to be broken (Room detached before game?, disconnected?)
		BUG: m_mOpponents doesn't get deleted
		BUG: Requires multitexturing.. remove
			
	FIXED (Alpha4b):
		BUG: Mouse pointer is behind
		BUG: No message when you couldn't create a room
		BUG: No message when server disconnects
		BUG: Client crash when quiten from a game
		BUG: Tighten up room start/stop code.
		BUG: When leaving a room that is in multiplayer mode and started should winning conditions be checked
		BUG: Can place anywhere even if position is blocked
		BUG: You can't start the room again when someone has won
		BUG: You can still manuouver when the game is over
		BUG: Tighten up end/win conditions
		BUG: Autoscroller seam to be not working
		FEAT: Mark around rotation block
		FEAT: More sound effects
		FEAT: Falling blobs
		FEAT: Animations
		FEAT: Version number in protocol
		FEAT: Word wrap i chatten
		BUG: Tighten up stone sending. The server must not send blocks at once. should be 1 warning round first.
		BUG: Window Moving
		FEAT: Key repeat
		FEAT: Winning conditions
		FEAT: Sound Effects
		FEAT: Guest logon
		FEAT: Music
		FEAT: Hard break

		FEAT: Throw shit at each other (tweak it?)

		FIX: Clean up project
*/

/*

	while game is running
		Server: clNewBlocks
		Client: svDropBlocks
	end while


	clNewBlocks
		Complete Board
		2 Blocks

	svDropBlocks
		X Position
		Rotation


	BoardUpdate
		if(state.mode == gravity)
			if(!DoGravity())
				state.mode = zapblocks
		else if (state.mode == zapblocks)
			if(ZapChains())
				state.mode == gravity
			else
				state.mode == stable
*/

class CPatternStream : public CSource
{
public:
	TDynamicArray<CSample*> m_lSamples;
	
	CPatternStream()
	{
	}

	void Load(CInputStream &rStream)
	{
		CSample *pSample = new CSample;
		pSample->Load(rStream);
		m_lSamples.Add(pSample);
	}

	~CPatternStream()
	{
		m_lSamples.DeleteAll();
	}
	
	enum
	{
		ESet_Wanted = CSource::ESet_End,
		ESet_Loop,
		ESet_Queue,
		ESet_Hardbreak
	};


	virtual CInstance *CreateInstance(CSoundDevice *pDevice);
};

class CPatternStreamInstance : public CInstance
{
public:
	CPatternStream *m_pPatternStream;
	TDynamicArray<int32> m_lQueue;
	int32 m_Sample;
	int32 m_Last;
	
	int32 m_Queued;
	int32 m_Current;
	int32 m_Wanted;

	bool m_Loop;
	bool m_HardBreak;
	//int32 m_Current;

	int32 m_Position;

	CPatternStreamInstance(CSource *pSource, CSoundDevice *pDevice)
	: CInstance(pSource, pDevice)
	{
		m_pPatternStream = (CPatternStream *)pSource;
		//m_Current = 0;
		m_Current = -1;
		m_Last = -1;
		m_Queued = -1;
		m_Position = -1;
		m_Loop = true;
		m_HardBreak = false;
	}

	virtual bool Set(int32 What, int32 Value)
	{
		if(What == CPatternStream::ESet_Wanted)
		{
			NLog::WriteF("Wanted=%d\n", Value);
			m_Wanted = Value;
		}
		else if(What == CPatternStream::ESet_Loop)
			m_Loop = Value != 0;
		else if(What == CPatternStream::ESet_Queue)
			m_Queued = Value;
		else if(What == CPatternStream::ESet_Hardbreak)
			m_HardBreak = Value != 0;
		else
			return CInstance::Set(What, Value);

		return true;
	}

	void EndOfSample()
	{
		if(m_Wanted >= 0)
		{
			m_Current = m_Wanted;
			m_Wanted = m_Queued;
			m_Queued = -1;
		}
		else if(!m_Loop)
			m_Current = -1;

		if(m_Current >= 0 && m_Current < m_pPatternStream->m_lSamples.Len())
			m_Position = 0;

		m_HardBreak = false;
	}

	virtual void PullData(CBuffer &rBuffer, int64 Position)
	{
		rBuffer.Clear();
		if(m_Position == -1 || (m_Current != m_Wanted && m_Wanted != -1 && m_HardBreak))
			EndOfSample();

		if(m_Current >= 0 && m_Current < m_pPatternStream->m_lSamples.Len() && m_Position >= 0)
		{
			CSample *pSample = m_pPatternStream->m_lSamples[m_Current];
			int32 Left = (pSample->m_DataSize/4-m_Position);
			int32 ChunkSize = Left < 4092 ? Left : 4092;
			rBuffer.m_pData = (uint8*)pSample->m_pData + m_Position*4;
			rBuffer.m_Samples = ChunkSize; //m_pPatternStream->m_lSamples[m_Wanted]->m_DataSize/4;
			m_Position += ChunkSize;

			if(m_Position == pSample->m_DataSize/4)
				m_Position = -1;
			
		}
		m_Last = m_Current;
	}
};


CInstance *CPatternStream::CreateInstance(CSoundDevice *pDevice)
{
	return new CPatternStreamInstance(this, pDevice);
}




//
// texture view
//
class CTextureView : public CView
{
protected:
	virtual void OnDraw(CDrawer &rDrawer)
	{
		Utils.SetTexture(m_iTexture);
		Utils.Quad_Begin();
		CPoint p = GetWorldPosition();
		Utils.SetColor(r, g, b, 1);
		Utils.SetBlend(C2DUtils::EBlend_Alpha);
		//Utils.Quad_DrawTL(p.x, p.y, 512, 256);
		Utils.Quad_DrawTL(p.x, p.y, GetSize().x, GetSize().y);
		Utils.Quad_End();
	}

public:
	CTextureView(N3D::CDevice *pDevice, CView *pParent=NULL, const int8 *pName=NULL, CGUI *pGUI=NULL)
	: CView(pParent, pName, pGUI)
	{
		Utils.Init(pDevice);
		r = g = b = 1;
	}

	C2DUtils Utils;
	int32 m_iTexture;
	fp32 r, g, b;
};

//
//
//
class CRoomItem : public CStringItem
{
public:
	int32 m_iRoom;

	CRoomItem(int32 iRoom, const int8 *pString)
	: m_iRoom(iRoom), CStringItem(pString, true)
	{}
};

//
//
//
class CSpecialText
{
public:
	fp32 x, y;
	int32 m_Type;
	int64 m_StartTick;
	int64 m_Duration;
	fp32 m_Scale;
	fp32 m_ScaleSpeed;
	fp32 m_Rot;
	fp32 m_RotSpeed;
};

class CRect
{
public:
	CString m_Name;
	fp32 x, y;	
	fp32 w, h;
};

//
//
//
class CPuyoGame : public CConsoleClient, public CMessageReceiver
{
public:
	CCore m_Core;

	// modules
	CNetworkClient *m_pClient;
	CNetworkAuthClient *m_pAuth;
	CNetworkChatClient *m_pChat;
	CNetworkRoomClient *m_pRoom;
	CNetworkGameClient *m_pGame;
	
	CKeyboardInput *m_pKeyboardInput;
	CMouseInput *m_pMouseInput;
	CRender *m_pRender;
	CSound *m_pSound;
	
	// rendering
	N3D::CDevice *m_pDevice;
	NSound::CSoundDevice *m_pSoundDevice;
	C2DUtils *m_pUtils;
	C2DUtils::CFont m_SmallFont;

	// textures
	int32 m_iBlobTexture;
	int32 m_iTitleTexture;
	int32 m_iFontTexture;
	int32 m_iTextsTexture;
	int32 m_iGUITexture;
	int32 m_iBackGroundTexture;
	int32 m_iYouLoseTexture;
	int32 m_iYouWinTexture; //    \o/
	int32 m_iMLPTexture;

	int32 m_iTeeTexture;

	//
	TDynamicArray<CSpecialText> m_lTexts;

	TDynamicArray<CRect> m_lRects;
	
	// sounds
	CSample m_ClickSample;

	CSample m_Anc_HurryUp;
	CSample m_Anc_Big;
	CSample m_Anc_Chain;
	CSample m_Anc_Combo;
	CSample m_Anc_Go;
	CSample m_Anc_Cheer[9];

	CSample m_ZapSample[10];
	CSample m_PunishSample[5];

	CSample m_BadMoveSample;
	CSample m_RotateSample;

	CPatternStream m_PatternStream;
	int32 m_iPatternSound;

	//
	CGUI m_GUI;
	CPainter_2DUtils m_GUIPainter;

	// events
	enum
	{
		EUIEvent_Connect,
		EUIEvent_Disconnect,
		EUIEvent_Chat,

		EUIEvent_ConsoleCommand,
		EUIEvent_JoinRoom,
		EUIEvent_DesignRoom,
		EUIEvent_CreateRoom,
		EUIEvent_ShowLobby,
		EUIEvent_LeaveRoom,
		EUIEvent_StartGame,

		EUIEvent_Quit,
		EUIEvent_Back,

		ENetEvent_Connected,
		ENetEvent_Disconnected,

		EGameEvent,

		//EGameEvent_Start,
		//EGameEvent_End,

		ERoomEvent_Update,
		ERoomEvent_ChangedRoom,
		ERoomEvent_RoomCreateError,
		
		EChatEvent_Message,

		EAuthEvent_Success,
		EAuthEvent_Failed,

		ELastEvent
	};

	//CMessageQueue m_MsgQueue;

	// gui 
	CView *m_pRoot;

	CView *m_pLoginView;
	CView *m_pStatusView;
	CView *m_pLobbyView;
	CView *m_pCreateRoomView;
	CView *m_pRoomView;

	CButton *m_pDisconnectButton;

	CTextBox *m_pCommandLine;

	CLabel *m_pStatusLabel;
	CView *m_pBackView;

	//
	//
	CPuyoGame()
	{
		m_pBackView = 0;
	}

	//
	//
	~CPuyoGame()
	{
		m_pSoundDevice->Destroy(m_iPatternSound);
		m_GUI.GetRoot()->AddChild(m_pLoginView);
		m_GUI.GetRoot()->AddChild(m_pStatusView);
		m_GUI.GetRoot()->AddChild(m_pLobbyView);
		m_GUI.GetRoot()->AddChild(m_pCreateRoomView);
		m_GUI.GetRoot()->AddChild(m_pRoomView);
		m_GUI.GetRoot()->AddChild(m_pDisconnectButton);
		m_GUI.GetRoot()->AddChild(m_pCommandLine);

		m_GUI.GetRoot()->CheckTree(NULL);
		m_GUI.GetRoot()->DestroyTree();

		SafeDelete(m_pRoot);
		m_GUI.SetRoot(m_pRoot);
	}

	//
	//
	//
	int32 LoadTexture(const int8 *pFilename)
	{
		CFileReader File(pFilename);
		CImage Img;
		Assert(Img.Load(File));
		Assert(m_pDevice);
		int32 iTex = m_pDevice->CreateTexture(Img, ETexFlag_NoMipMap);
		Assert(iTex >= 0);
		return iTex;
	}

	int32 FindRect(const int8 *pName)
	{
		for(int32 i = 0; i < m_lRects.Len(); i++)
			if(m_lRects[i].m_Name == pName)
				return i;
			
		return -1;
	}


	//
	//
	//
	void ReadOGG(CSample &rSample, const int8 *pFilename)
	{
		//NLog::WriteF("File: %s\n", pFilename);
		FILE *pFile = fopen(pFilename, "rb");
		if(!pFile)
			return;
		OggVorbis_File vf;
		int current_section = 0;
	  
		if(ov_open(pFile, &vf, NULL, 0) < 0)
			return;
			//throw "Input does not appear to be an Ogg bitstream.";

		rSample.m_DataSize = ov_pcm_total(&vf, -1)*4;
		rSample.m_pData = new int8 [rSample.m_DataSize];
		int8 *pDst = (int8 *)rSample.m_pData;
		int32 Left = rSample.m_DataSize;

		while(1)
		{
			long ret = ov_read(&vf, pDst, Left, 0, 2, 1, &current_section);
			if (ret == 0)
				break;
			else if (ret < 0)
			{
				NLog::WriteF("Error in ogg");
				break;
			}
			else
			{
				Left -= ret;
				pDst += ret;
			}
		}
		ov_clear(&vf);
		fclose(pFile);
	}

	//
	//
	//
	void LoadSample(CSample &rSample, const int8 *pFilename)
	{
		ReadOGG(rSample, pFilename);
	}

	//
	//
	//
	void RenderLoading()
	{
		static int32 a = 0;
		m_pDevice->Clear_Color(0,0,0,1);
		m_pDevice->Clear(N3D::EBuffer_Color);
		m_pUtils->Text_Begin(&m_SmallFont);
		CString s = "Loading ";
		for(int32 i = 0; i < a; i++)
			s += ".";
		m_pUtils->Text_Draw(15, 480-15-6, 0, "%s", s.GetData());
		m_pUtils->Text_End();
		m_pDevice->SwapBuffers();

		NCore::UpdateAndPump();
		a++;
	}

	CRegistry m_Config;

	//
	//
	//
	int32 GetConfigInt(const int8 *pName, int32 Default)
	{
		CRegistry *pReg = m_Config.GetChild(pName);
		if(!pReg)
			return Default;
		return pReg->m_Value.GetInt();
	}

	//
	//
	//
	CString GetConfigString(const int8 *pName, CString Default)
	{
		CRegistry *pReg = m_Config.GetChild(pName);
		if(!pReg)
			return Default;
		return pReg->m_Value;
	}


	//
	//
	//
	void Init()
	{
		//
		m_ServerAddress = "172.18.26.198";
		//m_ServerAddress = "ki.teewars.com";

		// load config registry
		{
			CFileReader RegFile("config.registry");
			if(RegFile.IsReady())
				m_Config.Read(RegFile);
		}
		g_FrameLimit = GetConfigInt("fpslimit", 60);
		g_Music = GetConfigInt("music", 1) != 0;
		g_HighContrast = GetConfigInt("highcontrast", 1) != 0;
		g_Fullscreen = GetConfigInt("fullscreen", 1) != 0;

		// init modules
		Assert(m_Core.LoadModule(m_pClient, "NetworkClient"));
		Assert(m_Core.LoadModule(m_pAuth, "NetworkAuthClient"));
		Assert(m_Core.LoadModule(m_pChat, "NetworkChatClient"));
		Assert(m_Core.LoadModule(m_pRoom, "NetworkRoomClient"));
		Assert(m_Core.LoadModule(m_pGame, "NetworkGameClient"));

		Assert(m_Core.LoadModule(m_pRender, "Render"));
		Assert(m_Core.LoadModule(m_pKeyboardInput, "KeyboardInput"));
		Assert(m_Core.LoadModule(m_pMouseInput, "MouseInput"));
		Assert(m_Core.LoadModule(m_pSound, "Sound"));


		Assert(m_pDevice = m_pRender->GetDevice());
		Assert(m_pUtils = m_pRender->Get2DUtils());

		if(g_Fullscreen)
			Assert(m_pDevice->Init(N3D::CMode(g_ScreenWidth, g_ScreenHeight, true, 0)));
		else
			Assert(m_pDevice->Init(N3D::CMode(g_ScreenWidth, g_ScreenHeight, false, 0)));

		Assert(m_pUtils->Init(m_pDevice));
		m_pUtils->m_ScreenWidth = g_ScreenWidth;
		m_pUtils->m_ScreenHeight = g_ScreenHeight;

		Assert(m_pSoundDevice = m_pSound->GetDevice());

		Assert(m_pSoundDevice->Init(CFormat(44100, 2, 16), Clamp(GetConfigInt("sndbuffersize", 60), 1, 500)));

		m_pDevice->State_SetDefaults(true);

		//
		m_pClient->SetVersion(TEEPOP_NETWORK_VERSION);

		// bind events
		m_pClient->Bind(CNetworkClient::EEvent_Connected, this, ENetEvent_Connected);
		m_pClient->Bind(CNetworkClient::EEvent_Disconnected, this, ENetEvent_Disconnected);
		m_pAuth->Bind(CNetworkAuthClient::EEvent_Success, this, EAuthEvent_Success);
		m_pAuth->Bind(CNetworkAuthClient::EEvent_Failed, this, EAuthEvent_Failed);
		m_pChat->Bind(CNetworkChatClient::EEvent_ChatMessage, this, EChatEvent_Message);

		m_pRoom->Bind(CNetworkRoomClient::EEvent_RoomsUpdated, this, ERoomEvent_Update);
		m_pRoom->Bind(CNetworkRoomClient::EEvent_RoomChange, this, ERoomEvent_ChangedRoom);
		m_pRoom->Bind(CNetworkRoomClient::EEvent_RoomCreateError, this, ERoomEvent_RoomCreateError);
		
		m_pGame->Bind(CNetworkGameClient::EEvent_GameStart, this, EGameEvent);
		m_pGame->Bind(CNetworkGameClient::EEvent_GameEnd, this, EGameEvent);
		m_pGame->Bind(CNetworkGameClient::EEvent_BadMove, this, EGameEvent);
		m_pGame->Bind(CNetworkGameClient::EEvent_Rotate, this, EGameEvent);
		m_pGame->Bind(CNetworkGameClient::EEvent_Punish, this, EGameEvent);
		m_pGame->Bind(CNetworkGameClient::EEvent_SentStones, this, EGameEvent);
		m_pGame->Bind(CNetworkGameClient::EEvent_EvadedStones, this, EGameEvent);
		m_pGame->Bind(CNetworkGameClient::EEvent_GoneHigh, this, EGameEvent);
		m_pGame->Bind(CNetworkGameClient::EEvent_GoneLow, this, EGameEvent);
		m_pGame->Bind(CNetworkGameClient::EEvent_LevelIncrease, this, EGameEvent);
		

		// register commands
		m_Core.RegisterCommand("server", this);

		// init input
		m_pKeyboardInput->GetKeyboard()->UseEvents(true);
		m_pMouseInput->GetMouse()->UseEvents(true);
		m_pMouseInput->GetMouse()->SetMode(CMouse::EMode_Loose);

		// load and create font (so we can display loading screen)
		m_iFontTexture = LoadTexture("data/smallfont.png");

		m_SmallFont.BuildNormalAscii();
		m_SmallFont.m_iTexture = m_iFontTexture;
		m_SmallFont.m_XCells = 16;
		m_SmallFont.m_YCells = 16;
		m_SmallFont.m_Width = 16;
		m_SmallFont.m_Height = 16;
		m_SmallFont.m_Spacing = 6;
		
		RenderLoading();

		// load textures
		m_iBlobTexture = LoadTexture("data/blobs.png"); RenderLoading();
		m_iBackGroundTexture = LoadTexture("data/bgtile.png"); RenderLoading();
		m_iTitleTexture = LoadTexture("data/title.png"); RenderLoading();
		m_iGUITexture = LoadTexture("data/guiskin.png"); RenderLoading();
		m_iYouLoseTexture = LoadTexture("data/youlose.png"); RenderLoading();
		m_iYouWinTexture = LoadTexture("data/youwin.png"); RenderLoading();
		m_iTeeTexture = LoadTexture("data/cry.png"); RenderLoading();
		m_iTextsTexture = LoadTexture("data/text.png"); RenderLoading();


		{
			CMemoryReader Png((void*)g_mlppng, sizeof(g_mlppng));
			Png.SetMime("image/png");
			CImage Image;
			Image.Load(Png);
			m_iMLPTexture = m_pDevice->CreateTexture(Image, ETexFlag_NoMipMap);
		}

		


		LoadSample(m_ClickSample, "data/tick.ogg"); RenderLoading();

		
		LoadSample(m_Anc_HurryUp, "data/anc_hurryup.ogg"); RenderLoading();
		LoadSample(m_Anc_Big, "data/anc_big.ogg"); RenderLoading();
		LoadSample(m_Anc_Chain, "data/anc_chain.ogg"); RenderLoading();
		LoadSample(m_Anc_Combo, "data/anc_combo.ogg"); RenderLoading();
		LoadSample(m_Anc_Go, "data/anc_go.ogg"); RenderLoading();

		LoadSample(m_Anc_Cheer[0], "data/anc_cheer01.ogg"); RenderLoading();
		LoadSample(m_Anc_Cheer[1], "data/anc_cheer02.ogg"); RenderLoading();
		LoadSample(m_Anc_Cheer[2], "data/anc_cheer03.ogg"); RenderLoading();
		LoadSample(m_Anc_Cheer[3], "data/anc_cheer04.ogg"); RenderLoading();
		LoadSample(m_Anc_Cheer[4], "data/anc_cheer05.ogg"); RenderLoading();
		LoadSample(m_Anc_Cheer[5], "data/anc_cheer06.ogg"); RenderLoading();
		LoadSample(m_Anc_Cheer[6], "data/anc_cheer07.ogg"); RenderLoading();
		LoadSample(m_Anc_Cheer[7], "data/anc_cheer08.ogg"); RenderLoading();
		LoadSample(m_Anc_Cheer[8], "data/anc_cheer09.ogg"); RenderLoading();

		LoadSample(m_ZapSample[0], "data/bleep_combo_001.ogg"); RenderLoading();
		LoadSample(m_ZapSample[1], "data/bleep_combo_002.ogg"); RenderLoading();
		LoadSample(m_ZapSample[2], "data/bleep_combo_003.ogg"); RenderLoading();
		LoadSample(m_ZapSample[3], "data/bleep_combo_004.ogg"); RenderLoading();
		LoadSample(m_ZapSample[4], "data/bleep_combo_005.ogg"); RenderLoading();
		LoadSample(m_ZapSample[5], "data/bleep_combo_006.ogg"); RenderLoading();
		LoadSample(m_ZapSample[6], "data/bleep_combo_007.ogg"); RenderLoading();
		LoadSample(m_ZapSample[7], "data/bleep_combo_008.ogg"); RenderLoading();
		LoadSample(m_ZapSample[8], "data/bleep_combo_009.ogg"); RenderLoading();
		LoadSample(m_ZapSample[9], "data/bleep_combo_010.ogg"); RenderLoading();

		LoadSample(m_PunishSample[0], "data/bleep_punish_001.ogg"); RenderLoading();
		LoadSample(m_PunishSample[1], "data/bleep_punish_002.ogg"); RenderLoading();
		LoadSample(m_PunishSample[2], "data/bleep_punish_003.ogg"); RenderLoading();
		LoadSample(m_PunishSample[3], "data/bleep_punish_004.ogg"); RenderLoading();
		LoadSample(m_PunishSample[4], "data/bleep_punish_005.ogg"); RenderLoading();
		
		LoadSample(m_BadMoveSample, "data/gui_illaction.ogg"); RenderLoading();
		LoadSample(m_RotateSample, "data/gui_rotate.ogg"); RenderLoading();


		{
			CSample *pSample = new CSample;
			LoadSample(*pSample, "data/music_start.ogg");
			m_PatternStream.m_lSamples.Add(pSample);
		}
		{
			CSample *pSample = new CSample;
			LoadSample(*pSample, "data/music_low.ogg");
			m_PatternStream.m_lSamples.Add(pSample);
		}
		{
			CSample *pSample = new CSample;
			LoadSample(*pSample, "data/music_high.ogg");
			m_PatternStream.m_lSamples.Add(pSample);
		}
		{
			CSample *pSample = new CSample;
			LoadSample(*pSample, "data/music_end.ogg");
			m_PatternStream.m_lSamples.Add(pSample);
		}
		
		//Load(File);

		/*
		{ CFileReader File("data/music_start.ogg"); m_PatternStream.Load(File); } RenderLoading();
		{ CFileReader File("data/music_low.ogg"); m_PatternStream.Load(File); } RenderLoading();
		{ CFileReader File("data/music_high.ogg"); m_PatternStream.Load(File); } RenderLoading();
		{ CFileReader File("data/music_end.ogg"); m_PatternStream.Load(File); } RenderLoading();
		}*/

		// load sounds
		/*

		LoadSample(m_ClickSample, "data/tick.raw"); RenderLoading();

		LoadSample(m_Anc_Big, "data/anc_big.raw");
		LoadSample(m_Anc_Chain, "data/anc_chain.raw");
		LoadSample(m_Anc_Combo, "data/anc_combo.raw");
		LoadSample(m_Anc_Go, "data/anc_go.raw"); RenderLoading();

		LoadSample(m_Anc_Cheer[0], "data/anc_cheer01.raw"); RenderLoading();
		LoadSample(m_Anc_Cheer[1], "data/anc_cheer02.raw"); RenderLoading();
		LoadSample(m_Anc_Cheer[2], "data/anc_cheer03.raw"); RenderLoading();

		LoadSample(m_ZapSample[0], "data/bleep_combo_001.raw"); RenderLoading();
		LoadSample(m_ZapSample[1], "data/bleep_combo_002.raw"); RenderLoading();
		LoadSample(m_ZapSample[2], "data/bleep_combo_003.raw"); RenderLoading();
		LoadSample(m_ZapSample[3], "data/bleep_combo_004.raw"); RenderLoading();
		LoadSample(m_ZapSample[4], "data/bleep_combo_005.raw"); RenderLoading();
		LoadSample(m_ZapSample[5], "data/bleep_combo_006.raw"); RenderLoading();
		LoadSample(m_ZapSample[6], "data/bleep_combo_007.raw"); RenderLoading();
		LoadSample(m_ZapSample[7], "data/bleep_combo_008.raw"); RenderLoading();
		LoadSample(m_ZapSample[8], "data/bleep_combo_009.raw"); RenderLoading();
		LoadSample(m_ZapSample[9], "data/bleep_combo_010.raw"); RenderLoading();

		LoadSample(m_PunishSample[0], "data/bleep_punish_001.raw"); RenderLoading();
		LoadSample(m_PunishSample[1], "data/bleep_punish_002.raw"); RenderLoading();
		LoadSample(m_PunishSample[2], "data/bleep_punish_003.raw"); RenderLoading();
		LoadSample(m_PunishSample[3], "data/bleep_punish_004.raw"); RenderLoading();
		LoadSample(m_PunishSample[4], "data/bleep_punish_005.raw"); RenderLoading();
		
		LoadSample(m_BadMoveSample, "data/gui_illaction.raw"); RenderLoading();
		LoadSample(m_RotateSample, "data/gui_rotate.raw"); RenderLoading();

		{ CFileReader File("data/music_start.raw"); m_PatternStream.Load(File); } RenderLoading();
		{ CFileReader File("data/music_low.raw"); m_PatternStream.Load(File); } RenderLoading();
		{ CFileReader File("data/music_high.raw"); m_PatternStream.Load(File); } RenderLoading();
		{ CFileReader File("data/music_end.raw"); m_PatternStream.Load(File); } RenderLoading();

		*/
		
		// setup GUI
		m_GUIPainter.m_pUtils = m_pUtils;
		m_GUIPainter.Init();

		m_GUIPainter.m_iTexture = m_iGUITexture;
		m_GUIPainter.m_TextureHeight = m_GUIPainter.m_TextureWidth = 64;
		m_GUIPainter.m_Font = m_SmallFont;
		m_GUI.m_Drawer.m_pPainter = &m_GUIPainter;
		m_GUI.m_Drawer.Init();

		// map events
		m_GUI.AddMapping("connect", this, EUIEvent_Connect);
		m_GUI.AddMapping("designroom", this, EUIEvent_DesignRoom);
		m_GUI.AddMapping("joinroom", this, EUIEvent_JoinRoom);
		m_GUI.AddMapping("createroom", this, EUIEvent_CreateRoom);
		m_GUI.AddMapping("leaveroom", this, EUIEvent_LeaveRoom);
		m_GUI.AddMapping("chat", this, EUIEvent_Chat);
		m_GUI.AddMapping("disconnect", this, EUIEvent_Disconnect);
		m_GUI.AddMapping("quit", this, EUIEvent_Quit);
		m_GUI.AddMapping("showlobby", this, EUIEvent_ShowLobby);
		m_GUI.AddMapping("startgame", this, EUIEvent_StartGame);
		m_GUI.AddMapping("back", this, EUIEvent_Back);

		RenderLoading();

		// load rect registry
		CRegistry RectReg;
		{
			CFileReader RegFile("data/texts.registry");
			RectReg.Read(RegFile);
		}
		RenderLoading();

		for(int32 i = 0; i < RectReg.m_lValues.Len(); i++)
		{
			CRect Rect;
			CString Name = RectReg.m_lValues[i]->m_Name;
			CString Value = RectReg.m_lValues[i]->m_Value;
			Rect.m_Name = Name;
			Rect.x = Value.GetStrSep(",").GetInt();
			Rect.y = Value.GetStrSep(",").GetInt();
			Rect.w = Value.GetStrSep(",").GetInt();
			Rect.h = Value.GetStrSep(",").GetInt();
			m_lRects.Add(Rect);
		}
		RenderLoading();
				
		// load gui registry
		CRegistry GUIReg;
		{
			CFileReader RegFile("data/gui.registry");
			GUIReg.Read(RegFile);
		}
		RenderLoading();
		//GUIReg.Dump();

		// create root window
		m_pRoot = new CView(NULL, NULL, &m_GUI);
		m_pRoot->SetSize(CPoint(g_ScreenWidth,g_ScreenHeight));
		m_GUI.SetRoot(m_pRoot);

		// create command line
		{
			m_pCommandLine = new CTextBox(NULL, "commandline", &m_GUI);
			m_pCommandLine->SetGeometry(20,20, 600, 16);
			m_pCommandLine->Bind(CTextBox::EEvent_Enter, this, EUIEvent_ConsoleCommand);
		}

		// create disconnect button
		{
			m_pDisconnectButton = new CButton(&*m_pRoot);
			m_pDisconnectButton->SetCaption("Disconnect");
			m_pDisconnectButton->SetGeometry(640-20-90, 480-20-20, 90, 20);
			m_pDisconnectButton->Bind(CButton::EEvent_Clicked, this, EUIEvent_Disconnect);
		}

		// create status window
		m_pStatusView = CreateViewFromReg(&GUIReg, "status");
		m_pStatusLabel = (CLabel *)&*m_pStatusView->GetChild("status");

		// create login view
		m_pLoginView = CreateViewFromReg(&GUIReg, "login");
		if(m_pLoginView != NULL)
		{
			CTextureView *pTitle = new CTextureView(m_pRender->GetDevice(), &*m_pLoginView);
			pTitle->SetGeometry(640/2-512/2, 250/2-256/2, 512, 256);
			pTitle->m_iTexture = m_iTitleTexture;

			CLabel *pVersionLabel = (CLabel *)m_pLoginView->GetChild("version");
			if(pVersionLabel != NULL)
			{
				CString s = pVersionLabel->GetText();
				pVersionLabel->SetText(CString("%s, %s", TEEPOP_CLIENT_VERSION, s.GetData()));
			}
			
		}

		m_pCreateRoomView = CreateViewFromReg(&GUIReg, "designroom");
		m_pLobbyView = CreateViewFromReg(&GUIReg, "lobby");
		m_pRoomView = CreateViewFromReg(&GUIReg, "room");

		RenderLoading();

		//
		if(g_Music)
			m_iPatternSound = m_pSoundDevice->Play(&m_PatternStream, 0);

		//m_pSoundDevice->Set(m_iPatternSound, CPatternStream::ESet_Wanted, 1);
		//m_pSoundDevice->Set(m_iPatternSound, 1, 0);

		// show login
		ShowLogin();
		//ShowView(NULL);
	}

	CView *CreateViewFromReg(CRegistry *pMasterReg, const int8 *pName)
	{
		CRegistry *pReg = pMasterReg->GetChild(pName);
		if(pReg && pReg->m_lValues.Len())
			return m_GUI.CreateFromRegistry(pReg->m_lValues[0]);
		return NULL;
	}

	//
	void ShowCommandLine()
	{
		if(m_pCommandLine->GetParent() == NULL) // show commandline
			m_GUI.GetRoot()->AddChild(&*m_pCommandLine);
		m_GUI.Select(&*m_pCommandLine);
	}

	void HideCommandLine()
	{
		m_GUI.Select(m_GUI.GetRoot());
		m_GUI.GetRoot()->RemoveChild(&*m_pCommandLine);
	}

	void ShowView(CView *pView, const int8 *pFocus="")
	{
		m_GUI.GetRoot()->RemoveChild(m_pLoginView);
		m_GUI.GetRoot()->RemoveChild(m_pStatusView);
		m_GUI.GetRoot()->RemoveChild(m_pLobbyView);
		m_GUI.GetRoot()->RemoveChild(m_pRoomView);
		m_GUI.GetRoot()->RemoveChild(m_pCreateRoomView);
		m_GUI.GetRoot()->AddChild(pView);
		
		if(pView)
			m_GUI.Select(pView->GetChild(pFocus));

		CListBox *pListBox = (CListBox*)m_GUI.GetRoot()->GetChild("chat");
		if(pListBox)
			pListBox->SetScrollAmount(1);
	}

	void ShowStatus() { ShowView(m_pStatusView); }
	void ShowLobby() { ShowView(m_pLobbyView, "chatinput"); }
	void ShowLogin() { ShowView(m_pLoginView, "username"); }
	void ShowRoom() { ShowView(m_pRoomView, "chatinput"); }
	void ShowCreateRoom() { ShowView(m_pCreateRoomView, "roomname"); }

	void SetMessage(const int8 *pMessage, CView *pBack)
	{
		m_pStatusLabel->SetText(pMessage);
		
		CView *pBackButton = m_pStatusView->GetChild("back");
		if(pBackButton && pBack)
			pBackButton->ClearFlags(CView::EFlag_Hidden);
		if(pBackButton && !pBack)
			pBackButton->SetFlags(CView::EFlag_Hidden);
		m_pBackView = pBack;
		ShowStatus();
	};

	//
	//
	//
	virtual void OnCommand(CString &rCmd, CString &rArgs)
	{
		if(rCmd == "server")
			m_ServerAddress = rArgs;
	}

	void AddToChat(CString s)
	{
		CListBox *pListBox = (CListBox*)m_GUI.GetRoot()->GetChild("chat");
		if(pListBox != NULL)
			pListBox->AddItem(new CStringItem(s, true));
	}

	
	//
	CString m_ServerAddress;
	CString m_Username;
	CString m_Password;

	//
	// message handler
	//
	void PostMessage(CMessage &rMsg)
	{
		switch(rMsg.m_What)
		{
		case EUIEvent_Quit:
			{
				NCore::AddMessage(NCore::CMessage(NCore::EMsg_Quit));
			} break;
		case EUIEvent_Disconnect:
			{
				m_pClient->Disconnect();
				ShowLogin();
			} break;

		case EUIEvent_Connect:
			{
				SetMessage("Connecting...", NULL);
				CTextBox *pUsername = (CTextBox*)m_pLoginView->GetChild("username");
				CTextBox *pPassword = (CTextBox*)m_pLoginView->GetChild("password");
				if(pUsername != NULL && pPassword != NULL)
				{
					m_Username = pUsername->GetText();
					m_Password = pPassword->GetText();
					NLog::WriteF("LOGIN: user=%s pw=%s\n", m_Username.GetData(), m_Password.GetData());
					//m_pClient->Connect(AddrFromHostname("ki.teewars.com", 8303));
					m_pClient->Connect(AddrFromHostname(m_ServerAddress, 8303));
				}
			} break;

		case EUIEvent_JoinRoom:
			{
				CListBox *pListBox = (CListBox*)m_GUI.GetRoot()->GetChild("rooms");
				if(pListBox != NULL)
				{
					CRoomItem *pItem = (CRoomItem *)pListBox->GetSelectedItem();
					if(pItem)
						m_pRoom->JoinRoom(pItem->m_iRoom);
				}
			} break;
		case EUIEvent_DesignRoom:
			{
				ShowCreateRoom();
			} break;

		case EUIEvent_LeaveRoom:
			{
				m_pRoom->LeaveRoom();
			} break;

		case EUIEvent_CreateRoom:
			{
				CTextBox *pRoomname = (CTextBox*)m_pCreateRoomView->GetChild("roomname");
				CTextBox *pPassword = (CTextBox*)m_pCreateRoomView->GetChild("password");
				CTextBox *pMaxPlayers = (CTextBox*)m_pCreateRoomView->GetChild("maxplayers");
				

				if(pRoomname != NULL && pPassword != NULL && pMaxPlayers != NULL)
				{
					int32 MaxPlayers = Clamp(pMaxPlayers->GetText().GetInt(), 1, 4);
					
					SetMessage("Creating room...", NULL);
					m_pRoom->CreateRoom(pRoomname->GetText(), pPassword->GetText(), MaxPlayers);
				}

			} break;

		case EUIEvent_ShowLobby:
			{
				ShowLobby();
			} break;

		case EUIEvent_Chat:
			{
				CTextBox *pChatInput = (CTextBox*)m_GUI.GetRoot()->GetChild("chatinput");
				m_pChat->Chat(pChatInput->GetText());
				pChatInput->SetText("");
			} break;

		case EUIEvent_StartGame:
			{
				m_pGame->StartGame();
			} break;

		case EChatEvent_Message:
			{
				AddToChat(CString("%s> %s", rMsg.GetString("user", "").GetData(), rMsg.GetString("message", "").GetData()));
			} break;

		case EUIEvent_ConsoleCommand:
			{
				m_Core.DispatchCommand(m_pCommandLine->GetText());
				m_pCommandLine->SetText("");
				HideCommandLine();
			} break;

		case EUIEvent_Back:
			{
				if(m_pBackView)
				{
					ShowView(m_pBackView);
					m_pBackView = NULL;
				}
			} break;

		case EGameEvent:
			{
				switch(rMsg.m_OrgMsg)
				{
				case CNetworkGameClient::EEvent_BadMove:
					m_pSoundDevice->Play(&m_BadMoveSample);
					break;
				case CNetworkGameClient::EEvent_Rotate:
					m_pSoundDevice->Play(&m_RotateSample);
					break;
				case CNetworkGameClient::EEvent_Punish:
					m_pSoundDevice->Play(&m_PunishSample[Clamp(rMsg.GetInt("numstones",0)/3, 0, 4)]);
					break;
				case CNetworkGameClient::EEvent_EvadedStones:
					{
						int32 iClient = rMsg.GetInt("clientid",0);
						int32 Stones = rMsg.GetInt("numstones",0);
						CString ClientName = m_pChat->GetUserName(iClient);

						if(Stones > 1)
							AddToChat(CString("\abbff%s\affff dodged \abbff%d\affff stones", ClientName.GetData(), Stones));
						else
							AddToChat(CString("\abbff%s\affff dodged \abbff%d\affff stone", ClientName.GetData(), Stones));
					} break;
				case CNetworkGameClient::EEvent_SentStones:
					{
						int32 iClient = rMsg.GetInt("clientid",0);
						int32 Stones = rMsg.GetInt("numstones",0);
						CString ClientName = m_pChat->GetUserName(iClient);

						if(iClient == m_pClient->ClientID())
						{
							if(Stones > 1)
								AddToChat(CString("\aafaf%s\affff sent \aafaf%d\affff stones", ClientName.GetData(), Stones));
							else
								AddToChat(CString("\aafaf%s\affff sent \aafaf%d\affff stone", ClientName.GetData(), Stones));
						}
						else
						{
							if(Stones > 1)
								AddToChat(CString("\afaaf%s\affff sent \afaaf%d\affff stones", ClientName.GetData(), Stones));
							else
								AddToChat(CString("\afaaf%s\affff sent \afaaf%d\affff stone", ClientName.GetData(), Stones));
						}
					}
					break;

				case CNetworkGameClient::EEvent_LevelIncrease:
					AddToChat(CString("\affaf -- Level increased to %d --\affff", rMsg.GetInt("level", 1)+1));
					break;
					
				case CNetworkGameClient::EEvent_GoneHigh:
					m_pSoundDevice->Play(&m_Anc_HurryUp);
					m_pSoundDevice->Set(m_iPatternSound, CPatternStream::ESet_Wanted, 2);
					m_pSoundDevice->Set(m_iPatternSound, CPatternStream::ESet_Hardbreak, 1); // hard break
					break;
				case CNetworkGameClient::EEvent_GoneLow:
					m_pSoundDevice->Set(m_iPatternSound, CPatternStream::ESet_Wanted, 1);
					m_pSoundDevice->Set(m_iPatternSound, CPatternStream::ESet_Hardbreak, 1); // hard break
					break;
				case CNetworkGameClient::EEvent_GameStart:
					m_pSoundDevice->Play(&m_Anc_Go);
					m_pSoundDevice->Set(m_iPatternSound, CPatternStream::ESet_Wanted, 0);
					m_pSoundDevice->Set(m_iPatternSound, CPatternStream::ESet_Loop, 1); // loop on
					m_pSoundDevice->Set(m_iPatternSound, CPatternStream::ESet_Queue, 1); // queue
					AddTextCenter("go");
					AddToChat("\aafff   -- GAME STARTS --\affff");
					break;
				case CNetworkGameClient::EEvent_GameEnd:
					m_pSoundDevice->Set(m_iPatternSound, CPatternStream::ESet_Wanted, 3);
					m_pSoundDevice->Set(m_iPatternSound, CPatternStream::ESet_Loop, 0); // loop off
					m_pSoundDevice->Set(m_iPatternSound, CPatternStream::ESet_Hardbreak, 1);
					AddToChat("\aafff   -- GAME ENDED --\affff");
					break;
				}
			} break;
		case ENetEvent_Connected:
			{
				SetMessage("Authenticating...", NULL);
				m_pClient->AttachRecviver(m_pAuth);
				m_pAuth->Logon(m_Username, m_Password);
			} break;
		case ENetEvent_Disconnected:
			{
				CString Msg = rMsg.GetString("errormsg", "");
				SetMessage(CString("Lost connection to server (%s)", Msg.GetData()), m_pLoginView);
			} break;
			

		case EAuthEvent_Failed:
			{
				SetMessage(m_pAuth->GetErrorMessage(), NULL);
				//m_pStatusLabel->SetText(m_pAuth->GetErrorMessage());
			} break;
		case EAuthEvent_Success:
			{
				m_pClient->AttachRecviver(m_pChat);
				m_pClient->AttachRecviver(m_pRoom);

				CTextBox *pBox = (CTextBox *)m_pCreateRoomView->GetChild("roomname");
				if(pBox)
					pBox->SetText(CString("%s's room", m_Username.GetData()));

				if(m_Username == "Myran")
					m_iBackGroundTexture = m_iMLPTexture;

				//m_pChat->SetChannel("global");
				//m_pStatusLabel->SetText("Ok!");
				ShowLobby();
			} break;

		case ERoomEvent_Update:
			{
				CListBox *pListBox = (CListBox*)m_pLobbyView->GetChild("rooms");
				if(pListBox != NULL)
				{
					pListBox->Clear();

					for(int32 i = 0; i < m_pRoom->m_lRooms.Len(); i++)
					{
						CRoom &rRoom = m_pRoom->m_lRooms.GetValue(i);
						pListBox->AddItem(new CRoomItem(rRoom.m_ID, rRoom.GetString()));
					}
				}
			} break;

		case ERoomEvent_RoomCreateError:
			{
				CString Msg = rMsg.GetString("errormsg", "");
				SetMessage(CString("Couldn't create room. %s", Msg.GetData()), m_pCreateRoomView);
			} break;

		case ERoomEvent_ChangedRoom:
			{
				int32 roomid = rMsg.GetInt("roomid");

				if(roomid == -1)
				{
					ShowLobby();
					m_pClient->DetachRecviver(m_pGame);
				}
				else
				{
					ShowRoom();
					m_pClient->AttachRecviver(m_pGame);
				}

				CListBox *pListBox = (CListBox*)m_GUI.GetRoot()->GetChild("chat");
				if(pListBox != NULL)
				{
					pListBox->Clear();
					pListBox->SetScrollAmount(1);
				}

			} break;
		}
	}

	//
	//
	//
	void Update()
	{
		// update core
		m_Core.Update();

		//
		if(m_pClient->IsClosed())
		{
			if(m_pDisconnectButton->GetParent() != NULL)
				m_GUI.GetRoot()->RemoveChild(&*m_pDisconnectButton);
		}
		else
		{
			if(m_pDisconnectButton->GetParent() == NULL)
				m_GUI.GetRoot()->AddChild(&*m_pDisconnectButton);
		}

		// update input
		CKeyboard *pKeyboard = m_pKeyboardInput->GetKeyboard();
		while(pKeyboard->NumEvents())
		{
			CInput::CEvent Event = pKeyboard->GetEvent();

			if(Event.m_Key == NCore::EBtn_Key_F1 && Event.m_Pressed)
			{
				ShowCommandLine();
				continue;
			}

			if(Event.m_Pressed && m_pGame->m_GameState == CNetworkGameClient::EGameState_Playing)
				m_GUI.Select(m_GUI.GetRoot()->GetChild("chatinput"));

			if(Event.m_Pressed)
				m_GUI.KeyDown(Event.m_Key, Event.m_Char);
			else
				m_GUI.KeyUp(Event.m_Key, Event.m_Char);

			if(Event.m_Pressed)
			{
				if(Event.m_Key == NCore::EBtn_Key_F5)		m_pSoundDevice->Set(m_iPatternSound, CSource::ESet_Volume, 0);
				else if(Event.m_Key == NCore::EBtn_Key_F6)	m_pSoundDevice->Set(m_iPatternSound, CSource::ESet_Volume, 333);
				else if(Event.m_Key == NCore::EBtn_Key_F7)		m_pSoundDevice->Set(m_iPatternSound, CSource::ESet_Volume, 666);
				else if(Event.m_Key == NCore::EBtn_Key_F8)		m_pSoundDevice->Set(m_iPatternSound, CSource::ESet_Volume, 1000);
			}

			if(Event.m_Pressed)
			{
				if(m_pGame->m_BoardState == CNetworkGameClient::EState_Placing)
				{
					if(Event.m_Key == NCore::EBtn_Key_Left)
						m_pGame->CtlMoveLeft();
					else if(Event.m_Key == NCore::EBtn_Key_Right)
						m_pGame->CtlMoveRight();
					else if(Event.m_Key == NCore::EBtn_Key_Up)
						m_pGame->CtlRotateCW();
					else if(Event.m_Key == NCore::EBtn_Key_Down)
						m_pGame->CtlDropBlocks();
				}
			}
		}

		//
		CListBox *pListBox = (CListBox*)m_pLobbyView->GetChild("players");
		if(pListBox != NULL)
		{
			pListBox->Clear();

			for(int32 i = 0; i < m_pChat->m_lUsers.Len(); i++)
				pListBox->AddItem(new CStringItem(m_pChat->m_lUsers.GetValue(i)));
		}

		// poll input
		CMouse *pMouse = m_pMouseInput->GetMouse();
		m_GUI.MouseMoveAbs(pMouse->MovementX(), pMouse->MovementY());

		while(pMouse->NumEvents())
		{
			CInput::CEvent Event = pMouse->GetEvent();
			if(Event.m_Pressed)
			{
				m_GUI.MouseDown(Event.m_Key);
				m_pSoundDevice->Play(&m_ClickSample);
				
			}
			else
				m_GUI.MouseUp(Event.m_Key);
		}

		Render();

	}

	//
	//
	//
	void RenderBlockPrepare(int32 color, fp32 mod=1)
	{
		fp32 aColors[][5] = {{0,0,0,1},{1,0.7f,0.7f,1},{0.7f,1,0.7f,1},{0.7f,0.7f,1,1},{1,1,0.7f,1}, {1,1,0.95f,1}};
		fp32 aHigh[][5] = {{0,0,0,1},{1,0,0,1},{0.0f,1,0.0f,1},{0,1,1,1},{1,0,1,1},{1,1,0.95f,1}};
		if(g_HighContrast)
			m_pUtils->SetColor(aHigh[color][0]*mod, aHigh[color][1]*mod, aHigh[color][2]*mod, aHigh[color][3]*mod);
		else
			m_pUtils->SetColor(aColors[color][0]*mod, aColors[color][1]*mod, aColors[color][2]*mod, aColors[color][3]*mod);
	}

	void RenderBlock(fp32 x, fp32 y, int32 connectionindex, int32 color, fp32 scale)
	{
		RenderBlockPrepare(color);
		fp32 u = (connectionindex%4)/8.0f;
		fp32 v = (connectionindex/4)/8.0f;
		m_pUtils->SetTexCoord(u,v,u+1/8.0f,v+1/8.0f);
		x = (fp32)((int32)x);
		y = (fp32)((int32)y);
		m_pUtils->Quad_DrawTL(x, y, scale, scale);
	}

	//
	//
	//
	void RenderOutline(fp32 basex, fp32 basey, fp32 Scale)
	{
		m_pUtils->Line_Begin();
		m_pUtils->Line_Draw(basex, basey, basex+CBoard::EWidth*Scale, basey);
		m_pUtils->Line_Draw(basex, basey, basex, basey+CBoard::EHeight*Scale);
		m_pUtils->Line_Draw(basex+CBoard::EWidth*Scale, basey, basex+CBoard::EWidth*Scale, basey+CBoard::EHeight*Scale);
		m_pUtils->Line_Draw(basex, basey+CBoard::EHeight*Scale, basex+CBoard::EWidth*Scale, basey+CBoard::EHeight*Scale);
		m_pUtils->Line_End();
	}

	//
	//
	//
	void RenderBoard(CBoard *pBoard, fp32 basex, fp32 basey, fp32 scale, fp32 IntertickTime)
	{
		fp32 time = IntertickTime;
		for(int32 y = 0; y < CBoard::EHeight; y++)
			for(int32 x = 0; x < CBoard::EWidth; x++)
			{
				int32 c = pBoard->m_aField[x][y];
				int32 e = pBoard->m_aExtra[x][y];
				if(c)
				{
					int32 i = pBoard->GetConnectionIndex(x,y);
					if(e&CBoard::EExtra_Falled)
						RenderBlock(basex+x*scale, basey+y*scale-scale*(1-time), i, c, scale);
					else
						RenderBlock(basex+x*scale, basey+y*scale, i, c, scale);
				}

				if(e&CBoard::EExtra_Zapped)
				{
					int32 Last = e>>CBoard::EExtra_LastShift;
					RenderBlockPrepare(Last, 1.2f);
					//RenderBlockPrepare(5);
					int32 Frame = time*5.0f;
					if(Frame < 4)
					{
						fp32 u = 1/8.0f*(Frame);
						fp32 v = 5/8.0f;
						if(Last==CBoard::EStone)
							v += 1/8.0f;
						fp32 SizeMod = scale*0.75f*time;
						m_pUtils->SetTexCoord(u,v,u+1/8.0f,v+1/8.0f);
						m_pUtils->Quad_Draw(basex+x*scale+scale/2, basey+y*scale+scale/2, scale+SizeMod, scale+SizeMod);
					}
				}
			}
	};

	void AddText(const int8 *pType, fp32 x, fp32 y, fp32 Rot, fp32 RotSpeed, fp32 Scale, fp32 ScaleSpeed)
	{
		CSpecialText Text;
		Text.x = x;//m_pGame->m_LocalBoard.m_aZaps[0].m_x*32+16;
		Text.y = y;//m_pGame->m_LocalBoard.m_aZaps[0].m_y*32+16;
		Text.m_Type = FindRect(pType);
		Text.m_StartTick = GetTickCount();
		Text.m_Duration = GetTickFrequency()/3;
		Text.m_Rot = Rot;
		Text.m_RotSpeed = RotSpeed;
		if(Text.m_Rot > 0)
			Text.m_RotSpeed = -Text.m_RotSpeed;
		Text.m_Scale = Scale;
		Text.m_ScaleSpeed = ScaleSpeed;
		m_lTexts.Add(Text);

	}

	void AddText(const int8 *pType, fp32 x, fp32 y)
	{
		AddText(pType, x, y, (Random()-0.5f)*2, 0.1f+Random(), 0.4f, 0.6f);
	}

	void AddTextCenter(const int8 *pType)
	{
		AddText(pType, 32*6/2, 32*4+16, -0.05f, 0.1f, 0.25f, 1);
	}

	//
	//
	//
	void Render()
	{
		if(m_pGame->m_LocalBoard.m_Updated)
		{
			m_pGame->m_LocalBoard.m_Updated = false;
			if(m_pGame->m_LocalBoard.m_NumZaps)
			{
				bool GotBig = false;
				bool GotChain = m_pGame->m_LocalBoard.m_ZapRound > 1;
				bool GotCombo = m_pGame->m_LocalBoard.m_NumZaps > 1;

				fp32 cx = 0;
				fp32 cy = 0;
				for(int32 i = 0; i < m_pGame->m_LocalBoard.m_NumZaps; i++)
				{
					cx += m_pGame->m_LocalBoard.m_aZaps[0].m_x;
					cy += m_pGame->m_LocalBoard.m_aZaps[0].m_y;

					if(m_pGame->m_LocalBoard.m_aZaps[0].m_Count >= 6)
					{
						GotBig = true;
						AddText("big", m_pGame->m_LocalBoard.m_aZaps[i].m_x*32+16,
										m_pGame->m_LocalBoard.m_aZaps[i].m_y*32+16);
					}
				}
				cx /= m_pGame->m_LocalBoard.m_NumZaps;
				cy /= m_pGame->m_LocalBoard.m_NumZaps;


				m_pSoundDevice->Play(&m_ZapSample[Clamp(m_pGame->m_LocalBoard.m_Multiplyer, 0, 9)]);
				if(m_pGame->m_LocalBoard.m_NumZaps > 1)
				{
					for(int32 i = 0; i < m_pGame->m_LocalBoard.m_NumZaps; i++)
					{
						if(m_pGame->m_LocalBoard.m_aZaps[0].m_Count >= 6)
							continue;
						AddText("combo", m_pGame->m_LocalBoard.m_aZaps[i].m_x*32+16,
											m_pGame->m_LocalBoard.m_aZaps[i].m_y*32+16);
					}
				}
				
				if(m_pGame->m_LocalBoard.m_ZapRound > 1)
					AddText("chain", cx*32+16, cy*32+16);

				if(GotBig)
					m_pSoundDevice->Play(&m_Anc_Big);
				else if(GotCombo)
					m_pSoundDevice->Play(&m_Anc_Combo);
				else if(GotChain)
					m_pSoundDevice->Play(&m_Anc_Chain);

				//m_pGame->m_TickLength = GetTickFrequency()*0.35f;
			}
			//else
			//	m_pGame->m_TickLength = GetTickFrequency()/30;

			if(m_pGame->m_BoardState != CNetworkGameClient::EState_Itterating)
			{
				if(m_pGame->m_LocalBoard.m_Scored > 50) m_pSoundDevice->Play(&m_Anc_Cheer[8]);
				else if(m_pGame->m_LocalBoard.m_Scored >=35) m_pSoundDevice->Play(&m_Anc_Cheer[7]);
				else if(m_pGame->m_LocalBoard.m_Scored >=25) m_pSoundDevice->Play(&m_Anc_Cheer[6]);
				else if(m_pGame->m_LocalBoard.m_Scored >=20) m_pSoundDevice->Play(&m_Anc_Cheer[5]);
				else if(m_pGame->m_LocalBoard.m_Scored >=16) m_pSoundDevice->Play(&m_Anc_Cheer[4]);
				else if(m_pGame->m_LocalBoard.m_Scored >=13) m_pSoundDevice->Play(&m_Anc_Cheer[3]);
				else if(m_pGame->m_LocalBoard.m_Scored >=10) m_pSoundDevice->Play(&m_Anc_Cheer[2]);
				else if(m_pGame->m_LocalBoard.m_Scored >= 7) m_pSoundDevice->Play(&m_Anc_Cheer[1]);
				else if(m_pGame->m_LocalBoard.m_Scored >= 4) m_pSoundDevice->Play(&m_Anc_Cheer[0]);

			}
		}

		//
		//m_pDevice->Clear_Color(0.005f,0.025f,0.05f,0);
		//m_pDevice->Clear(EBuffer_Color);

		// draw bg
		m_pUtils->SetDefaults();
		m_pUtils->SetTexture(m_iBackGroundTexture);
		//m_pUtils->SetColor(0.5f, 0.2f, 0.15f, 1.0f);
		//m_pUtils->SetColor(0.3f, 0.4f, 0.7f, 1.0f);
		m_pUtils->SetColor(0.7f, 0.4f, 0.3f, 1.0f);
		//m_pUtils->SetColor(0.3f, 0.6f, 0.4f, 1.0f);

		int32 bgsize = 256;
		int32 s = (GetTickCount()/(GetTickFrequency()/10000))%(1000*bgsize);
		fp32 xstart = -s/1000.0f;
		fp32 ystart = s/1000.0f*2;
		m_pUtils->Quad_Begin();
		for(fp32 y = ystart-bgsize*2; y < g_ScreenHeight; y += bgsize)
			for(fp32 x = xstart-bgsize*2; x < g_ScreenWidth; x += bgsize)
				m_pUtils->Quad_DrawTL(x,y,bgsize,bgsize);
		m_pUtils->Quad_End();

		//
		m_pUtils->SetDefaults();
		m_pUtils->Text_Begin(&m_SmallFont);
		m_pUtils->Text_Draw(10, 10, 0, "NetworkClient: %s", m_pClient->GetStatusString());

		// draw GUI
		m_pUtils->SetDefaults();
		m_GUI.Refresh();
		m_GUI.Render();
		m_pUtils->SetDefaults();
		m_pUtils->SetClipping(false);

		// draw game
		if(m_pGame->IsActive())
		{
			CView *apBoards[4] = {	m_GUI.GetRoot()->GetChild("localboard"),
									m_GUI.GetRoot()->GetChild("board1"),
									m_GUI.GetRoot()->GetChild("board2"),
									m_GUI.GetRoot()->GetChild("board3")
								};

			CLabel *apPlayerBox[4] ={(CLabel*)m_GUI.GetRoot()->GetChild("playername1"),
									(CLabel*)m_GUI.GetRoot()->GetChild("playername2"),
									(CLabel*)m_GUI.GetRoot()->GetChild("playername3"),
									(CLabel*)m_GUI.GetRoot()->GetChild("playername4")
									};

			CLabel *pBlockCount = (CLabel*)m_GUI.GetRoot()->GetChild("blockcount");
			CView *pNextBlock = m_GUI.GetRoot()->GetChild("nextblocks");

			fp32 aScales[4] = {32,16,16,16};
			int32 LocalID = m_pClient->ClientID();
			int32 aID[4] = {LocalID, -1, -1, -1};
			//CString aNames[4] = {m_pChat->m_lUsers.Find(LocalID, ""), "", "", ""};

			for(int32 i = 0; i < 4; i++)
			{
				if(apPlayerBox[i])
					apPlayerBox[i]->SetText("");
				if(apBoards[i])
					apBoards[i]->SetFlags(CView::EFlag_Hidden);
			}

			int32 c=1;
			for(int32 i = 0; i < m_pChat->m_lUsers.Len(); i++)
			{
				int32 id = m_pChat->m_lUsers.GetKey(i);
				if(id != LocalID)
				{
					if(c < 4)
					{
						if(apPlayerBox[c] != NULL)
							apPlayerBox[c]->SetText(m_pChat->m_lUsers.GetValue(i));
						if(apBoards[c])
							apBoards[c]->ClearFlags(CView::EFlag_Hidden);
						aID[c++] = id;
					}
				}
				else
				{
					if(apPlayerBox[0] != NULL)
						apPlayerBox[0]->SetText(m_pChat->m_lUsers.GetValue(i));
					if(apBoards[0])
						apBoards[0]->ClearFlags(CView::EFlag_Hidden);
				}

			}
	

			m_pUtils->SetTexture(m_iBlobTexture);
			m_pUtils->SetBlend(C2DUtils::EBlend_Alpha);

			// render boards
			for(int32 i = 0; i < c; i++)
			{
				if(apBoards[i] == NULL)
					continue;


				fp32 x = apBoards[i]->GetPosition().x;
				fp32 y = apBoards[i]->GetPosition().y;
				fp32 scale = aScales[i];
				
				m_pUtils->SetClipping(true, x, y, CBoard::EWidth*scale, CBoard::EHeight*scale);
				m_pUtils->Quad_Begin();

				if(i == 0)
				{
					RenderBoard(&m_pGame->m_LocalBoard, x, y, scale, m_pGame->m_LocalBoard.m_IntertickTime);

					//
					// render current blocks
					if(m_pGame->m_BoardState == CNetworkGameClient::EState_Placing && apBoards[0] != NULL)
					{
						CBoard &rLocalBoard = m_pGame->m_LocalBoard;
						//fp32 BaseX = apBoards[0]->GetPosition().x;
						//fp32 BaseY = apBoards[0]->GetPosition().y;
						//fp32 Scale = 32;

						//m_pUtils->SetClipping(true, x, y, CBoard::EWidth*scale, CBoard::EHeight*scale);
						//m_pUtils->Quad_Begin();


						fp32 xpos = m_pGame->m_XPos;
						fp32 ypos = m_pGame->m_YPos;
						fp32 yoffset = -scale*(1-m_pGame->m_LocalBoard.m_IntertickTime);
						int32 rot = m_pGame->m_Rotation;

						// draw halo
						m_pUtils->SetColor(1,1,1,0.75f);
						m_pUtils->SetTexCoord(3/8.0f,4/8.0f, 3/8.0f+1/8.0f, 4/8.0f+1/8.0f);
						fp32 hx = x+(xpos+m_aDirectionOffsets[0][rot][0])*scale+scale/2;
						fp32 hy = y+(ypos+m_aDirectionOffsets[0][rot][1])*scale+scale/2+yoffset;
						hx = (fp32)((int32)hx);
						hy = (fp32)((int32)hy);
						m_pUtils->Quad_Draw(hx, hy, scale*1.4f, scale*1.4f);

						// Draw BALLS!
						for(int32 i = 0; i < 2; i++)
						{
							fp32 fx = x+(xpos+m_aDirectionOffsets[i][rot][0])*scale;
							fp32 fy = y+(ypos+m_aDirectionOffsets[i][rot][1])*scale+yoffset;
							RenderBlock(fx, fy, 0, rLocalBoard.m_aCurrentBlocks[i], scale);
						}
						//m_pUtils->Quad_End();
					}

				}
				else
				{
					CClientBoard *pBoard = m_pGame->m_mOpponentBoards.Find(aID[i], NULL);
					if(pBoard)
						RenderBoard(pBoard, x, y, scale, pBoard->m_IntertickTime);
				}
				m_pUtils->Quad_End();


			}

			m_pUtils->SetClipping(false);

			// render next blocks
			if(pNextBlock)
			{
				m_pUtils->Quad_Begin();
				int32 x = pNextBlock->GetPosition().x;
				int32 y = pNextBlock->GetPosition().y;

				RenderBlock(x, y, 0, m_pGame->m_LocalBoard.m_aNextBlocks[0], aScales[0]);
				RenderBlock(x+aScales[0], y, 0, m_pGame->m_LocalBoard.m_aNextBlocks[1], aScales[0]);
				m_pUtils->Quad_End();
			}


			m_pUtils->SetTexture(m_iTextsTexture);
			m_pUtils->SetBlend(C2DUtils::EBlend_Alpha);
			m_pUtils->Quad_Begin();

			// render/update texts
			int64 Tick = GetTickCount();
			fp32 BaseX = apBoards[0]->GetPosition().x;
			fp32 BaseY = apBoards[0]->GetPosition().y;
			for(int32 t = 0; t < m_lTexts.Len(); t++)
			{
				CSpecialText Text = m_lTexts[t];
				if(Tick > (Text.m_StartTick+Text.m_Duration))
				{
					m_lTexts.RemoveByIndex(t);
					t--;
					continue;
				}

				if(Text.m_Type < 0 || Text.m_Type > m_lRects.Len())
					continue;

				CRect Rect = m_lRects[Text.m_Type];
				fp32 AnimateTime = (Tick-Text.m_StartTick)/(fp32)Text.m_Duration;
				fp32 ScaleMod = (Text.m_Scale+AnimateTime*Text.m_ScaleSpeed);

				m_pUtils->SetColor(1,1,1,(1-pow(AnimateTime,7)));
				m_pUtils->SetTexCoord(Rect.x/512.0f, Rect.y/512.0f, Rect.x/512.0f+Rect.w/512.0f, Rect.y/512.0f+Rect.h/512.0f);
				m_pUtils->Quad_SetRotation(Text.m_Rot+Text.m_RotSpeed*AnimateTime);
				m_pUtils->Quad_Draw(BaseX+Text.x, BaseY+Text.y, Rect.w*ScaleMod, Rect.h*ScaleMod);

			}
			m_pUtils->Quad_End();



			// set block count
			if(pBlockCount)
			{
				if(m_pGame->m_LocalBoard.m_NumStones > 0)
					pBlockCount->SetText(CString("<< %d", m_pGame->m_LocalBoard.m_NumStones));
				else if(m_pGame->m_LocalBoard.m_SendStones > 0)
					pBlockCount->SetText(CString("%d >>", m_pGame->m_LocalBoard.m_SendStones));
				else
					pBlockCount->SetText("");
			}
			
			
	

			//
			if(	m_pGame->m_GameState == CNetworkGameClient::EGameState_Win ||
				m_pGame->m_GameState == CNetworkGameClient::EGameState_Lose && apBoards[0] != NULL)
			{
				m_pUtils->SetDefaults();
				m_pUtils->SetBlend(C2DUtils::EBlend_Alpha);
				fp32 BaseX = apBoards[0]->GetPosition().x;
				fp32 BaseY = apBoards[0]->GetPosition().y;
				fp32 SizeX = 32*5;
				fp32 SizeY = 128*(32*5/256.0f);

				m_pUtils->SetColor(0,0,0,0.25f);
				m_pUtils->Quad_Begin();
				m_pUtils->Quad_DrawTL(BaseX, BaseY, 32*6, 32*12);
				m_pUtils->Quad_End();

				if(m_pGame->m_GameState == CNetworkGameClient::EGameState_Win)
					m_pUtils->SetTexture(m_iYouWinTexture);
				else
					m_pUtils->SetTexture(m_iYouLoseTexture);

				m_pUtils->SetColor(1,1,1,1);
				m_pUtils->Quad_Begin();
				m_pUtils->Quad_Draw(BaseX+32*6/2, BaseY+32*12/2, SizeX, SizeY);
				m_pUtils->Quad_End();

				if(m_pGame->m_GameState == CNetworkGameClient::EGameState_Lose)
				{
					int32 Frame = (GetTickCount()/(GetTickFrequency()/10))%4;
					m_pUtils->SetTexture(m_iTeeTexture);
					m_pUtils->Quad_Begin();
					fp32 Start = Frame/4.0f;
					m_pUtils->SetTexCoord(Start, 0, Start+1/4.0f, 1/4.0f);
					m_pUtils->Quad_Draw(BaseX+32*6/2, BaseY+32*12-64, 64, 64);
					m_pUtils->Quad_End();

				}
			}


		}
		
		/*
		m_pUtils->Text_Begin(&m_SmallFont);
		m_pUtils->Text_Draw(5, 5, C2DUtils::ETextDraw_TL, "%10d  %10d  %10d", g_DebugVar/10000, g_DebugVar, g_DebugVar2);
		m_pUtils->Text_End();
		*/

		m_GUI.RenderCursor();

		Assert(m_pDevice->SwapBuffers());
	};
};

/*
CLock g_TestLock;

void ThreadTest(void *)
{
	g_TestLock.Aquire();
	while(1);
}
*/


//
int32 FooMain(int8 *cmdline)
{
	/*
	g_TestLock.Aquire();
	CreateThread(ThreadTest, NULL);
	g_TestLock.Release();
	g_TestLock.Aquire();
	*/
	

	CPuyoGame Game;
	Game.Init();

	int64 StartTime = GetTickCount();
	bool Run = true;
	while(Run)
	{
		Run = NCore::UpdateAndPump();
		Game.Update();
		Assert(NMemoryManager::CheckMemory(true));

		// nasty framerate limiter
		if(g_FrameLimit > 0)
		{
			while((GetTickCount()-StartTime)/(fp64)GetTickFrequency() < (1/((fp32)g_FrameLimit*1.2f)))
			{
				//Run = NCore::UpdateAndPump();
				Sleep(1);
			}
			while((GetTickCount()-StartTime)/(fp64)GetTickFrequency() < (1/(fp32)g_FrameLimit))
				;//Run = NCore::UpdateAndPump();
		}
		StartTime = GetTickCount();
	}

	return 0;
}
