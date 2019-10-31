using _3DTools;
using System;
using System.Collections.Generic;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;

/*
 *  xmlns:PICBROWSER="clr-namespace:PICBROWSER"
 *  <PICBROWSER:PicBrowser x:Name="PicBrow"/>
 *  
 *          List<BitmapImage> images = new List<BitmapImage>();
            images.Add(new BitmapImage(new Uri("D:\\1.jpg")));
            PicBrow.showImgList(images);
 *  
 */


namespace PICBROWSER
{
    /// <summary>
    /// PicBrowser.xaml 的交互逻辑
    /// </summary>
    public partial class PicBrowser : UserControl
    {

        #region CurrentMidIndexProperty
        public static readonly DependencyProperty CurrentMidIndexProperty = DependencyProperty.Register(
            "CurrentMidIndex", typeof(double), typeof(PicBrowser),
            new FrameworkPropertyMetadata(new PropertyChangedCallback(CurrentMidIndexPropertyChangedCallback)));

        private static void CurrentMidIndexPropertyChangedCallback(DependencyObject sender, DependencyPropertyChangedEventArgs arg)
        {
            PicBrowser win = sender as PicBrowser;
            if (win != null)
            {
                win.ReLayoutInteractiveVisual3D();
            }
        }

        /// <summary>
        /// 获取或设置当前中间项序号
        /// </summary>
        public double CurrentMidIndex
        {
            get
            {
                return (double)this.GetValue(CurrentMidIndexProperty);
            }
            set
            {
                this.SetValue(CurrentMidIndexProperty, value);
            }
        }
        #endregion

        #region ModelAngleProperty
        public static readonly DependencyProperty ModelAngleProperty = DependencyProperty.Register(
            "ModelAngle", typeof(double), typeof(PicBrowser),
            new FrameworkPropertyMetadata(10.0, new PropertyChangedCallback(ModelAnglePropertyChangedCallback)));


        private static void ModelAnglePropertyChangedCallback(DependencyObject sender, DependencyPropertyChangedEventArgs arg)
        {
            PicBrowser win = sender as PicBrowser;
            if (win != null)
            {
                win.ReLayoutInteractiveVisual3D();
            }
        }

        /// <summary>
        /// 获取或设置模型沿Y轴的旋转角度
        /// </summary>
        public double ModelAngle
        {
            get
            {
                return (double)this.GetValue(ModelAngleProperty);
            }
            set
            {
                this.SetValue(ModelAngleProperty, value);
            }
        }
        #endregion

        #region XDistanceBetweenModelsProperty
        public static readonly DependencyProperty XDistanceBetweenModelsProperty = DependencyProperty.Register(
            "XDistanceBetweenModels", typeof(double), typeof(PicBrowser),
            new FrameworkPropertyMetadata(0.8, XDistanceBetweenModelsPropertyChangedCallback));

        private static void XDistanceBetweenModelsPropertyChangedCallback(DependencyObject sender, DependencyPropertyChangedEventArgs arg)
        {
            PicBrowser win = sender as PicBrowser;
            if (win != null)
            {
                win.ReLayoutInteractiveVisual3D();
            }
        }

        /// <summary>
        /// 获取或设置X方向上两个模型间的距离
        /// </summary>
        public double XDistanceBetweenModels
        {
            get
            {
                return (double)this.GetValue(XDistanceBetweenModelsProperty);
            }
            set
            {
                this.SetValue(XDistanceBetweenModelsProperty, value);
            }
        }
        #endregion

        #region ZDistanceBetweenModelsProperty
        public static readonly DependencyProperty ZDistanceBetweenModelsProperty = DependencyProperty.Register(
            "ZDistanceBetweenModels", typeof(double), typeof(PicBrowser),
            new FrameworkPropertyMetadata(1.2, ZDistanceBetweenModelsPropertyChangedCallback));

        private static void ZDistanceBetweenModelsPropertyChangedCallback(DependencyObject sender, DependencyPropertyChangedEventArgs arg)
        {
            PicBrowser win = sender as PicBrowser;
            if (win != null)
            {
                win.ReLayoutInteractiveVisual3D();
            }
        }

        /// <summary>
        /// 获取或设置Z方向上两个模型间的距离
        /// </summary>
        public double ZDistanceBetweenModels
        {
            get
            {
                return (double)this.GetValue(ZDistanceBetweenModelsProperty);
            }
            set
            {
                this.SetValue(ZDistanceBetweenModelsProperty, value);
            }
        }
        #endregion

        #region MidModelDistanceProperty
        public static readonly DependencyProperty MidModelDistanceProperty = DependencyProperty.Register(
            "MidModelDistance", typeof(double), typeof(PicBrowser),
            new FrameworkPropertyMetadata(1.0, MidModelDistancePropertyChangedCallback));

        private static void MidModelDistancePropertyChangedCallback(DependencyObject sender, DependencyPropertyChangedEventArgs arg)
        {
            PicBrowser win = sender as PicBrowser;
            if (win != null)
            {
                win.ReLayoutInteractiveVisual3D();
            }
        }

        /// <summary>
        /// 获取或设置中间的模型距离两边模型的距离
        /// </summary>
        public double MidModelDistance
        {
            get
            {
                return (double)this.GetValue(MidModelDistanceProperty);
            }
            set
            {
                this.SetValue(MidModelDistanceProperty, value);
            }
        }
        #endregion


        public PicBrowser()
        { 
            InitializeComponent();
            viewport3D.MouseLeftButtonDown += MouseLeft_ButtonDown;
        }
        void MouseLeft_ButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed) this.CurrentMidIndex++;
            else this.CurrentMidIndex--;
        }
        private void showImg(string filename)
        {
            IMGSHOW.Source = null;
            if (File.Exists(filename)) IMGSHOW.Source = new BitmapImage(new Uri(filename));
        }
        public void showImgList(List<BitmapImage> images)
        {  
            if (images == null) return;
            this.viewport3D.Children.Clear();
            for (int i = 0; i < images.Count; i++)
            {
                InteractiveVisual3D iv3d = new InteractiveVisual3D();
                iv3d.Visual = this.CreateVisual(images[i], i);
                iv3d.Geometry = this.CreateGeometry3D();
                iv3d.Transform = this.CreateEmptyTransform3DGroup();

                ModelVisual3D Panel = new ModelVisual3D();
                AmbientLight Light = new AmbientLight();
                Panel.Content = Light;
                this.viewport3D.Children.Add(Panel);
                this.viewport3D.Children.Add(iv3d);
            }
            this.CurrentMidIndex = (int)(images.Count / 2);
            this.ReLayoutInteractiveVisual3D();
        }
        /// <summary>
        ///  由指定的图片路径创建一个可视对象
        /// </summary>
        /// <param name="imageFile">图片路径</param>
        /// <returns>创建的可视对象</returns>
        private Visual CreateVisual(BitmapImage bmp, int index)
        {
            Image img = new Image();
            img.Width = 50;
            img.Source = bmp;

            Border outBordre = new Border();
            outBordre.BorderBrush = Brushes.White;
            outBordre.BorderThickness = new Thickness(0.5);
            outBordre.Child = img;

            outBordre.MouseDown += delegate (object sender, MouseButtonEventArgs e)
            {
                this.CurrentMidIndex = index;
                e.Handled = true;
            };

            return outBordre;
        }

        /// <summary>
        /// 创建3D图形
        /// </summary>
        /// <returns>创建的3D图形</returns>
        private Geometry3D CreateGeometry3D()
        {
            MeshGeometry3D geometry = new MeshGeometry3D();

            geometry.Positions = new Point3DCollection();
            geometry.Positions.Add(new Point3D(-1, 1, 0));
            geometry.Positions.Add(new Point3D(-1, -1, 0));
            geometry.Positions.Add(new Point3D(1, -1, 0));
            geometry.Positions.Add(new Point3D(1, 1, 0));

            geometry.TriangleIndices = new Int32Collection();
            geometry.TriangleIndices.Add(0);
            geometry.TriangleIndices.Add(1);
            geometry.TriangleIndices.Add(2);
            geometry.TriangleIndices.Add(0);
            geometry.TriangleIndices.Add(2);
            geometry.TriangleIndices.Add(3);

            geometry.TextureCoordinates = new PointCollection();
            geometry.TextureCoordinates.Add(new Point(0, 0));
            geometry.TextureCoordinates.Add(new Point(0, 1));
            geometry.TextureCoordinates.Add(new Point(1, 1));
            geometry.TextureCoordinates.Add(new Point(1, 0));

            return geometry;
        }



        /// <summary>
        /// 创建一个空的Transform3DGroup
        /// </summary>
        /// <returns></returns>
        private Transform3DGroup CreateEmptyTransform3DGroup()
        {
            Transform3DGroup group = new Transform3DGroup();
            group.Children.Add(new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(0, 1, 0), 0)));
            group.Children.Add(new TranslateTransform3D(new Vector3D()));
            group.Children.Add(new ScaleTransform3D());

            return group;
        }

        /// <summary>
        /// 依照InteractiveVisual3D在列表中的序号来变换其位置等
        /// </summary>
        /// <param name="index">在列表中的序号</param>
        /// <param name="midIndex">列表中被作为中间项的序号</param>
        private void GetTransformOfInteractiveVisual3D(int index, double midIndex, out double angle, out double offsetX, out double offsetZ)
        {
            double disToMidIndex = index - midIndex;


            //旋转,两翼的图片各旋转一定的度数
            angle = 0;
            if (disToMidIndex < 0)
            {
                angle = this.ModelAngle;//左边的旋转N度
            }
            else if (disToMidIndex > 0)
            {
                angle = (-this.ModelAngle);//右边的旋转-N度
            }



            //平移,两翼的图片逐渐向X轴负和正两个方向展开
            offsetX = 0;//中间的不平移

            this.MidModelDistance = 2.3;

            if (Math.Abs(disToMidIndex) <= 1)
            {
                offsetX = disToMidIndex * this.MidModelDistance;
            }
            else if (disToMidIndex != 0)
            {
                offsetX = disToMidIndex * this.XDistanceBetweenModels + (disToMidIndex > 0 ? this.MidModelDistance : -this.MidModelDistance);
            }


            //两翼的图片逐渐向Z轴负方向移动一点,造成中间突出(离观众较近的效果)
            offsetZ = Math.Abs(disToMidIndex) * -this.ZDistanceBetweenModels;

        }

        /// <summary>
        /// 重新布局3D内容
        /// </summary>
        private void ReLayoutInteractiveVisual3D()
        {
            int j = 0;
            for (int i = 0; i < this.viewport3D.Children.Count; i++)
            {
                InteractiveVisual3D iv3d = this.viewport3D.Children[i] as InteractiveVisual3D;
                if (iv3d != null)
                {
                    double angle = 0;
                    double offsetX = 0;
                    double offsetZ = 0;
                    this.GetTransformOfInteractiveVisual3D(j++, this.CurrentMidIndex, out angle, out offsetX, out offsetZ);


                    NameScope.SetNameScope(this, new NameScope());
                    this.RegisterName("iv3d", iv3d);
                    Duration time = new Duration(TimeSpan.FromSeconds(0.3));

                    DoubleAnimation angleAnimation = new DoubleAnimation(angle, time);
                    DoubleAnimation xAnimation = new DoubleAnimation(offsetX, time);
                    DoubleAnimation zAnimation = new DoubleAnimation(offsetZ, time);

                    Storyboard story = new Storyboard();
                    story.Children.Add(angleAnimation);
                    story.Children.Add(xAnimation);
                    story.Children.Add(zAnimation);

                    Storyboard.SetTargetName(angleAnimation, "iv3d");
                    Storyboard.SetTargetName(xAnimation, "iv3d");
                    Storyboard.SetTargetName(zAnimation, "iv3d");

                    Storyboard.SetTargetProperty(
                        angleAnimation,
                        new PropertyPath("(ModelVisual3D.Transform).(Transform3DGroup.Children)[0].(RotateTransform3D.Rotation).(AxisAngleRotation3D.Angle)"));

                    Storyboard.SetTargetProperty(
                        xAnimation,
                        new PropertyPath("(ModelVisual3D.Transform).(Transform3DGroup.Children)[1].(TranslateTransform3D.OffsetX)"));
                    Storyboard.SetTargetProperty(
                        zAnimation,
                        new PropertyPath("(ModelVisual3D.Transform).(Transform3DGroup.Children)[1].(TranslateTransform3D.OffsetZ)"));

                    story.Begin(this);

                }
            }
        }

    }
}
