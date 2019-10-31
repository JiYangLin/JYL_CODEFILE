using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media.Animation;

namespace PageContainer
{
    public enum PageTransitionType
    {
        Fade,
        Slide,
        SlideAndFade,
        Grow,
        GrowAndFade,
        Flip,
        FlipAndFade,
        Spin,
        SpinAndFade
    }
    public class CenterConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return (double)value / 2;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }





    /// <summary>
    /// PageTransition.xaml 的交互逻辑
    /// </summary>
    public partial class PageTransition : UserControl
    {
        Stack<UserControl> pages = new Stack<UserControl>();

        public UserControl CurrentPage { get; set; }

        public static readonly DependencyProperty TransitionTypeProperty = DependencyProperty.Register("TransitionType",
            typeof(PageTransitionType),
            typeof(PageTransition), new PropertyMetadata(PageTransitionType.SlideAndFade));

        public PageTransitionType TransitionType
        {
            get
            {
                return (PageTransitionType)GetValue(TransitionTypeProperty);
            }
            set
            {
                SetValue(TransitionTypeProperty, value);
            }
        }

        public PageTransition()
        {
            InitializeComponent();
        }

        public void ShowPage(UserControl newPage)
        {
            pages.Push(newPage);
            Task.Factory.StartNew(() => ShowNewPage());
        }

        public void UnShowPage()
        {
            Storyboard hidePage = (Resources[string.Format("{0}Out", TransitionType.ToString())] as Storyboard).Clone();
            
            hidePage.Begin(contentPresenter);
        }

        void ShowNewPage()
        {
            Dispatcher.Invoke((Action)delegate
            {

                if (contentPresenter.Content != null)
                {
                    UserControl oldPage = contentPresenter.Content as UserControl;

                    if (oldPage != null)
                    {
                        oldPage.Loaded -= newPage_Loaded;

                        UnloadPage(oldPage);

                    }
                }
                else
                {
                    ShowNextPage();
                }

            });
        }

        void ShowNextPage()
        {
            UserControl newPage = pages.Pop();

            newPage.Loaded += newPage_Loaded;

            contentPresenter.Content = newPage;
        }

        void UnloadPage(UserControl page)
        {
            Storyboard hidePage = (Resources[string.Format("{0}Out", TransitionType.ToString())] as Storyboard).Clone();

            hidePage.Completed += hidePage_Completed;

            hidePage.Begin(contentPresenter);
        }

        void newPage_Loaded(object sender, RoutedEventArgs e)
        {
            Storyboard showNewPage = Resources[string.Format("{0}In", TransitionType.ToString())] as Storyboard;

            showNewPage.Begin(contentPresenter);

            CurrentPage = sender as UserControl;
        }

        void hidePage_Completed(object sender, EventArgs e)
        {
            contentPresenter.Content = null;
            ShowNextPage();
        }
    }
}


//xmlns:PageContainer="clr-namespace:PageContainer"
//<PageContainer:PageTransition  x:Name="page_Container" TransitionType="SlideAndFade"  />

//UserControl1 uc = new UserControl1();
//page_Container.ShowPage(uc);
//page_Container.UnShowPage();