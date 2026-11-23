#include "view.h"
#include "scrollbar.h"

namespace NFoo {
namespace NGUI {

	class CListItem
	{
	protected:
		int32 m_Width;
		int32 m_Height;
	public:
		virtual ~CListItem() {}
		void SetWidth(int32 _w);
		int32 GetHeight();
		
		virtual void OnDraw(CDrawer &_rDrawer, bool Selected) {}
		virtual void OnWidthChange() {}
	};

	class CStringItem : public CListItem
	{
		CString m_Text;
		bool m_Multiline;
	public:
		CStringItem();
		CStringItem(const int8 *_pString, bool _Multiline = false);

		virtual void OnWidthChange();
		virtual void OnDraw(CDrawer &_rDrawer, bool Selected);
	};

	//
	// a listbox
	//
	class CListBox : public CView
	{
		CScrollBar m_ScrollBar;
		TDynamicArray<class CListItem*> m_lItems;
		int32 m_ItemsHeight;
		int32 m_SelectedItem;
	protected:
		int32 GetStartY();
		int32 GetItemsHeight();
		//CScrollBar *GetScrollBar();
		virtual void OnSize();
		virtual void OnDraw(CDrawer &_rDrawer);
		virtual void OnMouseDown(CPoint _Pos, int32 _Button);
		virtual void OnDoubleClick(CPoint _Pos, int32 _Button);
		virtual void OnLostFocus();
	public:
		CListBox(CView *pParent=NULL, const int8 *pName=NULL, CGUI *pGUI=NULL);
		~CListBox();

		void SetScrollAmount(fp32 ScrollAmount) { m_ScrollBar.SetAmount(ScrollAmount); }
		void AddItem(CListItem *_pItem);
		void Clear();
		virtual void Set(CString &rSetting, CString &rValue);
		CListItem *GetSelectedItem();

		enum
		{
			EEvent_Select,
			EEvent_DoubleClicked
		};
	};

}
}
