using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Vito.Resgen
{
    /// <summary>
    /// Класс логики главного окна приложения
    /// </summary>
    internal sealed partial class MainWindow : Window
    {
        #region fields, ctor
        /// <summary>
        /// Диалог выбора шрифта
        /// </summary>
        private readonly FontDialog fontDialog;
        /// <summary>
        /// Диалог выбора цвета
        /// </summary>
        private readonly ColorDialog colorDialog;
        /// <summary>
        /// Диалог выбора файла
        /// </summary>
        private readonly OpenFileDialog fileDialog;

        /// <summary>
        /// Конструктор по умолчанию
        /// </summary>
        public MainWindow()
        {
            InitializeComponent();
            fontDialog = new FontDialog();
            colorDialog = new ColorDialog();
            fileDialog = new OpenFileDialog();
        }
        #endregion

        #region methods
        /// <summary>
        /// Событие закрытия окна
        /// </summary>
        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
            fileDialog.Dispose();
            fontDialog.Dispose();
            colorDialog.Dispose();
        }

        /// <summary>
        /// Конвертирование цвета фона элемента границы в другое представление цвета
        /// </summary>
        /// <param name="border"></param>
        /// <returns></returns>
        private static System.Drawing.Color ColorFromBorderBackground(Border border)
        {
            var color = ((SolidColorBrush)border.Background).Color;
            return System.Drawing.Color.FromArgb(color.R, color.G, color.B);
        }

        /// <summary>
        /// Показ диалога с сообщением о ошибке
        /// </summary>
        private static void ShowErrorBox(string message)
        {
            System.Windows.MessageBox.Show(message);
        }

        /// <summary>
        /// Генерация кода по шрифту
        /// </summary>
        private void GenereateFromFont()
        {
            var cp1251 = Encoding.GetEncoding(1251);
            // Размер буфера рендера шрифта в пикселях 
            var bufferArea = new System.Drawing.Rectangle(0, 0, 255, 255);
            
            // Получаем исходные данные
            var font = fontDialog.Font;
            var foreground = ColorFromBorderBackground(fontForeColor);
            var background = ColorFromBorderBackground(fontBackColor);
            var foregroundBrush = new SolidBrush(foreground);

            var bytes = new List<byte>();
            // Рендер
            using (var buffer = new Bitmap(bufferArea.Width, bufferArea.Height))
            {
                var g = Graphics.FromImage(buffer);
                // Получаем высоту шрифта
                var height = (byte)g.MeasureString("0", font, int.MaxValue, StringFormat.GenericTypographic).Height;
                bytes.Add(height);
                // Обход символов
                foreach (var c in fontSymbolLabel.Text.Select(c => new string(c, 1)))
                {
                    // Вывод символа
                    g.Clear(background);
                    g.DrawString(c, font, foregroundBrush, bufferArea.X, bufferArea.Y, StringFormat.GenericTypographic);
                    // Определение реальных размеров символа
                    var symbolArea = ImageTools.ImageBounds(buffer, bufferArea, background);
                    // По горизонтали не учитываем
                    symbolArea.Y = bufferArea.Y;
                    symbolArea.Height = height;
                    // Конвертирование в 16-бит формат
                    var pixels = ImageTools.Convert565(buffer, symbolArea);
                    // Компрессия
                    var raw = ByteTools.RleCompressor(pixels);
                    // Запись заголовка
                    // Выводимый символ
                    bytes.Add(cp1251.GetBytes(c).First());
                    // Ширина в пикселях
                    bytes.Add((byte)symbolArea.Width);
                    // Длинна в байтах
                    bytes.AddRange(BitConverter.GetBytes((ushort)raw.Length));
                    // Данные
                    bytes.AddRange(raw);
                }
            }
            // EOF
            bytes.AddRange(BitConverter.GetBytes(0));
            // Конвертирование в массив байт
            outputBox.Text = ByteTools.BytesToCppBlock(bytes);
        }

        /// <summary>
        /// Генерация кода по изображению
        /// </summary>
        private void GenereateFromImage()
        {
            // Открытие изображения
            System.Drawing.Image image;
            try
            {
                image = System.Drawing.Image.FromFile(imageFilePathBox.Text);
            }
            catch
            {
                ShowErrorBox("Не удалось открыть файл изображения!");
                return;
            }
            if (image.Width > 255 || image.Height > 255)
            {
                ShowErrorBox("Не удалось открыть файл изображения!");
                return;
            }
            var bytes = new List<byte>();
            // Конвертирование в битмап
            using (var buffer = new Bitmap(image))
            {
                var area = new System.Drawing.Rectangle(0, 0, buffer.Width, buffer.Height);
                // Конвертирование в 16-бит формат
                var pixels = ImageTools.Convert565(buffer, area);
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
            outputBox.Text = ByteTools.BytesToCppBlock(bytes);
        }
        #endregion

        #region events
        /// <summary>
        /// Событие клика по надписи выбора шрифта
        /// </summary>
        private void fontLabel_MouseUp(object sender, MouseButtonEventArgs e)
        {
            // Показ диалога
            if (fontDialog.ShowDialog() != System.Windows.Forms.DialogResult.OK)
                return;
            fontLabel.FontFamily = new System.Windows.Media.FontFamily(fontDialog.Font.Name);
            fontLabel.FontSize = fontDialog.Font.Size * 96.0 / 72.0;
            fontLabel.FontWeight = fontDialog.Font.Bold ? FontWeights.Bold : FontWeights.Regular;
            fontLabel.FontStyle = fontDialog.Font.Italic ? FontStyles.Italic : FontStyles.Normal;
        }

        /// <summary>
        /// Событие клика по кнопке генерации кода
        /// </summary>
        private void generateButton_Click(object sender, RoutedEventArgs e)
        {
            var state = useFontRadioButton.IsChecked;
            if (state.HasValue && state.Value)
                GenereateFromFont();
            else
                GenereateFromImage();
        }

        /// <summary>
        /// Событие клика по полю выбора цвета
        /// </summary>
        private void ColorBorder_MouseUp(object sender, MouseButtonEventArgs e)
        {
            var border = (Border)sender;
            // Показ диалога
            colorDialog.Color = ColorFromBorderBackground(border);
            if (colorDialog.ShowDialog() != System.Windows.Forms.DialogResult.OK)
                return;
            // Установка цвета
            var newColor = colorDialog.Color;
            border.Background = new SolidColorBrush(System.Windows.Media.Color.FromRgb(newColor.R, newColor.G, newColor.B));
        }

        /// <summary>
        /// Событие клика по кнопке выбора изображения
        /// </summary>
        private void selectImageFileButton_Click(object sender, RoutedEventArgs e)
        {
            if (fileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                imageFilePathBox.Text = fileDialog.FileName;
        }
        #endregion
    }
}
