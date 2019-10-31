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
	{//line末尾自带\n

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


		//等待数据处理线程结束
		while(!ThreadIsIdle()){}


		//将m_str中剩余数据写入
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

	//当m_str中数据行数到m_LineCountToWrite时,转入m_gpsSrcThr,在线程中存储
	string m_str;
	UINT m_LineCount;
	string m_strThr;

	HANDLE m_hThreadProc;					//线程句柄
	HANDLE m_EventProc;					//等待事件
};