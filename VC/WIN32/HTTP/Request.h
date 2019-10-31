#pragma once

#include <windows.h>
#pragma comment(lib,"ws2_32.lib")
#include <string>
#include <vector>
using namespace std;

#define  __WS2
#ifdef __WS2
#include <WS2tcpip.h>
#endif


struct HttpRequestResult
{
	HttpRequestResult():buf(0), bufLen(0){}
	char *buf;
	int   bufLen;
};
#define HEAD_LEN  10240
class CHttpRequest
{
	char* m_RequestHead;
	int   m_RequestHeadBufLen;
	int   m_RequestHeadLen;
	void GetRequestHead(const char *page, const  char *ContentType, const char *pData , int pDataLen)
	{
		memset(m_RequestHead, 0, m_RequestHeadBufLen);
		if (NULL == pData)
		{
			sprintf_s(m_RequestHead, m_RequestHeadBufLen, "GET %s HTTP/1.1\r\nHost: %s:%d\r\nUser-Agent: Mozilla/5.0\r\n\r\n", page, m_ip.c_str(), m_port);
			m_RequestHeadLen = strlen(m_RequestHead);
		}
		else
		{
			sprintf_s(m_RequestHead, m_RequestHeadBufLen, "POST %s HTTP/1.1\r\nHost: %s:%d\r\nUser-Agent: Mozilla/5.0\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n", page, m_ip.c_str(), m_port, pDataLen, ContentType);
			int strHeadLen = strlen(m_RequestHead);
			m_RequestHeadLen = strHeadLen + pDataLen;
			if (m_RequestHeadBufLen < m_RequestHeadLen)
			{
				m_RequestHeadBufLen = m_RequestHeadLen;
				char *pNew = new char[m_RequestHeadBufLen];
				memcpy(pNew, m_RequestHead, strHeadLen);	
				delete[] m_RequestHead;
				m_RequestHead = pNew;
			}
			memcpy(m_RequestHead + strHeadLen, pData, pDataLen);
		}
	}
	bool connectToServer()
	{
		closesocket(m_sockClient);
		m_sockClient = socket(AF_INET, SOCK_STREAM, 0);
		if (SOCKET_ERROR == connect(m_sockClient, (SOCKADDR*)&m_addrSrv, sizeof(SOCKADDR))) return false;
		return true;
	}
	string       m_revBuf;
	SOCKADDR_IN  m_addrSrv;
	SOCKET       m_sockClient;
	int          m_port;
	string       m_ip;
public:
	CHttpRequest()
	{
		m_RequestHeadBufLen = 1024;
		m_RequestHead = new char[m_RequestHeadBufLen];
		m_RequestHeadLen = 0;
		m_port = 0;
		m_ip = "";
		m_sockClient = 0;
		WSADATA wsaData;
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSAStartup( wVersionRequested, &wsaData);
	}
	bool init(const char *ip, int port)
	{
		if (ip == m_ip && port == m_port) return true;
	
#ifdef __WS2
		inet_pton(AF_INET, ip, &m_addrSrv.sin_addr.S_un.S_addr);
#else
		m_addrSrv.sin_addr.S_un.S_addr = inet_addr(ip);
#endif   
		m_addrSrv.sin_family = AF_INET;
		m_addrSrv.sin_port = htons(port);
		m_port = port;
		m_ip = ip;

		return connectToServer();
	}
	HttpRequestResult GetRequest(const char *page, const char *ContentType = NULL, const char *pData = NULL, int pDataLen = 0)
	{
		HttpRequestResult result;
		for (size_t i = 0; i < 30; i++)
		{
			GetRequestRun(result,page, ContentType, pData, pDataLen);
			if (result.buf != NULL) return result;
			Sleep(10);
			for (size_t j = 0; j < 30; j++)
			{
				if (connectToServer()) break;
				Sleep(10);
			}	
		}
		return result;
	}
private:
	void GetRequestRun(HttpRequestResult &result,const char *page, const char *ContentType,const char *pData,int pDataLen)
	{
		GetRequestHead(page, ContentType, pData, pDataLen);
		int ret = send(m_sockClient,m_RequestHead, m_RequestHeadLen,0);
		if (SOCKET_ERROR == ret) return;
	
		int len = GetDatLen();
		if (0 == len) return;

		int bufLen = len + 1;
		m_revBuf.resize(bufLen);
		result.buf = &m_revBuf[0];
		result.bufLen = len;
		memset(result.buf, 0, bufLen);


		int  offset = 0;
		int  rc = 0;
		while (rc = recv(m_sockClient, result.buf + offset, 1024, 0))
		{
			offset += rc;
			if (offset >= len) break;
		}
	}
	vector<char>  m_pos;
	int GetDatLen()
	{
		m_pos.clear();

		char recvBufHead[HEAD_LEN] = {0};
		int  offset = 0;
		int  rc = 0 ;
		char revDat;
		bool valid = false;
		while(rc = recv(m_sockClient, &revDat, 1,0))
		{
			if(offset >= HEAD_LEN) return false;
			if(0 == offset) if('H' != revDat) return false;
			*(recvBufHead+offset) = revDat;
			offset += rc;
		    if(revDat == '\r' || revDat == '\n') m_pos.push_back(revDat);
			else m_pos.clear();
			valid = Vaild();
			if(valid) break;
		}

		if(!valid) return 0;
		const char* strFind = "Content-Length:";
		int strFindLen = strlen(strFind);
		char* pos = strstr(recvBufHead, strFind);
		if (pos == 0) return 0;
		char* pLen = pos + strFindLen;
		int len = atoi(pLen);
		return len;
	}
	bool Vaild()
	{
		if(m_pos.size() != 4) return false;
		return true;
	}
};


//  http://127.0.0.1:8080/page?name=xx
//m_CHttpRequest.init("127.0.0.1", 8080);
//HttpRequestResult result = m_CHttpRequest.GetRequest("/page?name=xx");


//  http://127.0.0.1:8080/page
//m_CHttpRequest.init("127.0.0.1", 8080);
//const char *pData = "name=xxx&age=123";
//int   pDataLen = strlen(pData);
//const char *ContentType = "text/plain";
//HttpRequestResult result = m_CHttpRequest.GetRequest("/page", ContentType, pData, pDataLen);