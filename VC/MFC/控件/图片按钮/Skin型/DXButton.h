#pragma once

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#include "memdc.h"
#include "kittool.h"
#pragma comment(lib, "winmm.lib")
#include <Mmsystem.h>
using namespace Gdiplus;

// CDXButton
#define WM_DXSET_RADIO	WM_USER+0x200

enum Draw_Text_Mode
{
	DRAW_TEXT_NEAR = 0,
	DRAW_TEXT_CENTER = 1,
	DRAW_TEXT_FAR = 2
};


class CDXButton : public CButton
{
	DECLARE_DYNAMIC(CDXButton)

public:
	enum BUTTON_STATE
	{
		BUTTON_ENABLE = 0,
		BUTTON_HOVER,
		BUTTON_CLICK,
		BUTTON_DISABLE,
		BUTTON_PUSHEDOVER,
		STATE_COLORS
	};
	enum Draw_Bitmap_Mode
	{
		DRAW_BITMAP_STRETCH,
		DRAW_BITMAP_LEFT,
		DRAW_BITMAP_TOP,
		DRAW_BITMAP_CENTER,
		DRAW_BITMAP_RIGHT,
		DRAW_BITMAP_BOTTOM
	};

	enum Draw_Mode
	{
		Draw_Center,
		Draw_Stretch,
		Draw_Tile
	};

	struct struct_sound
	{
		TCHAR		szSound[_MAX_PATH];
		LPCTSTR		lpszSound;
		HMODULE		hMod;
		DWORD		dwFlags;
	};

	ULONG_PTR m_gdiplusToken;
protected:
	Bitmap*			m_pAnimate[30];
	UINT			m_nCurFrame;
	UINT			m_nAnimateFrames;
	BOOL			m_bAnimate;
	UINT			m_nAniRounds;
	UINT			m_nAniCurRound;

	CBitmap			m_bitmap;
	CBitmap			m_bmp[STATE_COLORS];
	Bitmap*			m_pBitmap[STATE_COLORS];
	Image*			m_pFill;
	CRect			m_rtMargeFill;
	BOOL			m_bFill;
	Image*			m_pImage;
	CRect			m_rtMargeImage;
	CRect			m_rcMargeText;
	CRect			m_rtClickMargeImage;
	CFont			m_font;
	CFont			myLogFont;
	StringFormat	m_StringFormat;

	BOOL			m_bDisable;
	BOOL			m_bHover;
	CToolTipCtrl	m_toolTip;
	HCURSOR			m_hCursor;
	BOOL			m_bCursorOnButton;
	BOOL			m_bDown;
	COLORREF		m_colMask;
	COLORREF		m_clrText;
	COLORREF		m_clrBorder;
	UINT			m_nBorderWidth;

	struct_sound	m_sound[2];
	BOOL			m_bSound;

	UINT			m_DrawTextMode;
	UINT			m_DrawBitmapMode;

	BOOL			m_bOwnDraw;
	BOOL			m_bCheckRadio;

	DWORD			m_style;
	BOOL			m_bCheck;

	BOOL			m_bTransparent;
	CDC				m_dcBk;
	CBitmap			m_bmpBk;
	CBitmap*		m_pbmpOldBk;

	UINT			m_DrawMode;

	COLORREF		m_clrState[STATE_COLORS];

	CSize			m_szBmp;
	int				m_nBmpLitWidth;
	int				m_nBmpLitHeight;

	CSize			m_szBox;



	DECLARE_MESSAGE_MAP()







public:
	CDXButton::CDXButton()
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

		m_bDisable			= FALSE;
		m_bCursorOnButton	= FALSE;
		m_bHover			= FALSE;

		m_bOwnDraw = FALSE;

		m_DrawTextMode = 0;
		m_DrawBitmapMode = DRAW_BITMAP_STRETCH;

		// default text' color is BLACK
		m_clrText = 0;
		m_clrBorder = RGB(255, 255, 255);
		m_nBorderWidth = 0;

		m_hCursor = NULL;
		m_colMask = 0;
		m_bCheckRadio = false;
		m_bCheck = false;
		m_bDown = false;

		m_pbmpOldBk = NULL;
		m_DrawMode = Draw_Center;

		int i;
		for(i = 0; i < STATE_COLORS; i++)
		{
			m_pBitmap[i] = NULL;
		}
		m_pFill = NULL;
		m_bFill = FALSE;
		m_pImage = NULL;
		m_rtMargeFill.SetRect(0,0,0,0);
		m_rtMargeImage.SetRect(0,0,0,0);
		m_rcMargeText.SetRect(0, 0, 0, 0);

		m_StringFormat.GenericDefault();
		SetAlignment();

		::ZeroMemory(&m_sound, sizeof(m_sound));

		for(i = 0; i < 30; i++)
		{
			m_pAnimate[i] = NULL;
		}
		m_nCurFrame = m_nAnimateFrames = 0;
		m_bAnimate = FALSE;

		LOGFONT  lf;                   //定义字体结构  
		lf.lfWeight=10;                //字体磅数=10  
		lf.lfHeight=20;                //字体高度56    
		lf.lfWidth=20;                 //字体宽度20  
		lf.lfUnderline=FALSE;          //无下划线  
		lf.lfStrikeOut=FALSE;          //无删除线  
		lf.lfItalic=FALSE;             //非斜体  
		lf.lfEscapement=0;
		lf.lfCharSet=DEFAULT_CHARSET;  //使用缺省字符集  
		//wstrcpy(lf.lfFaceName,_T("黑体"));  //字体名=@system  

		//定义字体对象  
		myLogFont.CreateFontIndirect(&lf);  //创建逻辑字体
	}

	CDXButton::~CDXButton()
	{
		int i;
		if (m_dcBk.m_hDC && m_pbmpOldBk)
		{
			m_dcBk.SelectObject(m_pbmpOldBk);
		} // if

		for(i = 0; i < STATE_COLORS; i++)
		{
			if (m_pBitmap[i])
				delete m_pBitmap[i];
			m_pBitmap[i] = NULL;
		}

		for(i = 0; i < 30; i++)
		{
			if (m_pAnimate[i])
				delete m_pAnimate[i];
			m_pAnimate[i] = NULL;
		}

		if (m_pFill)
			delete m_pFill;
		if (m_pImage)
			delete m_pImage;
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}




	void CDXButton::SetSkin(LPWSTR lpNormal, LPWSTR lpHover, LPWSTR lpClick,
		LPWSTR lpPushedOver, LPWSTR lpDisabled, LPRECT lpRect)
	{
		for(int i = 0; i < STATE_COLORS; i++)
		{
			if (m_pBitmap[i])
				delete m_pBitmap[i];
			m_pBitmap[i] = NULL;
		}

		Graphics *pGraphics = NULL;
		Bitmap *bmpTemp  = Bitmap::FromFile(lpNormal);

		RectF rfSrc((float)lpRect->left,(float)lpRect->top, 
			(float)lpRect->right - lpRect->left, (float)lpRect->bottom - lpRect->top);

		int width = (int)rfSrc.Width;
		int height = (int)rfSrc.Height;

		RectF rfSize(0,0, rfSrc.Width, rfSrc.Height);

		// normal
		m_pBitmap[BUTTON_ENABLE] = new Bitmap(width, height);
		pGraphics = Graphics::FromImage(m_pBitmap[BUTTON_ENABLE]);
		pGraphics->DrawImage(bmpTemp, rfSize, rfSrc.X,rfSrc.Y,
			rfSrc.Width,rfSrc.Height,UnitPixel);
		delete pGraphics;
		delete bmpTemp;

		// hover
		bmpTemp = Bitmap::FromFile(lpHover);
		m_pBitmap[BUTTON_HOVER] = new Bitmap(width, height);
		pGraphics = Graphics::FromImage(m_pBitmap[BUTTON_HOVER]);
		pGraphics->DrawImage(bmpTemp, rfSize, rfSrc.X,rfSrc.Y,
			rfSrc.Width,rfSrc.Height,UnitPixel);
		delete pGraphics;
		delete bmpTemp;

		// click
		bmpTemp = Bitmap::FromFile(lpClick);
		m_pBitmap[BUTTON_CLICK] = new Bitmap(width, height);
		pGraphics = Graphics::FromImage(m_pBitmap[BUTTON_CLICK]);
		pGraphics->DrawImage(bmpTemp, rfSize, rfSrc.X,rfSrc.Y,
			rfSrc.Width,rfSrc.Height,UnitPixel);
		delete pGraphics;
		delete bmpTemp;

		// pushover
		bmpTemp = Bitmap::FromFile(lpPushedOver);
		m_pBitmap[BUTTON_PUSHEDOVER] = new Bitmap(width, height);
		pGraphics = Graphics::FromImage(m_pBitmap[BUTTON_PUSHEDOVER]);
		pGraphics->DrawImage(bmpTemp, rfSize, rfSrc.X,rfSrc.Y,
			rfSrc.Width,rfSrc.Height,UnitPixel);
		delete pGraphics;
		delete bmpTemp;

		// disable
		bmpTemp = Bitmap::FromFile(lpDisabled);
		m_pBitmap[BUTTON_DISABLE] = new Bitmap(width, height);
		pGraphics = Graphics::FromImage(m_pBitmap[BUTTON_DISABLE]);
		pGraphics->DrawImage(bmpTemp, rfSize, rfSrc.X,rfSrc.Y,
			rfSrc.Width,rfSrc.Height,UnitPixel);
		delete pGraphics;
		delete bmpTemp;
	}
	void EnableCheckRadio(BOOL bEnable = TRUE)
	{
		m_bCheckRadio = bEnable;
	};
	void CDXButton::SetSkin(UINT nResourceID, LPCTSTR sTR, BOOL bCheck = FALSE)
	{
		int i;
		for(i = 0; i < STATE_COLORS; i++)
		{
			if (m_pBitmap[i])
				delete m_pBitmap[i];
			m_pBitmap[i] = NULL;
		}

		Graphics *pGraphics = NULL;
		Image *bmpTemp = NULL;
		ImageFromIDResource(nResourceID, sTR, bmpTemp);

		if (bmpTemp == NULL || bmpTemp->GetLastStatus())
			return;

		REAL w, h, wLit;
		h = (float)bmpTemp->GetHeight();
		w = (float)bmpTemp->GetWidth();

		int count = bCheck ? STATE_COLORS : STATE_COLORS - 1;
		wLit = w / count;
		for (i = 0; i < count; i++)
		{
			m_pBitmap[i] = new Bitmap((int)wLit, (int)h);
			pGraphics = Graphics::FromImage(m_pBitmap[i]);
			pGraphics->DrawImage(bmpTemp, RectF(0,0,wLit,h), i * wLit, 0, 
				wLit, h,UnitPixel);
			delete pGraphics;
		}
		delete bmpTemp;
	}

	void CDXButton::SetSkin(LPCWSTR lpFile, BOOL bCheck=FALSE)
	{
		int i;
		for(i = 0; i < STATE_COLORS; i++)
		{
			if (m_pBitmap[i])
				delete m_pBitmap[i];
			m_pBitmap[i] = NULL;
		}

		Graphics *pGraphics = NULL;
		Bitmap *bmpTemp  = Bitmap::FromFile(lpFile);

		REAL w, h, wLit;
		h = (float)bmpTemp->GetHeight();
		w = (float)bmpTemp->GetWidth();

		int count = bCheck ? STATE_COLORS : STATE_COLORS - 1;
		wLit = w / count;
		for (i = 0; i < count; i++)
		{
			m_pBitmap[i] = new Bitmap((int)wLit, (int)h);
			pGraphics = Graphics::FromImage(m_pBitmap[i]);
			pGraphics->DrawImage(bmpTemp, RectF(0,0,wLit,h), i * wLit, 0, 
				wLit, h,UnitPixel);
			delete pGraphics;
		}
	}

	void CDXButton::SetSkin(LPWSTR lpNormal, LPWSTR lpHover, LPWSTR lpClick,
		LPWSTR lpPushedOver, LPWSTR lpDisabled)
	{
		for(int i = 0; i < STATE_COLORS; i++)
		{
			if (m_pBitmap[i])
				delete m_pBitmap[i];
			m_pBitmap[i] = NULL;
		}

		Bitmap bmp(lpNormal);

		m_pBitmap[BUTTON_ENABLE] = Bitmap::FromFile(lpNormal);

		if (lpHover)
			m_pBitmap[BUTTON_HOVER] = Bitmap::FromFile(lpHover);
		else
			m_pBitmap[BUTTON_HOVER] = Bitmap::FromFile(lpNormal);

		if (lpClick)
			m_pBitmap[BUTTON_CLICK] = Bitmap::FromFile(lpClick);
		else
			m_pBitmap[BUTTON_CLICK] = Bitmap::FromFile(lpNormal);

		if (lpPushedOver)
			m_pBitmap[BUTTON_PUSHEDOVER] = Bitmap::FromFile(lpPushedOver);
		else
			m_pBitmap[BUTTON_PUSHEDOVER] = Bitmap::FromFile(lpHover);

		if (lpDisabled)
			m_pBitmap[BUTTON_DISABLE] = Bitmap::FromFile(lpDisabled);
		else
			m_pBitmap[BUTTON_DISABLE] = Bitmap::FromFile(lpNormal);

		m_bOwnDraw = TRUE;
	}

	void CDXButton::SetRgn(BUTTON_STATE bs)
	{
		int width, height;
		CRgn  rgnWnd, rgnTemp;
		Color color;

		Bitmap *bmp = m_pBitmap[bs];
		if (bmp == NULL || bmp->GetLastStatus())
			return;

		width = bmp->GetWidth();
		height = bmp->GetHeight();

		rgnWnd.CreateRectRgn(0, 0, width, height);
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				bmp->GetPixel(x, y, &color);
				/*if (clrTransparent.GetR() == color.GetR()
				&& clrTransparent.GetG() == color.GetG()
				&& clrTransparent.GetB() == color.GetB())*/
				if (color.GetA() == 0)
				{
					rgnTemp.CreateRectRgn(x, y, x +1, y + 1);
					rgnWnd.CombineRgn(&rgnWnd, &rgnTemp, RGN_XOR);
					rgnTemp.DeleteObject();
				}
			}
		}
		SetWindowRgn((HRGN)rgnWnd, TRUE);
	}

	void CDXButton::SetRgn(LPWSTR lpMask, Color/* clrTransparent*/)
	{
		int width, height;
		CRgn  rgnWnd, rgnTemp;
		Color color;

		Bitmap *bmp = Bitmap::FromFile(lpMask);

		width = bmp->GetWidth();
		height = bmp->GetHeight();

		rgnWnd.CreateRectRgn(0, 0, width, height);
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				bmp->GetPixel(x, y, &color);
				/*if (clrTransparent.GetR() == color.GetR()
				&& clrTransparent.GetG() == color.GetG()
				&& clrTransparent.GetB() == color.GetB())*/
				if (color.GetA() == 0)
				{
					rgnTemp.CreateRectRgn(x, y, x +1, y + 1);
					rgnWnd.CombineRgn(&rgnWnd, &rgnTemp, RGN_XOR);
					rgnTemp.DeleteObject();
				}
			}
		}
		delete bmp;
		SetWindowRgn((HRGN)rgnWnd, TRUE);
	}

	void CDXButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{

		// TODO:  添加您的代码以绘制指定项

		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		// disable state...
		//	if( m_bDisable == TRUE )
		//Draw(pDC,BUTTON_DISABLE);

		//else
		{
			// radio state
			if (m_bCheck)
			{
				if (m_bCursorOnButton)
					Draw(pDC, BUTTON_PUSHEDOVER);
				else if (lpDrawItemStruct->itemState & ODS_DISABLED)
				{
					Draw(pDC,BUTTON_DISABLE);
				}
				else
					Draw(pDC,BUTTON_CLICK);
			}
			// click state
			else if( lpDrawItemStruct->itemState & ODS_SELECTED)
			{
				Draw(pDC,BUTTON_CLICK);
			}
			else if (lpDrawItemStruct->itemState & ODS_DISABLED)
			{
				Draw(pDC,BUTTON_DISABLE);
			}
			// hover state
			else if ( m_bHover)
				Draw(pDC,BUTTON_HOVER);

			// enable state
			else
				Draw(pDC,BUTTON_ENABLE);
		}
	}

	void CDXButton::OnMouseMove(UINT nFlags, CPoint point)
	{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		if( m_bCursorOnButton == FALSE )
		{
			TRACKMOUSEEVENT tme;
			ZeroMemory(&tme,sizeof(TRACKMOUSEEVENT));
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_hWnd;
			tme.dwFlags = TME_LEAVE|TME_HOVER;
			tme.dwHoverTime = 1;
			m_bCursorOnButton = _TrackMouseEvent(&tme);
		}

		CButton::OnMouseMove(nFlags, point);
	}

	LRESULT CDXButton::OnMouseLeave(WPARAM /*wParam*/, LPARAM/* lParam*/)
	{
		m_bCursorOnButton	= FALSE;
		m_bHover			= FALSE;

		Invalidate();
		return 0;
	}

	LRESULT CDXButton::OnMouseHover(WPARAM /*wParam*/, LPARAM /*wParam*/)
	{
		m_bHover = TRUE;
		if (m_sound[0].lpszSound)
			::PlaySound(m_sound[0].lpszSound, m_sound[0].hMod, m_sound[0].dwFlags);
		Invalidate();
		return 0;
	}

	void CDXButton::SetAlignment(UINT nLeft = DRAW_TEXT_CENTER, 
		UINT nTop = DRAW_TEXT_CENTER)
	{
		switch (nLeft)
		{
		case DRAW_TEXT_NEAR:
			m_StringFormat.SetAlignment(StringAlignmentNear);
			break;
		case DRAW_TEXT_CENTER:
			m_StringFormat.SetAlignment(StringAlignmentCenter);
			break;
		case DRAW_TEXT_FAR:
			m_StringFormat.SetAlignment(StringAlignmentFar);
			break;
		}
		switch (nTop)
		{
		case DRAW_TEXT_NEAR:
			m_StringFormat.SetLineAlignment(StringAlignmentNear);
			break;
		case DRAW_TEXT_CENTER:
			m_StringFormat.SetLineAlignment(StringAlignmentCenter);
			break;
		case DRAW_TEXT_FAR:
			m_StringFormat.SetLineAlignment(StringAlignmentFar);
			break;
		}
	}
	void CDXButton::SetTextColor(COLORREF clrText = 0, COLORREF clrBorder = 0, UINT nBorderWidth = 0)
	{
		m_clrText = clrText;
		m_clrBorder = clrBorder;
		m_nBorderWidth = nBorderWidth;
	}

	void CDXButton::Draw(CDC *pDC, CDXButton::BUTTON_STATE state)
	{
		//////////////////////////////////////////////////////////////
		// draw background
		CRect rc, rcBitmap, rcText;
		GetClientRect(&rc);
		rcText.CopyRect(&rc);
		CGridMemDC memDC(pDC, &rc);
		DrawBk(&memDC);
		if (m_pBitmap[state] == NULL || m_pBitmap[state]->GetLastStatus())
		{
			if (state == BUTTON_PUSHEDOVER)
				state = BUTTON_CLICK;
		}

		if (m_pBitmap[state] == NULL || m_pBitmap[state]->GetLastStatus())
		{
			if (m_pImage == NULL)
			{
				CRect rcMem(0, 0, rc.Width(), rc.Height());
				COLORREF clrHighLight, clrShadow, clrFace;
				clrHighLight = GetSysColor(COLOR_BTNHIGHLIGHT);
				clrShadow = GetSysColor(COLOR_BTNSHADOW);
				clrFace = GetSysColor(COLOR_BTNFACE);
				switch (state)
				{
				case BUTTON_DISABLE:
					break;
				case BUTTON_ENABLE:
					break;
				case BUTTON_PUSHEDOVER:
					break;
				case BUTTON_HOVER:
					break;
				case BUTTON_CLICK:
					clrHighLight = GetSysColor(COLOR_BTNSHADOW);
					clrShadow = GetSysColor(COLOR_BTNHIGHLIGHT);
					break;
				}
				memDC.Draw3dRect(&rcMem, clrHighLight, clrShadow);
				rcMem.DeflateRect(1, 1, 1, 1);
				memDC.FillRect(&rcMem, &CBrush(clrFace));
			}
		}
		else
		{
			Graphics g(memDC);
			int wBitmap = m_pBitmap[state]->GetWidth();
			int hBitmap = m_pBitmap[state]->GetHeight();
			int leftBitmap = rc.left + (rc.Width() - wBitmap) / 2;
			int topBitmap = rc.top + (rc.Height() - hBitmap) / 2;
			rcBitmap.SetRect(leftBitmap, topBitmap, wBitmap + leftBitmap, topBitmap + hBitmap);
			//CKitTool::SizeToSize(CSize(wBitmap, hBitmap), CSize(rc.Width(), rc.Height()), rcBitmap);
			switch (m_DrawBitmapMode)
			{
			case DRAW_BITMAP_STRETCH:
				rcBitmap.CopyRect(&rc);
				break;
			case DRAW_BITMAP_LEFT:
				//	rcBitmap.MoveToX(rc.left);
				rcText.left = rcBitmap.right;
				break;
			case DRAW_BITMAP_CENTER:
				break;
			case DRAW_BITMAP_RIGHT:
				//	rcBitmap.MoveToX(rc.right - rcBitmap.Width());
				rcText.right = rcBitmap.left;
				break;
			}
			if (m_pBitmap[state] && !(m_pBitmap[state]->GetLastStatus()))
			{
				RectF rfClient((float)rcBitmap.left,(float)rcBitmap.top,
					(float)rcBitmap.Width(),(float)rcBitmap.Height());
				g.DrawImage(m_pBitmap[state],rfClient,(float)0,(float)0,
					(float)m_pBitmap[state]->GetWidth(), 
					(float)m_pBitmap[state]->GetHeight(), UnitPixel);
			}
			else
			{

			}
			g.ReleaseHDC(memDC);
		}
		DrawImage(&memDC, state);
		TCHAR szText[MAX_PATH];
		if (GetWindowText(szText, MAX_PATH) > 0)
		{
			DrawText(&memDC, (LPCTSTR)szText, &rcText, state);
		}
		pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);
	}

	void CDXButton::DrawText(CDC *pDC, LPCTSTR lpszText, CRect *rcClient, CDXButton::BUTTON_STATE state)
	{
		rcClient->DeflateRect(&m_rcMargeText);
		//CRect rcTemp = *rcClient;
		CFont *fontOld = pDC->SelectObject(&m_font);
		//pDC->DrawText(lpszText, -1, rcClient,DT_WORDBREAK | DT_CENTER | DT_CALCRECT);
		//switch (m_DrawTextMode)
		//{
		//case DRAW_TEXT_LEFT:
		//	rcClient->MoveToXY(rcTemp.left, (rcTemp.Height() - rcClient->Height())/2);
		//	break;
		//case DRAW_TEXT_CENTER:
		//	rcClient->OffsetRect((rcTemp.Width() - rcClient->Width())/2, (rcTemp.Height() - rcClient->Height())/2);
		//	break;
		//case DRAW_TEXT_RIGHT:
		//	rcClient->OffsetRect(rcTemp.Width() - rcClient->Width(), (rcTemp.Height() - rcClient->Height())/2);
		//	break;
		//case DRAW_TEXT_UP:
		//	rcClient->OffsetRect((rcTemp.Width() - rcClient->Width())/2, 0);
		//	break;
		//case DRAW_TEXT_DOWN:
		//	rcClient->OffsetRect((rcTemp.Width() - rcClient->Width())/2, rcTemp.Height() - rcClient->Height());
		//	break;
		//}

		//Font font(*pDC, *GetFont());
		Graphics g(*pDC);
		GraphicsPath gp;
		//FontFamily fontFamily;
		//font.GetFamily(&fontFamily);
		RectF r((REAL)rcClient->left, (REAL)rcClient->top, 
			(REAL)rcClient->Width(), (REAL)rcClient->Height());
		//	gp.AddString(lpszText, -1, &fontFamily, font.GetStyle(),
		//		font.GetSize(), r, &m_StringFormat);

		Color c1, c2;
		c1.SetFromCOLORREF(m_clrText);
		c2.SetFromCOLORREF(m_clrBorder);
		m_nBorderWidth = (int)( 3);
		if (m_nBorderWidth > 0) 
		{
			g.SetSmoothingMode(SmoothingModeAntiAlias);
			Pen pen(c2, (REAL)m_nBorderWidth);
			//pen.SetAlignment(PenAlignment::PenAlignmentCenter);
			pen.SetLineJoin(LineJoinRound);
			g.DrawPath(&pen, &gp);
			g.FillPath(&SolidBrush(c1), &gp);
		}
		else
		{
			g.SetTextRenderingHint(TextRenderingHintAntiAlias);
			g.SetSmoothingMode(SmoothingModeAntiAlias);
			//	g.DrawString(lpszText, -1, &font, r, &m_StringFormat, 
			//		&SolidBrush(c1));
		}
		g.ReleaseHDC(*pDC);

		//if (state == BUTTON_DISABLE)
		//{
		//	pDC->SetBkMode(TRANSPARENT);
		//	rcClient->OffsetRect(1, 1);
		//	pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
		//	pDC->DrawText(lpszText, -1, rcClient, DT_WORDBREAK | DT_CENTER);
		//	rcClient->OffsetRect(-1, -1);
		//	pDC->SetTextColor(::GetSysColor(COLOR_3DSHADOW));
		//	pDC->DrawText(lpszText, -1, rcClient, DT_WORDBREAK | DT_CENTER);
		//}
		//else
		//{
		//	if (state == BUTTON_CLICK && m_DrawBitmapMode == DRAW_BITMAP_CENTER)
		//		rcClient->OffsetRect(1, 1);
		//	int bkOld = pDC->SetBkMode(TRANSPARENT);
		//	COLORREF clrOld = pDC->SetTextColor(m_clrText);
		//	pDC->DrawText(lpszText, -1, rcClient,DT_WORDBREAK | DT_CENTER);
		//	pDC->SetBkMode(bkOld);
		//	pDC->SetTextColor(clrOld);
		//}
		pDC->SelectObject(fontOld);
	}

	void CDXButton::SetToolTipText(CString strText)
	{
		m_toolTip.Create(this);
		m_toolTip.Activate(TRUE);
		m_toolTip.AddTool(this, (LPCTSTR)strText);
	}

	void CDXButton::SetToolTipTextW(LPCWSTR lpText)
	{
		m_toolTip.Create(this);
		m_toolTip.Activate(TRUE);
		//	m_toolTip.AddTool(this, lpText);
	}

	void CDXButton::SetToolTipTextA(LPCTSTR lpText)
	{
		m_toolTip.Create(this);
		m_toolTip.Activate(TRUE);
		m_toolTip.AddTool(this, lpText);
	}

	void CDXButton::SetCursor(HCURSOR hCursor)
	{
		if (NULL != m_hCursor) DestroyCursor(m_hCursor);
		m_hCursor = hCursor;
	}

	BOOL CDXButton::PreTranslateMessage(MSG* pMsg)
	{
		// TODO: 在此添加专用代码和/或调用基类
		if( m_toolTip.GetSafeHwnd() != NULL )
			m_toolTip.RelayEvent(pMsg);

		return CButton::PreTranslateMessage(pMsg);
	}

	void CDXButton::OnLButtonDown(UINT nFlags, CPoint point)
	{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		m_bDown = TRUE;
		if (m_sound[1].lpszSound)
			::PlaySound(m_sound[1].lpszSound, m_sound[1].hMod, m_sound[1].dwFlags);
		Invalidate();
		if (!m_bDisable)
			CButton::OnLButtonDown(nFlags, point);
	}

	void CDXButton::SizeToContent()
	{
		if (m_pBitmap[BUTTON_CLICK])
		{
			CRect rcPos;
			CWnd *pWnd = this->GetParent();
			GetWindowRect(rcPos);
			pWnd->ScreenToClient(rcPos);
			rcPos.right	= rcPos.left + m_pBitmap[BUTTON_CLICK]->GetWidth();
			rcPos.bottom	= rcPos.top  + m_pBitmap[BUTTON_CLICK]->GetHeight();
			MoveWindow(rcPos);
		}

	}
	BOOL CDXButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
	{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		if (m_hCursor)
		{
			::SetCursor(m_hCursor);
			return TRUE;
		}
		else
			return CButton::OnSetCursor(pWnd, nHitTest, message);
	}

	void CDXButton::SetRgnFromBitmap(HBITMAP hBmp, COLORREF color)
	{
		if (!hBmp) return;

		BITMAP bm;
		GetObject( hBmp, sizeof(BITMAP), &bm );	// get bitmap attributes

		CDC dcBmp;
		dcBmp.CreateCompatibleDC(GetDC());	//Creates a memory device context for the bitmap
		dcBmp.SelectObject(hBmp);			//selects the bitmap in the device context

		const DWORD RDHDR = sizeof(RGNDATAHEADER);
		const DWORD MAXBUF = 40;		// size of one block in RECTs
		// (i.e. MAXBUF*sizeof(RECT) in bytes)
		LPRECT	pRects;								
		DWORD	cBlocks = 0;			// number of allocated blocks

		INT		i, j;					// current position in mask image
		INT		first = 0;				// left position of current scan line
		// where mask was found
		bool	wasfirst = false;		// set when if mask was found in current scan line
		bool	ismask;					// set when current color is mask color

		// allocate memory for region data
		RGNDATAHEADER* pRgnData = (RGNDATAHEADER*)new BYTE[ RDHDR + ++cBlocks * MAXBUF * sizeof(RECT) ];
		memset( pRgnData, 0, RDHDR + cBlocks * MAXBUF * sizeof(RECT) );
		// fill it by default
		pRgnData->dwSize	= RDHDR;
		pRgnData->iType		= RDH_RECTANGLES;
		pRgnData->nCount	= 0;
		for ( i = 0; i < bm.bmHeight; i++ )
			for ( j = 0; j < bm.bmWidth; j++ ){
				// get color
				ismask=(dcBmp.GetPixel(j,bm.bmHeight-i-1)!=color);
				// place part of scan line as RECT region if transparent color found after mask color or
				// mask color found at the end of mask image
				if (wasfirst && ((ismask && (j==(bm.bmWidth-1)))||(ismask ^ (j<bm.bmWidth)))){
					// get offset to RECT array if RGNDATA buffer
					pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
					// save current RECT
					pRects[ pRgnData->nCount++ ] = CRect( first, bm.bmHeight - i - 1, j+(j==(bm.bmWidth-1)), bm.bmHeight - i );
					// if buffer full reallocate it
					if ( pRgnData->nCount >= cBlocks * MAXBUF ){
						LPBYTE pRgnDataNew = new BYTE[ RDHDR + ++cBlocks * MAXBUF * sizeof(RECT) ];
						memcpy( pRgnDataNew, pRgnData, RDHDR + (cBlocks - 1) * MAXBUF * sizeof(RECT) );
						delete pRgnData;
						pRgnData = (RGNDATAHEADER*)pRgnDataNew;
					}
					wasfirst = false;
				} else if ( !wasfirst && ismask ){		// set wasfirst when mask is found
					first = j;
					wasfirst = true;
				}
			}
			dcBmp.DeleteDC();	//release the bitmap
			// create region
			/*  Under WinNT the ExtCreateRegion returns NULL (by Fable@aramszu.net) */
			//	HRGN hRgn = ExtCreateRegion( NULL, RDHDR + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
			/* ExtCreateRegion replacement { */
			HRGN hRgn=CreateRectRgn(0, 0, 0, 0);
			ASSERT( hRgn!=NULL );
			pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
			for(i=0;i<(int)pRgnData->nCount;i++)
			{
				HRGN hr=CreateRectRgn(pRects[i].left, pRects[i].top, pRects[i].right, pRects[i].bottom);
				VERIFY(CombineRgn(hRgn, hRgn, hr, RGN_OR)!=ERROR);
				if (hr) DeleteObject(hr);
			}
			ASSERT( hRgn!=NULL );
			/* } ExtCreateRegion replacement */

			delete pRgnData;
			SetWindowRgn(hRgn, TRUE);
	}

	BOOL CDXButton::OnEraseBkgnd(CDC* /*pDC*/)
	{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		//return CButton::OnEraseBkgnd(pDC);
		return TRUE;
	}

	void CDXButton::PreSubclassWindow()
	{
		// TODO: 在此添加专用代码和/或调用基类
		m_style = GetButtonStyle();
		if ((m_style & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX)
			m_style = BS_CHECKBOX;
		else if ((m_style & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON)
			m_style = BS_RADIOBUTTON;
		else
			m_style = BS_PUSHBUTTON;

		CButton::PreSubclassWindow();
		ModifyStyle(0, BS_OWNERDRAW);
	}

	void CDXButton::DrawImage(CDC *pDC, BUTTON_STATE state)
	{
		CRect rect;
		GetClientRect(rect);
		if (m_pImage && m_pImage->GetLastStatus() == 0)
		{
			Graphics g(pDC->GetSafeHdc());
			if ((m_pImage->GetWidth() > m_pImage->GetHeight() && rect.Width() < rect.Height())
				|| (m_pImage->GetWidth() < m_pImage->GetHeight() && rect.Width() > rect.Height()))
			{
				m_pImage->RotateFlip(Rotate270FlipNone);
			}
			int bmpWidth = m_pImage->GetWidth();
			int bmpHeight = m_pImage->GetHeight();
			CRect rc(&rect);
			switch (state)
			{
			case BUTTON_DISABLE:
				rect.DeflateRect(&m_rtMargeImage);
				break;
			case BUTTON_ENABLE:
				rect.DeflateRect(&m_rtMargeImage);
				break;
			case BUTTON_PUSHEDOVER:
				rect.DeflateRect(&m_rtMargeImage);
				break;
			case BUTTON_HOVER:
				rect.DeflateRect(&m_rtMargeImage);
				break;
			case BUTTON_CLICK:
				rect.DeflateRect(&m_rtClickMargeImage);
				break;
			}
			CRect rcSize(0,0,bmpWidth, bmpHeight);

			if (m_DrawMode == Draw_Center)
				CKitTool::SizeToSize(rcSize.Size(), rect.Size(), rc);

			rc.OffsetRect(rect.TopLeft());

			if (m_DrawMode == Draw_Tile)
			{
				float ff = rect.Width() / (float)rect.Height();
				int fw = int(m_pImage->GetHeight() * ff);
				Rect rf(rc.left,rc.top,rc.Width(), rc.Height());
				g.DrawImage(m_pImage, rf, 0, 0, fw, m_pImage->GetHeight(), UnitPixel);
				return;
			}
			else
				g.DrawImage(m_pImage, rc.left,rc.top,rc.Width(), rc.Height());
		}

	}

	void CDXButton::DrawBk(CDC *pDC)
	{
		CClientDC clDC(GetParent());
		CRect rect;
		CRect rect1;

		GetClientRect(rect);

		GetWindowRect(rect1);
		GetParent()->ScreenToClient(rect1);

		/*if (m_szBox.cx != rect.Width() || m_szBox.cy != rect.Height())
		{
		if (m_dcBk.m_hDC)
		{

		m_dcBk.SelectObject(m_pbmpOldBk);
		m_dcBk.DeleteDC();
		m_bmpBk.DeleteObject();
		}
		m_szBox.cx = rect.Width();
		m_szBox.cy = rect.Height();
		}*/

		if (m_dcBk.m_hDC == NULL)
		{
			m_dcBk.CreateCompatibleDC(&clDC);
			m_bmpBk.CreateCompatibleBitmap(&clDC, rect.Width(), rect.Height());
			m_pbmpOldBk = m_dcBk.SelectObject(&m_bmpBk);
			m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), &clDC, rect1.left, rect1.top, SRCCOPY);
		} // if

		pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &m_dcBk, 0, 0, SRCCOPY);

		if (m_bAnimate)
		{
			Graphics g(*pDC);
			g.DrawImage(m_pAnimate[m_nCurFrame], 0, 0, rect.Width(), rect.Height());
		}

		if (m_pImage)
		{
			if (m_pFill && m_bFill)
			{
				CRect rtFill = rect;
				rtFill.DeflateRect(&m_rtMargeFill);
				Graphics g(pDC->GetSafeHdc());
				g.DrawImage(m_pFill, rtFill.left, rtFill.top, rtFill.Width(), rtFill.Height());
			}
		}
	}

	void CDXButton::UpdateBk(CDC *pDC)
	{
		if (pDC)
		{
			// Restore old bitmap (if any)
			if (m_dcBk.m_hDC != NULL && m_pbmpOldBk != NULL)
			{
				m_dcBk.SelectObject(m_pbmpOldBk);
			} // if

			m_bmpBk.DeleteObject();
			m_dcBk.DeleteDC();

			CRect rect;
			CRect rect1;

			GetClientRect(rect);

			GetWindowRect(rect1);
			GetParent()->ScreenToClient(rect1);

			m_dcBk.CreateCompatibleDC(pDC);
			m_bmpBk.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
			m_pbmpOldBk = m_dcBk.SelectObject(&m_bmpBk);
			m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, rect1.left, rect1.top, SRCCOPY);
		} // if
	}

	LRESULT CDXButton::OnSetCheck(WPARAM wparam, LPARAM)
	{
		m_bCheck = wparam != 0;
		switch (m_style)
		{
		case BS_RADIOBUTTON:
			{
				if (m_bCheck)
				{
					HWND hthis, hwnd, hwndParent;
					hwndParent = GetParent()->GetSafeHwnd();
					hthis = hwnd = GetSafeHwnd();
					if (hthis && hwndParent)
					{
						while(1)
						{
							hwnd = ::GetNextDlgGroupItem(hwndParent, hwnd, 0);
							if (hwnd == hthis || hwnd == NULL)
								break;
							::PostMessage(hwnd, WM_DXSET_RADIO, 0, 0);
						}
					}
				}
			}
			break;
		case BS_PUSHBUTTON:
			m_bCheck = FALSE;
			ASSERT(false);
		}
		Invalidate();
		return 0;
	}

	LRESULT CDXButton::OnGetCheck(WPARAM, LPARAM)
	{
		return m_style;
	}

	LRESULT CDXButton::OnRadioInfo(WPARAM, LPARAM)
	{
		if (m_bCheck){	//only checked buttons need to be unchecked
			m_bCheck = false;
			Invalidate();
		}
		return 0;
	}

	void CDXButton::OnLButtonUp(UINT nFlags, CPoint point)
	{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		//m_bCursorOnButton = FALSE;
		if (m_style){ //track mouse for radio & check buttons
			POINT p2 = point;
			::ClientToScreen(m_hWnd, &p2);
			HWND mouse_wnd = ::WindowFromPoint(p2);
			if (mouse_wnd == m_hWnd){ // mouse is in button
				if (m_style==BS_CHECKBOX) SetCheck(m_bCheck ? 0 : 1);
				if (m_style==BS_RADIOBUTTON) 
				{
					if (m_bCheckRadio)
						SetCheck(m_bCheck ? 0 : 1);
					else
						SetCheck(1);
				}
			}
		}
		CButton::OnLButtonUp(nFlags, point);
	}



	BOOL IsChecked() { return m_bCheck; };
	void CDXButton::SetFill(LPCWSTR lpFile, CRect *rect = NULL)
	{
		if (m_pFill)
			delete m_pFill;
		m_pFill = NULL;
		m_bFill = FALSE;
		if (lpFile != NULL)
		{
			if (rect != NULL)
				m_rtMargeFill = *rect;
			CRect rc;
			GetClientRect(rc);
			m_pFill = Bitmap::FromFile(lpFile);
			/*Bitmap bitmap(lpFile);
			CRect rtSize;
			CKitTool::SizeToSize(CSize(bitmap.GetWidth(), bitmap.GetHeight()), 
			rc.Size(), rtSize);
			m_pFill = bitmap.GetThumbnailImage(rtSize.Width(), rtSize.Height());*/
			m_bFill = TRUE;
		}
	}



	void CDXButton::SetImage(Bitmap* bitmap, CRect *rect = NULL, 
		BOOL bClone = FALSE, UINT drawMode = Draw_Center)
	{
		if (m_pImage)
			delete m_pImage;
		if (bClone)
		{
			m_pImage = bitmap->GetThumbnailImage(bitmap->GetWidth(), bitmap->GetHeight());
		}
		else
			m_pImage = bitmap;
		if (rect != NULL)
		{
			m_rtMargeImage = *rect;
			m_rtClickMargeImage = *rect;
		}

		m_DrawMode = drawMode;
		Invalidate();
	}

	void CDXButton::SetImage(LPCWSTR lpFile, CRect *rect = NULL, 
		BOOL bClone = FALSE, UINT drawMode = Draw_Center)
	{
		if (m_pImage)
			delete m_pImage;
		m_pImage = NULL;
		if (lpFile != NULL)
		{
			if (rect != NULL)
			{
				m_rtMargeImage = *rect;
				m_rtClickMargeImage = *rect;
			}
			CRect rc;
			GetClientRect(rc);
			if (bClone)
			{
				Bitmap bitmap(lpFile);
				m_pImage = bitmap.GetThumbnailImage(bitmap.GetWidth(), bitmap.GetHeight());
			}
			else
				m_pImage = Bitmap::FromFile(lpFile);
			//Bitmap bitmap(lpFile);
			//CRect rtSize;
			//CKitTool::SizeToSize(CSize(bitmap.GetWidth(), bitmap.GetHeight()), 
			//	rc.Size(), rtSize);
			//m_pImage = bitmap.GetThumbnailImage(rtSize.Width(), rtSize.Height());
		}

		m_DrawMode = drawMode;
		Invalidate();
	}
	void CDXButton::EnableFill(BOOL bEnable=TRUE)
	{
		m_bFill = bEnable;
	}

	void CDXButton::SetTextFont(LOGFONT *pFont)
	{
		//m_font.DeleteObject();
		CFont *font = new CFont;
		font->CreateFontIndirect(pFont);
		SetFont(font);
	}

	void CDXButton::SetTextFont(int nHeight, BOOL bItalic, BOOL bBold, TCHAR *szFace)
	{
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfCharSet		= DEFAULT_CHARSET;
		lf.lfHeight			= nHeight;
		lf.lfItalic			= bItalic;
		lf.lfQuality		= DEFAULT_QUALITY;
		lf.lfWeight			= bBold ? 700 : 400;
		lstrcpy(lf.lfFaceName, szFace);
		SetTextFont(&lf);
	}


	UINT CDXButton::SetDrawBitmapMode(UINT mode, BOOL bRedraw = TRUE)
	{
		UINT oldMode = m_DrawBitmapMode;
		m_DrawBitmapMode = mode;
		if (bRedraw) 
			Invalidate();
		return oldMode;
	}
	void EnableSound(BOOL bSound = TRUE) { m_bSound = bSound; };
	void CDXButton::SetSound(LPCTSTR lpszSound, HMODULE hMod, BOOL bClick = FALSE)
	{
		int index = bClick ? 1 : 0;
		if (lpszSound)
		{
			if (hMod)	// From resource identifier ?
			{
				m_sound[index].lpszSound = lpszSound;
			} // if
			else
			{
				_tcscpy(m_sound[index].szSound, lpszSound);
				m_sound[index].lpszSound = m_sound[index].szSound;
			} // else

			m_sound[index].hMod = hMod;
			m_sound[index].dwFlags = SND_NODEFAULT | SND_NOWAIT;
			m_sound[index].dwFlags |= hMod ? SND_RESOURCE : SND_FILENAME;
			m_sound[index].dwFlags |= SND_ASYNC;
		} // if
		else
		{
			// Or remove any existing
			::ZeroMemory(&m_sound[index], sizeof(struct_sound));
		} // else
	}

	BOOL CDXButton::ImageFromIDResource(UINT nID, LPCTSTR sTR,Image* &pImg)
	{
		HINSTANCE hInst = AfxGetResourceHandle();
		HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type
		if (!hRsrc)
			return FALSE;
		// load resource into memory
		DWORD len = SizeofResource(hInst, hRsrc);
		BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
		if (!lpRsrc)
			return FALSE;
		// Allocate global memory on which to create stream
		HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
		BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
		memcpy(pmem,lpRsrc,len);
		IStream* pstm;
		CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);
		// load from stream
		pImg=Gdiplus::Image::FromStream(pstm);
		// free/release stuff
		GlobalUnlock(m_hMem);
		pstm->Release();
		GlobalFree(m_hMem);
		FreeResource(lpRsrc);

		return TRUE;
	}
	void CDXButton::SetTextMarge(CRect rcMarge, BOOL bRedraw = FALSE)
	{
		m_rcMargeText.CopyRect(&rcMarge);
		if (bRedraw)
			Invalidate();
	}

	void CDXButton::SetAnimateSkin(UINT nResourceID, UINT nCount)
	{
		int i;
		for(i = 0; i < 30; i++)
		{
			if (m_pAnimate[i])
				delete m_pAnimate[i];
			m_pAnimate[i] = NULL;
		}

		Graphics *pGraphics = NULL;
		Image *bmpTemp = NULL;
		ImageFromIDResource(nResourceID, _T("PNG"), bmpTemp);

		if (bmpTemp == NULL || bmpTemp->GetLastStatus())
			return;

		REAL w, h, wLit;
		h = (float)bmpTemp->GetHeight();
		w = (float)bmpTemp->GetWidth();

		m_nAnimateFrames = nCount;
		wLit = w / nCount;
		for (UINT i = 0; i < nCount; i++)
		{
			m_pAnimate[i] = new Bitmap((int)wLit, (int)h);
			pGraphics = Graphics::FromImage(m_pAnimate[i]);
			pGraphics->DrawImage(bmpTemp, RectF(0,0,wLit,h), i * wLit, 0, 
				wLit, h,UnitPixel);
			delete pGraphics;
		}
	}

	void CDXButton::StartAnimate(UINT nMillisecond, int nRound = 0)
	{
		if (m_bAnimate) return;
		m_nCurFrame = 0;
		m_bAnimate = TRUE;
		m_nAniCurRound = 0;
		m_nAniRounds = nRound;
		SetTimer(1, nMillisecond / m_nAnimateFrames, NULL);
	}

	void CDXButton::StopAnimate()
	{
		m_bAnimate = FALSE;
		KillTimer(1);
	}

	void CDXButton::OnTimer(UINT_PTR nIDEvent)
	{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		if (nIDEvent == 1)
		{
			if (m_nAniRounds > 0)
			{
				if ( m_nCurFrame >= m_nAnimateFrames - 1 )
				{
					m_nAniCurRound++;
				}
				if (m_nAniCurRound >= m_nAniRounds)
				{
					KillTimer(1);
					//m_bAnimate = FALSE;
				}
			}
			m_nCurFrame = (m_nCurFrame + 1) % m_nAnimateFrames;
			Invalidate();
		}
		CButton::OnTimer(nIDEvent);
	}

	void CDXButton::SetClickMargeImageRect(CRect rc)
	{
		m_rtClickMargeImage = rc;
	}
};

#define  Dx_Button   IMPLEMENT_DYNAMIC(CDXButton, CButton) \
BEGIN_MESSAGE_MAP(CDXButton, CButton)\
	ON_WM_DRAWITEM()\
	ON_WM_MOUSEMOVE()\
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover)\
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)\
	ON_MESSAGE(BM_GETCHECK, OnGetCheck)\
	ON_MESSAGE(BM_SETCHECK, OnSetCheck)\
	ON_MESSAGE(WM_DXSET_RADIO, OnRadioInfo)\
	ON_WM_LBUTTONDOWN()\
	ON_WM_SETCURSOR()\
	ON_WM_ERASEBKGND()\
	ON_WM_LBUTTONUP()\
	ON_WM_TIMER()\
END_MESSAGE_MAP()


inline void SetBtn(CDXButton & btn , CString str1,CString str2)
{
	str1= _T("skin\\") + str1;
	str2=_T("skin\\") + str2;
	CString str3 = str1;
	btn.SetSkin((str1.AllocSysString()),(str2.AllocSysString()),(str3.AllocSysString()),NULL,NULL);
}