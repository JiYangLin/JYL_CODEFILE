using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace IMG_CAT
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
 
            try
            {
                string path = System.AppDomain.CurrentDomain.BaseDirectory + "img.jpg";
                FileInfo ff = new FileInfo(path);
                FileStream fs = new FileStream(path, FileMode.Open);
                byte[] data = new byte[ff.Length];
                fs.Read(data, 0, (int)ff.Length);
                fs.Close();
                
                BitmapImage bim = new BitmapImage();
                bim.BeginInit();
                bim.StreamSource = new MemoryStream(data);
                bim.EndInit();
                img.Source = bim;
            }
            catch(Exception e)
            {
                this.Close();
            }

   
        }

        private bool isMouseLeftButtonDown = false;
        Point previousMousePoint = new Point(0, 0);

        private void img_MouseDown(object sender, MouseButtonEventArgs e)
        {
            isMouseLeftButtonDown = true;
            previousMousePoint = e.GetPosition(img);
        }

        private void img_MouseUp(object sender, MouseButtonEventArgs e)
        { 
            isMouseLeftButtonDown = false;
        }

        private void img_MouseLeave(object sender, MouseEventArgs e)
        {
            isMouseLeftButtonDown = false;
        }

        private void img_MouseMove(object sender, MouseEventArgs e)
        {
            if (isMouseLeftButtonDown == true)
            {
                Point position = e.GetPosition(img);
                tlt.X += position.X - this.previousMousePoint.X;
                tlt.Y += position.Y - this.previousMousePoint.Y;
            }
        }

        private void img_MouseWheel(object sender, MouseWheelEventArgs e)
        { 
            Point centerPoint = e.GetPosition(img);

            double val = (double)e.Delta / 2000;
            if (sfr.ScaleX < 0.3 && sfr.ScaleY < 0.3 && e.Delta < 0)
            {
                return;
            }
            sfr.CenterX = centerPoint.X;
            sfr.CenterY = centerPoint.Y;

            if (val < 0 )
            {
                if (sfr.ScaleX <= 0.6) return;
                if (sfr.ScaleY <= 0.6) return;
            }

            sfr.ScaleX += val;
            sfr.ScaleY += val;
        }

        private void Window_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            this.Close();
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Escape) this.Close();
        } 


    }
}
