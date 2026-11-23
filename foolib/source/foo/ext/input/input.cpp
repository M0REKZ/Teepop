#include "input.h"

using namespace NFoo;

class CMouse* CInput::m_pMouse = 0;
class CKeyboard* CInput::m_pKeyboard = 0;

namespace { CInput g_Input; } // Singelton

/*
int32 CInput::GetDevices(TDynamicArray<CDevice*> &_rlDevices, const int8 *_pType)
{
	return 0;
}
*/

//
CInput::CInput()
{
	m_pMouse = 0;
	m_pKeyboard = 0;
}

//
bool CInput::InitMouse()
{
	if (!m_pMouse)
		m_pMouse = (CMouse*)NClassRegistry::CreateByTag("mouse");

	return true;
}

//
bool CInput::InitKeyboard()
{
	if (!m_pKeyboard)
		m_pKeyboard = (CKeyboard*)NClassRegistry::CreateByTag("keyboard");

	return true;
}

