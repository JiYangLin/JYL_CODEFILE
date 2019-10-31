#pragma once
#include <Windows.h>
#include <list>
using namespace std;
#define MaxThdNum    10

template<typename ThrProc>
class Thrd
{
public:
	Thrd()
	{
		m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_thr = ::CreateThread(NULL,0,ProcThread,this,0,NULL);
	}

	~Thrd()
	{
	   Stop();
	}
	void Stop()
	{
		::TerminateThread(m_thr,0);
		CloseHandle(m_thr);
		m_thr = NULL;
		m_ThreadProc.Release();
	}
private:
	static DWORD WINAPI ProcThread( LPVOID pParam )
	{
		Thread *pProcThread = (Thread*)pParam;
		while (1)
		{
			WaitForSingleObject(pProcThread->m_hEvent, INFINITE);

			pProcThread->Process();

			ResetEvent(pProcThread->m_hEvent);
		}
		return 0;
	}

public:
	//�߳��еĴ�����
	void Process()
	{
		m_ThreadProc.Proc();
	}

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
	ThrProc m_ThreadProc;
	HANDLE m_thr;
	HANDLE m_hEvent;
	typedef ThrProc ThrProcType;
};

template<typename ThrType>
class ThdPool
{
public:
	ThdPool(int thrMaxNum = MaxThdNum):m_thrMaxNum(thrMaxNum){}
	~ThdPool(){ Release();}
	void SetThrMaxNum(int thrMaxNum)
	{
		m_thrMaxNum = thrMaxNum;
	}
	bool Run(typename ThrType::ThrProcType::ParamType t)
	{

		ThrType*  Thr = NULL;

		for (list<ThrType*>::iterator it = m_Thread.begin(); it != m_Thread.end(); ++it)
		{
			Thr = *it;
			if (Thr->ThreadIsIdle())
			{
				Thr->m_ThreadProc.PreProc(t);
				Thr->ThreadActive();
				return true;
			}
		}


		//���û�п����̣߳��򴴽����߳�
		if (m_Thread.size() >= m_thrMaxNum) return false; //�����߳�����

		Thr = new ThrType();
		m_Thread.push_back(Thr);
		Thr->m_ThreadProc.PreProc(t);
		Thr->ThreadActive();
		return true;
	}
	void Release()
	{
		for (list<ThrType*>::iterator it = m_Thread.begin(); it != m_Thread.end(); ++it)
		{ 
			delete (*it);
		}
		m_Thread.swap(list<ThrType*>());
	}
private:
	list<ThrType*> m_Thread;
	size_t m_thrMaxNum;
};






///===���Ͷ���
//typedef char Parm;
//
//class Process
//{
//public:
//	typedef Parm ParamType;//**��������
//	Process() :m_count(0) {}
//  void Release(){}//**�ͷŲ���
//	void Proc()//**�̴߳�����
//	{
//		int i = 3;
//		while (i)
//		{
//			Sleep(1000);
//			--i;
//			cout << m_count << "\t" << m_x << endl;
//			++m_count;
//		}
//	}
//	void PreProc(const ParamType& x)//**Ԥ���������
//	{
//		m_x = x;
//	}
//private:
//	int m_count;//�߳��д�����һֱʹ�õı���
//  int m_x;//�߳��д�����һֱʹ�õı��������ⲿ�ı�
//};
//typedef Thrd<Process> Thread;//**�߳�����



///===ʹ��
//ThdPool<Thread>  pool(2);//����2���߳�
//while (true)
//{
//	char x;
//	cin >> x;
//	pool.Run(x);
//}