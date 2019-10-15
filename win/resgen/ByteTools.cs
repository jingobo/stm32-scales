using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Vito.Resgen
{
    /// <summary>
    /// Класс набора утилитных функций работы с набором байт
    /// </summary>
    internal static class ByteTools
    {
        /// <summary>
        /// Компрессор входных 16-бит данных в 8-ми битный поток по RLE алгоритму
        /// </summary>
        public static byte[] RleCompressor(IEnumerable<ushort> data)
        {
            var result = new List<byte>();
            // Подготовка
            ushort last = 0;
            byte lastRepeat = 0;
            // Добавление чанка
            Action addChunk = () =>
            {
                if (lastRepeat <= 0)
                    return;
                result.Add(lastRepeat);
                result.AddRange(BitConverter.GetBytes(last));
                lastRepeat = 0;
            };
            // Обход входной последовательности
            foreach (var value in data)
            {
                // Добавляем чанк если данные сменились или повторы превысили 255
                if ((lastRepeat > 0 && last != value) || lastRepeat >= 255)
                    addChunk();
                last = value;
                lastRepeat++;
            }
            addChunk();
            return result.ToArray();
        }

        /// <summary>
        /// Конвертирование набора байт в блок инициалзиации C/C++
        /// </summary>
        public static string BytesToCppBlock(IList<byte> data, int columnWidth = 8, int indentWidth = 4)
        {
            var indentStart = new String(' ', indentWidth);
            // Подготовка буфера
            var result = new StringBuilder();
            result.AppendFormat("{0}// Size {1}", indentStart, data.Count);
            // Подготовка отступа
            var indent = Environment.NewLine + indentStart;
            // Обход байт
            var columnCounter = columnWidth;
            foreach (var c in data)
            {
                // Отступ
                if (columnCounter >= columnWidth)
                {
                    result.Append(indent);
                    columnCounter = 0;
                }
                // Данные
                result.AppendFormat("0x{0:X2}, ", c);
                columnCounter++;
            }
            return result.ToString();
        }
    }
}
