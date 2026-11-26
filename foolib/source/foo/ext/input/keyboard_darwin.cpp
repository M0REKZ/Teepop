#ifdef FOO_PLATFORM_DARWIN
	#include <foo/core/messages.h>
	#include <foo/core/core_darwin.h>
	#include "keyboard.h"
	using namespace NFoo;
	using namespace NFoo::NCore;

	class CKeyboard_Darwin : public CKeyboard
	{
		char SDLScancodeToAscii(SDL_Scancode key)
		{
			if(key >= 128)
				return 0;
			if(key >= SDL_SCANCODE_A && key <= SDL_SCANCODE_Z)
			{
				bool caps_lock = ((SDL_GetModState() & SDL_KMOD_CAPS) | (SDL_GetModState() & SDL_KMOD_SHIFT)) != 0;
				return caps_lock ? (key - SDL_SCANCODE_A) + 'A' : (key - SDL_SCANCODE_A) + 'a';
			}
			else if(key >= SDL_SCANCODE_1 && key <= SDL_SCANCODE_0)
			{
				if(key == SDL_SCANCODE_0)
					return '0';
				return (key - SDL_SCANCODE_1) + '1';
			}
			else if(key == SDL_SCANCODE_SPACE)
			{
				return ' ';
			}
			else if(key == SDL_SCANCODE_KP_COLON)
			{
				return ':';
			}
			else if(key == SDL_SCANCODE_PERIOD)
			{
				return '.';
			}
			else if(key == SDL_SCANCODE_COMMA)
			{
				return ',';
			}
			else if(key == SDL_SCANCODE_SEMICOLON)
			{
				return ';';
			}
			else
			{
				return 0;
			}
		}

		char SDLScancodeToBtnEvent(SDL_Scancode key)
		{
			if(key >= 128)
				return 0;
			switch (key)
			{
				case SDL_SCANCODE_BACKSLASH:
					return NCore::EBtn_Key_Paragraph;

				case SDL_SCANCODE_BACKSPACE:
					return NCore::EBtn_Key_Backspace;

				case SDL_SCANCODE_DELETE:
					return NCore::EBtn_Key_Delete;

				case SDL_SCANCODE_INSERT:
					return NCore::EBtn_Key_Insert;

				case SDL_SCANCODE_HOME:
					return NCore::EBtn_Key_Home;

				case SDL_SCANCODE_END:
					return NCore::EBtn_Key_End;

				case SDL_SCANCODE_PAGEUP:
					return NCore::EBtn_Key_Page_Up;

				case SDL_SCANCODE_PAGEDOWN:
					return NCore::EBtn_Key_Page_Down;

				case SDL_SCANCODE_TAB:
					return NCore::EBtn_Key_Tab;

				case SDL_SCANCODE_RETURN:
					return NCore::EBtn_Key_Enter;

				case SDL_SCANCODE_ESCAPE:
					return NCore::EBtn_Key_Escape;

				case SDL_SCANCODE_LSHIFT:
				case SDL_SCANCODE_RSHIFT:
					return NCore::EBtn_Key_Shift;

				case SDL_SCANCODE_LCTRL:
				case SDL_SCANCODE_RCTRL:
					return NCore::EBtn_Key_Control;

				case SDL_SCANCODE_LALT:
					return NCore::EBtn_Key_LMenu;

				case SDL_SCANCODE_RALT:
					return NCore::EBtn_Key_RMenu;

				case SDL_SCANCODE_LGUI:
					return NCore::EBtn_Key_LSpecial;

				case SDL_SCANCODE_RGUI:
					return NCore::EBtn_Key_RSpecial;

				case SDL_SCANCODE_LEFT:
					return NCore::EBtn_Key_Left;

				case SDL_SCANCODE_UP:
					return NCore::EBtn_Key_Up;

				case SDL_SCANCODE_RIGHT:
					return NCore::EBtn_Key_Right;

				case SDL_SCANCODE_DOWN:
					return NCore::EBtn_Key_Down;

				case SDL_SCANCODE_F1:
					return NCore::EBtn_Key_F1;

				case SDL_SCANCODE_F2:
					return NCore::EBtn_Key_F2;

				case SDL_SCANCODE_F3:
					return NCore::EBtn_Key_F3;

				case SDL_SCANCODE_F4:
					return NCore::EBtn_Key_F4;

				case SDL_SCANCODE_F5:
					return NCore::EBtn_Key_F5;

				case SDL_SCANCODE_F6:
					return NCore::EBtn_Key_F6;

				case SDL_SCANCODE_F7:
					return NCore::EBtn_Key_F7;

				case SDL_SCANCODE_F8:
					return NCore::EBtn_Key_F8;

				case SDL_SCANCODE_F9:
					return NCore::EBtn_Key_F9;

				case SDL_SCANCODE_F10:
					return NCore::EBtn_Key_F10;

				case SDL_SCANCODE_F11:
					return NCore::EBtn_Key_F11;

				case SDL_SCANCODE_F12:
					return NCore::EBtn_Key_F12;

				default:
					return 0;
			}
		}

	public:
		virtual bool Update()
		{
			NCore::CCore_OSX *pCore = NCore::CCore_OSX::Get();

			for(int32_t i = 0; i < pCore->m_lSDLEvents.Len(); i++)
			{
				SDL_Event & sdlevent = pCore->m_lSDLEvents[i];
				if(sdlevent.type == SDL_EVENT_KEY_DOWN)
				{
					char c = SDLScancodeToAscii(sdlevent.key.scancode);
					if(c)
						PressButton(c, c);
					else
						PressButton(SDLScancodeToBtnEvent(sdlevent.key.scancode), c);
				}
				else if(sdlevent.type == SDL_EVENT_KEY_UP)
				{
					char c = SDLScancodeToAscii(sdlevent.key.scancode);
					if(c)
						ReleaseButton(c, c);
					else
						ReleaseButton(SDLScancodeToBtnEvent(sdlevent.key.scancode), c);
				}
			}
			
			return true;
		}
	};
	MACRO_REGISTER_CLASS(CKeyboard_Darwin, "keyboard")

#endif
