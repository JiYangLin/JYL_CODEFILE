#pragma once
#include <Windows.h>
#include <opencv2\opencv.hpp>

class IProcFrame
{
public:
	virtual void ProcFrame(IplImage* frame) = 0;
};


class  CaptureOpt
{
public:
	HANDLE m_thr;
	CaptureOpt() :m_pCapture(NULL), m_thr(NULL),m_IProcFrame(NULL){}
	~CaptureOpt()
	{
		Relaese();
	}
	bool Run(IProcFrame*  _IProcFrame)
	{
		m_IProcFrame = _IProcFrame;
		m_pCapture = cvCaptureFromCAM(0);
		if (NULL == m_pCapture) return false;
		m_thr = ::CreateThread(NULL, 0, Show, this, 0, NULL);
		return true;
	}
	void Relaese()
	{
		TerminateThread(m_thr, 0);
		m_thr = NULL;
		cvReleaseCapture(&m_pCapture);
		m_pCapture = NULL;
	}
private:
	static DWORD WINAPI Show(LPVOID p)
	{
		CaptureOpt* pCap = (CaptureOpt*)p;
		pCap->Show();
		return 0;
	}
	void Show()
	{
		IplImage* frame = NULL; 
		while (1)
		{
			frame = cvQueryFrame(m_pCapture);	
			if (NULL == frame) continue;

			//处理
			m_IProcFrame->ProcFrame(frame);

		}
	}
private:
	CvCapture* m_pCapture;
	IProcFrame*  m_IProcFrame;
};


//对话框使用
//class CDlg : public CDialogEx, public IProcFrame
//{
//	virtual void ProcFrame(IplImage* frame)
//	{
//		//接口函数获取捕获的图像进行显示
//	}
//	CaptureOpt m_CaptureOpt;
//};
//启动
//m_CaptureOpt.Run(this);

//m_CaptureOpt.Relaese();