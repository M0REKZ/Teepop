#ifdef FOO_PLATFORM_DARWIN

#include "keyboard.h"

namespace NFoo
{
	class CKeyboard_Darwin : public CKeyboard
	{
	public:
		virtual bool Update()
		{
			return true;
		}
	};

}

using namespace NFoo;
MACRO_REGISTER_CLASS(CKeyboard_Darwin, "keyboard")

#endif
