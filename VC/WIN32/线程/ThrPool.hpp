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
	//线程中的处理函数
	void Process()
	{
		m_ThreadProc.Proc();
	}

	//判断线程是否处于空闲状态
	bool ThreadIsIdle()
	{
		return WAIT_TIMEOUT == WaitForSingleObject(m_hEvent,0);
	}

	//激活线程处理,使用前先调用ThreadIsIdle()判断线程是否空闲
	void ThreadActive()
	{
		//激活运行线程
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


		//如果没有空闲线程，则创建新线程
		if (m_Thread.size() >= m_thrMaxNum) return false; //限制线程数量

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






///===类型定义
//typedef char Parm;
//
//class Process
//{
//public:
//	typedef Parm ParamType;//**参数类型
//	Process() :m_count(0) {}
//  void Release(){}//**释放操作
//	void Proc()//**线程处理函数
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
//	void PreProc(const ParamType& x)//**预处理传入参数
//	{
//		m_x = x;
//	}
//private:
//	int m_count;//线程中处理类一直使用的变量
//  int m_x;//线程中处理类一直使用的变量，由外部改变
//};
//typedef Thrd<Process> Thread;//**线程类型



///===使用
//ThdPool<Thread>  pool(2);//限制2个线程
//while (true)
//{
//	char x;
//	cin >> x;
//	pool.Run(x);
//}