//==========================================================================================
// Copyright (c), Clas M. Auvinen 2002
//==========================================================================================

#ifdef FOO_PLATFORM_DARWIN

#include <unistd.h>
#include <foo/base/log.h>
#include <foo/base/string.h>

#include "core.h"
#include "core_darwin.h"

// WaitNextEvent(

bool g_QuitRequest = false;

using namespace NFoo;
using namespace NFoo::NCore;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define	rMenuBar	128	// menu bar
#define	mApple		128	// Apple menu
#define	iAbout		1
#define	mFile		129	// File menu
#define	iNew		1
#define	iClose		4
#define	iQuitSeparator	10
#define	iQuit		11
#define	mEdit		130	// Edit menu
#define kAboutBox	200	// Dialog resource for About box
#define kSleepTime	32767
/*
class CKeyboard_OSX : public CKeyboard
{
public:
	virtual bool Update()
	{
		// Keyboard
		EventRecord	event;
		while(WaitNextEvent(keyDownMask|keyUpMask, &event, 0, nil))
		{
			int32_t CharKey = event.message & charCodeMask;
			int32_t Key = event.message & charCodeMask;
			int32_t KeyCode = (event.message & keyCodeMask)>>8;
			//CMessage Message;

			if(Key >= 'a' && Key <= 'z')
				Key -= 32;
			
			// F1 = 122
			// F2 = 120
			// F3 = 99
			// F4 = 118
			// F5 = 96
			
			// should be an translation table
			if(KeyCode == 122) Key = EBtn_Key_F1;
			if(KeyCode == 120) Key = EBtn_Key_F2;
			if(KeyCode == 99) Key = EBtn_Key_F3;
			if(KeyCode == 128) Key = EBtn_Key_F4;
			if(KeyCode == 96) Key = EBtn_Key_F5;
			if(KeyCode == 97) Key = EBtn_Key_F6;
			if(KeyCode == 98) Key = EBtn_Key_F7;
			if(KeyCode == 100) Key = EBtn_Key_F8;
			if(KeyCode == 101) Key = EBtn_Key_F9;
			if(KeyCode == 109) Key = EBtn_Key_F10;
			if(KeyCode == 103) Key = EBtn_Key_F11;
			if(KeyCode == 111) Key = EBtn_Key_F12;
			

			if(event.what == keyDown)
				PressButton(Key);
			else
				ReleaseButton(Key);

			//AddMessage(Message);
		}
		
		return true;
	}
};
MACRO_REGISTER_CLASS(CKeyboard_OSX, aKeyboardClassTag)
//*/

void CCore_OSX::Init()
{
	//CKeyboard_OSX *keyboard = new CKeyboard_OSX;
	//keyboard->m_pClassHandle = NClassRegistry::FindHandle("CKeyboard_OSX");
	//m_lKeyboard.Add(keyboard);
}

bool CCore_OSX::Update()
{
	SDL_Event sdlevent;
	while(SDL_PollEvent(&sdlevent))
	{
	    m_lSDLEvents.Add(sdlevent);
	}

	//detect quit event
	for(int32_t i = 0; i < m_lSDLEvents.Len(); i++)
	{
		if(m_lSDLEvents[i].type == SDL_EVENT_QUIT)
		{
			g_QuitRequest = true;
		}
	}

	if(g_QuitRequest)
	{
		g_QuitRequest = false;
		AddMessage(CMessage(EMsg_Quit));
	}

	bool core_res = CCore::Update();
	m_lSDLEvents.Clear();
	return core_res;
}

//
// basic logger
//
class CStdOutLogger : public NLog::CLogger
{
public:
	CStdOutLogger() { NLog::Attach(this); NLog::Write("CStdOutLogger attached\n"); }
	virtual ~CStdOutLogger() { NLog::Write("CStdOutLogger deattching\n"); NLog::Detach(this); fflush(stdout); }
	virtual void Write(const char *_pBuffer) { fprintf(stdout, "%s", _pBuffer); }
};

//
// Main
//
int main(int argc, char* argv[])
{
	CStdOutLogger Logger;
	
	//
	// Create The Core
	//
	InitCore(new CCore_OSX());
	//new CKeyboard_OSX;

	NLog::Write("MAIN:Initing MacOSX\n");
	
	// FIXME
	char aDir[512];
	getwd(aDir);
	NLog::WriteF("MAIN: Working directory is %s\n", aDir);
	
	if(StringLength(aDir) < 2)
	{
		for(int32_t i = strlen(argv[0])-1; i > 0; i--)
			if(argv[0][i] == '/')
			{
				argv[0][i] = 0;
				break;
			}
			
		// change dir
		NLog::WriteF("MAIN: Chaning to %s\n", argv[0]);
		chdir(argv[0]);
	}

	//
	// Init
	//
	{
		//+KZ: This probably only existed in old Carbon?
		//InitCursor();
	}

	NLog::Write("MAIN:Calling FooMain()\n");
	FooMain("Empty");

	SafeDelete(g_pCore);
	NMemoryManager::DumpMemInfo();

	return 1;
}

#endif

