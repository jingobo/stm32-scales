using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;

namespace Vito.Resgen
{
    /// <summary>
    /// Класс утилитных функция для работы с изображением
    /// </summary>
    internal static class ImageTools
    {
        #region convert
        /// <summary>
        /// Конвертирование RGB изображения в 8-бит градацию серого
        /// </summary>
        public static byte[] ConvertGrayscale(Bitmap image, Rectangle area)
        {
            var index = 0;
            var result = new byte[area.Width * area.Height];
            // Обход пикселей
            for (var y = area.Top; y < area.Bottom; y++)
            {
                for (var x = area.Left; x < area.Right; x++)
                {
                    var color = image.GetPixel(x, y);
                    // Среднее значение RGB
                    var avg = 255 - (color.R + color.G + color.B) / 3;
                    // Применение альфа канала
                    avg = avg * color.A / 255;
                    // В буфер
                    result[index++] = (byte)avg;
                }
            }
            return result;
        }
        #endregion

        #region bounds
        /// <summary>
        /// Поиск точки в прямом направлении
        /// </summary>
        private static int FindPointForward(int findLeft, int findRight, int testLeft, int testRight, Func<int, int, bool> check)
        {
            for (var x = findLeft; x < findRight; x++)
                for (var y = testLeft; y < testRight; y++)
                    if (check(x, y))
                        return x;
            return -1;
        }

        /// <summary>
        /// Поиск точки в обратном направлении
        /// </summary>
        private static int FindPointBackward(int findLeft, int findRight, int testLeft, int testRight, Func<int, int, bool> check)
        {
            for (var x = findRight - 1; x >= findLeft; x--)
                for (var y = testLeft; y < testRight; y++)
                    if (check(x, y))
                        return x;
            return -1;
        }

        /// <summary>
        /// Сравнение двух цветов
        /// </summary>
        private static bool IsDifferentColors(Color a, Color b)
        {
            return a.ToArgb() != b.ToArgb();
        }

        /// <summary>
        /// Определение размеров реального изображения с учетом белого цвета фона
        /// </summary>
        public static Rectangle ImageBounds(Bitmap image, Rectangle area)
        {
            var left = FindPointForward(
                area.Left, area.Right, area.Top, area.Bottom, 
                (x, y) => IsDifferentColors(image.GetPixel(x, y), Color.White));

            var right = FindPointBackward(
                area.Left, area.Right, area.Top, area.Bottom, 
                (x, y) => IsDifferentColors(image.GetPixel(x, y), Color.White));

            var top = FindPointForward(
                area.Top, area.Bottom, area.Left, area.Right,
                (y, x) => IsDifferentColors(image.GetPixel(x, y), Color.White));

            var bottom = FindPointBackward(
                area.Top, area.Bottom, area.Left, area.Right,
                (y, x) => IsDifferentColors(image.GetPixel(x, y), Color.White));

            return new Rectangle(left, top, right - left, bottom - top);
        }
        #endregion
    }
}
