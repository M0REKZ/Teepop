#include <foo/base/memory.h>
#include <foo/base/log.h>
#include "input.h"
#include "keyboard.h"
#include "mouse.h"

using namespace NFoo;

CInput::CDevice::CDevice(int32_t _NumButtons, int32_t _NumAxis)
{
	m_pAxis = NULL;
	m_pButtonStates = NULL;

	m_NumButtons = _NumButtons;
	m_NumAxis = _NumAxis;

	if(m_NumButtons > 0)
	{
		m_pButtonStates = new uint8_t[m_NumButtons];
		MemorySet(m_pButtonStates, 0, m_NumButtons);
	}

	if(m_NumAxis > 0)
	{
		m_pAxis = new fp32[m_NumAxis];
		MemorySet(m_pAxis, 0, m_NumAxis*sizeof(fp32));
	}
}

CInput::CDevice::~CDevice()
{
	SafeArrayDelete(m_pButtonStates);
	SafeArrayDelete(m_pAxis);
}

void CInput::CDevice::PressButton(int32_t _iButton, char _Char)
{
	m_pButtonStates[_iButton] |= EMask_Pressed;
	if(m_UseEvents)
		m_lEvents.Add(CEvent(_iButton, true, _Char));
}

void CInput::CDevice::ReleaseButton(int32_t _iButton, char _Char)
{
	m_pButtonStates[_iButton] &= ~EMask_Pressed;
	if(m_UseEvents)
		m_lEvents.Add(CEvent(_iButton, false, _Char));
}

CInput::CEvent CInput::CDevice::GetEvent()
{
	if(m_lEvents.Len())
	{
		CEvent e = m_lEvents[0];
		m_lEvents.RemoveByIndex(0);
		return e;
	}

	return CEvent(0, false);
}

CKeyboard::CKeyboard()
: CInput::CDevice(EMaxKeys, 0)
{}

CMouse::CMouse()
: CInput::CDevice(EMaxKeys, ENumAxis), m_Mode(EMode_Capture)
{}


















