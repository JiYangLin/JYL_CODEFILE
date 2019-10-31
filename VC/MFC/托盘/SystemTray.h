#pragma once
#define WM_ICONNOTIFY      WM_USER + 9000
#define WM_TRAY_CLOSEPRJ   WM_USER + 9001

class CSystemTray : public CWnd
{
public:
	CSystemTray(){ memset(&m_tnd, 0, sizeof(m_tnd));m_bEnabled   = FALSE;}
    virtual ~CSystemTray()
	{
		TrayExit();
	}

	void TrayExit()
	{
		if (!m_bEnabled) return;
		m_tnd.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE, &m_tnd);
		m_bEnabled = FALSE;

		DestroyWindow();
	}
	BOOL Create(CWnd* pParent, LPCTSTR szToolTip, HICON icon)
	{
		VERIFY(m_bEnabled = ( GetVersion() & 0xff ) >= 4);
		if (!m_bEnabled) return FALSE;  
		ASSERT(_tcslen(szToolTip) <= 64);
		CWnd::CreateEx(0, AfxRegisterWndClass(0), _T(""), WS_POPUP, 0,0,10,10, NULL, 0);

		m_tnd.cbSize = sizeof(NOTIFYICONDATA);
		m_tnd.hWnd   = pParent->GetSafeHwnd()? pParent->GetSafeHwnd() : m_hWnd;  
		m_tnd.hIcon  = icon;
		m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		m_tnd.uCallbackMessage = WM_ICONNOTIFY; 
		_tcscpy_s(m_tnd.szTip, szToolTip);
		VERIFY(m_bEnabled = Shell_NotifyIcon(NIM_ADD, &m_tnd));
		return m_bEnabled;
	}

	BOOL  SetBubbleInfo(LPCTSTR pszTitle,LPCTSTR pszInfo,int timeout)
	{
		if (!m_bEnabled) return FALSE;

		m_tnd.uFlags = NIF_INFO;
		_tcscpy_s(m_tnd.szInfoTitle, pszTitle);
		_tcscpy_s(m_tnd.szInfo, pszInfo);
		m_tnd.uTimeout=timeout;
		m_tnd.dwInfoFlags=NIIF_NONE;

		return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
	}
private:
	BOOL            m_bEnabled; 
	NOTIFYICONDATA  m_tnd;
};


#define TRAY_DECLARE  CSystemTray m_tray;WINDOWPLACEMENT m_wp; afx_msg LRESULT onNotify(WPARAM wparam,LPARAM lparam){if(WM_LBUTTONDOWN==lparam){SetWindowPlacement(&m_wp); }return 0;}
#define TRAY_DECLARE_ColseFun  CSystemTray m_tray;WINDOWPLACEMENT m_wp; afx_msg LRESULT onNotify(WPARAM wparam,LPARAM lparam){if(WM_LBUTTONDOWN==lparam){SetWindowPlacement(&m_wp); }else if (WM_RBUTTONDOWN==lparam){CMenu menu;menu.CreatePopupMenu();menu.AppendMenu(MF_STRING,WM_TRAY_CLOSEPRJ,_T("关闭"));CPoint pt;GetCursorPos(&pt);menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);}return 0;}


#define TRAY_HIDE  ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);WINDOWPLACEMENT wp;wp.length=sizeof(WINDOWPLACEMENT);wp.flags=WPF_RESTORETOMAXIMIZED;wp.showCmd=SW_HIDE;SetWindowPlacement(&wp);

//如果是64位Relase程序，程序启动起来程序界面错误
//TXT这个参数用字面值
#define TRAY_INIT(TXT)   CenterWindow();  SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);	m_tray.Create(this,TXT,m_hIcon); GetWindowPlacement(&m_wp); TRAY_HIDE
#define TRAY_64Bug CString strTray; strTray.Format(_T("%d"),10);

//必须同时声明，且按钮TrayCloseBtnID无其他响应事件
#define TRAY_MSG(DLG,TrayCloseBtnID)   ON_MESSAGE(WM_ICONNOTIFY,onNotify) 	ON_COMMAND(WM_TRAY_CLOSEPRJ, &DLG::TrayColsePrj)   	ON_COMMAND(TrayCloseBtnID, &DLG::OnBnClickedTrayClose)
#define TrayFun  afx_msg void TrayColsePrj(){ SendMessage(WM_CLOSE,0,0);}  afx_msg void OnBnClickedTrayClose(){TRAY_HIDE}


#define TRAYEXIT m_tray.TrayExit();




//1.类声明
//TRAY_DECLARE;
//TrayFun;

//2.BEGIN_MESSAGE_MAP
//TRAY_MSG(类名,关闭按钮ID)

//3.OnInitDialog
//TRAY_INIT(EXENAME);

//4.DestroyWindow()
//TRAYEXIT;


//弹出气泡通知(低版本无效)
//m_tray.SetBubbleInfo("title","text",1);