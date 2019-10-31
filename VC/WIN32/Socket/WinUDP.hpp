#pragma once
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")

//socket需要初始化
inline bool InitSocket(){
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAStartup( wVersionRequested, &wsaData );
	return true;
}


#define  REV_BUF_SIZE     1024

class IRecv
{
public:
	virtual void OnRec(const char* connectIp,char *recvBuf, int BufLen) = 0;
};
class WinServerUDP
{
public:
	WinServerUDP() :m_pIRecv(NULL), m_sockSrv(NULL),m_Thread(NULL) {}
	~WinServerUDP()
	{
		TerminateThread(m_Thread, 0);
		CloseHandle(m_Thread);
		closesocket(m_sockSrv);
	}
	bool Start(int port, IRecv *pIRecv)
	{
		m_pIRecv = pIRecv;

		m_sockSrv = socket(AF_INET, SOCK_DGRAM, 0);
		m_addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		m_addrSrv.sin_family = AF_INET;
		m_addrSrv.sin_port = htons(port);

		if (SOCKET_ERROR == bind(m_sockSrv, (SOCKADDR*)&m_addrSrv, sizeof(SOCKADDR)))
		{
			return false;
		}

		m_Thread = CreateThread(NULL, 0, Rev, this, 0, NULL);

		return true;
	}
private:
	static DWORD WINAPI Rev(LPVOID p)
	{
		WinServerUDP *pSer = (WinServerUDP *)p;
		if (NULL == pSer) return 0;
		pSer->OnRev();
		return 0;
	}
	void OnRev()
	{
		int len = sizeof(SOCKADDR);
		char recvBuf[REV_BUF_SIZE];
		int RecvLen = SOCKET_ERROR;
		while (true)
		{
			RecvLen = recvfrom(m_sockSrv, recvBuf, REV_BUF_SIZE, 0, (SOCKADDR*)&m_addrSrv, &len);
			
			char connectIP[100] = {0};
			sprintf_s(connectIP,100,"%s",inet_ntoa(m_addrSrv.sin_addr));

			m_pIRecv->OnRec(connectIP,recvBuf, RecvLen);
			if (RecvLen == SOCKET_ERROR) return;
		}
		
	}
private:
    HANDLE m_Thread;
	SOCKET m_sockSrv;
	SOCKADDR_IN m_addrSrv;
	IRecv *m_pIRecv;
};

class WinClientUDP
{
public:
	~WinClientUDP()
	{
		closesocket(m_sockClient);
	}
	void Init(const char* ip,int port)
	{
		m_sockClient = socket(AF_INET, SOCK_DGRAM, 0);
		m_addrSrv.sin_addr.S_un.S_addr = inet_addr(ip);
		m_addrSrv.sin_family = AF_INET;
		m_addrSrv.sin_port = htons(port);
	}
	bool Send(const char *pBuf, int BufLen)
	{
		if (BufLen != sendto(m_sockClient, pBuf, BufLen, 0, (SOCKADDR*)&m_addrSrv, sizeof(SOCKADDR))) return false;
		return true;
	}	
private:
	SOCKET m_sockClient;
	SOCKADDR_IN m_addrSrv;
};




/////服务器
//class Ser:public IRecv
//{
//	virtual void OnRec(const char *pConnectIP,char *recvBuf, int BufLen)
//	{
//		string str(recvBuf,recvBuf+BufLen);
//		cout<<"ip: "<<pConnectIP<<endl;
//		cout<<str<<endl;
//	}
//	WinServerUDP m_WinServerUDP;
//
//public:
//	void Run()
//	{
//
//		if(!m_WinServerUDP.Start(1234,this))
//			cout<<"打开服务器失败"<<endl;
//		while(1){}
//	}
//};


/////=========调用
//InitSocket();
//Ser m_Ser;
//m_Ser.Run();






////客户端
//InitSocket();
//WinClientUDP m_WinClientUDP;
//
//m_WinClientUDP.Init("192.168.0.1",1234);
//
//string str;
//while (1)
//{
//	cin>>str;
//	m_WinClientUDP.Send(str.c_str(),str.length());
//}