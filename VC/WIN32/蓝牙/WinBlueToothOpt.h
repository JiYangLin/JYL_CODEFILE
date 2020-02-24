#pragma once

#include <iostream>
#include <atlstr.h>
#include <string>
#include <WinSock2.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"irprops.lib")
#include <list>
using namespace std;

struct BTINFO
{
	CString name;
	CString addr;
};
class BlueToothDevInfo
{
	BTINFO GetName(TCHAR* szName,BLUETOOTH_ADDRESS &stu)
	{
		BTINFO inf;

		SOCKADDR_BTH sa;
		memset(&sa,0,sizeof(SOCKADDR_BTH));
		sa.addressFamily = AF_BTH;
		sa.btAddr = stu.ullLong;
		TCHAR buf[1024] = {0};
		DWORD len;
		WSAAddressToString((LPSOCKADDR)&sa,sizeof(SOCKADDR_BTH),NULL,buf,&len);
		inf.name = szName;
		inf.name += buf;

		inf.addr.Format(_T("%02x:%02x:%02x:%02x:%02x:%02x"),
			stu.rgBytes[5],
			stu.rgBytes[4],
			stu.rgBytes[3],
			stu.rgBytes[2],
			stu.rgBytes[1],
			stu.rgBytes[0]);
		return inf;
	}
public:
	void ListLocalDev(list<BTINFO> &_LocalDevList)
	{
		BLUETOOTH_FIND_RADIO_PARAMS btfrp = { sizeof(btfrp) };
		HANDLE hRadio = NULL;
		HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&btfrp,&hRadio);
		while(hFind)
		{
			if (!hRadio) continue;
			BLUETOOTH_RADIO_INFO RadioInfo;
			memset(&RadioInfo,0,sizeof(BLUETOOTH_RADIO_INFO));
			RadioInfo.dwSize = sizeof(BLUETOOTH_RADIO_INFO);
			int RET = BluetoothGetRadioInfo(hRadio,&RadioInfo);
			if (ERROR_SUCCESS != RET) continue;
			_LocalDevList.push_back(GetName(RadioInfo.szName,RadioInfo.address));

			BluetoothEnableIncomingConnections(hRadio,TRUE);
			BluetoothEnableDiscovery(hRadio,TRUE);

			if(!BluetoothFindNextRadio(hFind,&hRadio)) break;
		}
		BluetoothFindRadioClose(hFind);
	}
	void ListNearDev(list<BTINFO> &_NearDevList)
	{
		BLUETOOTH_DEVICE_SEARCH_PARAMS  btdp = { sizeof(btdp) };
		BLUETOOTH_DEVICE_INFO Devinfo;
		HANDLE hRadio = NULL;
		memset(&btdp,0,sizeof(btdp));
		btdp.hRadio = hRadio;
		btdp.fReturnAuthenticated = TRUE;
		btdp.fReturnRemembered = TRUE;
		btdp.dwSize = sizeof(btdp);
		btdp.cTimeoutMultiplier = 10;
		btdp.fIssueInquiry = TRUE;
		btdp.fReturnUnknown = TRUE;
		Devinfo.dwSize = sizeof(Devinfo);
		HBLUETOOTH_DEVICE_FIND hFind = BluetoothFindFirstDevice(&btdp,&Devinfo);
		if ( hFind == NULL) return;

		do
		{
			if ( ERROR_SUCCESS != BluetoothGetDeviceInfo(hRadio,&Devinfo)) break;
			_NearDevList.push_back(GetName(Devinfo.szName, Devinfo.Address));			

		} while(BluetoothFindNextDevice(hFind,&Devinfo));
		BluetoothFindDeviceClose(hFind);
	}
};


class BlueToothServer
{
public:
	BlueToothServer()
	{
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSAStartup( wVersionRequested, &wsaData );
	}
	bool Start()
	{		
		BlueToothDevInfo inf;
		list<BTINFO> _LocalDevList;
		inf.ListLocalDev(_LocalDevList);	
		if (_LocalDevList.size() == 0) return false;

		SOCKET socketTcp = socket(AF_BTH,SOCK_STREAM,BTHPROTO_RFCOMM);
		if (SOCKET_ERROR == socketTcp) return false;

		SOCKADDR_BTH sa;
		memset(&sa,0,sizeof(SOCKADDR_BTH));
		sa.addressFamily=AF_BTH;
		sa.btAddr = 0;
		sa.port = 20;
		int RET = bind(socketTcp,(SOCKADDR*)&sa,sizeof(SOCKADDR_BTH));
		if (SOCKET_ERROR == RET) return false;
		listen(socketTcp,1);

		Rev(socketTcp);

		return true;
	}
	void Rev(SOCKET socketTcp)
	{
		SOCKADDR_BTH addrClient;
		int len = sizeof(addrClient);
		memset(&addrClient,0,len);
		SOCKET sockConn = accept(socketTcp,(SOCKADDR*)&addrClient,&len);
		if (SOCKET_ERROR == sockConn) return;
		
		printf("连接成功\n");
		while(true)
		{

			const int bufSize = 100;
			char recvBuf[bufSize ] = {0};
			int rev = recv(sockConn,recvBuf,bufSize ,0);
			if (rev <= 0)
			{
				 Rev( socketTcp);
				return;
			}
			printf("%s\n",recvBuf);
		}
	}
};
class BlueToothClient
{
public:
	BlueToothClient()
	{
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSAStartup( wVersionRequested, &wsaData );
	}
	bool Start(TCHAR* devAddr)
	{//%02x:%02x:%02x:%02x:%02x:%02x

		SOCKADDR_BTH sa={0};
		int  sa_len = sizeof(SOCKADDR_BTH);
		memset(&sa, 0 ,sa_len);
		SOCKET sockClient = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM); 
		if(sockClient == SOCKET_ERROR) return false;
		if(SOCKET_ERROR == WSAStringToAddress(devAddr,AF_BTH,NULL,(LPSOCKADDR)&sa,&sa_len )) return false;
		sa.addressFamily = AF_BTH;
		sa.port = 20;


		if(SOCKET_ERROR==connect(sockClient,(SOCKADDR*)&sa,sa_len)) return false;
		//接收
		const int bufSize = 100;
		char recvBuf[bufSize ]={0};
		recv(sockClient,recvBuf,bufSize ,0);
		printf("%s\n",recvBuf);
		//发送
		send(sockClient,"clientSend",strlen("client")+1,0);
		return true;
	}
};