#include "view.h"

namespace NFoo {
namespace NGUI {

	//
	// a textbox
	//
	class CTextBox : public CView
	{
		CString m_Text;
		int32_t m_ViewStart;
		int32_t m_CursorPos;
		bool m_HideType;
	protected:
		virtual void OnDraw(CDrawer &_rDrawer);
		virtual void OnKeyPress(int32_t _Key, int8 _Char, bool Repeat);
		virtual void OnMouseDown(CPoint _Pos, int32_t _Button);

		void CursorPosCheck();
	public:
		enum
		{
			EEvent_Enter
		};

		CTextBox(CView *pParent=NULL, const int8 *pName=NULL, CGUI *pGUI=NULL);

		void SetHideType(bool hide);
		CString GetText();
		void SetText(CString _s);
		virtual void Set(CString &rSetting, CString &rValue);
	};

	//
	// label
	//
	class CLabel : public CView
	{
		CString m_Text;
	protected:
		virtual void OnDraw(CDrawer &_rDrawer);
	public:
		CLabel(CView *pParent=NULL, const int8 *pName=NULL, CGUI *pGUI=NULL);
		CString GetText();
		void SetText(CString _s);
		virtual void Set(CString &rSetting, CString &rValue);
	};
}
}
