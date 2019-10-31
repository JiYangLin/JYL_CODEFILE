using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace MySocket
{
    interface IRevProc
    {
        //recvBytes长度为null时，代表出错，此时返回erroMsg
        void Rev(byte[] recvBytes, string erroMsg = "");
    }
	
    //recvBytes长度为null时，代表出错，此时返回erroMsg
    delegate void Rev(byte[] revBuf, long bufLen,string erroMsg = "");

    class UdpServer
    {
        Socket socketServer = null;
        Thread RevTh = null;
        public void Close()
        {
            RevTh.Abort();
        }
        public void Start(int port, Rev rev)
        {

            socketServer = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            IPEndPoint endpoint = new IPEndPoint(IPAddress.Any, port);
            socketServer.Bind(endpoint);
            IPEndPoint sender = new IPEndPoint(IPAddress.Any, 0);
            EndPoint Remote = (EndPoint)sender;


            RevTh = new Thread(() => {
                const int RevBuf = 1024;
                byte[] revBuf = new byte[RevBuf];
                long revLen = 0;

                while (true)
                {
                    try
                    {
                        revLen = socketServer.ReceiveFrom(revBuf, 0, RevBuf, SocketFlags.None, ref Remote);
                        rev(revBuf, revLen);
                    }
                    catch (Exception e)
                    {
                        rev(null, 0, e.Message);
                    }

                }
            });

            RevTh.IsBackground = true;
            RevTh.Start();
        }
    }



    class UdpClient
    {
        Socket Client = null;
        IPEndPoint endpoint = null;

        public void StartSocket(string IP, int prort)
        {
            IPAddress address = IPAddress.Parse(IP);
            endpoint = new IPEndPoint(address, prort);
            Client = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        }

        public void Send(CombinSendByte conbin)
        {
            byte[] sendByte = new byte[(int)Param.REV_LEN];
            conbin(sendByte);

            Client.SendTo(sendByte, endpoint);
        }
    }
}

//服务端
 //           UdpServer ser = new UdpServer();
 //           ser.Start(7777, (byte[] revBuf, long bufLen, string erroMsg) =>
 //           {
 //               if (revBuf == null)
 //                   Console.WriteLine(erroMsg);

 //               byte[] Rev = new byte[bufLen];
 //               Array.Copy(revBuf, 0, Rev, 0, bufLen);


 //               string recvStr = Encoding.ASCII.GetString(Rev);
 //               Console.WriteLine(recvStr);

 //           });


//客户端
  //          UdpClient client = new UdpClient();
  //          client.StartSocket("192.168.0.101",7777);

  //          string str;
  //          str = Console.ReadLine();
  //          while (true)
  //          {
  //              client.Send((byte[] sendByte) => { Encoding.ASCII.GetBytes(str).CopyTo(sendByte, 0); });
  //              Thread.Sleep(500);
  //          }