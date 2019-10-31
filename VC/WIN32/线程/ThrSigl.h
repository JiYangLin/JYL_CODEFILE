#pragma once
#include "stdafx.h"
#include <list>
using namespace std;
typedef int ParamType;

//使用两个缓冲区，可变大小
//插入线程使用一个，处理线程使用一个
//当处理线程空闲时切换缓冲区
//适合数据有序存储

enum InsertUse
{
	InsertUse_A=0,
	InsertUse_B=1,
};

class ProcManager
{
private:
	//缓存
	list<ParamType> m_datBufA;
	list<ParamType> m_datBufB;
private:
	//插入数据使用
	InsertUse m_InsertUse;
	list<ParamType> *m_pDatBufInsert;

private:
	//处理线程
	list<ParamType> *m_pDatBufProc;
	int m_ProcBufCount;
	HANDLE m_thr;
	HANDLE m_hEvent;//挂起/激活线程
private:
	static DWORD WINAPI ProcThread( LPVOID pParam )
	{
		ProcManager *pProc = (ProcManager*)pParam;
		while(1)
		{
			WaitForSingleObject(pProc->m_hEvent, INFINITE);
			if (NULL != pProc->m_pDatBufProc) pProc->Process();
			ResetEvent(pProc->m_hEvent);
		}
		return 0;
	}
	void Process()
	{
		if (NULL == m_pDatBufProc) return;
		list<ParamType> &Dat =*m_pDatBufProc;
		for (auto it = Dat.begin(); it != Dat.end();++it)
		{
			ParamType val = *it;
			FILE *pf = fopen("F:\\xx.txt","a");
			if (NULL == pf) return;
			fprintf_s(pf,"%d\n",val);
			fclose(pf);
			Sleep(500);
		}

		Dat.clear();
	}
private:
	void RunProc()
	{
		SetEvent(m_hEvent);
	}
	bool ProcThrIdle()
	{
		return WAIT_TIMEOUT == WaitForSingleObject(m_hEvent, 0);
	}

public:
	ProcManager()
	{	
		m_pDatBufProc = NULL;
		m_InsertUse = InsertUse_A;
		m_pDatBufInsert = &m_datBufA;
		m_ProcBufCount = 0;

		m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_thr = ::CreateThread(NULL,0,ProcThread,this,0,NULL);	
	}
	void Run(ParamType val)
	{
		m_pDatBufInsert->push_back(val);

		if (!ProcThrIdle()) return;

		Run();
	}
private:
	void Run()
	{
		//切换缓冲区，启动线程运行
		m_ProcBufCount = m_pDatBufInsert->size();
		if (m_InsertUse == InsertUse_A)
		{
			m_pDatBufProc =  &m_datBufA;
			m_pDatBufInsert = &m_datBufB;
			m_InsertUse = InsertUse_B;
		}
		else
		{
			m_pDatBufProc =  &m_datBufB;
			m_pDatBufInsert = &m_datBufA;
			m_InsertUse = InsertUse_A;
		}
		RunProc();		
	}
public:
	int GetBufSize()
	{
		if (NULL == m_pDatBufInsert) return m_ProcBufCount;
		return m_pDatBufInsert->size() + m_ProcBufCount;
	}
	void WaitProcEnd()
	{
		while(1) if (ProcThrIdle()) break;
		if(m_pDatBufInsert->size() > 0) Run();
		while(1) if (ProcThrIdle()) break;
	}
};