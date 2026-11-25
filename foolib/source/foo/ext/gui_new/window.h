#include "view.h"

namespace NFoo {
namespace NGUI {

	class CMenuItem : public CView
	{
		friend class CMenu;
	protected:
		CMenuItem(CView *pParent=NULL, const char *pName=NULL, CGUI *pGUI=NULL) : CView(pParent, pName, pGUI) {};
		virtual void OnDraw(CDrawer &_rDrawer);
	public:
	};

	class CMenu : public CView
	{
	protected:
		TDynamicArray<CMenuItem*> m_lItems;
		CPoint FindDimentions();
	public:
		CMenu(CView *pParent=NULL, const char *pName=NULL, CGUI *pGUI=NULL);
		
		void Activate(CPoint _Pos);
		void Deactivate();

		void AddItem(CMenuItem *_pItem);
	};

	/*
	class CMenuBar : public CView
	{
	protected:
		virtual void OnMouseDown(CPoint _Pos, int32_t _iButton);
		virtual void OnMouseUp(CPoint _Pos, int32_t _iButton);
		virtual void OnDraw(CDrawer &_rDrawer);
	public:
		CMenuBar(class CGUI &_rGUI);
	};
	*/

	//
	// frame
	//
	class CFrame : public CView
	{
	protected:
		virtual void OnDraw(CDrawer &_rDrawer);
	public:
		CFrame(CView *pParent=NULL, const char *pName=NULL, CGUI *pGUI=NULL);
	};

	//
	//
	//
	class CWindow : public CView
	{
		enum
		{
			EHeaderSize = 13
		};

		bool m_Draging;
		bool m_Sizing;
		CPoint m_DragStart;
		CString m_Caption;

	protected:
		virtual void OnMouseDown(CPoint _Pos, int32_t _iButton);
		virtual void OnMouseUp(CPoint _Pos, int32_t _iButton);
		virtual void OnMouseMove(CPoint _Pos);
		virtual void OnDraw(CDrawer &_rDrawer);
	public:
		CWindow(CView *pParent=NULL, const char *pName=NULL, CGUI *pGUI=NULL);
		void SetCaption(const char *_pCap);
	};
}
}
