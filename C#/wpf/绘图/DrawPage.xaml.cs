using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;

namespace Comm
{
    public enum shapeType
    {
        Rectangle,
        Curve,
        Line,
    }
    public class DrawData
    {
        public DrawData() { guid = Guid.NewGuid().ToString("N"); }
        public string guid;
        public string drawName;
        public shapeType shape;
        public List<Point> ptList;
    }


    public class PtThrans
    {
        public static List<System.Windows.Point> ToScreenPt(List<Point> ptList,double img_ScreenRatio, shapeType shape = shapeType.Rectangle)
        {
            if (shape == shapeType.Rectangle)
            {
                if (ptList.Count != 2) return null;
                return PtThrans.RectangleScreenPtCombine(PtThrans.ImgToScreenPt(ptList[0], img_ScreenRatio),
                    PtThrans.ImgToScreenPt(ptList[1], img_ScreenRatio));
            }
            else
            {//shape == shapeType.Line   shape == shapeType.Curve
                List<System.Windows.Point> ScreenPtList = new List<System.Windows.Point>();
                foreach (var item in ptList) ScreenPtList.Add(PtThrans.ImgToScreenPt(item, img_ScreenRatio));
                return ScreenPtList;
            }
        }
        public static List<System.Windows.Point> ToImgPt(List<Point> ptScreenList, double img_ScreenRatio)
        {
           List<System.Windows.Point> ImgPtList = new List<System.Windows.Point>();
           foreach (var item in ptScreenList) ImgPtList.Add(ScreenToImgPt(item, img_ScreenRatio));
           return ImgPtList;
        }
        static Point ScreenToImgPt(System.Windows.Point ScreenPt, double img_ScreenRatio)
        {
            Point pos = new Point();
            pos.X = ScreenPt.X * img_ScreenRatio;
            pos.Y = ScreenPt.Y * img_ScreenRatio;
            return pos;
        }


        static Point ImgToScreenPt(System.Windows.Point imgPt, double img_ScreenRatio)
        {
            Point pos = new Point();
            pos.X = imgPt.X / img_ScreenRatio;
            pos.Y = imgPt.Y / img_ScreenRatio;
            return pos;
        }
        public static List<System.Windows.Point> RectangleScreenPtCombine(System.Windows.Point ptScreen1, System.Windows.Point ptScreen2)
        {
            List<System.Windows.Point> ScreenPtList = new List<Point>();
            ScreenPtList.Add(ptScreen1);
            ScreenPtList.Add(new System.Windows.Point(ptScreen1.X, ptScreen2.Y));
            ScreenPtList.Add(ptScreen2);
            ScreenPtList.Add(new System.Windows.Point(ptScreen2.X, ptScreen1.Y));
            ScreenPtList.Add(ptScreen1);
            return ScreenPtList;
        }
        public static List<System.Windows.Point> LineScreenPtCombine(System.Windows.Point ptScreen1, System.Windows.Point ptScreen2)
        {
            List<System.Windows.Point> ScreenPtList = new List<Point>();
            ScreenPtList.Add(ptScreen1);
            ScreenPtList.Add(ptScreen2);
            return ScreenPtList;
        }
    }


    public delegate void OneDrawFinish(DrawData dat);
    public delegate void MouseWheel(bool up);
    public delegate void DeleteOneDraw(DrawData deldat);

    /// <summary>
    /// DrawPage.xaml 的交互逻辑
    /// </summary>
    public partial class DrawPage : UserControl
    {
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

        public DrawPage()
        {
            InitializeComponent();
            DataContext = viewModel;
        }

        Brush brRed = new SolidColorBrush(Color.FromRgb(255, 0, 0));
        byte m_alpha = 255;
        ViewModel viewModel = new ViewModel{    InkStrokes = new StrokeCollection()};

        string m_DrawTypeName = "";
        shapeType m_ShapeType = shapeType.Rectangle;

        double m_img_ScreenRatio = 1;
        List<System.Windows.Point> m_ScreenPointList = new List<System.Windows.Point>();


        List<DrawData> m_dataHistory = null;

        public bool DisableUserDraw { get; set; }

        int m_selectedIndex = -1;
        void ResetSelected() { m_selectedIndex = -1; }


        OneDrawFinish m_OneDrawFinish = null;
        DeleteOneDraw m_DeleteOneDraw = null;
        MouseWheel m_MouseWheel = null;
        public void InitFun(OneDrawFinish addfun , DeleteOneDraw delFun, MouseWheel mvFun)
        { m_OneDrawFinish = addfun; m_DeleteOneDraw = delFun; m_MouseWheel = mvFun; }


        public void SetDrawName(string DrawTypeName) { m_DrawTypeName = DrawTypeName; }
        public void SetShapeType(shapeType ShapeType) { m_ShapeType = ShapeType; }


        public void Init(byte[] imgDat, List<DrawData> dat = null)
        {
            ResetSelected();
            MemoryStream ms = new MemoryStream(imgDat);
            ImageSourceConverter imgSrcCvt = new ImageSourceConverter();
            imgMeasure.Source = imgSrcCvt.ConvertFrom(ms) as System.Windows.Media.Imaging.BitmapFrame;

            m_dataHistory = dat;
            if (null == m_dataHistory) m_dataHistory = new List<DrawData>();
            CalcRatio();

            ClearDraw();
        }
        public void Init(string imgDatImg, List<DrawData> dat = null)
        {
            ResetSelected();
            try
            {
                FileInfo fi = new FileInfo(imgDatImg);
                byte[] imgDat = new byte[fi.Length];
                FileStream fs = new FileStream(imgDatImg, FileMode.Open);
                fs.Read(imgDat,0, imgDat.Length);
                fs.Close();
                Init(imgDat, dat);
            }
            catch (Exception e) { }
        }




        void ClearDraw()
        {
            inkCanvasMeasure.Children.Clear();
            viewModel.InkStrokes.Clear();

            if (null == m_dataHistory) return;
            for(int i = 0; i < m_dataHistory.Count;++i)
            {
                Draw(m_dataHistory[i].drawName, PtThrans.ToScreenPt(m_dataHistory[i].ptList, m_img_ScreenRatio, m_dataHistory[i].shape),m_selectedIndex == i);
            }
               
        }   
        void Draw(string DrawName, List<System.Windows.Point> ScreenPtlist, bool isChoice = false)
        {
            if (ScreenPtlist.Count == 0) return;

            DrawingAttributes attr = new DrawingAttributes()
            {
                Width = 2,
                Height = 2,
                StylusTip = StylusTip.Rectangle,
                IsHighlighter = false,
                IgnorePressure = true,
            };
            if (isChoice)
            {
                attr.Color = Color.FromArgb(m_alpha, 255, 255, 0);
                attr.Width = 3;
                attr.Height = 3;
            }
            else attr.Color = Color.FromArgb(m_alpha, 255, 0, 0);

            StylusPointCollection point = new StylusPointCollection(ScreenPtlist);
            Stroke stroke = new Stroke(point) { DrawingAttributes = attr };
            viewModel.InkStrokes.Add(stroke);
            ShowTxt(DrawName, ScreenPtlist[0]);
        }
        
        void ShowTxt(string txt, Point pt)
        {
            TextBlock tb = new TextBlock();
            inkCanvasMeasure.Children.Add(tb);
            tb.Text = txt;
            tb.Width = 100;
            tb.Foreground = brRed;
            tb.TextWrapping = TextWrapping.Wrap;
            InkCanvas.SetLeft(tb, pt.X);
            InkCanvas.SetTop(tb, pt.Y);
        }



        void CalcRatio() { m_img_ScreenRatio = imgMeasure.Source.Width / inkCanvasMeasure.Width; }
        void inkCanvasMeasure_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            CalcRatio();
            ClearDraw();
        }
        

        void inkCanvasMeasure_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (DisableUserDraw) return;
            ResetSelected();
            m_ScreenPointList.Clear();
            ClearDraw();
            m_ScreenPointList.Add(e.GetPosition(inkCanvasMeasure));
        }
        void inkCanvasMeasure_MouseMove(object sender, MouseEventArgs e)
        {
            if (DisableUserDraw) return;
            if (e.LeftButton != MouseButtonState.Pressed)
            {
                if (m_ScreenPointList.Count == 0) return;
                m_ScreenPointList.Clear();
                ClearDraw();
                return;
            }
         
            Point pt = e.GetPosition(inkCanvasMeasure);
            if (m_ShapeType == shapeType.Rectangle )
            {
                ClearDraw();
                Draw(m_DrawTypeName, PtThrans.RectangleScreenPtCombine(m_ScreenPointList[0], pt));
            }
            else if(m_ShapeType == shapeType.Line)
            {
                ClearDraw();
                Draw(m_DrawTypeName, PtThrans.LineScreenPtCombine(m_ScreenPointList[0], pt));
            }
            else
            {
                m_ScreenPointList.Add(pt);
                Draw(m_DrawTypeName, m_ScreenPointList);
            }
        }

        void inkCanvasMeasure_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (DisableUserDraw) return;
            if (m_ScreenPointList.Count == 0) return;
          
            if (m_ShapeType == shapeType.Rectangle || m_ShapeType == shapeType.Line) m_ScreenPointList.Add(e.GetPosition(inkCanvasMeasure));
            if (InvalidData())
            {
                ClearDraw();
                return;
            }

            DrawData dat = new DrawData();
            dat.drawName = m_DrawTypeName;
            dat.shape = m_ShapeType;
            dat.ptList = PtThrans.ToImgPt(m_ScreenPointList, m_img_ScreenRatio);
            m_OneDrawFinish?.Invoke(dat);
            m_dataHistory.Add(dat);
            m_ScreenPointList.Clear();
        }
        bool InvalidData()
        {
            if (m_ScreenPointList.Count >= 2)
            {
                double xLen = Math.Abs(m_ScreenPointList[0].X - m_ScreenPointList[1].X);
                double yLen = Math.Abs(m_ScreenPointList[0].Y - m_ScreenPointList[1].Y);
                double len = Math.Sqrt(xLen * xLen + yLen * yLen);
                if (len > 25) return false;
            }
            m_ScreenPointList.Clear();
            return true;
        }

        void UserControl_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            if (null == m_MouseWheel) return;
            if (e.Delta > 0) m_MouseWheel(true);
            else m_MouseWheel(false);
        }

        void UserControl_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (DisableUserDraw) return;
        }


        void UserControl_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            ++m_selectedIndex;
            if (m_selectedIndex >= m_dataHistory.Count) m_selectedIndex = 0;
            ClearDraw();
        }
        void UserControl_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.Enter)
            {
                if (m_selectedIndex < 0 || m_selectedIndex >= m_dataHistory.Count) return;
                DrawData delData = m_dataHistory[m_selectedIndex];
                m_DeleteOneDraw?.Invoke(delData);
                m_dataHistory.RemoveAt(m_selectedIndex);
                ResetSelected();
                ClearDraw();
            }
            else if(e.Key == Key.C)
            {
                if (m_dataHistory.Count == 0) return;
                foreach(var item in m_dataHistory) m_DeleteOneDraw?.Invoke(item);
                m_dataHistory.Clear();
                ResetSelected();
                ClearDraw();
            }
        }
    }
}
