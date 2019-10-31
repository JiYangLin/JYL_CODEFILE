#pragma once
#include "stdafx.h"
typedef int ParamType;

////һ���̼߳������ݵ����ݳ�
////��һ���߳������ݳ���ȡ����
////�ʺϴ���ʱ����Ҫ˳�������


enum
{
	ProcThrNum =  2,//�����߳�����
	ProcBufSize =  40,//��������
	PerThrBufSize = ProcBufSize/ProcThrNum,//ÿ���̵߳Ļ�������
};



class ProcBuf
{
public:
	ProcBuf()
	{
		m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

public:
	bool IsBufBusy()
	{
		return WAIT_TIMEOUT != WaitForSingleObject(m_hEvent,0);
	}
	void MarkBufBusy()
	{
		SetEvent(m_hEvent);
	}
	void MarkBufIdle()
	{
		ResetEvent(m_hEvent);
	}

public:
	void SetParam(ParamType par)
	{
		m_val = par;
	}
	void Proc()
	{		
		FILE *pf = fopen("F:\\xx.txt","a");
		if (NULL == pf) return;
		fprintf_s(pf,"%d\n",m_val);
		fclose(pf);
		Sleep(1500);
	}
	
private:
	HANDLE m_hEvent;
	
	ParamType m_val;	
};

class ProcThr
{
public:
	ProcThr()
	{
		m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);



        for (int i = 0 ; i < PerThrBufSize;++i)
        {
			m_BufVec[i] = new ProcBuf();
        }

		m_thr = ::CreateThread(NULL,0,ProcThread,this,0,NULL);	
	}
public:
	int GetAvaliBufCount()
	{
		int size = 0;
		for (int i = 0; i < PerThrBufSize;++i)
		{
			if (!m_BufVec[i]->IsBufBusy())
			{
				++size;
			}
		}
		return size;
	}
	bool Run(ParamType val)
	{		
		//��������
        ProcBuf *pBuf = NULL;
		for (int i = 0; i < PerThrBufSize;++i)
		{
			if (!m_BufVec[i]->IsBufBusy())
			{
				pBuf = m_BufVec[i];
			}
		}
        if (NULL == pBuf) return false;

		//���û������ݺ�״̬
		pBuf->SetParam(val);
		pBuf->MarkBufBusy();

		//��ǻ�����������
		SetEvent(m_hEvent);

		return true;
	}
private:
	static DWORD WINAPI ProcThread( LPVOID pParam )
	{
		ProcThr *pProc = (ProcThr*)pParam;
		pProc->Process();
		return 0;
	}
	void Process()
	{
		while(1)
		{
			//��ȡ�ʹ�������
			ProcBuf* pBuf = NULL;

			//��ȡ����
			for (int i = 0; i < PerThrBufSize;++i)
			{
				if (m_BufVec[i]->IsBufBusy())
				{
					pBuf = m_BufVec[i];
					continue;
				}
			}
			if (NULL == pBuf)
			{
				//�ȴ�������������
				WaitForSingleObject(m_hEvent, INFINITE);
				ResetEvent(m_hEvent);
				continue;
			}  


			//��������
		    pBuf->Proc();

			//���û���״̬
			pBuf->MarkBufIdle();
		}

	}
private:
	HANDLE m_thr;

    ProcBuf* m_BufVec[PerThrBufSize];

	HANDLE m_hEvent;//����/�����߳�
};



class ProcManager
{
	ProcThr* m_ProcThr[ProcThrNum];
	int m_thrUse;
public:
	ProcManager()
	{
		m_thrUse = 0;
		for (int i = 0; i < ProcThrNum;++i)
		{
			m_ProcThr[i] = new ProcThr();
		}
	}
	bool Run(ParamType val)
	{
		//���봦���߳�
		++m_thrUse;
		if (m_thrUse >= ProcThrNum) m_thrUse = 0;
		return m_ProcThr[m_thrUse]->Run(val);
	}
public:
	int GetAvaliBufCount()
	{
		//���㻺������
		int size = 0;
		for (int i = 0; i < ProcThrNum;++i)
		{
			size += m_ProcThr[i]->GetAvaliBufCount();
		}
		return size;
	}
	void WaitProcEnd()
	{
		while(1) if (GetAvaliBufCount()==ProcBufSize) return;
	}
};