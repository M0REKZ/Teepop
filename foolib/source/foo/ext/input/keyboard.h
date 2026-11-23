#ifndef FOO_EXT_INPUT_KEYBOARD_H
#define FOO_EXT_INPUT_KEYBOARD_H

#include "input.h"

namespace NFoo
{
	class CKeyboard : public CInput::CDevice
	{
	protected:
		enum { EMaxKeys=256 };
		CKeyboard();
	public:
	};
}

#endif // FOO_EXT_INPUT_KEYBOARD_H
