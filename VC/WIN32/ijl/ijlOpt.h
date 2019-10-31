#pragma once
#include "ijl.h"
#include <string>
using namespace std;
typedef   IJLERR(WINAPI *IJLINIT)(JPEG_CORE_PROPERTIES* jcprops);
typedef   IJLERR(WINAPI *IJLWRITE)(JPEG_CORE_PROPERTIES* jcprops, IJLIOTYPE iotype);
typedef   IJLERR(WINAPI *IJLREAD)(JPEG_CORE_PROPERTIES* jcprops, IJLIOTYPE iotype);
typedef   IJLERR(WINAPI *IJLFREE)(JPEG_CORE_PROPERTIES* jcprops);

class Ijl
{
public:
	Ijl()
	{
		const TCHAR* dllPath;
#ifdef _WIN64
		dllPath = TEXT("ijl20x64.dll");
#else
		dllPath =  TEXT("ijl15.dll");
#endif
 
		HINSTANCE hdll = LoadLibrary(dllPath);
		if (NULL == hdll)  vaild(hdll, TEXT("加载ijl失败"));

		ijlInit = (IJLINIT)GetProcAddress(hdll, "ijlInit");
		vaild(ijlInit, TEXT("加载ijlInit失败"));
		ijlWrite = (IJLWRITE)GetProcAddress(hdll, "ijlWrite");
		vaild(ijlWrite, TEXT("加载ijlWrite失败"));
		ijlRead = (IJLREAD)GetProcAddress(hdll, "ijlRead");
		vaild(ijlRead, TEXT("加载ijlRead失败"));
		ijlFree = (IJLFREE)GetProcAddress(hdll, "ijlFree");
		vaild(ijlFree, TEXT("加载ijlFree失败"));

		if (ijlInit(&m_jcprops) != IJL_OK) vaild(NULL, TEXT("初始化jcprops失败"));
	}
	~Ijl()
	{
		if (NULL == ijlFree) return;
		ijlFree(&m_jcprops);
	}

	void Init(int width,int height,int channel)
	{
		ImgSize = width * height;

		m_jcprops.DIBWidth = width;
		m_jcprops.DIBHeight = height;
		if (channel == 1)//黑白
		{
			m_jcprops.DIBColor = IJL_G;
			m_jcprops.DIBChannels = 1;
		}
		else
		{
			m_jcprops.DIBColor = IJL_BGR;
			m_jcprops.DIBChannels = 3;
		}
		m_jcprops.DIBPadBytes = IJL_DIB_PAD_BYTES(m_jcprops.DIBWidth, m_jcprops.DIBChannels);

		m_jcprops.JPGWidth = m_jcprops.DIBWidth;
		m_jcprops.JPGHeight = m_jcprops.DIBHeight;
		m_jcprops.JPGChannels = m_jcprops.DIBChannels;
		m_jcprops.jprops.iotype = IJL_SETUP;
		m_jcprops.jquality = 80;
		if (channel == 1)
		{
			m_jcprops.JPGColor = IJL_G;
		}
		else
		{
			m_jcprops.JPGColor = IJL_YCBCR;
		}
		m_jcprops.JPGSubsampling = IJL_NONE;
	}
	void ResetJpgSize()
	{
		m_jcprops.JPGSizeBytes = ImgSize;
	}
	JPEG_CORE_PROPERTIES m_jcprops;
	IJLINIT  ijlInit;
	IJLWRITE ijlWrite;
	IJLFREE  ijlFree;
	IJLREAD  ijlRead;
private:
	int ImgSize;//宽度*高度
	void vaild(void *p,const TCHAR* err)
	{
		if (p != NULL) return;
		MessageBox(NULL, err, _T(""), MB_OK); exit(0);
	}
};



class DibToJpg
{
public:
	string m_JpgBuf;
	int m_jpgBufSize;
	void Init(int width,int height,int channel)
	{
		m_Ijl.Init(width,height,channel);
		m_JpgBuf.resize(width*height*channel);
		m_Ijl.m_jcprops.JPGBytes = (unsigned char*)&m_JpgBuf[0];
	}
	//dib转jpg流
	bool SaveToJPGBuffer(BYTE* pDibData)
	{
		m_Ijl.m_jcprops.DIBBytes = pDibData;
		m_Ijl.ResetJpgSize();

		if (m_Ijl.ijlWrite(&m_Ijl.m_jcprops, IJL_JBUFF_WRITEWHOLEIMAGE)) return false; 

		m_jpgBufSize = m_Ijl.m_jcprops.JPGSizeBytes;
		return true;
	}


	//dib存储为jpg文件
	void DibSaveToJPGFile(BYTE* pDibData,const  char* filePath)
	{
		m_Ijl.m_jcprops.DIBBytes = pDibData;
		m_Ijl.m_jcprops.JPGFile = filePath;
		if (m_Ijl.ijlWrite(&m_Ijl.m_jcprops, IJL_JFILE_WRITEWHOLEIMAGE)) throw string("保存jpg文件错误");
	}
private:
	Ijl m_Ijl;
};



struct DibOpt
{
	union BmpInfo
	{
		char  chBmpBuf[2048];
		BITMAPINFO   bmpInfo;
	};
	BmpInfo m_BmpInfo;
	HBRUSH m_hbrush;

	DibOpt()
	{
		m_hbrush = CreateSolidBrush(RGB(0,0,0));

		m_BmpInfo.bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_BmpInfo.bmpInfo.bmiHeader.biPlanes = 1;
		m_BmpInfo.bmpInfo.bmiHeader.biCompression = BI_RGB;
		for(int i = 0;i < 256;i++)
		{
			m_BmpInfo.bmpInfo.bmiColors[i].rgbBlue = i;
			m_BmpInfo.bmpInfo.bmiColors[i].rgbGreen = i;
			m_BmpInfo.bmpInfo.bmiColors[i].rgbRed = i;
			m_BmpInfo.bmpInfo.bmiColors[i].rgbReserved = i;
		}
	}
	void init(int width,int height,int channel)
	{
		m_BmpInfo.bmpInfo.bmiHeader.biWidth = width;
		m_BmpInfo.bmpInfo.bmiHeader.biHeight = height;
		m_BmpInfo.bmpInfo.bmiHeader.biBitCount = channel * 8;
	}
	void Show(BYTE *pDibBuf,HWND Wnd,int top = 0, int bottom = 0,int left = 0 , int right = 0)
	{
		if (NULL == pDibBuf) return;
		if (NULL == Wnd) return;

		HDC hdc= GetDC(Wnd);
		RECT wrect;
		GetClientRect(Wnd,&wrect);
		int wrectWidth = wrect.right - wrect.left;
		int wrectHeight = wrect.bottom - wrect.top;

		RECT rectDraw = wrect;
		rectDraw.left += left;
		rectDraw.top += top;
		rectDraw.right -= right;
		rectDraw.bottom -= bottom;		
		int rectDrawWidth = rectDraw.right - rectDraw.left;
		int rectDrawHeight = rectDraw.bottom - rectDraw.top;


		HDC MemDC = CreateCompatibleDC(hdc);
		if (NULL == MemDC) return;
		HBITMAP bitmap = CreateCompatibleBitmap(hdc, wrectWidth, wrectHeight);
		SelectObject(MemDC,bitmap);
		BitBlt(MemDC,0,0, wrectWidth, wrectHeight, hdc,left,right,SRCCOPY);


		FillRect(hdc,&rectDraw,m_hbrush);
		int DesX =0;
		int DesY = rectDrawHeight;
		int DesWidth = rectDrawWidth;
		int DesHeight = rectDrawHeight;
		double ratio = m_BmpInfo.bmpInfo.bmiHeader.biWidth/(double)m_BmpInfo.bmpInfo.bmiHeader.biHeight;
		int WidthRequest = int(ratio * wrectHeight);
		int HeightRequest = int(wrectWidth/ratio);
		if (rectDrawWidth < WidthRequest)
		{
			DesHeight = int(DesWidth / ratio);
			DesY = (rectDrawHeight - DesHeight)/2 + DesHeight;
		}
		else if (rectDrawHeight < HeightRequest)
		{
			DesWidth = int(DesHeight*ratio);
			DesX = (rectDrawWidth - DesWidth)/2;
		}

		::SetStretchBltMode(hdc, COLORONCOLOR);
		::StretchDIBits(hdc,
			DesX+left, DesY+top, DesWidth, -DesHeight,
			0, 0, m_BmpInfo.bmpInfo.bmiHeader.biWidth, m_BmpInfo.bmpInfo.bmiHeader.biHeight,
			pDibBuf, &m_BmpInfo.bmpInfo,
			DIB_RGB_COLORS, SRCCOPY);


		BitBlt(hdc,0,0, wrectWidth, wrectHeight,MemDC,0,0,SRCCOPY);


		ReleaseDC(Wnd,MemDC);
		ReleaseDC(Wnd,hdc);
	}
};


class JpgBufToDib
{
public:
	bool TransToDibBuffer(BYTE *pJpgBuf, int jpgBufSize)
	{
		m_Ijl.m_jcprops.JPGBytes = pJpgBuf;
		m_Ijl.m_jcprops.JPGSizeBytes = jpgBufSize;
		return ReadFun(IJL_JBUFF_READPARAMS, IJL_JBUFF_READWHOLEIMAGE);
	}
	void Show(HWND hwnd,int top = 0, int bottom = 0,int left = 0 , int right = 0)
	{
		if (m_DibBuf.size() == 0) return;
		m_DibOpt.Show((BYTE*)&m_DibBuf[0], hwnd,top,bottom,left,right);
	}
private:
	bool ReadFun(_IJLIOTYPE  readInfoType, _IJLIOTYPE  TransType)
	{
		if (m_Ijl.ijlRead(&m_Ijl.m_jcprops, readInfoType) != IJL_OK)  return false;
		if (m_Ijl.m_jcprops.JPGChannels == 1)
		{
			m_Ijl.m_jcprops.JPGColor = IJL_G;

			m_Ijl.m_jcprops.DIBColor = IJL_G;
			m_Ijl.m_jcprops.DIBChannels = 1;
		}
		else
		{
			m_Ijl.m_jcprops.JPGColor = IJL_YCBCR;

			m_Ijl.m_jcprops.DIBColor = IJL_BGR;
			m_Ijl.m_jcprops.DIBChannels = 3;
		}
		int width = m_Ijl.m_jcprops.JPGWidth;
		int height = m_Ijl.m_jcprops.JPGHeight;


		m_DibOpt.init(width,height,m_Ijl.m_jcprops.DIBChannels);

		size_t bufSize = m_DibOpt.m_BmpInfo.bmpInfo.bmiHeader.biBitCount * width * height;
		if (m_DibBuf.size() < bufSize) m_DibBuf.resize(bufSize);
		if (m_DibBuf.size() == 0) return false;

		m_Ijl.m_jcprops.jprops.iotype = IJL_SETUP;
		m_Ijl.m_jcprops.JPGSubsampling = IJL_NONE;
		m_Ijl.m_jcprops.DIBPadBytes = IJL_DIB_PAD_BYTES(width, m_Ijl.m_jcprops.DIBChannels);
		m_Ijl.m_jcprops.DIBWidth = width;
		m_Ijl.m_jcprops.DIBHeight = height;
		m_Ijl.m_jcprops.DIBBytes = (BYTE*)&m_DibBuf[0];

		return m_Ijl.ijlRead(&m_Ijl.m_jcprops, TransType) == IJL_OK;
	}

public:
	void GetInfo(int &width,int &height,int &nchanel)
	{
		width = m_DibOpt.m_BmpInfo.bmpInfo.bmiHeader.biWidth;
		height = m_DibOpt.m_BmpInfo.bmpInfo.bmiHeader.biHeight;
		nchanel = m_DibOpt.m_BmpInfo.bmpInfo.bmiHeader.biBitCount/8;
	}
	void GetDib(BYTE *&pDib,int &dibBufSize)
	{
		if (m_DibBuf.size() == 0) return;
		dibBufSize = m_Ijl.m_jcprops.JPGWidth * m_Ijl.m_jcprops.JPGHeight * m_Ijl.m_jcprops.DIBChannels;
		pDib = (BYTE*)&m_DibBuf[0];
	}
private:
	Ijl    m_Ijl;
	string m_DibBuf;
	DibOpt m_DibOpt;
};