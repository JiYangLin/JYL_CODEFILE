using System;
using System.Threading;

namespace MyThr
{
    class BackThr
    {
        ManualResetEvent manualEvent = new ManualResetEvent(false);
        Thread thr = null;
        public BackThr()
        {
            thr = new Thread(Run);
            thr.IsBackground = true;
            thr.Start();
        }

        void Run()
        {
            while (true)
            {
                manualEvent.WaitOne();
                Proc();
                manualEvent.Reset();
            }
        }

        //判断线程是否处于空闲状态
        public bool ThreadIsIdle()
        {
            return !manualEvent.WaitOne(0);
        }
        //激活线程处理,使用前先调用ThreadIsIdle()判断线程是否空闲
        public void ThreadActive()
        {
            //激活运行线程
            manualEvent.Set();
        }

        public virtual void Proc(){}
    }


    //class Sample : BackThr
    //{
    //    string m_str;
    //    public override void Proc()
    //    {
    //        Console.WriteLine(m_str);
    //    }
    //    public void Run(string str)
    //    {
    //        if (ThreadIsIdle())
    //        {
    //            m_str = str;
    //            ThreadActive();
    //        }
    //    }
    //}

    class MyBool
    {
        ManualResetEvent manualEvent = new ManualResetEvent(false);

        public void SetTrue()
        {
            manualEvent.Set();
        }
        public void SetFalse()
        {
            manualEvent.Reset();
        }
        public bool IsTrue()
        {
            return manualEvent.WaitOne(0);
        }
        public bool IsFalse()
        {
            return !manualEvent.WaitOne(0);
        }
    };
}
