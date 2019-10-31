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
    public class RenderingListener : DispatcherObject, IDisposable
    {
        public RenderingListener() { }

        public void StartListening()
        {
            requireAccessAndNotDisposed();

            if (!m_isListening)
            {
                m_isListening = true;
                CompositionTarget.Rendering += compositionTarget_Rendering;
            }
        }

        public void StopListening()
        {
            requireAccessAndNotDisposed();

            if (m_isListening)
            {
                m_isListening = false;
                CompositionTarget.Rendering -= compositionTarget_Rendering;
            }
        }

        public void WireParentLoadedUnloaded(FrameworkElement parent)
        {
            requireAccessAndNotDisposed();

            parent.Loaded += delegate (object sender, RoutedEventArgs e)
            {
                this.StartListening();
            };

            parent.Unloaded += delegate (object sender, RoutedEventArgs e)
            {
                this.StopListening();
            };
        }

        public bool IsDisposed
        {
            get
            {
                VerifyAccess();
                return m_disposed;
            }
        }

        public event EventHandler Rendering;

        protected virtual void OnRendering(EventArgs args)
        {
            requireAccessAndNotDisposed();

            EventHandler handler = Rendering;
            if (handler != null)
            {
                handler(this, args);
            }
        }

        public void Dispose()
        {
            requireAccessAndNotDisposed();
            StopListening();

            Delegate[] invocationlist = Rendering.GetInvocationList();
            foreach (Delegate d in invocationlist)
            {
                Rendering -= (EventHandler)d;
            }

            m_disposed = true;
        }

        [DebuggerStepThrough]
        private void requireAccessAndNotDisposed()
        {
            VerifyAccess();
            if (m_disposed)
            {
                throw new ObjectDisposedException(string.Empty);
            }
        }

        private void compositionTarget_Rendering(object sender, EventArgs e)
        {
            OnRendering(e);
        }

        private bool m_isListening;
        private bool m_disposed;

    }
}
