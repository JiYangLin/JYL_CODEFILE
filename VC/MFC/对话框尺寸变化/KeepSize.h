#pragma once
#include <Afxtempl.h >

#define  KS_VAR            CList<CRect> listRect;

#define  KS_OnInitDialog   CRect rectWnd;\
GetWindowRect(&rectWnd);\
listRect.AddTail(&rectWnd);\
CWnd *pWndChild = GetWindow(GW_CHILD);\
while (pWndChild)\
{\
	pWndChild->GetWindowRect(&rectWnd);\
	listRect.AddTail(&rectWnd);\
	pWndChild = pWndChild->GetNextWindow();\
}


#define KS_OnSize  float fRateScaleX;\
float fRateScaleY;\
if (listRect.GetCount()>0)\
{\
	CRect rectDlgNow;\
	GetWindowRect(&rectDlgNow);\
	POSITION mp = listRect.GetHeadPosition();\
	CRect rectDlgSaved;\
	rectDlgSaved = listRect.GetNext(mp);\
	ScreenToClient(rectDlgNow);\
	fRateScaleX = (float)(rectDlgNow.right - rectDlgNow.left) / (rectDlgSaved.right - rectDlgSaved.left);\
	fRateScaleY = (float)(rectDlgNow.bottom - rectDlgNow.top) / (rectDlgSaved.bottom - rectDlgSaved.top);\
	ClientToScreen(rectDlgNow);\
	CRect rectChildSaved;\
	CWnd *pWndChild = GetWindow(GW_CHILD);\
	while (pWndChild)\
	{\
		rectChildSaved = listRect.GetNext(mp);\
		rectChildSaved.left = rectDlgNow.left + (int)((rectChildSaved.left - rectDlgSaved.left)*fRateScaleX);\
		rectChildSaved.top = rectDlgNow.top + (int)((rectChildSaved.top - rectDlgSaved.top)*fRateScaleY);\
		rectChildSaved.right = rectDlgNow.right + (int)((rectChildSaved.right - rectDlgSaved.right)*fRateScaleX);\
		rectChildSaved.bottom = rectDlgNow.bottom + (int)((rectChildSaved.bottom - rectDlgSaved.bottom)*fRateScaleY);\
		ScreenToClient(rectChildSaved);\
		pWndChild->MoveWindow(rectChildSaved);\
		pWndChild = pWndChild->GetNextWindow();\
	}\
}\
Invalidate(); 