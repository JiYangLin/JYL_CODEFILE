using UnityEngine;
using System.Collections;

using System;
using System.Threading;
using System.Collections.Generic;
using System.IO.Ports;
public class COMMON : MonoBehaviour
{
    private SerialPort sp;
    private Queue<string> queueDataPool;
    private Thread tPortDeal;
    public int len = 5;//接收的16进制串口消息的长度
    void Start()
    {
        sp = new SerialPort("COM2", 9600, Parity.None, 8, StopBits.One);
        if (!sp.IsOpen)
        {
            sp.Open();
        }
        tPortDeal = new Thread(ReceiveData);
        tPortDeal.Start();
    }
    // Update is called once per frame  
    void Update()
    {
        if (!tPortDeal.IsAlive)
        {
            tPortDeal = new Thread(ReceiveData);
            tPortDeal.Start();
        }

        /////////发送
        if (Input.GetKeyUp(KeyCode.A))
        {
            string str = "ae 01 ff";
            int size = 0;
            byte[] send = strToToHexByte(str, ref size);
            sp.Write(send, 0, size);
        }
    }
    private void ReceiveData()
    {
        try
        {
            while (true)
            {
                string result = string.Empty;
                Byte[] temp0 = new Byte[1];
                if (sp.IsOpen)
                {
                    sp.Read(temp0, 0, 1);//阻塞函数，第一次只能接收一个字节
                }
                result += temp0[0].ToString("X2");

                Byte[] temp = new Byte[len-1];
                if (sp.IsOpen)
                {
                    sp.Read(temp, 0, len-1);
                }
                if (temp != null)
                {
                    for (int i = 0; i < temp.Length; i++)
                    {
                        result += temp[i].ToString("X2");
                    }
                    
                    print(result);
                }
            }
        }
        catch (Exception ex)
        {
            Debug.Log(ex);
        }
    }


    void OnApplicationQuit()
    {
        sp.Close();
    }
    private static byte[] strToToHexByte(string hexString, ref int size)
    {
        hexString = hexString.Replace(" ", "");
        if ((hexString.Length % 2) != 0)
            hexString += "";
        byte[] returnBytes = new byte[hexString.Length / 2];
        int i = 0;
        for (; i < returnBytes.Length; i++)
            returnBytes[i] = Convert.ToByte(hexString.Substring(i * 2, 2), 16);
        size = i;
        return returnBytes;
    }

} 

