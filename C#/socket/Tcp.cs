using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace MySocket
{
    //错误时接收数据为null
    interface ISocketrRev
    {
        void Rev(int mark, byte[] recvBytes, int revSize, string erroMsg = "");
    }

    class SocketConnect
    {
        public ISocketrRev revProc = null;
        public Socket connSocket = null;
        public Thread revThread = null;
        public int connMark = -1;
        public int port=-1;

        public void StartRev()
        {
            revThread = new Thread(_OnRev);
            revThread.IsBackground = true;
            revThread.Start();
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
                revProc.Rev(connMark, null, 0, e.Message);
            }
        }

        void OnRev()
        {
            const int bufsize = 1024;
            byte[] recvBytes = new byte[bufsize];
            while (true)
            {
                int revLen = connSocket.Receive(recvBytes, 0, bufsize, 0);
                revProc.Rev(connMark, recvBytes, revLen);
            }
        }
        public bool Send(byte[] msg,int msgOffset,int msgLen)
        {
            try
            {
                connSocket.Send(msg, msgOffset, msgLen, 0);
            }
            catch (Exception ex)
            {
                return false;
            }
            return true;

        }

        public void Close()
        {
            if(null != revThread) revThread.Abort();
            if (null != connSocket)  connSocket.Close();
        }
    }
}






 //服务端
 namespace MySocket
{
    class SocketServer
    {
        public SocketConnect connect = new SocketConnect();
        Socket socketServer = null;


        public void Start(int mark,int port, ISocketrRev _revProc)
        {
            connect.connMark = mark;
            connect.revProc = _revProc;
            connect.port = port;

            StartRun();
        }
        public void ReStart()
        {
            StartRun();
        }

        void StartRun()
        {
            if(null != socketServer)  socketServer.Close();

            socketServer = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint endpoint = new IPEndPoint(IPAddress.Any, connect.port);
            socketServer.Bind(endpoint);
            socketServer.Listen(1);

            Thread thr = new Thread(_Accept);
            thr.IsBackground = true;
            thr.Start();
        }
        void _Accept()
        {
            connect.connSocket = socketServer.Accept();
            connect.StartRev();
        }
    }




    class TcpSocketServerPackage
    {
        Dictionary<int, SocketServer> mark_ClientConnect = new Dictionary<int, SocketServer>();
        Mutex mutex = new Mutex();

        public void StartServer(int mark, int prort, ISocketrRev revProc)
        {
            if (mark_ClientConnect.ContainsKey(mark)) return ;

            SocketServer ser = new SocketServer();
            ser.Start(mark, prort, revProc);
            mark_ClientConnect.Add(mark, ser);
        }
        public void ReStartServer(int mark)
        {
            if (!mark_ClientConnect.ContainsKey(mark)) return;

            mark_ClientConnect[mark].ReStart();
        }


        public bool Send(int mark, byte[] msg, int msgOffset, int msgLen)
        {
            mutex.WaitOne();
            if (!mark_ClientConnect.ContainsKey(mark))
            {
                mutex.ReleaseMutex();
                return false;
            }

            bool ret = mark_ClientConnect[mark].connect.Send(msg,msgOffset,msgLen);
            mutex.ReleaseMutex();
            return ret;
        }

        public void Colse()
        {
            foreach (SocketServer conn in mark_ClientConnect.Values)
            {
                conn.connect.Close();
            }
            mark_ClientConnect.Clear();
        }
    }
}





//客户端
namespace MySocket
{
    class SocketClient
    {
        public SocketConnect connect = new SocketConnect();

        public bool Start(int mark, string IP, int prort, ISocketrRev revProc)
        {
            try
            {
                connect.revProc = revProc;
                connect.connMark = mark;

                IPAddress address = IPAddress.Parse(IP);
                IPEndPoint endpoint = new IPEndPoint(address, prort);
                connect.connSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                connect.connSocket.Connect(endpoint);

                connect.StartRev();
            }
            catch (Exception e)
            {
                return false;
            }
            return true;
        }
    }




    class TcpSocketClientPackage
    {
        Dictionary<int, SocketClient> mark_ClientConnect = new Dictionary<int, SocketClient>();
        Mutex mutex = new Mutex();

        public bool Connect(int mark, string IP, int prort, ISocketrRev revProc)
        {
            SocketClient conn = new SocketClient();
            if (!conn.Start(mark, IP, prort, revProc)) return false;

            if (mark_ClientConnect.ContainsKey(mark))
            {
                mark_ClientConnect[mark].connect.Close();
                mark_ClientConnect.Remove(mark);
            }

            mark_ClientConnect.Add(mark, conn);
            return true;
        }
        public bool Send(int mark, byte[] msg, int msgOffset, int msgLen)
        {
            mutex.WaitOne();
            if (!mark_ClientConnect.ContainsKey(mark))
            {
                mutex.ReleaseMutex();
                return false;
            }

            bool ret = mark_ClientConnect[mark].connect.Send(msg, msgOffset,msgLen);
            mutex.ReleaseMutex();
            return ret;
        }
        public void Colse()
        {
            foreach (SocketClient conn in mark_ClientConnect.Values)
            {
                conn.connect.Close();
            }
            mark_ClientConnect.Clear();
        }
    }
}