#ifndef __FOOEXT_INPUT_MOUSE__
#define __FOOEXT_INPUT_MOUSE__

#include "input.h"

namespace NFoo
{
	class CMouse : public CInput::CDevice
	{
	protected:
		int32 m_Mode;
		enum { EMaxKeys=3, ENumAxis=2 };

		CMouse();

	public:
		enum { EMode_Capture, EMode_Loose };

		virtual void SetMode(int32 iMode) { m_Mode = iMode; }
		int32 MovementX()	{ return (int32)m_pAxis[0]; }
		int32 MovementY()	{ return (int32)m_pAxis[1]; }
	};
}

#endif // __FOOEXT_INPUT_MOUSE__
