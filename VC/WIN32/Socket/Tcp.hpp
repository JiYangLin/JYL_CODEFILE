#pragma once
#include <Windows.h>
#include <list>
#include <string>
#include <map>
#include <iostream>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

#define  __WS2

#ifdef __WS2
#include <WS2tcpip.h>
#endif

////////////使用前调用InitWinSocket()进行初始化/////////
inline bool InitWinSocket(){
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAStartup( wVersionRequested, &wsaData );
	return true;
}



//接口收到recvBuf为NULL表示断开连接
class ISocketrRev
{
public:
	virtual void Rev(int mark, char * recvBytes, int revSize)=0;
};

class SocketConnect
{
public:
	 ISocketrRev* revProc;
	 SOCKET connSocket;
	 HANDLE revThread;
	 int connMark = -1;
	 int port = -1;
	void StartRev()
	{
		revThread = ::CreateThread(NULL, 0, _OnRev, this, 0, NULL); 
	}
	static DWORD WINAPI _OnRev(LPVOID p)
	{
		SocketConnect *pObj = (SocketConnect*)p;
		pObj->OnRev();
		return 0;
	}

	void OnRev()
	{
		const int bufsize = 1024;
		char recvBytes[bufsize] = {0};
		while (true)
		{
			memset(recvBytes, 0, bufsize);
			int revLen = recv(connSocket, recvBytes, bufsize, 0);
			if (revLen == SOCKET_ERROR)
			{
				revProc->Rev(connMark, NULL, 0);
				return;
			}
			revProc->Rev(connMark, recvBytes, revLen);
		}
	}
	bool Send(const char* msg, int msgLen)
	{
		return msgLen == send(connSocket, msg, msgLen, 0);
	}
	void Close()
	{
		TerminateThread(revThread, 0);
		revThread = NULL;
		closesocket(connSocket);
	}
};



class SocketServer
{
	SOCKET socketServer;
public:
	SocketConnect connectObj;
public:
	SocketServer()
	{
		socketServer = -1;
	}
	bool Start(int mark, int port, ISocketrRev* _revProc)
	{
		connectObj.connMark = mark;
		connectObj.revProc = _revProc;
		connectObj.port = port;

		return StartRun();
	}
	bool ReStart()
	{
		return StartRun();
	}

	bool StartRun()
	{
		if (-1 != socketServer)  closesocket(socketServer);

		socketServer = socket(AF_INET, SOCK_STREAM, 0);
		SOCKADDR_IN addrSrv;
		addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_port = htons(connectObj.port);

		if (SOCKET_ERROR == bind(socketServer, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR))) return false;
		if (SOCKET_ERROR == listen(socketServer, 1)) return false;

		CloseHandle(::CreateThread(NULL, 0, _Accept, this, 0, NULL));
		return true;
	}
	static DWORD WINAPI _Accept(LPVOID p)
	{
		SocketServer *pObj = (SocketServer*)p;
		pObj->AcceptRun();
		return 0;
	}
	void AcceptRun()
	{
		SOCKADDR_IN addrClient;
		int len = sizeof(SOCKADDR);
		connectObj.connSocket = accept(socketServer, (SOCKADDR*)&addrClient, &len);
		connectObj.StartRev();
	}
};
class TcpSocketServerPackage
{
	map<int, SocketServer> mark_ClientConnect;
	CRITICAL_SECTION  m_cs;
public:
	TcpSocketServerPackage(){InitWinSocket(); InitializeCriticalSection(&m_cs);}
	void StartServer(int mark, int prort, ISocketrRev* revProc)
	{
		if (mark_ClientConnect.find(mark) != mark_ClientConnect.end()) return;
		mark_ClientConnect[mark].Start(mark, prort, revProc);
	}
	void ReStartServer(int mark)
	{
		if (mark_ClientConnect.find(mark) == mark_ClientConnect.end()) return;

		mark_ClientConnect[mark].ReStart();
	}


	bool Send(int mark, const char* msg, int msgLen)
	{
		EnterCriticalSection(&m_cs);
		if (mark_ClientConnect.find(mark) == mark_ClientConnect.end())
		{
			LeaveCriticalSection(&m_cs);
			return false;
		}

		bool ret = mark_ClientConnect[mark].connectObj.Send(msg, msgLen);
		LeaveCriticalSection(&m_cs);
		return ret;
	}

	void Colse()
	{
		for(auto iter = mark_ClientConnect.begin(); iter != mark_ClientConnect.end();++iter)
		{
			iter->second.connectObj.Close();
		}
		mark_ClientConnect.clear();
	}

};






class SocketClient
{
public:
	SocketConnect connectObj;
	bool Start(int mark, string IP, int port, ISocketrRev* revProc)
	{
		connectObj.revProc = revProc;
		connectObj.connMark = mark;
		connectObj.port = port;

		connectObj.connSocket = socket(AF_INET, SOCK_STREAM, 0);
		SOCKADDR_IN addrSrv;
#ifdef __WS2
		inet_pton(AF_INET, IP.c_str(), &addrSrv.sin_addr.S_un.S_addr);
#else
		addrSrv.sin_addr.S_un.S_addr = inet_addr(IP.c_str());
#endif  
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_port = htons(port);
		if (SOCKET_ERROR == connect(connectObj.connSocket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR))) return false;
		connectObj.StartRev();

		return true;
	}
};
class TcpSocketClientPackage
{
	map<int, SocketClient> mark_ClientConnect;
	CRITICAL_SECTION  m_cs;
public:
	TcpSocketClientPackage() { InitWinSocket(); InitializeCriticalSection(&m_cs);}
	bool Connect(int mark, string IP, int prort, ISocketrRev* revProc)
	{
		auto iter = mark_ClientConnect.find(mark);
		if (iter != mark_ClientConnect.end())
		{
			mark_ClientConnect[mark].connectObj.Close();
			mark_ClientConnect.erase(iter);
		}

		if (!mark_ClientConnect[mark].Start(mark, IP, prort, revProc)) return false;
		return true;
	}
	bool Send(int mark, const char* msg,  int msgLen)
	{
		EnterCriticalSection(&m_cs);
		if (mark_ClientConnect.find(mark) == mark_ClientConnect.end())
		{
			LeaveCriticalSection(&m_cs);
			return false;
		}

		bool ret = mark_ClientConnect[mark].connectObj.Send(msg, msgLen);
		LeaveCriticalSection(&m_cs);
		return ret;
	}
	void Colse()
	{
		for (auto iter = mark_ClientConnect.begin(); iter != mark_ClientConnect.end(); ++iter)
		{
			iter->second.connectObj.Close();
		}
		mark_ClientConnect.clear();
	}
};