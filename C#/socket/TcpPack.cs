﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;



namespace MySocket
{
    using NewMsgSizeType = UInt32;
    using ConnMark = Byte;

    //错误时接收数据为null
    interface IServerRev
    {
        void Rev(ConnMark mark, byte[] recvBytes, UInt32 revSize, string erroMsg = "");
    }
    interface IClientRev
    {
        void Rev(byte[] recvBytes, UInt32 revSize, string erroMsg = "");
    }

    enum Param
    {
        REV_LEN = 1024,//固定每个数据长度
        MAX_LISTEN_NUM = 20,

        GUID_LEN = 32,

        NewMsg_Size_LEN = 4,

        MarkLen = 1,
        ConnMark_Def = 0xFF,
    }
    class newMsgMark
    {
        public newMsgMark()
        {
            string _NewMsgMarkStr = "6DB62435625348A28426C876F6B04A88";
            NewMsgMarkStr = Encoding.Default.GetBytes(_NewMsgMarkStr);

            string _NewMsgMarkStrEnd = "CE1D1E7C9DBF4951BB414B6D9D2AA1C1";
            NewMsgMarkStrEnd = Encoding.Default.GetBytes(_NewMsgMarkStrEnd);
        }
        public Byte[] NewMsgMarkStr { get; set; }
        public Byte[] NewMsgMarkStrEnd { get; set; }
        public bool isNewMsgMark(Byte[] recvBuf)
        {
            int guidLen = (int)Param.GUID_LEN;
            for (int i = 0; i < guidLen; ++i)
                if (NewMsgMarkStr[i] != recvBuf[i]) return false;


            int EndGDIDPos = (int)Param.REV_LEN - guidLen;
            for (int i = 0; i < guidLen; ++i)
                if (NewMsgMarkStrEnd[i] != recvBuf[i + EndGDIDPos]) return false;

            for (int i = guidLen + (int)Param.NewMsg_Size_LEN; i < EndGDIDPos; ++i)
                if (0 != recvBuf[i]) return false;

            return true;
        }
    }

    delegate void RevFinish(byte[] revByte, UInt32 revSize);
    class MsgCtl
    {
        class Receiver
        {
            UInt32 m_revSize = 0;
            UInt32 m_msgSize = 0;
            Byte[] m_buf = new Byte[(int)Param.REV_LEN];
            newMsgMark m_newMsgMark = new newMsgMark();

            public void rev(Byte[] recvBuf, RevFinish Fun)
            {
                if (m_newMsgMark.isNewMsgMark(recvBuf))
                {
                    m_revSize = 0;
                    m_msgSize = BitConverter.ToUInt32(recvBuf, (int)Param.GUID_LEN);
                    if (m_buf.Length < m_msgSize) m_buf = new Byte[m_msgSize];
                    return;
                }


                int CpySize = (int)Param.REV_LEN;
                if (m_revSize + (int)Param.REV_LEN > m_msgSize) CpySize = (int)(m_msgSize - m_revSize);


                if (0 == m_msgSize) return;
                if (m_buf.Length < m_revSize + CpySize) return;


                Array.Copy(recvBuf, 0, m_buf, m_revSize, CpySize);
                m_revSize = m_revSize + (UInt32)CpySize;


                if (m_revSize != m_msgSize) return;

                Fun(m_buf, m_revSize);
                m_msgSize = 0;
                m_revSize = 0;
            }
        }
        class Sender
        {
            Byte[] SendBuf = new Byte[(int)Param.REV_LEN];
            newMsgMark m_newMsgMark = new newMsgMark();

            public void SendMsg(Socket sock, byte[] msg, NewMsgSizeType msgSize)
            {
                Array.Clear(SendBuf, 0, (int)Param.REV_LEN);

                Array.Copy(m_newMsgMark.NewMsgMarkStr, 0, SendBuf, 0, (int)Param.GUID_LEN);
                Byte[] sizeBytes = BitConverter.GetBytes(msgSize);
                Array.Copy(sizeBytes, 0, SendBuf, (int)Param.GUID_LEN, sizeBytes.Length);
                Array.Copy(m_newMsgMark.NewMsgMarkStrEnd, 0, SendBuf, (int)Param.REV_LEN - (int)Param.GUID_LEN, (int)Param.GUID_LEN);


                sock.Send(SendBuf, (int)Param.REV_LEN, 0);

                UInt32 hasSendSize = 0;
                while (true)
                {
                    Array.Clear(SendBuf, 0, (int)Param.REV_LEN);

                    if (msgSize <= (int)Param.REV_LEN)
                    {
                        Array.Copy(msg, hasSendSize, SendBuf, 0, msgSize);
                        sock.Send(SendBuf, (int)Param.REV_LEN, 0);
                        return;
                    }


                    Array.Copy(msg, hasSendSize, SendBuf, 0, (int)Param.REV_LEN);
                    sock.Send(SendBuf, (int)Param.REV_LEN, 0);

                    hasSendSize += (int)Param.REV_LEN;
                    msgSize -= (int)Param.REV_LEN;
                }
            }
        }



        public void Rev(Byte[] recvBuf, RevFinish Fun)
        {
            m_Receiver.rev(recvBuf, Fun);
        }
        public void Send(Socket sock, Byte[] msg, NewMsgSizeType msgSize)
        {
            m_Sender.SendMsg(sock, msg, msgSize);
        }
        Receiver m_Receiver = new Receiver();
        Sender m_Sender = new Sender();
    }


    class Connect
    {
        Socket conn = null;
        Thread revThread = null;
        IServerRev m_IServerRev = null;
        IClientRev m_IClientRev = null;
        MsgCtl m_MsgCtl = new MsgCtl();
        ConnMark m_ConnMark = (ConnMark)Param.ConnMark_Def;
        newMsgMark m_newMsgMark = new newMsgMark();
        public bool connectNormal { get; set; }
        public ConnMark GetConnMark() { return m_ConnMark; }

        public void Close()
        {
            revThread.Abort();
            conn.Close();
        }
        public Connect(Socket _conn, IServerRev _IServerRev, IClientRev _IClientRev)
        {
            m_IServerRev = _IServerRev;
            m_IClientRev = _IClientRev;

            connectNormal = true;
            conn = _conn;

            revThread = new Thread(_OnRev);
            revThread.IsBackground = true;
            revThread.Start();
        }


        void Rev(byte[] msg, NewMsgSizeType msgSize, string erro)
        {
            if (null != m_IServerRev) m_IServerRev.Rev(m_ConnMark, msg, msgSize, erro);
            else m_IClientRev.Rev(msg, msgSize, erro);
        }
        void _OnRev()
        {
            try
            {
                OnRev();
            }
            catch (Exception e)
            {
                //错误时接收数据为null作为通知
                Rev(null, 0, e.Message);
                connectNormal = false;
            }
        }

        void OnRev()
        {
            byte[] recvBytes = new byte[(int)Param.REV_LEN];
            int recBufPos = 0;
            int BufLen = 0;

            if (m_IServerRev != null)
            {
                conn.Receive(recvBytes, 0, (int)Param.MarkLen, 0);
                m_ConnMark = recvBytes[0];
            }

            while (true)
            {

                BufLen = conn.Receive(recvBytes, recBufPos, (int)Param.REV_LEN - recBufPos, 0);
                recBufPos += BufLen;

                ////接收到足够一帧大小 Param.REV_LEN 后再执行
                if (recBufPos != (int)Param.REV_LEN) continue;
                recBufPos = 0;

                m_MsgCtl.Rev(recvBytes, (byte[] revByte, UInt32 revSize) =>
                {
                    Rev(revByte, revSize, "");
                });
            }
        }
        public bool Send(Byte[] msg, NewMsgSizeType msgSize)
        {
            try
            {
                m_MsgCtl.Send(conn, msg, msgSize);
            }
            catch (Exception ex)
            {
                return false;
            }
            return true;
           
        }
    }
}



//服务端
namespace MySocket
{
    using NewMsgSizeType = UInt32;
    using ConnMark = Byte;

    class SocketServer
    {
        Thread threadSocket = null;
        Socket socketServer = null;
        IServerRev revProc = null;
        List<Connect> SocketConnect = new List<Connect>();
        Mutex mutex = new Mutex();

        public void Start(int port, IServerRev _revProc)
        {
            revProc = _revProc;

            socketServer = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint endpoint = new IPEndPoint(IPAddress.Any, port);
            socketServer.Bind(endpoint);

            threadSocket = new Thread(Accept);
            threadSocket.IsBackground = true;
            threadSocket.Start();
        }
        public void SendToAllConn(Byte[] msg, NewMsgSizeType msgSize)
        {
            SendToConn(msg, msgSize);
        }
        public void SendToMarkConn(Byte mark, Byte[] msg, NewMsgSizeType msgSize)
        {
            SendToConn(msg, msgSize, mark);
        }
        void SendToConn(Byte[] msg, NewMsgSizeType msgSize, Byte mark = (Byte)Param.ConnMark_Def)
        {
            mutex.WaitOne();
            foreach (Connect conn in SocketConnect)
            {
                if (!conn.connectNormal) continue;

                if (mark != (Byte)Param.ConnMark_Def)
                    if (mark != conn.GetConnMark()) continue;
                conn.Send(msg, msgSize);
            }
            mutex.ReleaseMutex();
        }

        void Accept()
        {
            socketServer.Listen((int)Param.MAX_LISTEN_NUM);
            while (true)
            {
                Socket connSocket = socketServer.Accept();

                mutex.WaitOne();
                RemoveErroConnect();
                Connect conn = new Connect(connSocket, revProc, null);
                SocketConnect.Add(conn);
                mutex.ReleaseMutex();
            }
        }

        void RemoveErroConnect()
        {
            SocketConnect.RemoveAll((Connect conn) =>
            {
                if (!conn.connectNormal)
                {
                    conn.Close();
                }
                return !conn.connectNormal;
            });

        }
    }
}


//客户端
namespace MySocket
{
    using NewMsgSizeType = UInt32;
    using ConnMark = Byte;

    class SocketClient
    {
        Connect m_Connect = null;
        newMsgMark m_newMsgMark = new newMsgMark();
        public void Start(Byte mark, string IP, int prort, IClientRev revProc)
        {
            IPAddress address = IPAddress.Parse(IP);
            IPEndPoint endpoint = new IPEndPoint(address, prort);
            Socket Client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);


            Client.Connect(endpoint);
            Byte[] by = new Byte[1];
            by[0] = mark;
            Client.Send(by, 1, 0);
            m_Connect = new Connect(Client, null, revProc);


        }
        public bool Send(Byte[] msg, NewMsgSizeType msgSize)
        {
            if (null == m_Connect) return false;
            return m_Connect.Send(msg, msgSize);
        }
    }
}



//class Ser : IServerRev
//{
//    void IServerRev.Rev(Byte mark, byte[] recvBytes, UInt32 revSize, string erroMsg)
//    {
//        if (null == recvBytes) return;

//        string str = Encoding.Default.GetString(recvBytes);

//        Console.WriteLine(str);
//    }
//    public void Run()
//    {

//        m_SocketServer.Start(1234, this);

//        Console.WriteLine("server");

//        string str;
//        while (true)
//        {
//            Console.WriteLine("mark");

//            str = Console.ReadLine();
//            int mark = Convert.ToInt32(str);

//            Console.WriteLine("str");
//            str = Console.ReadLine();
//            byte[] bytes = Encoding.Default.GetBytes(str);
//            m_SocketServer.SendToMarkConn((byte)mark, bytes, (UInt32)bytes.Length);

//        }
//    }

//    SocketServer m_SocketServer = new SocketServer();
//}



//class Client : IClientRev
//{
//    void IClientRev.Rev(byte[] recvBytes, UInt32 revSize, string erroMsg)
//    {
//        if (null == recvBytes) return;
//        string str = Encoding.Default.GetString(recvBytes);

//        Console.WriteLine(str);
//    }
//    public void Run()
//    {
//        Console.WriteLine("CLient");

//        string str;
//        Console.WriteLine("mark");
//        str = Console.ReadLine();
//        int mark = Convert.ToInt32(str);

//        m_SocketClient.Start((Byte)mark, "127.0.0.1", 1234, this);


//        while (true)
//        {
//            Console.WriteLine("str:");
//            str = Console.ReadLine();
//            byte[] bytes = Encoding.Default.GetBytes(str);

//            m_SocketClient.Send(bytes, (UInt32)bytes.Length);
//        }
//    }

//    SocketClient m_SocketClient = new SocketClient();
//}