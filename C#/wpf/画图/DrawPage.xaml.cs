using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace UCP
{
    public enum shapeType
    {
        Line,
        Rectangle,       
    }

    /// <summary>
    /// DrawPage.xaml 的交互逻辑
    /// </summary>
    public partial class DrawPage : UserControl
    {
        #region drawModel
        class ViewModel : INotifyPropertyChanged
        {
            public event PropertyChangedEventHandler PropertyChanged;
            protected virtual void OnPropertyChanged(string propertyName = null)
            {
                if (PropertyChanged != null)
                    PropertyChanged.Invoke(this, new PropertyChangedEventArgs(propertyName));
            }
            private StrokeCollection inkStrokes;
            public StrokeCollection InkStrokes
            {
                get { return inkStrokes; }
                set
                {
                    inkStrokes = value;
                    OnPropertyChanged("InkStrokes");
                }
            }
        }
        ViewModel viewModel = new ViewModel { InkStrokes = new StrokeCollection() };
        DrawingAttributes attr = new DrawingAttributes()
        {
            Width = 2,
            Height = 2,
            StylusTip = StylusTip.Rectangle,
            IsHighlighter = false,
            IgnorePressure = true,
            Color = Color.FromArgb(255, 255, 0, 0)
        };
        #endregion


        #region TransPt
        double m_img_ScreenRatio = 1;
        Point ScreenToImgPt(System.Windows.Point ScreenPt)
        {
            Point pos = new Point();
            pos.X = ScreenPt.X * m_img_ScreenRatio;
            pos.Y = ScreenPt.Y * m_img_ScreenRatio;
            return pos;
        }
        Point ImgToScreenPt(System.Windows.Point imgPt)
        {
            Point pos = new Point();
            pos.X = imgPt.X / m_img_ScreenRatio;
            pos.Y = imgPt.Y / m_img_ScreenRatio;
            return pos;
        }
        void RectangleScreenPtCombine(System.Windows.Point ptScreen1, System.Windows.Point ptScreen2)
        {
            m_ScreenPointList.Clear();
            m_ScreenPointList.Add(ptScreen1);
            m_ScreenPointList.Add(new System.Windows.Point(ptScreen1.X, ptScreen2.Y));
            m_ScreenPointList.Add(ptScreen2);
            m_ScreenPointList.Add(new System.Windows.Point(ptScreen2.X, ptScreen1.Y));
            m_ScreenPointList.Add(ptScreen1);
        }
        void LineScreenPtCombine(System.Windows.Point ptScreen1, System.Windows.Point ptScreen2)
        {
            m_ScreenPointList.Clear();
            m_ScreenPointList.Add(ptScreen1);
            m_ScreenPointList.Add(ptScreen2);
        }
        void PtCombine(System.Windows.Point ptScreen1, System.Windows.Point ptScreen2)
        {
            if (m_ShapeType == shapeType.Line) LineScreenPtCombine(ptScreen1, ptScreen2);
            else RectangleScreenPtCombine(ptScreen1, ptScreen2);
        }
        #endregion

        public bool DisableDraw { get; set; }

        public DrawPage()
        {
            InitializeComponent();
            DataContext = viewModel;
        }

        double m_srcWidth = 1;
        shapeType m_ShapeType = shapeType.Line;
        public System.Windows.Point m_imgPtStart;
        public System.Windows.Point m_imgPtEnd;
        System.Windows.Point m_ScreenPtStart;
        List<System.Windows.Point> m_ScreenPointList = new List<System.Windows.Point>();

        public void Init(string imgPathName, shapeType _shapeType = shapeType.Line)
        {
            m_showTxt = "";
            inkCanvasMeasure.Children.Clear();
            viewModel.InkStrokes.Clear();
            GetImgSrcWidth(imgPathName);
            m_ShapeType = _shapeType;
            m_ScreenPointList.Clear();
            imgMeasure.Source = new BitmapImage(new Uri(imgPathName, UriKind.RelativeOrAbsolute));
        }
        public void ShowRect(int x1,int y1,int x2,int y2,string inf)
        {
            m_showTxt = inf;
            m_ShapeType = shapeType.Rectangle;

            m_imgPtStart.X = x1;
            m_imgPtStart.Y = y1;

            m_imgPtEnd.X = x2;
            m_imgPtEnd.Y = y2;

            PtCombine(ImgToScreenPt(m_imgPtStart), ImgToScreenPt(m_imgPtEnd));
            Draw();
        }
        Brush brRed = new SolidColorBrush(Color.FromRgb(255, 0, 0));
        string m_showTxt = "";
        void ShowTxt(Point pt)
        {
            if ("" == m_showTxt) return;

            TextBlock tb = new TextBlock();
            inkCanvasMeasure.Children.Add(tb);
            tb.Text = m_showTxt;
            tb.Foreground = brRed;
            tb.TextWrapping = TextWrapping.Wrap;
            InkCanvas.SetLeft(tb, pt.X);
            InkCanvas.SetTop(tb, pt.Y);
        }


        public bool hasDrawData()
        {
            if (m_ScreenPointList.Count < 2) return false;
            return true;
        }
        void inkCanvasMeasure_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            m_img_ScreenRatio = m_srcWidth / inkCanvasMeasure.Width;
            if (m_ScreenPointList.Count == 0) return;
            PtCombine(ImgToScreenPt(m_imgPtStart), ImgToScreenPt(m_imgPtEnd));
            Draw();
        }
        void Draw()
        {
            inkCanvasMeasure.Children.Clear();
            viewModel.InkStrokes.Clear();
            if (m_ScreenPointList.Count == 0) return;
            StylusPointCollection point = new StylusPointCollection(m_ScreenPointList);
            Stroke stroke = new Stroke(point) { DrawingAttributes = attr };
            viewModel.InkStrokes.Add(stroke);
            ShowTxt(m_ScreenPointList[0]);
        }
        void GetImgSrcWidth(string imgpathname)
        {
            using (FileStream fs = new FileStream(imgpathname, FileMode.Open, FileAccess.Read))
            {
                System.Drawing.Image image = System.Drawing.Image.FromStream(fs);
                m_srcWidth = image.Width;
            }
        }




        void inkCanvasMeasure_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (DisableDraw) return;
            m_ScreenPtStart = e.GetPosition(inkCanvasMeasure);
            m_imgPtStart = ScreenToImgPt(m_ScreenPtStart);
        }
        void inkCanvasMeasure_MouseMove(object sender, MouseEventArgs e)
        {
            if (DisableDraw) return;
            if (e.LeftButton != MouseButtonState.Pressed) return;
            PtCombine(m_ScreenPtStart, e.GetPosition(inkCanvasMeasure));
            Draw();
        }
        void inkCanvasMeasure_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (DisableDraw) return;
            Point ptScreenEnd = e.GetPosition(inkCanvasMeasure);
            m_imgPtEnd = ScreenToImgPt(ptScreenEnd);
            PtCombine(m_ScreenPtStart, ptScreenEnd);
            Draw();
        }
    }
}
