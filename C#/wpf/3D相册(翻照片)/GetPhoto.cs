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
    public class GetPictures
    {
        public static IList<DiffuseMaterial> Process(string Path, int width, int height)
        {
            IList<DiffuseMaterial> materials;
            IList<string> files = null;
            if (!string.IsNullOrEmpty(Path))
            {
                files = new List<string>();
                try
                {
                    DirectoryInfo di = new DirectoryInfo(Path);
                    if (di.Exists)
                    {
                        foreach (FileInfo fileInfo in di.GetFiles("*.jpg"))
                        {
                            files.Add(fileInfo.FullName);
                            if (files.Count > 50)
                                break;
                        }
                    }
                }
                catch (IOException) { }
                catch (ArgumentException) { }
                catch (SecurityException) { }
            }
            if (files.Count > 0)
            {
                materials = new List<DiffuseMaterial>();

                foreach (string file in files)
                {
                    Uri uri = new Uri(file);

                    BitmapImage bitmapImage = new BitmapImage();
                    bitmapImage.BeginInit();
                    bitmapImage.UriSource = uri;
                    bitmapImage.DecodePixelWidth = width;
                    bitmapImage.DecodePixelHeight = height;
                    bitmapImage.EndInit();

                    bitmapImage.Freeze();

                    ImageBrush imageBrush = new ImageBrush(bitmapImage);
                    imageBrush.Stretch = Stretch.UniformToFill;
                    imageBrush.ViewportUnits = BrushMappingMode.Absolute;
                    imageBrush.Freeze();

                    DiffuseMaterial diffuseMaterial = new DiffuseMaterial(imageBrush);
                    materials.Add(diffuseMaterial);
                }
            }
            else
            {
                Brush[] brushes = new Brush[] {
                    Brushes.LightBlue,
                    Brushes.Pink,
                    Brushes.LightGray,
                    Brushes.Yellow,
                    Brushes.Orange,
                    Brushes.LightGreen };

                DiffuseMaterial[] materialsArray =
                    brushes.Select(brush => new DiffuseMaterial(brush)).ToArray();

                materials = materialsArray;
            }
            return materials;
        }
    }
}