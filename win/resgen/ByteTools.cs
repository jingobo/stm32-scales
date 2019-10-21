using System;
using System.Collections.Generic;
using System.Diagnostics;
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
        /// RLE сжатие
        /// </summary>
        public static byte[] RleCompressor(IEnumerable<byte> data)
        {
            var result = new List<byte>();
            // Подготовка
            byte last = 0;
            byte lastRepeat = 0;
            // Добавление чанка
            Action addChunk = () =>
            {
                if (lastRepeat <= 0)
                    return;
                result.Add(lastRepeat);
                result.Add(last);
                lastRepeat = 0;
            };
            foreach (var value in data)
            {
                // Добавляем чанк если данные сменились или количество повторов не переполнило байт
                if ((lastRepeat > 0 && last != value) || lastRepeat >= byte.MaxValue)
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
        public static string BytesToCppBlock(IList<byte> data, int columnWidth = 16, int indentWidth = 4)
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
