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
        /// Упаковка компоненты цвета
        /// </summary>
        private static int PackComponent(int value, int max, int pos)
        {
            return (value * max / 255) << pos;
        }

        /// <summary>
        /// Упаковка компонентов цвета в формат 5-6-5
        /// </summary>
        private static ushort PackColor(Color color)
        {
            var temp = PackComponent(color.R, 31, 11) |
                       PackComponent(color.G, 63, 6) |
                       PackComponent(color.B, 31, 0);
            // Свап (LE)
            return (ushort)(((temp >> 8) & 0xFF) | ((temp & 0xFF) << 8));
        }

        /// <summary>
        /// Конвертирование RGB изображения в 16-бит цвет формата 5-6-5
        /// </summary>
        public static ushort[] Convert565(Bitmap image, Rectangle area)
        {
            var index = 0;
            var result = new ushort[area.Width * area.Height];
            // Обход пикселей
            for (var y = area.Top; y < area.Bottom; y++)
            {
                for (var x = area.Left; x < area.Right; x++)
                    result[index++] = PackColor(image.GetPixel(x, y));
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
        /// Определение размеров реального изображения с учетом указанного цвета фона
        /// </summary>
        public static Rectangle ImageBounds(Bitmap image, Rectangle area, Color backgroundColor)
        {
            var left = FindPointForward(
                area.Left, area.Right, area.Top, area.Bottom, 
                (x, y) => IsDifferentColors(image.GetPixel(x, y), backgroundColor));

            var right = FindPointBackward(
                area.Left, area.Right, area.Top, area.Bottom, 
                (x, y) => IsDifferentColors(image.GetPixel(x, y), backgroundColor));

            var top = FindPointForward(
                area.Top, area.Bottom, area.Left, area.Right,
                (y, x) => IsDifferentColors(image.GetPixel(x, y), backgroundColor));

            var bottom = FindPointBackward(
                area.Top, area.Bottom, area.Left, area.Right,
                (y, x) => IsDifferentColors(image.GetPixel(x, y), backgroundColor));

            return new Rectangle(left, top, right - left, bottom - top);
        }
        #endregion
    }
}
