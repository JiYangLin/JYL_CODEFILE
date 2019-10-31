using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms.Integration;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

//System.Windows.Forms
//WindowsFormsIntegration

//xmlns:container="clr-namespace:AppContainers"
//<container:AppContainer x:Name="ExternExe" Margin="10,10,200,200"/>

//ExternExe.StartProcess(@"C:\Windows\system32\mspaint.exe");
namespace AppContainers
{
    public partial class AppContainer : UserControl
    {
        public bool StartProcess(string processPath)
        {
            bool isStartAndEmbedSuccess = false;
            _eventDone.Reset();

            _process = Process.Start(processPath);
            if (_process == null)
            {
                return false;
            }

            Thread thread = new Thread(new ThreadStart(() =>
            {
                while (true)
                {
                    if (_process.MainWindowHandle != (IntPtr)0)
                    {
                        _eventDone.Set();
                        break;
                    }
                    Thread.Sleep(10);
                }
            }));
            thread.Start();


            if (_eventDone.WaitOne(10000))
            {
                isStartAndEmbedSuccess = EmbedApp(_process);
                if (!isStartAndEmbedSuccess)
                {
                    CloseProcess();
                }
            }
            return isStartAndEmbedSuccess;
        }





        public AppContainer()
        {
            InitializeComponent();
        }



        [DllImport("user32.dll", SetLastError = true)]
        public static extern int SetParent(IntPtr hWndChild, IntPtr hWndNewParent);


        [DllImport("user32.dll", SetLastError = true)]
        public static extern bool MoveWindow(IntPtr hwnd, int x, int y, int cx, int cy, bool repaint);




        System.Windows.Forms.Panel _hostPanel = null;
        Process _process = null;
        IntPtr _embededWindowHandle;
        ManualResetEvent _eventDone = new ManualResetEvent(true);
        

        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            _hostPanel = new System.Windows.Forms.Panel();
            Process_Host.Child = _hostPanel;
        }


        bool EmbedExistProcess(Process process)
        {
            _process = process;
            return EmbedApp(process);
        }




        bool EmbedApp(Process process)
        {
            bool isEmbedSuccess = false;
            IntPtr processHwnd = process.MainWindowHandle;
            IntPtr panelHwnd = _hostPanel.Handle;

            if (processHwnd != (IntPtr)0 && panelHwnd != (IntPtr)0)
            {
                int setTime = 0;
                while (!isEmbedSuccess && setTime < 10)
                {
                    isEmbedSuccess = (SetParent(processHwnd, panelHwnd) != 0);
                    Thread.Sleep(100);
                    setTime++;
                }

                MoveExeWindow();
            }

            if (isEmbedSuccess)
            {
                _embededWindowHandle = _process.MainWindowHandle;
            }

            return isEmbedSuccess;
        }


        protected override void OnRender(DrawingContext drawingContext)
        {
            if (_process != null)
            {
                MoveExeWindow();
            }
            base.OnRender(drawingContext);
        }

        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            this.InvalidateVisual();
            base.OnRenderSizeChanged(sizeInfo);
        }

        void CloseProcess()
        {
            if (_process != null && !_process.HasExited)
            {
                _process.Kill();
            }
        }

        void MoveExeWindow()
        {
            Point ptLeftUp = new Point(0, 0);
            Point ptRightDown = new Point(this.ActualWidth, this.ActualHeight);
            ptLeftUp = this.PointToScreen(ptLeftUp);
            ptRightDown = this.PointToScreen(ptRightDown);

            int width = (int)(ptRightDown.X - ptLeftUp.X);
            int height = (int)(ptRightDown.Y - ptLeftUp.Y);

            MoveWindow(_process.MainWindowHandle, 0, 0, width, height, true);
        }
    }
}
