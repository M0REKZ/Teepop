#ifdef FOO_PLATFORM_DARWIN
	#include "keyboard.h"
	using namespace NFoo;
	using namespace NFoo::NCore;

	class CKeyboard_Darwin : public CKeyboard
	{
	public:
		virtual bool Update()
		{
			//empty for now
			
			return true;
		}
	};
	MACRO_REGISTER_CLASS(CKeyboard_Darwin, "keyboard")

#endif
