#pragma once

class BackThr
{
public:
	virtual void Process() = 0;

	//�ж��߳��Ƿ��ڿ���״̬
	bool ThreadIsIdle()
	{
		return WAIT_TIMEOUT == WaitForSingleObject(m_hEvent,0);
	}
	//�����̴߳���,ʹ��ǰ�ȵ���ThreadIsIdle()�ж��߳��Ƿ����
	void ThreadActive()
	{
		//���������߳�
		SetEvent(m_hEvent);
	}
public:
	BackThr()
	{
		m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_thr = ::CreateThread(NULL,0,ProcThread,this,0,NULL);
	}

	~BackThr()
	{
		::TerminateThread(m_thr,0);
		CloseHandle(m_thr);
	}
private:
	static DWORD WINAPI ProcThread( LPVOID pParam )
	{
		BackThr *pProcThread = (BackThr*)pParam;
		while (1)
		{
			WaitForSingleObject(pProcThread->m_hEvent, INFINITE);

			pProcThread->Process();

			ResetEvent(pProcThread->m_hEvent);
		}
		return 0;
	}
protected:
	HANDLE m_thr;
	HANDLE m_hEvent;
};
//class Smaple:public BackThr
//{
//	virtual void Process()
//	{
//		.....
//	}
//public:
//	void Run(....)
//	{
//		if (!ThreadIsIdle()) return;
//
//		..........
//
//		ThreadActive();
//	}
//};



class MyBool
{
public:
	MyBool()
	{
		m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
public:
	void SetTrue()
	{
		SetEvent(m_hEvent);
	}
	void SetFalse()
	{
		ResetEvent(m_hEvent);
	}
public:
	bool IsTrue()
	{
		return WAIT_TIMEOUT != WaitForSingleObject(m_hEvent,0);
	}
	bool IsFalse()
	{
		return WAIT_TIMEOUT == WaitForSingleObject(m_hEvent,0);
	}
private:
	HANDLE m_hEvent;
};