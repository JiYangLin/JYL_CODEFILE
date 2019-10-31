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
    public class FlipTile3D : WrapperElement<Viewport3D>
    {
        private string PicPath = "Picture";
        int PicWidth = 1920;
        int PicHeight = 1080;


        private const int c_xCount = 6, c_yCount = 4;

        private Point _lastMouse = new Point(double.NaN, double.NaN);
        private bool _isFlipped = false;

        private readonly TileData[] _tiles = new TileData[c_xCount * c_yCount];//界面上显示的每一个图像
        private readonly DiffuseMaterial _backMaterial = new DiffuseMaterial();
        private readonly RenderingListener m_listener = new RenderingListener();


        public FlipTile3D()
            : base(new Viewport3D())
        {

            Dispatcher.BeginInvoke(DispatcherPriority.ApplicationIdle, new Action(setup3D));

            m_listener.Rendering += tick;

            this.Unloaded += delegate(object sender, RoutedEventArgs e)
            {
                m_listener.StopListening();
            };
        }
        private void tick(object sender, EventArgs e)
        {
            Vector mouseFixed = fixMouse(_lastMouse, this.RenderSize);
            for (int i = 0; i < _tiles.Length; i++)
            {
                //unwire Render event if nothing happens
                _tiles[i].TickData(mouseFixed, _isFlipped);
            }
            
        }

        private void setup3D()
        {
          IList<DiffuseMaterial> _materials = GetPictures.Process(PicPath, PicWidth, PicHeight);

        //perf improvement. Clipping in 3D is expensive. Avoid if you can!
        WrappedElement.ClipToBounds = false;

            PerspectiveCamera camera = new PerspectiveCamera(
                new Point3D(0, 0, 3.73), //position
                new Vector3D(0, 0, -1), //lookDirection
                new Vector3D(0, 1, 0), //upDirection
                30 //FOV
                );

            WrappedElement.Camera = camera;

            Model3DGroup everything = new Model3DGroup();

            Model3DGroup lights = new Model3DGroup();
            DirectionalLight whiteLight = new DirectionalLight(Colors.White, new Vector3D(0, 0, -1));
            lights.Children.Add(whiteLight);

            everything.Children.Add(lights);

            ModelVisual3D model = new ModelVisual3D();

            double tileSizeX = 2.0 / c_xCount;
            double startX = -((double)c_xCount) / 2 * tileSizeX + tileSizeX / 2;
            double startY = -((double)c_yCount) / 2 * tileSizeX + tileSizeX / 2;

            Size tileTextureSize = new Size(1.0 / c_xCount, 1.0 / c_yCount);

            //so, tiles are numbers, left-to-right (ascending x), bottom-to-top (ascending y)
            for (int x = 0; x < c_xCount; x++)
            {
                for (int y = 0; y < c_yCount; y++)
               {
                    int index = y * c_xCount + x;

                    Rect backTextureCoordinates = new Rect(
                        x * tileTextureSize.Width,

                        // this will give you a headache. Exists since we are going 
                        // from bottom bottomLeft of 3D space (negative Y is down), 
                        // but texture coor are negative Y is up
                        1 - y * tileTextureSize.Height - tileTextureSize.Height,

                        tileTextureSize.Width, tileTextureSize.Height);

                    _tiles[index] = new TileData();
                    _tiles[index].Setup3DItem(
                        everything,
                        _materials[index % _materials.Count],
                        new Size(tileSizeX, tileSizeX),
                        new Point(startX + x * tileSizeX, startY + y * tileSizeX),
                        _backMaterial,
                        backTextureCoordinates);
                }
            }

            model.Content = everything;

            WrappedElement.Children.Add(model);

            //start the per-frame tick for the physics
            m_listener.StartListening();
        }



        protected override void OnRender(System.Windows.Media.DrawingContext drawingContext)
        {
            base.OnRender(drawingContext);
            drawingContext.DrawRectangle(Brushes.Transparent, null, new Rect(this.RenderSize));
        }
        protected override void OnMouseMove(System.Windows.Input.MouseEventArgs e)
        {
            _lastMouse = e.GetPosition(this);
        }
        protected override void OnMouseLeave(MouseEventArgs e)
        {
            _lastMouse = new Point(double.NaN, double.NaN);
        }
        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            _isFlipped = !_isFlipped;
            if (_isFlipped)
            {
                setClickedItem(e.GetPosition(this));
            }
        }
        private void setClickedItem(Point point)
        {
            //x方向位置
            double sizeX = this.ActualWidth / c_xCount;
            int xIndex = (int)(point.X / sizeX);

            //y方向位置
            point -= (new Vector(this.ActualWidth / 2, this.ActualHeight / 2));
            //flip y
            point.Y *= -1;
            //scale it to 1x1 dimentions
            double scale = Math.Min(this.ActualHeight, this.ActualWidth) / 2;
            point = (Point)(((Vector)point) / scale);
            //set it up so that bottomLeft = 0,0 & topRight = 1,1 (positive Y is up)
            point = (Point)(((Vector)point + new Vector(1, 1)) * .5);
            //scale up so that the point coordinates align w/ the x/y scale
            point.Y *= c_yCount;
            //now we have the indicies of the y of the tile clicked
            int yIndex = (int)Math.Floor(point.Y);

            int tileIndex = yIndex * c_xCount + xIndex;
            _backMaterial.Brush = _tiles[tileIndex].DiffuseMaterial.Brush;
        }



        private static Vector fixMouse(Point mouse, Size size)
        {

            Debug.Assert(size.Width >= 0 && size.Height >= 0);
            double scale = Math.Max(size.Width, size.Height) / 2;
            // Translate y going down to y going up
            mouse.Y = -mouse.Y + size.Height;

            mouse.Y -= size.Height / 2;
            mouse.X -= size.Width / 2;


            Vector v = new Vector(mouse.X, mouse.Y);
            v /= scale;

            return v;
        }
    }
}