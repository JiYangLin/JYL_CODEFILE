#ifndef LISOCKET_H
#define LISOCKET_H

#include <list>
#include <string.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
using namespace std;




#define  MAX_CONNECT_NUM  20
#define  SOCKET_REV_BUF_SIZE     1024   //固定每个数据长度

#define  GUID_LEN    32
#define  NewMsgMarkStr    "6DB62435625348A28426C876F6B04A88"
#define  NewMsgMarkStrEnd "CE1D1E7C9DBF4951BB414B6D9D2AA1C1"
#define  NewMsg_Size_LEN  4
typedef  u_int32_t NewMsgSizeType;

typedef  u_char BYTE;
typedef  BYTE  ConnMark;
#define  MarkLen        1
#define  ConnMark_Def   0xFF


//接口收到recvBuf为NULL表示断开连接
class IServerRecver
{
public:
    virtual void OnRec(ConnMark mark,char *recvBuf, int BufLen)=0;
};
class IClientRecver
{
public:
    virtual void OnRec(char *recvBuf, int BufLen)=0;
};



class  IRevFinish
{
public:
    virtual void OnRevFinish(char *recvBuf,u_int32_t BufLen)=0;
};
class MsgCtl
{
    struct Receiver
    {
        Receiver():m_revSize(0),m_msgSize(0){}
        void rev(BYTE *recvBuf,IRevFinish* Fun)
        {
            if (isNewMsgMark(recvBuf))
            {
                m_revSize = 0;
                m_msgSize = *((NewMsgSizeType*)(recvBuf + GUID_LEN));
                if (m_buf.size() < m_msgSize) m_buf.resize(m_msgSize);
                return ;
            }


            int CpySize = SOCKET_REV_BUF_SIZE;
            if (m_revSize + SOCKET_REV_BUF_SIZE > m_msgSize)  CpySize = m_msgSize - m_revSize;


            if (0 == m_msgSize) return;
            if (m_buf.size() < m_revSize + CpySize) return;

            memcpy(&m_buf[m_revSize],recvBuf,CpySize);
            m_revSize = m_revSize + CpySize;


            if(m_revSize != m_msgSize) return;

            Fun->OnRevFinish(&m_buf[0],m_revSize);
            m_msgSize = 0;
            m_revSize = 0;
        }
        void Release()
        {
            m_buf.clear();
        }
    private:
        bool isNewMsgMark(const BYTE *recvBuf)
        {
            if (!checkGUID(recvBuf,NewMsgMarkStr)) return false;
            int EndGDIDPos = SOCKET_REV_BUF_SIZE - GUID_LEN;
            if(!checkGUID(recvBuf,NewMsgMarkStrEnd,EndGDIDPos)) return false;
            for(int i = GUID_LEN + NewMsg_Size_LEN; i < EndGDIDPos; ++i)
                if (0 != recvBuf[i]) return false;
            return true;
        }
        bool checkGUID(const BYTE *recvBuf,const char *guid,int posStart = 0)
        {
            const BYTE * id = (const BYTE *)guid;
            for (int i = 0 ; i < GUID_LEN; ++i)
            {
                if (id[i] != recvBuf[i+posStart]) return false;
            }
            return true;
        }
        u_int32_t m_revSize;
        u_int32_t m_msgSize;
        string m_buf;
    };
    struct Sender
    {
        bool SendMsg(int sock,const char * msg,NewMsgSizeType msgSize)
        {
            char SendBuf[SOCKET_REV_BUF_SIZE] = {0};

            memcpy(SendBuf,NewMsgMarkStr,GUID_LEN);
            memcpy(SendBuf + GUID_LEN,&msgSize,NewMsg_Size_LEN);
            memcpy(SendBuf + SOCKET_REV_BUF_SIZE - GUID_LEN,NewMsgMarkStrEnd,GUID_LEN);
            bool suc = (SOCKET_REV_BUF_SIZE ==  send(sock, SendBuf, SOCKET_REV_BUF_SIZE, 0));
            if(!suc) return false;


            u_int32_t hasSendSize = 0;
            while(1)
            {
                memset(SendBuf,0,SOCKET_REV_BUF_SIZE);
                if (msgSize <= SOCKET_REV_BUF_SIZE)
                {
                    memcpy(SendBuf,msg + hasSendSize,msgSize);

                    return SOCKET_REV_BUF_SIZE ==  send(sock, SendBuf, SOCKET_REV_BUF_SIZE, 0);
                }


                memcpy(SendBuf,msg + hasSendSize,SOCKET_REV_BUF_SIZE);

                suc = (SOCKET_REV_BUF_SIZE ==  send(sock, SendBuf, SOCKET_REV_BUF_SIZE, 0));
                if(!suc) return false;


                hasSendSize += SOCKET_REV_BUF_SIZE;
                msgSize -= SOCKET_REV_BUF_SIZE;
            }
        }
    };
public:
    void Rev(BYTE *recvBuf,IRevFinish* Fun)
    {
        m_Receiver.rev(recvBuf,Fun);
    }
    bool Send(int sock,const char * msg,NewMsgSizeType msgSize)
    {
        return m_Sender.SendMsg(sock,msg,msgSize);
    }
    void Release()
    {
        m_Receiver.Release();
    }
private:
    Receiver m_Receiver;
    Sender m_Sender;
};






class CSocketConn:public IRevFinish
{
public:
    CSocketConn(int Conn,IServerRecver *pIServerRecver,IClientRecver *pIClientRecver):m_Conn(Conn), m_bConnect(true),m_ConnMark(ConnMark_Def)
    {
        m_pIServerRecver = pIServerRecver;
        m_pIClientRecver = pIClientRecver;
        pthread_create(&m_Thread,0,RecvThread,this);
    }
    virtual ~CSocketConn()
    {
        Release();
    }
    void Release()
    {
        pthread_cancel(m_Thread);
        m_Thread = 0;
        close(m_Conn);
        m_Conn = 0;
        m_MsgCtl.Release();
    }
    bool Send(const char * msg,NewMsgSizeType msgSize)
    {
        if (!m_bConnect) return false;
        return m_MsgCtl.Send(m_Conn,msg,msgSize);
    }
    bool ConnectCorrect() { return m_bConnect; }
    BYTE GetMark(){return m_ConnMark; }
private:
    static void*  RecvThread(void* p)
    {
        CSocketConn *pConnect = (CSocketConn *)p;
        pConnect->Recv();
        return 0;
    }
    bool RecvErro(bool bErro)
    {
        if (!bErro) return false;

        m_bConnect = false;

        OnRevFinish(NULL,0);//接口收到recvBuf为NULL表示断开连接

        return true;
    }
    virtual void OnRevFinish(char *recvBuf,u_int32_t BufLen)
    {
        if (NULL != m_pIServerRecver) m_pIServerRecver->OnRec(m_ConnMark,recvBuf,BufLen);
        else m_pIClientRecver->OnRec(recvBuf,BufLen);
    }
    void Recv()
    {
        char recvBuf[SOCKET_REV_BUF_SIZE] = {0};
        int recBufPos = 0;
        int BufLen = 0;

        if(m_pIServerRecver != NULL)
        {
            BufLen = recv(m_Conn,recvBuf,1,0);
            if(RecvErro(1 != BufLen)) return;
            m_ConnMark = recvBuf[0];
        }

        while(true)
        {

            BufLen = recv(m_Conn,recvBuf + recBufPos,SOCKET_REV_BUF_SIZE - recBufPos,0);
            recBufPos += BufLen;

            if(RecvErro(BufLen <= 0)) return;


            if (recBufPos != SOCKET_REV_BUF_SIZE) continue;
            recBufPos = 0;

            m_MsgCtl.Rev((BYTE*)recvBuf,this);
        }
    }
private:
    int            m_Conn;
    IServerRecver *m_pIServerRecver;
    IClientRecver *m_pIClientRecver;
    pthread_t      m_Thread;

    bool m_bConnect;

    MsgCtl m_MsgCtl;

    ConnMark m_ConnMark;
};

class ServerSocket
{
public:
    ServerSocket():m_pISocketRecver(NULL)
    {
        pthread_mutex_init(&m_lock,0);
    }
    ~ServerSocket()
    {
        pthread_cancel(m_AcceptThrHand);
        pthread_mutex_destroy(&m_lock);

        for (list<CSocketConn*>::iterator it = m_Connect.begin();it != m_Connect.end(); ++it)
        {
            (*it)->Release();
            delete *it;
            *it = NULL;
        }
        close(m_sockSrv);

        pthread_mutex_destroy(&m_lock);
    }
public:
    bool Start(int port,IServerRecver *pISocketRecver)
    {
        m_pISocketRecver = pISocketRecver;

        m_sockSrv = socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in  addrSrv;
        addrSrv.sin_family=AF_INET;
        addrSrv.sin_port=htons(port);
        addrSrv.sin_addr.s_addr = INADDR_ANY;
        bzero(&(addrSrv.sin_zero),8);

        if (bind(m_sockSrv, (struct sockaddr *)&addrSrv, sizeof(struct sockaddr)) == -1)
        {
            cout<<"bind erro"<<endl;
            return false;
        }
        if (listen(m_sockSrv, MAX_CONNECT_NUM) == -1)
        {
            cout<<"listen erro"<<endl;
            return false;
        }

        pthread_create(&m_AcceptThrHand,0,AcceptThread,this);

        return true;
    }
    bool SendToAllConn(const char * msg,NewMsgSizeType msgSize)
    {
        return SendToConn(msg,msgSize);
    }
    bool SendToMarkConn( BYTE mark,const char * msg,NewMsgSizeType msgSize)
    {
        return SendToConn(msg,msgSize,mark);
    }
private:
    bool SendToConn(const char * msg,NewMsgSizeType msgSize,BYTE mark = ConnMark_Def)
    {
        bool sendSuc = false;
        pthread_mutex_lock(&m_lock);
        for (list<CSocketConn*>::iterator it = m_Connect.begin();it != m_Connect.end(); ++it)
        {
            if (!(*it)->ConnectCorrect()) continue;

            if(mark != ConnMark_Def)
                if(mark != (*it)->GetMark()) continue;
            sendSuc = (*it)->Send(msg, msgSize);
        }
        pthread_mutex_unlock(&m_lock);
        return sendSuc;
    }
private:
    static void* AcceptThread(void* p)
    {
        ServerSocket *pSer = (ServerSocket *)p;
        pSer->OnAccept();
        return 0;
    }
    static bool needRemove(CSocketConn* pSoCo)
    {
        if (!pSoCo->ConnectCorrect())
        {
            delete pSoCo;
            return true;
        }
        return false;
    }
    void OnAccept()
    {
        while(true)
        {
            struct sockaddr_in addrClient;
            socklen_t len = sizeof(struct sockaddr_in);
            int sockConn = accept(m_sockSrv, (struct sockaddr *)&addrClient, &len);

            pthread_mutex_lock(&m_lock);

            m_Connect.remove_if(needRemove);

            m_Connect.push_back(new CSocketConn(sockConn, m_pISocketRecver,NULL));
            if (m_Connect.size() >= MAX_CONNECT_NUM) return;
            pthread_mutex_unlock(&m_lock);
        }
    }
private:
    int            m_sockSrv;
    IServerRecver *m_pISocketRecver;

    pthread_t     m_AcceptThrHand;
    list<CSocketConn*> m_Connect;
    pthread_mutex_t  m_lock;
};



class ClientSocket
{
public:
    ClientSocket() :m_Connect(NULL){}
    ~ClientSocket()
    {
        if (NULL == m_Connect) return;
        delete m_Connect;
    }

    bool Start(ConnMark mark,const char *Ip,int port,IClientRecver *Recver)
    {
        int sockConn = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addrSrv;


        struct hostent *host = gethostbyname(Ip);
        addrSrv.sin_family=AF_INET;
        addrSrv.sin_port=htons(port);
        addrSrv.sin_addr = *((struct in_addr *)host->h_addr);
        bzero(&(addrSrv.sin_zero),8);

        if (connect(sockConn, (struct sockaddr *)&addrSrv, sizeof(struct sockaddr)) == -1) return false;

        if (MarkLen != send(sockConn, (char*)&mark, MarkLen, 0)) return false;

        m_Connect = new CSocketConn(sockConn, NULL,Recver);

        return true;
    }
    bool Send(const char * msg,NewMsgSizeType msgSize)
    {
        if (NULL == m_Connect) return false;

        if (!m_Connect->ConnectCorrect()) return false;

        return m_Connect->Send(msg,msgSize);
    }
private:
    CSocketConn* m_Connect;
};


#endif

//class Ser:IServerRecver
//{
//public:
//	virtual void OnRec(ConnMark mark,char *recvBuf, int BufLen)
//	{
//		string str(recvBuf,recvBuf + BufLen);
//		cout<<str<<endl;
//	}
//	void Run()
//	{
//		if(!m_ServerSocket.Start(1234,this))
//		{
//			cout<<"start Erro"<<endl;
//			return;
//		}
//
//		string str;
//		int mark;
//		while(1)
//		{
//			cout<<"mark"<<endl;
//			cin>>mark;
//			cin.clear();
//
//			cout<<"str"<<endl;
//			cin>>str;
//
//			m_ServerSocket.SendToMarkConn(mark,&str[0],str.length());//向指定链接发送
//
//			//		m_ServerSocket.SendToAllConn(&str[0],str.length());
//		}
//	}
//
//	ServerSocket m_ServerSocket;
//};


//class Cl:IClientRecver
//{
//public:
//	virtual void OnRec(char *recvBuf, int BufLen )
//	{
//		string str(recvBuf,recvBuf + BufLen);
//		cout<<str<<endl;
//	}
//	void Run()
//	{
//
//		cout<<"client"<<endl;
//
//		cout<<"mark:"<<endl;
//		int mark = 0;
//		cin>>mark;
//		cin.clear();
//
//		if(!m_ClientSocket.Start(mark,"127.0.0.1",1234,this))////////mark是连接标志
//		{
//			cout<<"Connect Erro"<<endl;
//			return;
//		}
//		else
//		{
//			cout<<"Connect suc"<<endl;
//		}
//
//		string str;
//		while(1)
//		{
//			cin>>str;
//			m_ClientSocket.Send(&str[0],str.length());
//		}
//	}
//
//	ClientSocket m_ClientSocket;
//};