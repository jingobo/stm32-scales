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
        }

        /// <summary>
        /// Обработка полученных генератором данных
        /// </summary>
        private void ProcessGenerationData(ResourceGenerator.Data data)
        {
            if (data.IsError)
                System.Windows.MessageBox.Show(data.Text);
            else
                outputBox.Text = data.Text;
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
                ProcessGenerationData(ResourceGenerator.FromFont(fontDialog.Font, fontSymbolLabel.Text));
            else
                ProcessGenerationData(ResourceGenerator.FromImage(imageFilePathBox.Text));
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
