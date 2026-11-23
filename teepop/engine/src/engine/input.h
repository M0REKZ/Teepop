#ifndef FOO_FILE_ENGINE_INPUT_H
#define FOO_FILE_ENGINE_INPUT_H

#include <foo/ext/input/input.h>
#include "engine.h"

namespace NEngine
{
	//
	//
	//
	class CKeyboardInput : public CModule
	{
	protected:
		virtual bool Init();
	public:
		CKeyboard *GetKeyboard() { return CInput::Keyboard(); }
	};

	//
	//
	//
	class CMouseInput : public CModule
	{
	protected:
		virtual bool Init();
	public:
		CMouse *GetMouse() { return CInput::Mouse(); }
	};
}

#endif // FOO_FILE_ENGINE_INPUT_H
