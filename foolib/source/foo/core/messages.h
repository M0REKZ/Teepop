#ifndef __FOO_MESSAGES_H__
#define __FOO_MESSAGES_H__

namespace NFoo { namespace NCore {

	enum
	{
		EMsg_Empty = 0,
		EMsg_Quit = 0,
		
		EMsg_ButtonDown=0x10,
		EMsg_ButtonPressed=0x11,
		EMsg_ButtonUp=0x12,

		EMsg_MouseMove=0x20,
		EMsg_MouseBtnDown=0x21,
		EMsg_MouseBtnUp=0x22,

		EMsg_App=0x8000
	};

	enum
	{
		EBtn_Key_Delete=0x2E,
		EBtn_Key_Insert=0x2F,
		EBtn_Key_Home=0x30,
		EBtn_Key_End=0x31,
		EBtn_Key_Page_Up=0x32,
		EBtn_Key_Page_Down=0x33,
		
		EBtn_Key_Backspace=0x08,
		EBtn_Key_Tab=0x09,
		EBtn_Key_Enter=0x0D,
		EBtn_Key_Shift=0x10,
		EBtn_Key_Control=0x11,
		EBtn_Key_Escape=0x1B,
		EBtn_Key_Space=0x20,

		EBtn_Key_LShift=0xA0,
		EBtn_Key_RShift=0xA1,
		EBtn_Key_LControl=0xA0,
		EBtn_Key_RControl=0xA2,
		EBtn_Key_LMenu=0xA4,
		EBtn_Key_RMenu=0xA5,

		EBtn_Key_LSpecial=0x5B,
		EBtn_Key_RSpecial=0x5C,

		EBtn_Key_Left=0x25,
		EBtn_Key_Up=0x26,
		EBtn_Key_Right=0x27,
		EBtn_Key_Down=0x28,

		EBtn_Key_Paragraph=0xDC,

		EBtn_Key_F1=0x70,
		EBtn_Key_F2=0x71,
		EBtn_Key_F3=0x72,
		EBtn_Key_F4=0x73,
		EBtn_Key_F5=0x74,
		EBtn_Key_F6=0x75,
		EBtn_Key_F7=0x76,
		EBtn_Key_F8=0x77,
		EBtn_Key_F9=0x78,
		EBtn_Key_F10=0x79,
		EBtn_Key_F11=0x7A,
		EBtn_Key_F12=0x7B,

		EBtn_Mouse_Left = 0x0,
		EBtn_Mouse_Right,
		EBtn_Mouse_Middle
	};

} }

#endif // __FOO_MESSAGES_H__
