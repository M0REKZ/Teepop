#ifndef FOO_EXT_GUI_SCROLLBAR
#define FOO_EXT_GUI_SCROLLBAR

#include "view.h"

namespace NFoo {
namespace NGUI {


	//
	// scroll bar
	//
	class CScrollBar : public CView
	{
		class CHandle : public CView
		{
		protected:
			virtual void OnMouseDown(CPoint _Pos, int32 _iButton);
			virtual void OnMouseUp(CPoint _Pos, int32 _iButton);
			virtual void OnMouseMove(CPoint _Pos);
			virtual void OnDraw(CDrawer &_rDrawer);
		public:	
			int32 m_Amount;
			int32 m_Offset;
			bool m_Scrubbing;

			CHandle(CView *pParent, CGUI *pGUI);
		
			void SetPos(int32 p);
			int32 GetPos();
			int32 GetMin();
			int32 GetMax(int32 py = -1);
		};
		
		CHandle m_Handle;

		int32 ScrollArea(int32 h);
	protected:
		virtual void OnSize(CPoint _OldSize);
		virtual void OnDraw(CDrawer &_rDrawer);
	public:
		CScrollBar(CView *pParent=NULL, const int8 *pName=NULL, CGUI *pGUI=NULL);
		
		void SetAmount(fp32 _a);
		fp32 GetAmount();
	};
}
}

#endif // FOO_EXT_GUI_SCROLLBAR
