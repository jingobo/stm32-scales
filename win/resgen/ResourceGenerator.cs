using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;

namespace Vito.Resgen
{
    /// <summary>
    /// Класс методов генерации ресурсов
    /// </summary>
    internal static class ResourceGenerator
    {
        #region types
        /// <summary>
        /// Класс результатов генерации
        /// </summary>
        public sealed class Data
        {
            /// <summary>
            /// Получает текст результата
            /// </summary>
            public string Text
            {
                get;
                private set;
            }

            /// <summary>
            /// Получает флаг ошибки
            /// </summary>
            public bool IsError
            {
                get;
                private set;
            }

            /// <summary>
            /// Конструктор по умолчанию
            /// </summary>
            public Data(string text, bool error = true)
            {
                // Проверка аргументов
                if (String.IsNullOrEmpty(text))
                    throw new ArgumentNullException("text");
                // Инициализация полей
                IsError = error;
                Text = text;
            }
        }
        #endregion

        #region methods
        /// <summary>
        /// Генерация из шрифта
        /// </summary>
        public static Data FromFont(Font font, string fontSymbols)
        {
            // Проверка аргументов
            if (font == null)
                throw new ArgumentNullException("font");
            if (String.IsNullOrEmpty(fontSymbols))
                throw new ArgumentNullException("fontSymbols");

            // Символ пробела
            const string SPACE = " ";
            // Размер буфера рендера шрифта в пикселях
            // TODO: размер подобрать под размер шрифта
            var bufferArea = new Rectangle(0, 0, 64, 64);

            // Подготовка
            var bytes = new List<byte>();
            var cp1251 = Encoding.GetEncoding(1251);
            // Удаляем пробелы
            fontSymbols = fontSymbols.Replace(SPACE, String.Empty);

            // Рендер
            using (var buffer = new Bitmap(bufferArea.Width, bufferArea.Height))
            {
                var g = Graphics.FromImage(buffer);
                // Получаем высоту шрифта
                var height = (byte)g.MeasureString("0", font, int.MaxValue, StringFormat.GenericTypographic).Height;
                // Проверка размера
                if (bufferArea.Height < height)
                    return new Data("Шрифт сликом большой!");
                var widthAvg = 0;
                // Высота шрифта
                bytes.Add(height);
                // Функция добавления символа
                Action<String, Rectangle> convert = (symbol, area) =>
                {
                    // Конвертирование в оттенки серого
                    var pixels = ImageTools.ConvertGrayscale(buffer, area);
                    // Компрессия
                    var raw = ByteTools.RleCompressor(pixels);
                    // Запись заголовка
                    // Выводимый символ
                    bytes.Add(cp1251.GetBytes(symbol).First());
                    // Ширина в пикселях
                    bytes.Add((byte)area.Width);
                    // Длинна в байтах
                    bytes.AddRange(BitConverter.GetBytes((ushort)raw.Length));
                    // Данные
                    bytes.AddRange(raw);
                };
                // Обход символов
                foreach (var c in fontSymbols.Select(c => new string(c, 1)))
                {
                    // Вывод символа
                    g.Clear(Color.White);
                    g.DrawString(c, font, Brushes.Black, bufferArea.X, bufferArea.Y, StringFormat.GenericTypographic);
                    // Определение реальных размеров символа
                    var symbolArea = ImageTools.ImageBounds(buffer, bufferArea);
                    // По горизонтали не учитываем
                    symbolArea.Y = bufferArea.Y;
                    symbolArea.Height = height;
                    // Конвертирование
                    convert(c, symbolArea);
                    // Усредение ширины
                    widthAvg += symbolArea.Width;
                }
                // Пробел
                g.Clear(Color.White);
                convert(SPACE, new Rectangle(0, 0, widthAvg / fontSymbols.Length / 2, height));
            }
            // EOF
            bytes.AddRange(BitConverter.GetBytes(0));
            // Конвертирование в массив байт
            return new Data(ByteTools.BytesToCppBlock(bytes), false);
        }

        /// <summary>
        /// Генерация из изображения
        /// </summary>
        public static Data FromImage(string fileName)
        {
            // Открытие изображения
            Image image;
            try
            {
                image = Image.FromFile(fileName);
            }
            catch
            {
                return new Data("Не удалось открыть файл изображения!");
            }
            if (image.Width > 255 || image.Height > 255)
            {
                return new Data("Изображение превышает размеры 255x255!");
            }
            var bytes = new List<byte>();
            // Конвертирование в битмап
            using (var buffer = new Bitmap(image))
            {
                var area = new Rectangle(0, 0, buffer.Width, buffer.Height);
                // Конвертирование в оттенки серого
                var pixels = ImageTools.ConvertGrayscale(buffer, area);
                // Компрессия
                var raw = ByteTools.RleCompressor(pixels);
                // Запись заголовка
                bytes.Add((byte)area.Width);
                bytes.Add((byte)area.Height);
                // Запись данных
                bytes.AddRange(raw);
            }
            image.Dispose();
            // Конвертирование в массив байт
            return new Data(ByteTools.BytesToCppBlock(bytes), false);
        }
        #endregion
    }
}
