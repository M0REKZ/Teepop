#include "input.h"

using namespace NEngine;

//
//
//
bool CKeyboardInput::Init()
{
	CModule::Init();
	CInput::InitKeyboard();
	return true;
}
MACRO_REGISTER_CLASS(CKeyboardInput, "EngineModule")

//
//
//
bool CMouseInput::Init()
{
	CModule::Init();
	CInput::InitMouse();
	return true;
}
MACRO_REGISTER_CLASS(CMouseInput, "EngineModule")
