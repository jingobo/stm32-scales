#ifndef __LCD_H
#define __LCD_H

#include "typedefs.h"

// Структура цвета
typedef uint16_t lcd_color_t;

// Ширина дисплея
#define LCD_SIZE_WIDTH              240
// Высота дисплея
#define LCD_SIZE_HEIGHT             320

// Битовые позиции компонент в не перевернутом цвете
#define LCD_COLOR_POS_R             11
#define LCD_COLOR_POS_G             5
#define LCD_COLOR_POS_B             0
// Максимальные значения компонент в сыром виде
#define LCD_COLOR_MAX_R             0x1F
#define LCD_COLOR_MAX_G             0x3F
#define LCD_COLOR_MAX_B             0x1F
// Получает значение не перевернутого цвета из компонент
#define LCD_COLOR_RAW(r, g, b)      ((lcd_color_t)(((r) << LCD_COLOR_POS_R) | ((g) << LCD_COLOR_POS_G) | ((b) << LCD_COLOR_POS_B)))

// Максимальное значение одной компоненты цвета
#define LCD_COLOR_COMP_MAX          255
// Подсчет компонент цвета
#define LCD_COLOR_COMP(v, m)        ((v) * (m) / LCD_COLOR_COMP_MAX)
#define LCD_COLOR_COMP_R(v)         LCD_COLOR_COMP(v, LCD_COLOR_MAX_R)
#define LCD_COLOR_COMP_G(v)         LCD_COLOR_COMP(v, LCD_COLOR_MAX_G)
#define LCD_COLOR_COMP_B(v)         LCD_COLOR_COMP(v, LCD_COLOR_MAX_B)
// Макрос расчета цвета
#define LCD_COLOR_MAKE(r, g, b)     SWAP_16(LCD_COLOR_RAW(LCD_COLOR_COMP_R(r), LCD_COLOR_COMP_G(g), LCD_COLOR_COMP_B(b)))

// Стандартные цвета
#define LCD_COLOR_BLACK             LCD_COLOR_MAKE(0, 0, 0)
#define LCD_COLOR_RED               LCD_COLOR_MAKE(LCD_COLOR_COMP_MAX, 0, 0)
#define LCD_COLOR_GREEN             LCD_COLOR_MAKE(0, LCD_COLOR_COMP_MAX, 0)
#define LCD_COLOR_BLUE              LCD_COLOR_MAKE(0, 0, LCD_COLOR_COMP_MAX)
#define LCD_COLOR_WHITE             LCD_COLOR_MAKE(LCD_COLOR_COMP_MAX, LCD_COLOR_COMP_MAX, LCD_COLOR_COMP_MAX)

#define LCD_COLOR_NAV               LCD_COLOR_MAKE(0, 178, 232)

// Инициализация модуля
void lcd_init(void);
// Обалсти вывода данных
void lcd_area(uint16_t left, uint16_t top, uint16_t width, uint16_t height);
// Вывод пикселей
void lcd_out(const lcd_color_t *data, size_t count);
// Смена местами байт цвета
inline lcd_color_t lcd_color_swap(lcd_color_t from)
{
    return __REV16(from);
}

#endif // __LCD_H
