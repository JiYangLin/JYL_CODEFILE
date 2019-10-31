#pragma once
#include <Windows.h>
#include <process.h>
#include <vector>
#include <string>
#include <queue>
#include <map>
using namespace std;
#define  SerialPort_REV_MAX  2048

class IProcRevDat
{
public:
	virtual void ProcessReadByte(BYTE *dat,UINT DatSize)= 0;
	

	virtual void InitRevMsgCheck() = 0;

	//长度对齐和数据校验
	//m_MsgLen信息长度
	//m_Pos_ChekVal校验内容
	size_t  m_MsgLen;
	map<size_t,BYTE> m_Pos_ChekVal;//下标位置_校验值


	//根据首尾判断一帧完整数据
	bool m_checkBE;
	BYTE m_Begin;
	BYTE m_End;

public:
	IProcRevDat():m_MsgLen(0),m_checkBE(false),m_findBegin(false),m_findEnd(false){}
	~IProcRevDat(){clear();}

	void OnRev(BYTE *dat,UINT BytesInQue)
	{

		if (0 != m_MsgLen)
		{	
			ProcContent(dat,BytesInQue);
			return;
		}
		else if (m_checkBE)
		{
			ProcBEContent(dat,BytesInQue);
			return;
		}


		ProcessReadByte(dat,BytesInQue);
	}

	void clear()
	{
		m_que.swap(queue<BYTE>());
		checkDat.swap(vector<BYTE>());
	}

private:
	queue<BYTE> m_que;
	//长度对齐和数据校验
	void ProcContent(BYTE *dat,UINT BytesInQue)
	{
		for (size_t i = 0; i < BytesInQue;++i) m_que.push(dat[i]);	

		size_t pos = 0;
		while (true)
		{
			if (pos == 0 && m_que.size() < m_MsgLen) return;

			dat[pos++] = m_que.front();
			m_que.pop();

			if(!checkSig(dat,pos-1)) 
			{
				pos = 0;
				continue;
			}

			if (pos < m_MsgLen) continue;
			ProcessReadByte(dat, (UINT)m_MsgLen);
			pos = 0;
		}
	}
	bool checkSig(BYTE *dat,size_t pos)
	{
		if (m_Pos_ChekVal.find(pos) == m_Pos_ChekVal.end()) return true;
		return dat[pos] == m_Pos_ChekVal[pos];
	}
private:
	//校验首尾
	bool m_findBegin;
	bool m_findEnd;
	vector<BYTE> checkDat;
	void ProcBEContent(BYTE *dat,UINT BytesInQue)
	{
		for (UINT i = 0 ; i < BytesInQue;++i)
		{
			BYTE *pVal = dat + i;

			if (m_Begin == *pVal)
				m_findBegin = true;
			if (m_End == *pVal)
				m_findEnd = true;

			if (m_findBegin || m_findEnd)
			{
				checkDat.push_back(*pVal);
			}


			if (m_findEnd)
			{
				ProcessReadByte(&checkDat[0], (UINT)checkDat.size());
				m_findBegin = false;
				m_findEnd = false;
				checkDat.clear();
			}
		}
	}
};





class CMsSerialPort
{
public:
	CMsSerialPort(void)
	{
		m_hComm = NULL;
		m_hListenThread = NULL;
		m_pIProcRevDat = NULL;
	}
	~CMsSerialPort(void)
	{
		Close();
	}
public:
	bool Start(UINT  portNo, UINT  baud,IProcRevDat *pIProcRevDat)
	{	
		if(!Init(portNo,baud)) return false;
		m_pIProcRevDat = pIProcRevDat;
		if (NULL == m_pIProcRevDat) return false;
		pIProcRevDat->InitRevMsgCheck();
		OpenListenThread();
		return true;
	}
	bool Start(UINT  portNo, UINT  baud)
	{//不进行接收监听
		return Init(portNo,baud);
	}

	void Close()
	{
		CloseListenTread();
		ClosePort();	
	}
	bool WriteData(const BYTE *dat, DWORD size)
	{
		if (m_hComm == NULL)   return false;

		DWORD  BytesToSend = 0;
		BOOL bResult = WriteFile(m_hComm, dat, size, &BytesToSend, NULL);
		if (!bResult)
		{
			PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
			return false;
		}
		return true;
	}
private:
	bool Init(UINT  portNo = 1, UINT  baud = CBR_9600, char  parity = 'N', UINT  databits = 8, UINT  stopsbits = 1, DWORD dwCommEvents = EV_RXCHAR)
	{
		if (!openPort(portNo))  return false;


		COMMTIMEOUTS  CommTimeouts;
		CommTimeouts.ReadIntervalTimeout         = 0;
		CommTimeouts.ReadTotalTimeoutMultiplier  = 0;
		CommTimeouts.ReadTotalTimeoutConstant    = 0;
		CommTimeouts.WriteTotalTimeoutMultiplier = 0;
		CommTimeouts.WriteTotalTimeoutConstant   = 0;
		if(!SetCommTimeouts(m_hComm, &CommTimeouts)) return false;


		char szDCBparam[50];
		sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);
		DCB  dcb;
		if(!GetCommState(m_hComm, &dcb) || !BuildCommDCBA(szDCBparam, &dcb)) return false;
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		if(!SetCommState(m_hComm, &dcb)) return false;


		if(!PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT)) return false;

		return true;
	}

	bool OpenListenThread()
	{
		if (m_hListenThread != NULL)  return false;

		UINT threadId;
		m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
		if (!m_hListenThread) return false;

		if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))  return false;
		
		return true;
	}
	static UINT WINAPI ListenThread(void* pParam)
	{
		CMsSerialPort *pSerialPort = reinterpret_cast<CMsSerialPort*>(pParam);
		pSerialPort->Read();
		return 0;
	}
	void CloseListenTread()
	{
		::CloseHandle(m_hListenThread);
		m_hListenThread = INVALID_HANDLE_VALUE;
		if(NULL == m_hListenThread) return;
		::TerminateThread(m_hListenThread,0);
		m_hListenThread = NULL;
	}
protected:
	UINT GetBytesNumInCOM()
	{	 
		COMSTAT  comstat;   
		memset(&comstat, 0, sizeof(COMSTAT));

		DWORD dwError = 0; 
		if (!ClearCommError(m_hComm, &dwError, &comstat)) return 0;

		return comstat.cbInQue;
	}

	
	bool ReadByte(BYTE *dat,UINT BytesInQue)
	{
		if(BytesInQue >= SerialPort_REV_MAX)
		{
			PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
			return false;
		}

	
		DWORD BytesRead = 0;
		memset(dat,0,SerialPort_REV_MAX);
		if (!ReadFile(m_hComm, dat, BytesInQue, &BytesRead, NULL))
		{
			PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
			return false;
		}

		return (BytesRead == BytesInQue);
	}
private:
	bool openPort(UINT  portNo)
	{
		char szPort[50];
		sprintf_s(szPort, "COM%d", portNo);

		m_hComm = CreateFileA(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);

		return m_hComm != INVALID_HANDLE_VALUE;
	}

	void ClosePort()
	{
		if (m_hComm == INVALID_HANDLE_VALUE) return;
		
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;	
	}
	
	void Read()
	{
		if (NULL == m_pIProcRevDat) return;
		m_pIProcRevDat->clear();

		BYTE dat[SerialPort_REV_MAX] = {0};
		UINT BytesInQue = 0;
		while (true)
		{
			BytesInQue = GetBytesNumInCOM();
			if (BytesInQue == 0)
			{
				Sleep(5);
				continue;
			}

	
			if (!ReadByte(dat,BytesInQue)) continue;

			m_pIProcRevDat->OnRev(dat,BytesInQue);
		}
	}
protected:
	HANDLE  m_hComm; 
	volatile HANDLE    m_hListenThread;
	IProcRevDat* m_pIProcRevDat;
};