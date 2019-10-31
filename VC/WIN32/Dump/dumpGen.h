#pragma once

#include < Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")


class DumpGen
{
public:
	static DumpGen* GetInstance()
	{
		if (NULL ==  m_pDumpGen)
		{
			 m_pDumpGen = new DumpGen();
		}
		return m_pDumpGen;
	}
	static DumpGen *m_pDumpGen;

	void Init(TCHAR *dumpName)
	{
		_tcscpy_s(m_dumpName,MAX_PATH,dumpName);
	}

	TCHAR m_dumpName[MAX_PATH];

public:
	static void CreateDumpFile(TCHAR* lpstrDumpfileName,EXCEPTION_POINTERS *pException)
	{
		HANDLE hFile = CreateFile(lpstrDumpfileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
		dumpInfo.ExceptionPointers = pException;
		dumpInfo.ThreadId = GetCurrentThreadId();
		dumpInfo.ClientPointers = TRUE;


		MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),hFile,MiniDumpNormal,&dumpInfo,NULL,NULL);

		CloseHandle(hFile);
	}
	static LONG ApplicationCrash(EXCEPTION_POINTERS *pException)
	{
		TCHAR  dumpFile[1024] = {0};

		SYSTEMTIME tm;

		GetLocalTime(&tm);
		_stprintf_s(dumpFile,1024,_T("%s_%d%d%d%d%d%d.dmp"), DumpGen::GetInstance()->m_dumpName,tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wHour);

		CreateDumpFile(dumpFile,pException);
		FatalAppExit(-1,dumpFile);
		return EXCEPTION_EXECUTE_HANDLER;
	}
private:
	DumpGen()
	{
		memset(m_dumpName,0,MAX_PATH);
	}
};


#define  DECLARE_DUMPGEN  DumpGen* DumpGen::m_pDumpGen = NULL;

#define  INIT_DUMPGEN(DumpName) \
 DumpGen* pDumpGen = DumpGen::GetInstance();\
 pDumpGen->Init(DumpName);\
 SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)pDumpGen->ApplicationCrash);


