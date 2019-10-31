using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Threading;
using System.IO;
using System.Security;

namespace PHOTO3D
{
    public abstract class WrapperElement<TElement> : FrameworkElement
       where TElement : UIElement
    {
        protected WrapperElement(TElement element)
        {
            m_element = element;

            AddVisualChild(m_element);
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            m_element.Measure(availableSize);
            return m_element.DesiredSize;
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            m_element.Arrange(new Rect(finalSize));
            return m_element.RenderSize;
        }

        protected override int VisualChildrenCount
        {
            get
            {
                return 1;
            }
        }

        protected override Visual GetVisualChild(int index)
        {
            return m_element;
        }

        protected TElement WrappedElement { get { return m_element; } }

        private readonly TElement m_element;
    }
}
