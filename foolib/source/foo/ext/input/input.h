#ifndef __FOOEXT_INPUT__
#define __FOOEXT_INPUT__

#include <foo/func/dynamicarray.h>
#include <foo/func/smartptr.h>
#include <foo/core/core.h>

//
//
//
namespace NFoo
{

	//
	class CInput
	{
	public:
		//
		class CEvent
		{
		public:
			CEvent() {}
			CEvent(int32 _Key, bool _Pressed, int8 _Char = 0) { m_Key = _Key; m_Pressed = _Pressed; m_Char = _Char; }
			int32 m_Key;
			int8 m_Char;
			bool m_Pressed;
			//class CInput::CDevice *m_pDevice;
		};
		
		// this should be revised futher
		class CDevice : public NCore::CSubCore
		{
			friend class CInput;

		public:
			//static const int8 ms_aTag[] = "InputDevice";
			// events
			int32	NumEvents()						{ return m_lEvents.Len(); };
			CEvent	GetEvent();
			void	UseEvents(bool _Yes)			{ m_UseEvents = _Yes; };

			int32	GetNumButtons()					{ return m_NumButtons; }
			int32	GetNumAxis()					{ return m_NumAxis; }
			fp32	GetAxis(int32 _iAxel)			{ return m_pAxis[_iAxel]; }
			void	ResetAxis()						{ for(int32 i = 0; i < GetNumAxis(); i++) m_pAxis[i] = 0; }

			bool	IsButtonPressed(int32 _iButton) { return m_pButtonStates[_iButton]&EMask_Pressed; };
			void	PressButton(int32 _iButton, int8 _Char = 0);
			void	ReleaseButton(int32 _iButton, int8 _Char = 0);

		protected:
			// buttons
			enum
			{
				EMask_Pressed = 1
			};

			CDevice(int32 _NumButtons, int32 _NumAxis);
			virtual ~CDevice();

			// events
			TDynamicArray<CEvent> m_lEvents;
			bool m_UseEvents;

			uint8 *m_pButtonStates;
			int32 m_NumButtons;

			fp32 *m_pAxis;
			int32 m_NumAxis;
		};

		CInput();

		//
		static bool InitMouse();
		static bool InitKeyboard();
		static class CMouse *Mouse()				{ return m_pMouse; }
		static class CKeyboard *Keyboard()			{ return m_pKeyboard; }

	private:
		static class CMouse *m_pMouse;
		static class CKeyboard *m_pKeyboard;
	};
}

#endif // __FOOEXT_INPUT__
