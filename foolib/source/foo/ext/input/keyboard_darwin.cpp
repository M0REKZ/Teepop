#ifdef FOO_PLATFORM_DARWIN
	#ifdef FOO_COMPILER_GCC
		// Carve some shit of Carbon.h
		//#define __CORESERVICES__
		//#define __APPLICATIONSERVICES__
		//#define __HITOOLBOX__
         
		#define __CARBONSOUND__
		#define __OPENSCRIPTING__
		#define __PRINT__
		#define __NAVIGATIONSERVICES__
		#define __COMMONPANELS__
		#define __HTMLRENDERING__
		#define __SPEECHRECOGNITION__
		#define __SECURITYHI__
                #define __INK__
		#define __HELP__
		#define __IMAGECAPTURE__
		
		#include <Carbon/Carbon.h>
	#elif defined(FOO_COMPILER_CW)
		#include <MSL MacHeadersMach-O.h>
	#else
		#include <Carbon/Carbon.h>
	#endif
	
	#include "keyboard.h"
	using namespace NFoo;
	using namespace NFoo::NCore;

	class CKeyboard_Darwin : public CKeyboard
	{
	public:
		virtual bool Update()
		{
			// Keyboard
			EventRecord	event;
			while(WaitNextEvent(keyDownMask|keyUpMask, &event, 0, nil))
			{
				int32 CharKey = event.message & charCodeMask;
				int32 Key = event.message & charCodeMask;
				int32 KeyCode = (event.message & keyCodeMask)>>8;
				//CMessage Message;

				if(Key >= 'a' && Key <= 'z')
					Key -= 32;
				
				// F1 = 122
				// F2 = 120
				// F3 = 99
				// F4 = 118
				// F5 = 96
				
				// should be an translation table
				if(KeyCode == 122) Key = EBtn_Key_F1;
				if(KeyCode == 120) Key = EBtn_Key_F2;
				if(KeyCode == 99) Key = EBtn_Key_F3;
				if(KeyCode == 128) Key = EBtn_Key_F4;
				if(KeyCode == 96) Key = EBtn_Key_F5;
				if(KeyCode == 97) Key = EBtn_Key_F6;
				if(KeyCode == 98) Key = EBtn_Key_F7;
				if(KeyCode == 100) Key = EBtn_Key_F8;
				if(KeyCode == 101) Key = EBtn_Key_F9;
				if(KeyCode == 109) Key = EBtn_Key_F10;
				if(KeyCode == 103) Key = EBtn_Key_F11;
				if(KeyCode == 111) Key = EBtn_Key_F12;
				

				if(event.what == keyDown)
					PressButton(Key, CharKey);
				else
					ReleaseButton(Key, CharKey);
			}
			
			return true;
		}
	};
	MACRO_REGISTER_CLASS(CKeyboard_Darwin, "keyboard")

#endif
