#pragma once

class Open3rdExe
{
public:
	Open3rdExe()
	{
		InitParam();
	}
	virtual ~Open3rdExe()
	{
		Close();
	}
	void Run(LPCTSTR exePathName,CWnd *pWnd,LPCTSTR windowName = NULL)
	{
		Close();
		InitParam();
		m_windowName = windowName;
		m_parentWnd = pWnd;

		StartProcess(exePathName);
		if (m_apphwnd != NULL)
		{
			LONG style = GetWindowLong(m_apphwnd, GWL_STYLE);
			style &= ~WS_CAPTION;
			style &= ~WS_THICKFRAME;
			SetWindowLong(m_apphwnd, GWL_STYLE, style);

			::SetParent(m_apphwnd, m_parentWnd->m_hWnd);

			SetPos();

			::ShowWindow(m_apphwnd, SW_SHOW);

			::InvalidateRect(NULL,NULL,TRUE);
		}
		else
			AfxMessageBox(_T("没有找到相应的窗口，程序没有正确启动"));

	}
public:
	void Close()
	{
		if (NULL == m_apphwnd) return;
		::PostMessage(m_apphwnd,WM_CLOSE,0,0);
		Sleep(200);
		InitParam();
	}
	void OnSize()
	{
		if (m_parentWnd == NULL) return;
		if (m_parentWnd->GetWindow(GW_CHILD) == NULL) return;
		SetPos();
		//::PostMessage(m_apphwnd, WM_DRAWITEM, 0, 0);
	}
private:
	HWND    m_apphwnd;
	DWORD   m_3rdExePID;
	LPCTSTR m_windowName;
	HANDLE  m_hProcess;
	CWnd*   m_parentWnd;
	void InitParam()
	{
		m_apphwnd = NULL;
		m_3rdExePID = 0;
		m_hProcess = NULL;
		m_parentWnd = NULL;
		m_windowName = NULL;
	}
	static int CALLBACK EnumWindowsProc(HWND hwnd, LPARAM param)
	{
		Open3rdExe *pObj = (Open3rdExe*)param;
		if (NULL == pObj->m_hProcess) return false;
		DWORD pID;
		DWORD TpID = GetWindowThreadProcessId(hwnd, &pID);//get process id
		if (TpID == pObj->m_3rdExePID)
		{
			pObj->m_apphwnd = hwnd;//hwnd is the window handle
			while (1)
			{
				hwnd = GetParent(hwnd);
				if (NULL == hwnd) break;
				else pObj->m_apphwnd = hwnd;
			}
		}
		return true;
	}
	void StartProcess(LPCTSTR program)
	{
		PROCESS_INFORMATION processInfo;
		STARTUPINFO startupInfo;
		::ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.wShowWindow = FALSE;
		if (::CreateProcess(program, NULL,
			NULL,  // process security
			NULL,  // thread security
			FALSE, // no inheritance
			0,     // no startup flags
			NULL,  // no special environment
			NULL,  // default startup directory
			&startupInfo,
			&processInfo))
		{
			m_3rdExePID = processInfo.dwThreadId;
			m_hProcess = processInfo.hProcess;

			for (int i = 0; i < 150; i++)
			{
				Sleep(30);	
				if (m_windowName == NULL) ::EnumWindows(&EnumWindowsProc, (LPARAM)this);//Iterate all windows
				else m_apphwnd = FindWindow(NULL,m_windowName);

				if (m_apphwnd) break;
			}
		}
	}
	void SetPos()
	{
		if (NULL == m_parentWnd) return;
		if (NULL == m_apphwnd) return;

		CRect rc;
		m_parentWnd->GetWindowRect(&rc);
		int nWidth = rc.Width();
		int nHeight = rc.Height();
		m_parentWnd->ScreenToClient(rc);
		::SetWindowPos(m_apphwnd, HWND_TOP, rc.left, rc.top, nWidth, nHeight, SWP_SHOWWINDOW | SWP_HIDEWINDOW);
	}
};