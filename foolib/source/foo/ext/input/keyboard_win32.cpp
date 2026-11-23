#ifdef FOO_PLATFORM_WIN32
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#include <windows.h> // oh crap
	#include <foo/core/core_win32.h>
	#include <foo/core/messages.h>
	#include "input.h"
	#include "keyboard.h"
	
	using namespace NFoo;

	class CKeyboard_Win32 : public CKeyboard
	{
	public:
		virtual bool Update()
		{
			NCore::CCore_Win32 *pCore = NCore::CCore_Win32::Get();

			for(int32 i = 0; i < pCore->m_lWinMessages.Len(); i++)
			{
				MSG WinMsg = pCore->m_lWinMessages[i];

				// keyboard
				if(WinMsg.message == WM_KEYDOWN && !(WinMsg.lParam&(1<<30)))
				{
					uint8 aState[256];
					uint16 Char = 0;
					GetKeyboardState(aState);
					ToAscii(WinMsg.wParam, (WinMsg.lParam>>16)&0xFF, aState, &Char, 0);
					PressButton(WinMsg.wParam, Char);
				}
				else if(WinMsg.message == WM_KEYUP)
				{
					uint8 aState[256];
					uint16 Char = 0;
					GetKeyboardState(aState);
					ToAscii(WinMsg.wParam, (WinMsg.lParam>>16)&0xFF, aState, &Char, 0);
					ReleaseButton(WinMsg.wParam, Char);
				}
			}
			return true;
		}
	};
	MACRO_REGISTER_CLASS(CKeyboard_Win32, "keyboard")

	//
	/*
	class CMouse_Win32 : public CMouse
	{
	public:
		POINT m_PrevPos;
		virtual bool Update()
		{
			NCore::CCore_Win32 *pCore = NCore::CCore_Win32::Get();
			POINT Pos;
			GetCursorPos(&Pos);
			
			m_pAxis[0] += Pos.x-m_PrevPos.x;
			m_pAxis[1] += Pos.y-m_PrevPos.y;
			m_PrevPos = Pos;

			for(int32 i = 0; i < pCore->m_lWinMessages.Len(); i++)
			{
				MSG WinMsg = pCore->m_lWinMessages[i];

				// keyboard
				if(WinMsg.message == WM_LBUTTONDOWN)
					PressButton(NCore::EBtn_Mouse_Left);
				else if(WinMsg.message == WM_LBUTTONUP)
					ReleaseButton(NCore::EBtn_Mouse_Left);

				else if(WinMsg.message == WM_RBUTTONDOWN)
					PressButton(NCore::EBtn_Mouse_Right);
				else if(WinMsg.message == WM_RBUTTONUP)
					ReleaseButton(NCore::EBtn_Mouse_Right);

				else if(WinMsg.message == WM_MBUTTONDOWN)
					PressButton(NCore::EBtn_Mouse_Middle);
				else if(WinMsg.message == WM_MBUTTONUP)
					ReleaseButton(NCore::EBtn_Mouse_Middle);
					//NLog::WriteF("(%f,%f)\n", m_pAxis[0], m_pAxis[1]);
			}
			
			return true;
		}
	};
	MACRO_REGISTER_CLASS(CMouse_Win32, "mouse")
		*/

	/*
	#define DIRECTINPUT_VERSION 0x0800
	#include <dinput.h>
	class CMouse_DirectInput8 : public CMouse
	{
		friend class CCore_Win32;

		// Direct Input
		DIMOUSESTATE			m_MouseState;
		LPDIRECTINPUT8			m_DI;
		LPDIRECTINPUTDEVICE8	m_DevMouse;

	public:
		~CMouse_DirectInput8() { Shutdown(); }

		CMouse_DirectInput8();
		bool Shutdown();
		bool Update();
	};
	MACRO_REGISTER_CLASS(CMouse_DirectInput8, "mouse")

	//
	CMouse_DirectInput8::CMouse_DirectInput8()
	{
		HINSTANCE hInstance = GetModuleHandle(NULL);
	//	HWND hWnd			= GetDesktopWindow();

		// Create the DI object
		if (FAILED(DirectInput8Create(	hInstance,
										DIRECTINPUT_VERSION,
										IID_IDirectInput8,
										(void **)&m_DI,
										NULL)))
			throw "argh";

		if (FAILED(m_DI->CreateDevice(GUID_SysMouse,&m_DevMouse,NULL)))
			throw "argh";

		if (FAILED(m_DevMouse->SetDataFormat(&c_dfDIMouse)))
			throw "argh";

	//	if (FAILED(m_DevMouse->SetCooperativeLevel(hWnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	//		return false;

		//return true;
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
	bool CMouse_DirectInput8::Update()
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
	}*/

#endif
