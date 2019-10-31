#pragma once
#include <string>


class FileOptThr
{
public:
	FileOptThr()
	{
		m_LineCount = 0;
		m_LineCountToWrite = 50;
		m_pFile = NULL;

		m_EventProc = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_hThreadProc= CreateThread(NULL,0,Proc,this,0,NULL);
	}
	~FileOptThr()
	{
		TerminateThread(m_hThreadProc,0);
		CloseHandle(m_hThreadProc);
		CloseHandle(m_EventProc);
	}

	void Init(const char *filePathName,const char *title,int lineCountToWrite = 50)
	{
		m_LineCountToWrite = lineCountToWrite;


		m_str.resize(m_LineCountToWrite*100);
		m_str.clear();
		m_strThr.resize(m_LineCountToWrite*100);
		m_strThr.clear();
	

		m_LineCount = 0;
		fopen_s(&m_pFile, filePathName, "w" );
		WriteLine(title);
	}

	void WriteLine(const char *line)
	{//lineĩβ�Դ�\n

		if (NULL == line) return;

		++m_LineCount;
		m_str.append(line);


		if (m_LineCount < m_LineCountToWrite) return;

		if (!ThreadIsIdle()) return;
		m_strThr = m_str;
		m_LineCount = 0;
		m_str = "";

		SetEvent(m_EventProc);
	}
	void Save()
	{
		if (NULL == m_pFile) return;


		//�ȴ����ݴ����߳̽���
		while(!ThreadIsIdle()){}


		//��m_str��ʣ������д��
		if (m_LineCount>0)
			fprintf_s(m_pFile,"%s",m_str.c_str());


		fclose(m_pFile);
		m_pFile = NULL;
	}

private:
	static DWORD WINAPI Proc(LPVOID p)
	{
		FileOptThr* pObject=(FileOptThr*)p;
		while(1)
		{
			WaitForSingleObject(pObject->m_EventProc,INFINITE);
			pObject->WriteContent();
			ResetEvent(pObject->m_EventProc);
		}
		return 0;
	}
	bool ThreadIsIdle()
	{
		return WAIT_TIMEOUT == WaitForSingleObject(m_EventProc,0);
	}


private:
	void WriteContent()
	{
		if (NULL == m_pFile) return;
		fprintf_s(m_pFile,"%s",m_strThr.c_str());
	}

private:
	FILE* m_pFile;
	
	UINT m_LineCountToWrite;

	//��m_str������������m_LineCountToWriteʱ,ת��m_gpsSrcThr,���߳��д洢
	string m_str;
	UINT m_LineCount;
	string m_strThr;

	HANDLE m_hThreadProc;					//�߳̾��
	HANDLE m_EventProc;					//�ȴ��¼�
};