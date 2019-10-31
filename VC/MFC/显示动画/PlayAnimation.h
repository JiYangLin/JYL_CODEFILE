#pragma once
#include <atlimage.h>
#include <vector>
using namespace std;

class CPlayAnimation
{
	vector<CImage*>  m_picVec;
	CWnd *m_pWnd;
	HANDLE m_thr;
	HANDLE m_hEvent;
public:
	CPlayAnimation()
	{
		m_thr = NULL;
		m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	~CPlayAnimation()
	{
		StopShow();
		::TerminateThread(m_thr, 0);
	}
	void Init(CWnd *pWnd,CString imgPath)
	{
		m_pWnd = pWnd;

		CFileFind finder;
		BOOL bFind = finder.FindFile(imgPath + _T("\\*.png"));
		while (bFind)
		{
			bFind = finder.FindNextFile();
			if (finder.IsDots() | finder.IsDirectory())
				continue;

			CString imgPath = finder.GetFilePath();
			
			CImage *pCImage = new CImage();
			pCImage->Load(imgPath);
			m_picVec.push_back(pCImage);
		}
		finder.Close();

		m_thr = CreateThread(NULL, 0, Run, this, 0, NULL);
	}
public:
	void StartShow()
	{
		SetEvent(m_hEvent);
	}
	void StopShow()
	{
		ResetEvent(m_hEvent);
	}
private:
	static DWORD WINAPI Run(LPVOID p)
	{
		CPlayAnimation *pCPlayAnimation = (CPlayAnimation*)p;
		pCPlayAnimation->RunThr();
		return 0;
	}
	void RunThr()
	{
		size_t pos = 0;
		size_t size = m_picVec.size();

		while (true)
		{
			WaitForSingleObject(m_hEvent, INFINITE);
			if (pos == size) pos = 0;

			ShowImg(pos);

			++pos;
			Sleep(200);
		}
	}
	void ShowImg(int pos)
	{
		if (pos >= m_picVec.size()) return;

		HDC hdc = m_pWnd->GetDC()->m_hDC;

		CRect rect;
		m_pWnd->GetClientRect(rect);
		::SetStretchBltMode(hdc, COLORONCOLOR);
		m_picVec[pos]->Draw(hdc, 0, 0, rect.Width(), rect.Height());
	}
};
