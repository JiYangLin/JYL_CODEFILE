#pragma once


class ICListCtrlEditCallBack
{
public:
	virtual void ListCtrlEditCallBack(CString txt)=0;
};
class CListCtrlEdit:public CEdit
{
	ICListCtrlEditCallBack* m_pICListCtrlEditCallBack;
	DECLARE_MESSAGE_MAP()
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (nChar == VK_RETURN)
		{
			CString txt;
			GetWindowText(txt);
			m_pICListCtrlEditCallBack->ListCtrlEditCallBack(txt);
			ShowWindow(SW_HIDE);
		}
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);	
	}
public:
	CListCtrlEdit(ICListCtrlEditCallBack* _pICListCtrlEditCallBack):m_pICListCtrlEditCallBack(_pICListCtrlEditCallBack){}
};
class EditListCtl:public CListCtrl,public ICListCtrlEditCallBack
{
	int m_hitCol;
	int m_hitRow;
	CListCtrlEdit *m_pEdit;
	DECLARE_MESSAGE_MAP()
public:
	EditListCtl()
	{
		m_hitCol = 0;
		m_hitRow = 0;
		m_pEdit = NULL;
	}
	void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
	{
		if (NULL == m_pEdit)
		{
			m_pEdit = new CListCtrlEdit(this);
			m_pEdit->Create(WS_CHILD|ES_LEFT|WS_BORDER|ES_AUTOHSCROLL|ES_WANTRETURN|ES_MULTILINE,CRect(0,0,0,0),this,1);
		}

		LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

		LVHITTESTINFO info;
		info.pt = pNMItemActivate->ptAction;

		if (SubItemHitTest(&info) != -1)
		{
			m_hitRow = info.iItem;
			m_hitCol = info.iSubItem;

			CRect rect;
			GetSubItemRect(m_hitRow,m_hitCol,LVIR_LABEL,rect);

			m_pEdit->SetWindowText(GetItemText(m_hitRow,m_hitCol));
			m_pEdit->MoveWindow(rect,TRUE);
			m_pEdit->ShowWindow(SW_SHOW);
			m_pEdit->SetFocus();
		}
		else
		{
			m_pEdit->ShowWindow(SW_HIDE);
		}

		*pResult = 0;
	}
	virtual void ListCtrlEditCallBack(CString txt)
	{
		SetItemText(m_hitRow,m_hitCol,txt);
	}
};

#define  DECLARE_EditListCtlMESSAGE_MAP \
BEGIN_MESSAGE_MAP(EditListCtl, CListCtrl)\
	ON_NOTIFY_REFLECT(NM_DBLCLK, &EditListCtl::OnNMDblclk)\
END_MESSAGE_MAP()\
BEGIN_MESSAGE_MAP(CListCtrlEdit, CEdit)\
	ON_WM_KEYDOWN()\
END_MESSAGE_MAP()