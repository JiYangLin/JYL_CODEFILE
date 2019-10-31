#pragma once


class CTxtShow : public CStatic
{
	DECLARE_DYNAMIC(CTxtShow)
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC) { return FALSE; }
	afx_msg void OnPaint()
	{
		CPaintDC dc(this);
		Flush();
	}
public:
	CTxtShow()
	{
		m_ShowTxt = false;
		m_clrBkColor = RGB(0,0,0);
		m_TxtColor = RGB(255,0,0);


		m_iMarginLeft = 0;
		m_iMarginRight = 0;
		m_iMarginTop = 0;
		m_iMarginBottom = 0;
		m_strLine = 1;

		m_txtPos = DT_CENTER;

		m_ProcIng = CreateEvent(NULL,TRUE,FALSE,NULL);
	}
	void Init(COLORREF BkColor,bool showTxt = false,COLORREF txtColor = RGB(255,0,0),int _txtPos = DT_LEFT)
	{//ÉèÖÃ±³¾°ÑÕÉ«
		m_clrBkColor = BkColor;
		m_TxtColor = txtColor;
		m_ShowTxt = showTxt;
		m_txtPos = _txtPos;
	}
	void SetMargin(int	iMarginTop = 0, int	iMarginBottom = 0, int	iMarginLeft = 0, int	iMarginRight = 0)
	{//ÉèÖÃ±ß¿òÎ»ÖÃ
		m_iMarginLeft = iMarginLeft;
		m_iMarginRight = iMarginRight;
		m_iMarginTop = iMarginTop;
		m_iMarginBottom = iMarginBottom;
	}
public:
	void ShowTxt(CString str,int strLine=1)
	{
		if (WAIT_TIMEOUT != WaitForSingleObject(m_ProcIng,0)) return;

		m_strLine = strLine;
		m_strTxt = str;
		Flush();
	}
private:
	void Draw(CDC* pDC, CRect &rectCtrl)
	{	
		SetEvent(m_ProcIng);

		pDC->FillSolidRect(rectCtrl, m_clrBkColor);

		if(!m_ShowTxt) return;


		if (m_strTxt.GetLength() == 0)
		{
			ResetEvent(m_ProcIng);
			return;
		}


		rectCtrl.left += m_iMarginLeft;
		rectCtrl.top += m_iMarginTop;
		rectCtrl.right -= m_iMarginRight;
		rectCtrl.bottom -= m_iMarginBottom;		


		pDC->SetTextColor(m_TxtColor);
		int fontSize = rectCtrl.Height()/m_strLine;
		if (fontSize > rectCtrl.Width()/m_strTxt.GetLength())
		{
			int sizeOrg = fontSize;
			fontSize = rectCtrl.Width()/m_strTxt.GetLength();
			rectCtrl.top += (sizeOrg - fontSize)/2;
		}

		SetPdcFont(pDC,fontSize);
		pDC->FillSolidRect(rectCtrl, m_clrBkColor);
		pDC->DrawText(m_strTxt.GetBuffer(),rectCtrl,m_txtPos);

		ResetEvent(m_ProcIng);
	}
	void Flush()
	{//Ë¢ÐÂÏÔÊ¾
		CClientDC dc(this);

		CDC memDC;
		memDC.CreateCompatibleDC(&dc);

		CRect rectCtrl;
		GetClientRect(&rectCtrl);
		CBitmap memBitmap;
		memBitmap.CreateCompatibleBitmap(&dc, rectCtrl.Width(), rectCtrl.Height());
		memDC.SelectObject(&memBitmap);

		Draw(&memDC, rectCtrl);

		dc.BitBlt(0, 0, rectCtrl.Width(), rectCtrl.Height(), &memDC, 0, 0, SRCCOPY);
	}
	void SetPdcFont(CDC * pDC, int fontHeight)
	{
		CFont font;
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = fontHeight;
		VERIFY(font.CreateFontIndirect(&lf));
		pDC->SelectObject(&font);
	}
private:
	bool m_ShowTxt;
	HANDLE m_ProcIng;

	CString m_strTxt;
	int m_strLine;

	COLORREF    m_TxtColor;
	COLORREF    m_clrBkColor;

	int			m_iMarginLeft;			   // left margin in pixels
	int			m_iMarginRight;			   // right margin in pixels
	int			m_iMarginTop;			   // top margin in pixels
	int			m_iMarginBottom;		   // bottom margin in pixels

	int         m_txtPos;
};

#define  TxtShow_IMPLEMENT   IMPLEMENT_DYNAMIC(CTxtShow, CStatic)\
	BEGIN_MESSAGE_MAP(CTxtShow, CStatic)\
	ON_WM_ERASEBKGND()\
	ON_WM_PAINT()\
END_MESSAGE_MAP()