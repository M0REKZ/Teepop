#include "view.h"

namespace NFoo {
namespace NGUI {

	//
	// a checkbox
	//
	class CCheckBox : public CView
	{
		bool m_Checked;
	protected:
		virtual void OnMouseDown(CPoint _Pos, int32_t _iButton);
		virtual void OnDraw(CDrawer &_rDrawer);
	public:
		CCheckBox(CView *pParent=NULL, const int8 *pName=NULL, CGUI *pGUI=NULL);
	};
}
}
