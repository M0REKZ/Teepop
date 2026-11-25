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
			virtual void OnMouseDown(CPoint _Pos, int32_t _iButton);
			virtual void OnMouseUp(CPoint _Pos, int32_t _iButton);
			virtual void OnMouseMove(CPoint _Pos);
			virtual void OnDraw(CDrawer &_rDrawer);
		public:	
			int32_t m_Amount;
			int32_t m_Offset;
			bool m_Scrubbing;

			CHandle(CView *pParent, CGUI *pGUI);
		
			void SetPos(int32_t p);
			int32_t GetPos();
			int32_t GetMin();
			int32_t GetMax(int32_t py = -1);
		};
		
		CHandle m_Handle;

		int32_t ScrollArea(int32_t h);
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
