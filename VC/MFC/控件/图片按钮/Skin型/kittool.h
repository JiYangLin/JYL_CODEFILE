#ifndef _KITTOOL_H_
#define _KITTOOL_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "gdiplus.h"
#include <tchar.h>

#pragma warning(disable : 4996)

enum ktRotate{
	ktRotateNone,
	ktRotate90,
	ktRotate180,
	ktRotate270,
	ktRotate180FlipX,
	ktRotate180FlipY
};

class CKitTool
{
public:
	CKitTool();
	~CKitTool();
	static COLORREF StringToCOLORREF(LPTSTR lpszBuff);
	static BOOL StringToFontInfo(LPTSTR lpszBuff, LOGFONT *pFont);
	static BOOL StringToRect(LPTSTR lpBuff, LPRECT lpRect);
	static BOOL StringToRectT(LPTSTR lpBuff, LPRECT lpRect);
	static BOOL StringToPoint(LPTSTR lpBuff, LPPOINT lpPoint);
	static void GetFullPath(LPTSTR lpFileName, LPTSTR lpFullPath);
	static void GetModuleDir(LPTSTR lpPath);
	static void ScaleRect(const RECT &rectDst, const RECT &rectSrc, RECT &rectResult);
	static BOOL CreateCircleCursor(HCURSOR &hCursor, int nSize);
	static BOOL CreateImageCursor(HCURSOR &hCursor, LPTSTR filename, 
										int w, int h, int xPot, int yPot);
	static BOOL CreateCircleCursor(HCURSOR &hCursor, int nSize, LPCTSTR  icon);
	static void GetMaskBitmaps(HBITMAP hSourceBitmap,COLORREF clrTransparent,
							   HBITMAP &hAndMaskBitmap,HBITMAP &hXorMaskBitmap);
	static HCURSOR CreateCursorFromBitmap(HBITMAP hSourceBitmap,COLORREF clrTransparent,
										  DWORD   xHotspot,DWORD   yHotspot);
	static HRESULT GetStreamSize(IStream* pStream,long* pdwSize);
	static HRESULT SetStreamPos(IStream* pStream,DWORD dwOrigin,DWORD dwMove,long* pdwSize);
	static long __strstr(const void* str, const long lenght, 
		const void* strSearch, const long lenghtSearch);
	static void SizeToSizeT(CRect &rectFix, CSize szBox, CSize szSrc);
	static void SizeToSize(int x1, int y1, int x2, int y2, int &left, int &top, int &width, int &height);
	static void SizeToSize(CSize s1, CSize s2, CRect &rectDst);
	static void SizeToSize1(int x1, int y1, int x2, int y2, int &left, int &top, int &width, int &height);
	static void SizeToSize1(CSize s1, CSize s2, CRect &rectDst);
	static long _round(double a);
	static int _round(float a);
	static void GetSysTimerString(LPTSTR str);
	static BOOL IsFileExist(LPTSTR  filename);

	static int GetFileExtName(LPTSTR lpExtName, LPCTSTR lpPath);
	static int GetFilePath(LPTSTR lpPath, LPCTSTR lpFullPath);

	static void DeleteDir(LPCTSTR lpszDir);

	static void RotateRectInBox(CRect &rect, const CRect box, ktRotate rotate)
	{
		CRect r = rect;
		switch (rotate)
		{
		case ktRotateNone:
			break;
		case ktRotate90:
			rect.SetRect(box.bottom - r.bottom, r.left, 
				box.bottom - r.top, box.right);
			break;
		case ktRotate180:
			break;
		case ktRotate270:
			rect.SetRect(r.top, box.right - r.right, 
				r.bottom, box.right - r.left);
			break;
		case ktRotate180FlipX:
			rect.SetRect(box.right - r.left, r.top, box.right - r.right, r.bottom);
			break;
		case ktRotate180FlipY:
			break;
		}
	}
};

inline void CKitTool::DeleteDir(LPCTSTR lpszDir)
{
	CFileFind   ff;   
	BOOL   bFound;   
	CString s;

	if (_tcslen(lpszDir) < 4) return;

	s.Format(_T("%s\\*.*"), lpszDir);
	bFound   =   ff.FindFile(s);   
	while(bFound)   
	{   
		bFound   =   ff.FindNextFile();   
		CString   sFilePath   =   ff.GetFilePath();   

		if(ff.IsDirectory())   
		{   
			if(!ff.IsDots())   
				DeleteDir(sFilePath);   
		}   
		else   
		{   
			if(ff.IsReadOnly())   
			{   
				SetFileAttributes(sFilePath,   FILE_ATTRIBUTE_NORMAL);   
			}   
			DeleteFile(sFilePath);   
		}   

	}   
	ff.Close();   
	SetFileAttributes(lpszDir,   FILE_ATTRIBUTE_NORMAL);   
	RemoveDirectory(lpszDir);   
}

inline COLORREF CKitTool::StringToCOLORREF(LPTSTR lpszBuff)
{
	LPCTSTR setp = TEXT(",");
	BYTE cr = _ttoi(_tcstok(lpszBuff, setp));
	BYTE cg = _ttoi(_tcstok(NULL, setp));
	BYTE cb = _ttoi(_tcstok(NULL, setp));

	return RGB(cr, cg, cb);
}
inline int CKitTool::GetFilePath(LPTSTR lpPath, LPCTSTR lpFullPath)
{
	LPCTSTR lpEnd = _tcsrchr(lpFullPath, '\\');
	int nPos = (lpEnd - lpFullPath + 1);
	return nPos;
}

inline int CKitTool::GetFileExtName(LPTSTR lpExtName, LPCTSTR lpPath)
{
	LPCTSTR lpStart, lpEnd;
	lpStart = _tcsrchr(lpPath, '\\');
	lpEnd = _tcsrchr(lpPath, '.');
	if (lpStart == NULL || lpEnd == NULL)
		return 0;
//	_tcsncpy_s(lpExtName, MAX_PATH, lpStart + 1, lpEnd - lpStart - 1); 
	return lpEnd - lpStart - 1;
}

inline BOOL CKitTool::StringToFontInfo(LPTSTR lpszBuff, LOGFONT* pFont)
{
	ASSERT(pFont);

	LPCTSTR setp = TEXT(",");	
	ZeroMemory(pFont, sizeof(LOGFONT));
	lstrcpy(pFont->lfFaceName, _tcstok(lpszBuff, setp));
	pFont->lfHeight		= _ttoi(_tcstok(lpszBuff, setp));
	pFont->lfWeight		= _ttoi(_tcstok(lpszBuff, setp)) ? FW_BOLD : FW_NORMAL;
	pFont->lfItalic		= _ttoi(_tcstok(lpszBuff, setp));
//	pFont->lfQuality	= _ttoi(_tcstok(lpszBuff, setp)) ? CLEARTYPE_QUALITY : ANTIALIASED_QUALITY;
	pFont->lfCharSet	= DEFAULT_CHARSET;

	return TRUE;
}
inline BOOL CKitTool::IsFileExist(LPTSTR  filename)
{
	WIN32_FIND_DATA fd;
	HANDLE hResult = ::FindFirstFile(filename, &fd);
	return hResult == INVALID_HANDLE_VALUE ? FALSE : TRUE;
}

inline long CKitTool::_round(double a)
{
	return (long)(a + 0.5);
}

inline int CKitTool::_round(float a)
{
	return (int)(a + 0.5);
}

inline void CKitTool::SizeToSize(int x1, int y1, int x2, int y2, int &left, int &top, int &width, int &height)
{
	float fw = (float)x2 / x1;
	float fh = (float)y2 / y1;
	float f = fw > fh ? fh : fw;
	width = _round(x1 * f);
	left = _round((float)(x2 - width) / 2);
	height = _round(y1 * f);
	top = _round((float)(y2 - height) / 2);
}

inline void CKitTool::SizeToSize(CSize s1, CSize s2, CRect &rectDst)
{
	int left, top, width, height;
	SizeToSize(s1.cx, s1.cy, s2.cx, s2.cy, left, top, width, height);
	rectDst.SetRect(left, top, left+width, top+height);
}

inline void CKitTool::SizeToSize1(int x1, int y1, int x2, int y2, int &left, int &top, int &width, int &height)
{
	float f2 = (float)x2 / y2;
	float f1 = (float)x1 / y1;
	if (f1 < f2)
	{
		width = x1;
		height = (int)(x1 / f2);
		left = 0;
		top = (y1 - height) / 2;
	}
	else
	{
		height = y1;
		width = (int)(y1 * f2);
		left = (x1 - width) / 2;
		top = 0;
	}
}

inline void CKitTool::SizeToSize1(CSize s1, CSize s2, CRect &rectDst)
{
	int left, top, width, height;
	SizeToSize1(s1.cx, s1.cy, s2.cx, s2.cy, left, top, width, height);
	rectDst.SetRect(left, top, left+width, top+height);
}

inline void CKitTool::SizeToSizeT(CRect &rectFix, CSize szBox, CSize szSrc)
{
	float fw = (float)szBox.cx / szSrc.cx;
	float fh = (float)szBox.cy / szSrc.cy;
	float f = fw > fh ? fw : fw;
	int left = int((szSrc.cx * f - szBox.cx) / 2);
	int top =int((szSrc.cy * f - szBox.cy) / 2);
	int right = szSrc.cx - left;
	int bottom = szSrc.cy - top;
	rectFix.SetRect(left, top, right, bottom);
}

inline long CKitTool::__strstr(const void *str, const long lenght, 
							   const void *strSearch, const long lenghtSearch)
{
	ASSERT(str);
	ASSERT(strSearch);
	long pos = 0;
	long result = 0;
	BYTE *str1 = (BYTE*)str;
	BYTE *str2 = (BYTE*)strSearch;
	for (long i = 0; i < lenght; i++)
	{
		if (str1[i] == str2[pos])
		{
			result=i;
			pos++;
			if (pos == lenghtSearch)
				break;
		}
		else
		{
			result=0;
			pos = 0;
		}
	}
	return result - lenghtSearch + 1;
}


//////////////////////////////////////////////////////////////////////
//  CreateCursorFromBitmap 
//  Function to create a cursor from HBITMAP. 
//  Pass bitmaps having standard cursor sizes like 16*16, 32*32...
//////////////////////////////////////////////////////////////////////
inline HCURSOR CKitTool::CreateCursorFromBitmap(HBITMAP hSourceBitmap,
											 COLORREF clrTransparent,
											 DWORD   xHotspot,DWORD   yHotspot)
{
	HCURSOR hRetCursor = NULL;

	do
	{
		if(NULL == hSourceBitmap)
		{
			break;
		}

		//Create the AND and XOR masks for the bitmap
		HBITMAP hAndMask = NULL;
		HBITMAP hXorMask = NULL;
		GetMaskBitmaps(hSourceBitmap,clrTransparent,hAndMask,hXorMask);
		if(NULL == hAndMask || NULL == hXorMask)
		{
			break;
		}

		//Create the cursor using the masks and the hotspot values provided
		ICONINFO iconinfo = {0};
		iconinfo.fIcon		= FALSE;
		iconinfo.xHotspot	= xHotspot;
		iconinfo.yHotspot	= yHotspot;
		iconinfo.hbmMask	= hAndMask;
		iconinfo.hbmColor	= hXorMask;

		hRetCursor = ::CreateIconIndirect(&iconinfo);

	}
	while(0);

	return hRetCursor;
}


//////////////////////////////////////////////////////////////////////
//  GetMaskBitmaps
//  Function to AND and XOR masks for a cursor from a HBITMAP. 
//////////////////////////////////////////////////////////////////////
inline void CKitTool::GetMaskBitmaps(HBITMAP hSourceBitmap, COLORREF clrTransparent, 
								  HBITMAP &hAndMaskBitmap, HBITMAP &hXorMaskBitmap)
{
	HDC hDC					= ::GetDC(NULL);
	HDC hMainDC				= ::CreateCompatibleDC(hDC); 
	HDC hAndMaskDC			= ::CreateCompatibleDC(hDC); 
	HDC hXorMaskDC			= ::CreateCompatibleDC(hDC); 

	//Get the dimensions of the source bitmap
	BITMAP bm;
	::GetObject(hSourceBitmap,sizeof(BITMAP),&bm);

	
	hAndMaskBitmap	= ::CreateCompatibleBitmap(hDC,bm.bmWidth,bm.bmHeight);
	hXorMaskBitmap	= ::CreateCompatibleBitmap(hDC,bm.bmWidth,bm.bmHeight);

	//Select the bitmaps to DC
	HBITMAP hOldMainBitmap = (HBITMAP)::SelectObject(hMainDC,hSourceBitmap);
	HBITMAP hOldAndMaskBitmap	= (HBITMAP)::SelectObject(hAndMaskDC,hAndMaskBitmap);
	HBITMAP hOldXorMaskBitmap	= (HBITMAP)::SelectObject(hXorMaskDC,hXorMaskBitmap);

	//Scan each pixel of the souce bitmap and create the masks
	COLORREF MainBitPixel;
	for(int x=0;x<bm.bmWidth;++x)
	{
		for(int y=0;y<bm.bmHeight;++y)
		{
			MainBitPixel = ::GetPixel(hMainDC,x,y);
			if(MainBitPixel == clrTransparent)
			{
				::SetPixel(hAndMaskDC,x,y,RGB(255,255,255));
				::SetPixel(hXorMaskDC,x,y,RGB(0,0,0));
			}
			else
			{
				::SetPixel(hAndMaskDC,x,y,RGB(0,0,0));
				::SetPixel(hXorMaskDC,x,y,MainBitPixel);
			}
		}
	}
	
	::SelectObject(hMainDC,hOldMainBitmap);
	::SelectObject(hAndMaskDC,hOldAndMaskBitmap);
	::SelectObject(hXorMaskDC,hOldXorMaskBitmap);

	::DeleteDC(hXorMaskDC);
	::DeleteDC(hAndMaskDC);
	::DeleteDC(hMainDC);

	::ReleaseDC(NULL,hDC);
}

inline BOOL CKitTool::CreateImageCursor(HCURSOR &hCursor, LPTSTR filename, 
										int w, int h, int xPot, int yPot)
{
	using namespace Gdiplus;
	CString sFile = filename;
	Image img(sFile.AllocSysString());
	if (img.GetLastStatus())
		return FALSE;
	int wCur = w < 1 ? img.GetWidth() : w;
	int hCur = h < 1 ? img.GetHeight() : h;

	HDC hdcMain = ::GetDC(NULL);
	HDC hdc = CreateCompatibleDC(hdcMain);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcMain, wCur, hCur);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdc, hBitmap);

	Graphics g(hdc);
	Rect rect(0, 0, wCur, hCur);
	g.DrawImage(&img, rect, 0, 0, img.GetWidth(), img.GetHeight(), UnitPixel);
	g.ReleaseHDC(hdc);

	::SelectObject(hdc,hOldBitmap);
	::DeleteDC(hdc);
	::ReleaseDC(NULL, hdcMain);

	if (hCursor != NULL) DestroyCursor(hCursor);
	hCursor = CreateCursorFromBitmap(hBitmap,RGB(0,0,0), xPot, yPot);
	DeleteObject(hBitmap);
	if (hCursor)
		return TRUE;
	else
		return FALSE;
}

inline BOOL CKitTool::CreateCircleCursor(HCURSOR &hCursor, int nSize, LPCTSTR  icon)
{
	using namespace Gdiplus;
	CString sFile = icon;
	Image img(sFile.AllocSysString());
	int w = img.GetWidth();
	int h = img.GetHeight();
	int wCur = nSize / 2 < w ? nSize / 2 + w : nSize / 2;
	int hCur = nSize / 2 < h ? nSize / 2 + h : nSize / 2;

	HDC hdcMain = ::GetDC(NULL);
	HDC hdc = CreateCompatibleDC(hdcMain);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcMain, wCur, hCur);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdc, hBitmap);
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	Ellipse(hdc, 0, 0, nSize, nSize);

	Graphics g(hdc);
	g.DrawImage(&img, nSize /2 , nSize / 2, w, h);
	g.ReleaseHDC(hdc);

	//::MoveToEx(hdc, nSize / 2 - 4, nSize / 2, NULL);
	//::LineTo(hdc, nSize / 2 + 4, nSize / 2);
	//MoveToEx(hdc, nSize / 2, nSize / 2 - 4, NULL);
	//LineTo(hdc, nSize / 2, nSize / 2 + 4);
	//MoveToEx(hdc, 0, 0, NULL);

	::SelectObject(hdc,hOldBrush);
	::SelectObject(hdc,hOldPen);
	::SelectObject(hdc,hOldBitmap);

	::DeleteObject(hBrush);
	::DeleteObject(hPen);
	::DeleteDC(hdc);
	::ReleaseDC(NULL, hdcMain);

	if (hCursor != NULL) DestroyCursor(hCursor);
	hCursor = CreateCursorFromBitmap(hBitmap,RGB(0,0,0), nSize / 2, nSize / 2);
	DeleteObject(hBitmap);
	if (hCursor)
		return TRUE;
	else
		return FALSE;
}

inline BOOL CKitTool::CreateCircleCursor(HCURSOR &hCursor, int nSize)
{
	HDC hdcMain = ::GetDC(NULL);
	HDC hdc = CreateCompatibleDC(hdcMain);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcMain, nSize, nSize);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdc, hBitmap);
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	Ellipse(hdc, 0, 0, nSize, nSize);

	::MoveToEx(hdc, nSize / 2 - 4, nSize / 2, NULL);
	::LineTo(hdc, nSize / 2 + 4, nSize / 2);
	MoveToEx(hdc, nSize / 2, nSize / 2 - 4, NULL);
	LineTo(hdc, nSize / 2, nSize / 2 + 4);
	MoveToEx(hdc, 0, 0, NULL);

	::SelectObject(hdc,hOldBrush);
	::SelectObject(hdc,hOldPen);
	::SelectObject(hdc,hOldBitmap);

	::DeleteObject(hBrush);
	::DeleteObject(hPen);
	::DeleteDC(hdc);
	::ReleaseDC(NULL, hdcMain);

	if (hCursor != NULL) DestroyCursor(hCursor);
	hCursor = CreateCursorFromBitmap(hBitmap,RGB(0,0,0), nSize / 2, nSize / 2);
	DeleteObject(hBitmap);
	if (hCursor)
		return TRUE;
	else
		return FALSE;
}

inline BOOL CKitTool::StringToPoint(LPTSTR lpBuff, LPPOINT lpPoint)
{
	LPTSTR setp = TEXT(",");
	LPTSTR lpResult = _tcstok(lpBuff, setp);
	lpPoint->x = _ttoi(lpResult);
	lpPoint->y = _ttoi(_tcstok(NULL, setp));
	return TRUE;
}

inline BOOL CKitTool::StringToRect(LPTSTR lpBuff, LPRECT lpRect)
{
	LPTSTR setp = TEXT(",");
	LPTSTR lpResult = _tcstok(lpBuff, setp);
	int left = _ttoi(lpResult);
	int top = _ttoi(_tcstok(NULL, setp));
	int right = _ttoi(_tcstok(NULL, setp));
	int bottom = _ttoi(_tcstok(NULL, setp));
	lpRect->left = left;
	lpRect->top = top;
	lpRect->right = right;
	lpRect->bottom = bottom;
	return TRUE;
}

inline BOOL CKitTool::StringToRectT(LPTSTR lpBuff, LPRECT lpRect)
{
	LPTSTR setp = TEXT(",");
	LPTSTR lpResult = _tcstok(lpBuff, setp);
	int left = _ttoi(lpResult);
	int top = _ttoi(_tcstok(NULL, setp));
	int width = _ttoi(_tcstok(NULL, setp));
	int height = _ttoi(_tcstok(NULL, setp));
	lpRect->left = left;
	lpRect->top = top;
	lpRect->right = lpRect->left + width;
	lpRect->bottom = lpRect->top + height;
	return TRUE;
}

inline void CKitTool::GetModuleDir(LPTSTR lpPath)
{
	TCHAR szModule[MAX_PATH];
	LPTSTR lpszDest;

	GetModuleFileName(NULL, szModule, MAX_PATH);
	lpszDest = _tcsrchr(szModule, '\\');
	int nPos = (lpszDest - szModule + 1);
//	_tcsncpy_s(lpPath, MAX_PATH, szModule, nPos);
//	_tcsncpy(lpPath, MAX_PATH, szModule, nPos);
}


inline void CKitTool::GetFullPath(LPTSTR lpFileName, LPTSTR lpFullPath)
{
	GetModuleDir(lpFullPath);
//	_tcscat_s(lpFullPath, MAX_PATH, lpFileName);
}

inline void CKitTool::ScaleRect(const RECT &rectDst, const RECT &rectSrc, RECT &rectResult)
{
	int wDest, hDest, wSrc, hSrc;
	int width, height, left, top;
	float fw, fh, f;

	wDest = rectDst.right - rectDst.left;
	hDest = rectDst.bottom - rectDst.top;
	wSrc = rectSrc.right - rectSrc.left;
	hSrc = rectSrc.bottom - rectSrc.top;

	fw = (float)wDest / wSrc;
	fh = (float)hDest / hSrc;
	f = fw > fh ? fh : fw;
	width = (int)(wSrc * f);
	left = (int)(wDest - width) / 2;
	height = (int)(hSrc * f);
	top = (int)(hDest - height) / 2;
	rectResult.left = left;
	rectResult.top = top;
	rectResult.right = left + width;
	rectResult.bottom = top + height;
}

//设置IStream对象的指针
inline HRESULT CKitTool::SetStreamPos(IStream* pStream,DWORD dwOrigin,DWORD dwMove,long* pdwSize)
{
 if(pStream==NULL)
  return E_FAIL;

 //
 LARGE_INTEGER li;
 li.HighPart=0;li.LowPart=dwMove;
 ULARGE_INTEGER li2;
 HRESULT hr=pStream->Seek(li,dwOrigin,&li2);
 if(FAILED(hr))
  return hr;
 
 if(pdwSize!=NULL)
  *pdwSize=(long)li2.LowPart;
 return S_OK;
}

//取得IStream对象中数据的大小
inline HRESULT CKitTool::GetStreamSize(IStream* pStream,long* pdwSize)
{
 return SetStreamPos(pStream,STREAM_SEEK_END,0,pdwSize);
}

#endif