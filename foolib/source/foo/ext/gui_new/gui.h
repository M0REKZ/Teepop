#ifndef FOO_EXT_GUI_H
#define FOO_EXT_GUI_H

#include <foo/func/smartptr.h>
#include <foo/func/message.h>
#include "shared.h"
#include "drawer.h"

namespace NFoo {
namespace NGUI {

	class CView;

	//
	//
	//
	class CGUI
	{
		friend class CView;

		CPoint m_MouseLastPosition;
		CPoint m_MousePosition;
		CPoint m_MouseLastClickPosition;
		int64 m_MouseLastClickTime;

		CView *m_pFocusedView;
		CView *m_pMouseCaptureView;
		CView *m_pRoot;

		CView *m_pMouseOverView;

		bool m_MouseMoved;

		bool m_KeyPressed;
		int32_t m_KeyStroke;
		int8 m_KeyAsciiCode;
		int64 m_KeyRepeatTime;

		void MouseBoundCheck();

		class CMapping
		{
		public:
			CString m_Name;
			CMessageTarget m_Target;
		};

		TDynamicArray<CMapping*> m_lMappings;

	public:
		CGUI();
		~CGUI();

		CDrawer m_Drawer;

		//
		CView *CreateFromRegistry(class CRegistry *pReg);

		//
		void AddMapping(const int8 *pName, CMessageReceiver *pReceiver, CMessage &rMsg);
		void AddMapping(const int8 *pName, CMessageReceiver *pReceiver, int32_t iMsg);
		CMessageTarget GetMapping(const int8 *pName);
		
		//
		void RenderCursor();
		void Render();
		void Refresh();

		// root
		CView *GetRoot();
		void SetRoot(CView *_pView);

		void Select(CView *_pView);

		// Mouse functions
		void CaptureMouse(CView *_pView);
		void ReleaseMouse();

		// Mouse
		void MouseMoveAbs(int32_t x, int32_t y);
		void MouseMoveRel(int32_t x, int32_t y);

		void MouseDown(int32_t _Button);
		void MouseUp(int32_t _Button);

		// Keyboard
		void KeyDown(int32_t _Stroke, uint8 _AsciiCode, bool Repeat=false);
		void KeyUp(int32_t _Stroke, uint8 _AsciiCode);
	};
}
}

#endif // FOO_EXT_GUI_H
