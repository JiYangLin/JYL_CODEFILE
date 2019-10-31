#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "ijlOpt.h"
using namespace std;

enum
{
	ProcNum =  2,
	ProcBufSize =  120,
	ImgBufSize = ProcBufSize/ProcNum,
	ImgBufWarning = ProcBufSize - 50,
};

struct ImgBuf
{
	ImgBuf(int _ImgBufSize)
	{
		m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_ImgBufSize = _ImgBufSize;
		m_pImgBuf = new BYTE[m_ImgBufSize];	
	}
	void SetParam(const BYTE *_pImgBuf,const string &outputPath)
	{
		memcpy(m_pImgBuf,_pImgBuf,m_ImgBufSize);
		m_outputPath = outputPath;
	}

	bool HasSignal()
	{
		return WAIT_TIMEOUT != WaitForSingleObject(m_hEvent,0);
	}
	void SetSignal()
	{
		SetEvent(m_hEvent);
	}
	void DelSignal()
	{
		ResetEvent(m_hEvent);
	}


	
	HANDLE m_hEvent;


	BYTE *m_pImgBuf;
	int m_ImgBufSize;

	string m_outputPath;
};

class ImgSaver
{
public:
	ImgSaver(int width,int height)
	{
		m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);


		int ImgbufSize = width*height;

        for (int i = 0 ; i < ImgBufSize;++i)
        {
			m_BufVec[i] = new ImgBuf(ImgbufSize);
        }

		m_DibToJpg.Init(width,height,1);
		m_thr = ::CreateThread(NULL,0,ProcThread,this,0,NULL);	
	}
public:
	int GetAvaliBufCount()
	{
		int size = 0;
		for (int i = 0; i < ImgBufSize;++i)
		{
			if (!m_BufVec[i]->HasSignal())
			{
				++size;
			}
		}
		return size;
	}
	bool Run(const BYTE *pImg,const string &outputPath)
	{		
        ImgBuf *pBuf = NULL;
		for (int i = 0; i < ImgBufSize;++i)
		{
			if (!m_BufVec[i]->HasSignal())
			{
				pBuf = m_BufVec[i];
			}
		}
        if (NULL == pBuf) return false;

		pBuf->SetParam(pImg,outputPath);
		pBuf->SetSignal();
		SetEvent(m_hEvent);

		return true;
	}
	bool ThreadIsIdle(){ return WAIT_TIMEOUT == WaitForSingleObject(m_hEvent,0);}
	void Stop()
	{
		while(!ThreadIsIdle()){}
		TerminateThread(m_thr,0);
	}
private:
	static DWORD WINAPI ProcThread( LPVOID pParam )
	{
		ImgSaver *pImgSaver = (ImgSaver*)pParam;
		pImgSaver->Process();
		return 0;
	}

	void Process()
	{
		while(1)
		{
			ImgBuf* pImgBuf = NULL;

			for (int i = 0; i < ImgBufSize;++i)
			{
				if (m_BufVec[i]->HasSignal())
				{
					pImgBuf = m_BufVec[i];
					continue;
				}
			}


			if (NULL == pImgBuf)
			{
				WaitForSingleObject(m_hEvent, INFINITE);
				ResetEvent(m_hEvent);
				continue;
			}  

			m_DibToJpg.SaveToJPGBuffer(pImgBuf->m_pImgBuf);
			WriteImg(pImgBuf->m_outputPath);


			 pImgBuf->DelSignal();
		}

	}
	bool WriteImg(string imgPath)
	{
		if (imgPath == "") return true;

		FILE *pFile = NULL;
		fopen_s(&pFile,imgPath.c_str(),"wb");
		if (NULL == pFile) return false;

		if(1 != fwrite((BYTE*)&m_DibToJpg.m_JpgBuf[0],m_DibToJpg.m_jpgBufSize,1,pFile))
		{
			fclose(pFile);
			return false;
		}

		fclose(pFile);

		if(!PathFileExistsA(imgPath.c_str())) return false;

		return true;
	}
private:
	DibToJpg m_DibToJpg;	

	HANDLE m_thr;

    ImgBuf* m_BufVec[ImgBufSize];

	HANDLE m_hEvent;
};



class ImgSaverManager
{
	vector<ImgSaver*> m_ImgSaver;
	int m_thrUse;
public:
	void InitImgSaver(int width,int height)
	{
		m_thrUse = 0;
		for (int i = 0; i < ProcNum;++i)
		{
			m_ImgSaver.push_back(new ImgSaver(width,height));
		}
	}
	bool Run(const BYTE *pImg,const string outputPath)
	{
		int size = 0;
		for (int i = 0; i < ProcNum;++i)
		{
			size += m_ImgSaver[i]->GetAvaliBufCount();
		}

		//if(size <= ImgBufWarning) {} //±¨¾¯
		//if (size >0 && size <= ImgBufWarning){}


		++m_thrUse;
		if (m_thrUse >= ProcNum) m_thrUse = 0;
		return m_ImgSaver[m_thrUse]->Run(pImg,outputPath);
	}
	void Stop()
	{
		for (int i = 0; i < ProcNum;++i)
		{
			m_ImgSaver[i]->Stop();
		}
	}
};