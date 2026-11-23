#ifndef __FOO_VIEW_H__
#define __FOO_VIEW_H__

#include <foo/func/dynamicarray.h>
#include <foo/func/map.h>
#include <foo/func/smartptr.h>
#include <foo/func/message.h>
#include "shared.h"
#include "drawer.h"

namespace NFoo {
namespace NGUI {

	class CGUI;

	//
	//
	//
	class CView : public CMessageSender
	{
		//TMap<int32, TDynamicArray<CSlot*> > m_lTargets;
		TDynamicArray<CView *> m_lChildren;
		CView *m_pParent;
	  
		CString m_Name;

		CPoint m_Position; // relative to parent
		CPoint m_Size; // (10,10)  = 10 width, 10 height

		int32 m_Flags;
		//int32 m_TabId;

		// sizing
		CPoint m_MinSize;
		CPoint m_MaxSize;
		int8 m_Follow;
		friend class CGUI;
		void ParentResized(CPoint _OldSize, CPoint _NewSize);

	protected:
		CGUI *m_pGUI;
		void RenderTree(CDrawer &_rRender, CPoint _Pos);

		//
		// Event handling
		// override these when you make a widget
		//
		virtual void OnSize(CPoint _Size) {}

		virtual void OnFocus() {}
		virtual void OnLostFocus() {}

		virtual void OnDoubleClick(CPoint _Pos, int32 _Button) {}
		virtual void OnMouseDown(CPoint _Pos, int32 _Button) {}
		virtual void OnMouseUp(CPoint _Pos, int32 _Button) {}
		virtual void OnMouseMove(CPoint _Pos) {}
		virtual void OnMouseEnter() {}
		virtual void OnMouseExit() {}

		virtual void OnKeyPress(int32 _Key, int8 _Char, bool Repeat);

		virtual void OnDraw(CDrawer &_rDrawer) {}

	public:
		enum
		{
			EFollow_Left=1,
			EFollow_Right=2,
			EFollow_Top=4,
			EFollow_Bottom=8,

			EFollow_Top_Bottom=EFollow_Top|EFollow_Bottom,
			EFollow_Left_Right=EFollow_Left|EFollow_Right,

			EFollow_All=EFollow_Top_Bottom|EFollow_Left_Right,

			EFlag_TabStop=1,
			EFlag_Indestructable=2,
			EFlag_Hidden=4
		};

		//
		// Constructors
		//
		CView(CView *pParent=NULL, const int8 *pName=NULL, CGUI *pGUI=NULL);
		virtual ~CView();

		void DestroyTree();
		void DumpTree();
		void CheckTree(void *p);

		CGUI *GetGUI() { return m_pGUI; }


		virtual void Set(CString &rSetting, CString &rValue);

		//
		//
		//
		const int8 *GetName() { return m_Name; }
		void SetName(const int8 *_pName) { m_Name = _pName; }

		//
		//
		//
		int32 Flags() { return m_Flags; }
		void SetFlags(int32 flags) { m_Flags |= flags; }
		void ClearFlags(int32 flags) { m_Flags &= ~flags; }

		//
		// resize mode
		//
		void SetResizeMode(int32 s);
		int32 GetResizeMode();

		//
		// Child, parent handling (is it only layered containers that can have children?)
		//
		CView *GetParent();
		int32 AddChild(CView *pChild);
		bool RemoveChild(CView *pChild);
		bool RemoveChild(int32 iLocal);
		bool IsChild(CView *pView);
		int32 GetNumChildren();
		CView *GetChild(int32 _iLocal);
		CView *GetChild(const int8 *_pName); // recursive
		CView *GetChildByPoint(CPoint _Point, bool HiddensAsWell=true);

		//
		// Geometry handling
		//
		void SetPosition(int32 x, int32 y);
		void SetPosition(CPoint p) { SetPosition(p.x, p.y); }
		void SetSize(int32 w, int32 h);
		void SetSize(CPoint p) { SetSize(p.x, p.y); }

		void SetSizeLimits(CPoint _MinSize, CPoint _MaxSize);
		void SetMaxSize(CPoint _Size);
		void SetMinSize(CPoint _Size);
		void SetGeometry(int32 x, int32 y, int32 w, int32 h);
		void SetGeometry(CPoint p, CPoint s) { SetGeometry(p.x, p.y, s.x, s.y); }

		CPoint GetPosition();
		CPoint GetSize();
	    
		CPoint GetWorldPosition();

		//
		// Z Position
		//
		void MoveToFront();

		//
		// State
		//
		bool IsFocused();
		bool IsMouseOver();
	};
	//typedef TSmarCView *spCView;
}
}

#endif // __FOO_VIEW_H__
