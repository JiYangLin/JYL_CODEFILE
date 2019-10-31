#pragma once


#define  DECLARE_SinglePrj   SinglePrj* SinglePrj::m_pSinglePrj = NULL;


#define  SinglePrj_Init(szPropName)    SinglePrj *pSinglePrj = SinglePrj::GetInstance();\
	if (NULL != pSinglePrj)\
	if(!pSinglePrj->InitInstance(szPropName)) return FALSE;


#define SinglePrj_Set(hWnd)    {SinglePrj *pSinglePrj = SinglePrj::GetInstance(); \
	if (NULL != pSinglePrj)  pSinglePrj->OnInitDialog(hWnd);}


class SinglePrj
{
public:
	static SinglePrj *m_pSinglePrj;
	static SinglePrj *GetInstance()
	{
		if (NULL == m_pSinglePrj) m_pSinglePrj = new SinglePrj();
		
		return m_pSinglePrj;
	}

	bool InitInstance(CString   szPropName)
	{
		m_szPropName = szPropName;
		m_hValue = (HANDLE)1;


		HWND oldHWnd = NULL;
		EnumWindows(EnumWndProc,(LPARAM)&oldHWnd);

		if (NULL == oldHWnd) return true;
	

		::ShowWindow(oldHWnd,SW_SHOW);
		::SetForegroundWindow(oldHWnd);	
		return false;
	}
	void OnInitDialog(HWND hwnd)
	{
		SetProp(hwnd,m_szPropName,m_hValue);
	}
private:
	static BOOL CALLBACK EnumWndProc(HWND hwnd,LPARAM lParam)
	{
		SinglePrj * pSinglePrj = GetInstance();
		if (NULL == pSinglePrj) return false;
		
		HANDLE h = GetProp(hwnd,pSinglePrj->m_szPropName);
		if( h == pSinglePrj->m_hValue)
		{
			*(HWND*)lParam = hwnd;
			return false;
		}

		return true;
	}
private:
	SinglePrj(){}
	CString   m_szPropName;
	HANDLE	  m_hValue;
};