#ifdef FOO_PLATFORM_WIN32

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h> // oh crap
//#include <dinput.h>

#include <foo/base/log.h>
#include <foo/base/string.h>
#include <foo/func/filestream.h>

#include "core.h"
#include "core_win32.h"

using namespace NFoo;
using namespace NFoo::NCore;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
class CKeyboard_Win32 : public CKeyboard
{
	friend class CCore_Win32;
public:
};
MACRO_REGISTER_CLASS(CKeyboard_Win32, aKeyboardClassTag)

// -----------------------------------------------------------------------

class CMouse_DirectInput8 : public CMouse
{
	friend class CCore_Win32;

	// Direct Input
	DIMOUSESTATE			m_MouseState;
	LPDIRECTINPUT8			m_DI;
	LPDIRECTINPUTDEVICE8	m_DevMouse;

public:
	~CMouse_DirectInput8() { Shutdown(); }

	bool Init();
	bool Shutdown();
	bool Poll();
};
MACRO_REGISTER_CLASS(CMouse_DirectInput8, aMouseClassTag)

//
bool CMouse_DirectInput8::Init()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
//	HWND hWnd			= GetDesktopWindow();

	// Create the DI object
	if (FAILED(DirectInput8Create(	hInstance,
									DIRECTINPUT_VERSION,
									IID_IDirectInput8,
									(void **)&m_DI,
									NULL)))
		return false;

	if (FAILED(m_DI->CreateDevice(GUID_SysMouse,&m_DevMouse,NULL)))
		return false;

	if (FAILED(m_DevMouse->SetDataFormat(&c_dfDIMouse)))
		return false;

//	if (FAILED(m_DevMouse->SetCooperativeLevel(hWnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
//		return false;

	return true;
}

//
bool CMouse_DirectInput8::Shutdown()
{
	if (FAILED(m_DevMouse->Release()))
		return false;

	if (FAILED(m_DI->Release()))
		return false;

	return true;
}

//
bool CMouse_DirectInput8::Poll()
{
	if (FAILED(m_DevMouse->GetDeviceState(sizeof (DIMOUSESTATE),&m_MouseState)))
	{
		if (FAILED(m_DevMouse->Acquire()))
			return false;

		if (FAILED(m_DevMouse->GetDeviceState(sizeof (DIMOUSESTATE),&m_MouseState)))
			return false;
	}

	// mouse deltas
	m_pAxis[0] = m_MouseState.lX;
	m_pAxis[1] = m_MouseState.lY;

	return true;
}

// -----------------------------------------------------------------------
*/
void CCore_Win32::Init()
{
	/*
	// keyboard
	m_pWin32Keyboard = (CKeyboard_Win32 *)NClassRegistry::CreateByName("CKeyboard_Win32");
	m_lKeyboard.Add(m_pWin32Keyboard);
	m_lSubCore.Add(m_pWin32Keyboard);

	// mouse
	m_pWin32Mouse = new CMouse_DirectInput8;
	m_pWin32Mouse->m_pClassHandle = NClassRegistry::FindHandle("CMouse_DirectInput8");
	m_pWin32Mouse->Init();	// FIXME: check return value (true/false)
	m_lMouse.Add(m_pWin32Mouse);		
	m_lSubCore.Add(m_pWin32Mouse);*/
}

bool CCore_Win32::Update()
{
	MSG WinMsg;
	while(PeekMessage(&WinMsg, 0, 0, 0, PM_REMOVE))
	{
		if(WinMsg.message == WM_QUIT)
			AddMessage(CMessage(EMsg_Quit));
		
		m_lWinMessages.Add(WinMsg);

		TranslateMessage(&WinMsg);
		DispatchMessage(&WinMsg);

		/*
		// keyboard
		if(WinMsg.message == WM_KEYDOWN && !(WinMsg.lParam&(1<<30)))
		{
			uint8 aState[256];
			uint16 Char = 0;
			GetKeyboardState(aState);
			ToAscii(WinMsg.wParam, (WinMsg.lParam>>16)&0xFF, aState, &Char, 0);
			//m_pWin32Keyboard->PressButton(WinMsg.wParam, Char);
		}
		else if(WinMsg.message == WM_KEYUP)
		{
			uint8 aState[256];
			uint16 Char = 0;
			GetKeyboardState(aState);
			ToAscii(WinMsg.wParam, (WinMsg.lParam>>16)&0xFF, aState, &Char, 0);
			//m_pWin32Keyboard->ReleaseButton(WinMsg.wParam, Char);
		}

		// mouse
		/*
		if(WinMsg.message == WM_LBUTTONDOWN)
			m_pWin32Mouse->PressButton(EBtn_Mouse_Left);
		else if(WinMsg.message == WM_LBUTTONUP)
			m_pWin32Mouse->ReleaseButton(EBtn_Mouse_Left);

		else if(WinMsg.message == WM_RBUTTONDOWN)
			m_pWin32Mouse->PressButton(EBtn_Mouse_Right);
		else if(WinMsg.message == WM_RBUTTONUP)
			m_pWin32Mouse->ReleaseButton(EBtn_Mouse_Right);

		else if(WinMsg.message == WM_MBUTTONDOWN)
			m_pWin32Mouse->PressButton(EBtn_Mouse_Middle);
		else if(WinMsg.message == WM_MBUTTONUP)
			m_pWin32Mouse->ReleaseButton(EBtn_Mouse_Middle);
			*/

	}

	//m_pWin32Mouse->Poll();

	bool RetVal = CCore::Update();
	m_lWinMessages.Clear();

	return RetVal;
}


//
// file logger
//
class CFileLogger : public NLog::CLogger
{
public:
	CFileWriter m_File;
	CFileLogger()
	{
		m_File.Open("corelog.txt");
		NLog::Attach(this);
		NLog::Write("CFileLogger attached\n");
	}

	~CFileLogger()
	{
		NLog::Write("CFileLogger deattching\n");
		NLog::Detach(this);
		m_File.Close();
	}

	virtual void Write(const int8 *_pBuffer)
	{
		m_File.Write(_pBuffer, StringLength(_pBuffer));
		m_File.Flush();
	}
};

//
// basic logger
//
class CDebugStringLogger : public NLog::CLogger
{
public:
	bool m_NoDetach;
	CDebugStringLogger(bool _NoDetach=false)
	{
		m_NoDetach = _NoDetach;
		NLog::Attach(this); NLog::Write("CDebugStringLogger attached\n");
	}
	
	~CDebugStringLogger()
	{
		if(!m_NoDetach)
		{
			NLog::Write("CDebugStringLogger detching\n");
			NLog::Detach(this);
		}
	}
	virtual void Write(const int8 *_pBuffer) { OutputDebugStringA(_pBuffer); }
};

int32_t Main()
{
	// Basic logger
	static CDebugStringLogger Debugger(true);
	CFileLogger FileLogger;

	// Create core
	InitCore(new CCore_Win32());

	CString CmdLine = GetCommandLine();

	// Call main
	//NLog::WriteF("cmdline: %s\n", lpCmdLine);
	NLog::WriteF("cmdline: %s\n", CmdLine.GetData());

	
	int32_t r;
	
	if(CmdLine.Find("catch_errors"))
	{
		NLog::WriteF("Exception catching enabled\n");
		try
		{
			r = FooMain(GetCommandLine());
		}
		catch(const char *pMsg)
		{
			MessageBox(NULL, pMsg, "Error", MB_OK);
			NLog::WriteF("Exception: %s\n", pMsg);
		}
		catch(...)
		{
			MessageBox(NULL, "Unknown Error", "Error", MB_OK);
			NLog::WriteF("Exception: Unknown\n");
		}
	}
	else
	{
		r = FooMain(GetCommandLine());
	}

	SafeDelete(g_pCore);
	NClassRegistry::Clear();

	return r;
}

int main()
{
	return Main();
}

int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)
{
	return Main();
} 
#endif
