using System;
using System.Windows.Controls;

//xmlns:MyProgress="clr-namespace:MyProgress"
// <MyProgress:Progress x:Name="myProgress" tipStr="正在处理。。。"></MyProgress:Progress>

// myProgress.Visibility = Visibility.Visible;
// myProgress.Step(i);

namespace MyProgress
{
    public partial class Progress : UserControl
    {
        public Progress()
        {
            InitializeComponent();
        }

        public string tipStr{get;set;}

        public void Step(int step)
        {
            this.Dispatcher.Invoke(new Action(() =>
            {
                progressBar.Value = step;

                TB_Tips.Text = tipStr;
                TB_Percentage.Text = "已完成：" + step + "%";
            }));
        }
    }
}
