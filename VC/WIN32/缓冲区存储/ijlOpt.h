#pragma once
#include "ijl.h"
#include <string>
#include <atlstr.h>
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
		CString dllPath;
#ifdef _WIN64
		dllPath = _T("ijl20x64.dll");	
#else
		dllPath =  _T("ijl15.dll");
#endif
 
		HINSTANCE hdll = LoadLibrary(dllPath);
		if (NULL == hdll) {MessageBox(NULL,_T("º”‘ÿijl ß∞‹"),_T(""),MB_OK); exit(0);}

		CString err;
		ijlInit = (IJLINIT)GetProcAddress(hdll, "ijlInit");
		if (NULL == ijlInit) err += _T("º”‘ÿijlInit ß∞‹");
		ijlWrite = (IJLWRITE)GetProcAddress(hdll, "ijlWrite");
		if (NULL == ijlWrite) err += _T("º”‘ÿijlWrite ß∞‹");
		ijlRead = (IJLREAD)GetProcAddress(hdll, "ijlRead");
		if (NULL == ijlRead) err += _T("º”‘ÿijlRead ß∞‹");
		ijlFree = (IJLFREE)GetProcAddress(hdll, "ijlFree");
		if (NULL == ijlFree) err += _T("º”‘ÿijlFree ß∞‹");

		if (ijlInit(&m_jcprops) != IJL_OK) err += _T("≥ı ºªØjcprops ß∞‹");

		if (_T("") == err) return;
		MessageBox(NULL,err,_T(""),MB_OK);
		exit(0);
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
		if (channel == 1)//∫⁄∞◊
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
	int ImgSize;//øÌ∂»*∏ﬂ∂»
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
	void Relase()
	{
		m_JpgBuf.swap(string());
	}
	void DibSaveToJPGFile(BYTE* pDibData,const  char* filePath)
	{
		m_Ijl.m_jcprops.DIBBytes = pDibData;
		m_Ijl.m_jcprops.JPGFile = filePath;
		if (m_Ijl.ijlWrite(&m_Ijl.m_jcprops, IJL_JFILE_WRITEWHOLEIMAGE)) throw string("±£¥ÊjpgŒƒº˛¥ÌŒÛ");
	}
	bool SaveToJPGBuffer(BYTE* pDibData)
	{
		m_Ijl.m_jcprops.DIBBytes = pDibData;
		m_Ijl.ResetJpgSize();

		if (m_Ijl.ijlWrite(&m_Ijl.m_jcprops, IJL_JBUFF_WRITEWHOLEIMAGE)) return false; 

		m_jpgBufSize = m_Ijl.m_jcprops.JPGSizeBytes;
		return true;
	}
private:
	Ijl m_Ijl;
};