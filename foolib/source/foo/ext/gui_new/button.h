#include "view.h"

namespace NFoo {
namespace NGUI {

	//
	// a button
	//
	class CButton : public CView
	{
		bool m_Pressed;
		CString m_Caption;
	protected:
		virtual void OnMouseDown(CPoint _Pos, int32 _iButton);
		virtual void OnMouseUp(CPoint _Pos, int32 _iButton);
		virtual void OnDraw(CDrawer &_rDrawer);
	public:
		enum
		{
			EEvent_Clicked
		};

		CButton(CView *pParent=NULL, const int8 *pName=NULL, CGUI *pGUI=NULL);
		void SetCaption(const int8 *_pCap);
		virtual void Set(CString &rSetting, CString &rValue);
	};
}
}
