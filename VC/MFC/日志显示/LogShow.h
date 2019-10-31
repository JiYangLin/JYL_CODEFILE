#pragma once
#include <vector>
using namespace std;

#define  MAX_LOG_DAT_COUNT_Thr        3000
#define  LOG_DAT_DEl_Pos              2000 
	
#define ADD_LOG_TIME  true

inline void WriteLog(CString line,bool addTime = false)
{
	CString Path;
	GetCurrentDirectory(MAX_PATH,Path.GetBuffer(MAX_PATH));
	Path.ReleaseBuffer();
	Path += _T("\\Log\\");
	SHCreateDirectoryEx(NULL,Path,NULL);

	CString Name = CTime::GetCurrentTime().Format(_T("%Y-%m-%d.txt"));

	CString filePathName = Path + Name;
	CString msLine;
	CString time = CTime::GetTickCount().Format(_T("%Y年%m月%d日  %H:%M:%S : "));

	if (addTime) msLine = time + line + _T('\n');
	else msLine = line + _T('\n');
	

	FILE *pf = NULL;
	fopen_s(&pf,CStringA(filePathName).GetBuffer(),"a");
	if (NULL == pf) return;
	fprintf_s(pf,"%s",CStringA(msLine).GetBuffer());
	fclose(pf);
}



struct LineInfo
{
	LineInfo(CString  &_strLine,COLORREF _clr):clr(_clr){
	strLine = CTime::GetTickCount().Format(_T("%Y年%m月%d日  %H:%M:%S : ")) + _strLine;
	}
	CString  strLine;
	COLORREF clr;
};
class CLogShow
{
	COLORREF            m_BkClr;
	CWnd               *m_pWnd;
	vector<LineInfo>    m_LineInfo;
public:
	CLogShow():m_pWnd(NULL),m_BkClr(RGB(0,0,0)){}
	void Init(CWnd *pWnd,COLORREF  _BkClr)
	{
		m_pWnd = pWnd;
		m_BkClr = _BkClr;
	}
public:
	void OutputInfo(CString info)
	{
		 m_LineInfo.push_back(LineInfo(info,RGB(0,255,0)));
		 WriteLog(_T("[信息]: ") + m_LineInfo.back().strLine);
		 Flush();
		 CheckDatSize();
	}
	void OutputWarning(CString warning)
	{
		m_LineInfo.push_back(LineInfo(warning,RGB(255,255,0)));
		WriteLog(_T("[警告]: ") + m_LineInfo.back().strLine);
		Flush();
		CheckDatSize();
	}
	void OutputErro(CString erro)
	{
		m_LineInfo.push_back(LineInfo(erro,RGB(255,0,0)));
		WriteLog(_T("[错误]: ") + m_LineInfo.back().strLine);
		Flush();
		CheckDatSize();
	}
	void Clear()
	{
		m_LineInfo.clear();
	}
public:
	void Flush()
	{
		if (NULL == m_pWnd) return;
	
		CDC *pDC= m_pWnd->GetDC();
		CRect wrect;
		m_pWnd->GetClientRect(&wrect);

		CDC MemDC;
		MemDC.CreateCompatibleDC(pDC);
		if (NULL == MemDC.m_hDC) return;	
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(pDC,wrect.Width(),wrect.Height());
		MemDC.SelectObject(&bitmap);
		MemDC.BitBlt(0,0,wrect.Width(),wrect.Height(),pDC,0,0,SRCCOPY);

		CBrush bk(m_BkClr);
		MemDC.FillRect(wrect,&bk);
		DrawLog(MemDC,wrect.Height());

		pDC->BitBlt(0,0,wrect.Width(),wrect.Height(),&MemDC,0,0,SRCCOPY);
		m_pWnd->ReleaseDC(&MemDC);
		m_pWnd->ReleaseDC(pDC);	
	}
	void DrawLog(CDC &MemDC,int WndHeight)
	{
		if (0 == m_LineInfo.size()) return;
	

		TEXTMETRIC tm;
		MemDC.GetTextMetrics(&tm);  

		MemDC.SetBkMode(TRANSPARENT);
		
		int TxtHeight =    tm.tmHeight + 10;
        int MaxLineCount = WndHeight/TxtHeight - 2;
		if (MaxLineCount < 0) MaxLineCount = 0;


		int size = int(m_LineInfo.size());
		int startPos = 0;
		int endPos = size-1;
		if (size > MaxLineCount) startPos = size - MaxLineCount;
		if (startPos < 0) startPos = 0;
	

		CPoint pt;
		pt.x = 10;
		pt.y = 0;
		for (int i = startPos ; i <= endPos;++i)
		{
			pt.y += TxtHeight;
			MemDC.SetTextColor(m_LineInfo[i].clr);
			MemDC.TextOut(pt.x,pt.y,m_LineInfo[i].strLine);
		}
	}
private:
	void CheckDatSize()
	{
		if(m_LineInfo.size() < MAX_LOG_DAT_COUNT_Thr) return;

		auto iterBegin = m_LineInfo.begin();
		auto iterEnd = iterBegin;
		iterEnd += LOG_DAT_DEl_Pos;

		m_LineInfo.erase(iterBegin,iterEnd);
	}
};

