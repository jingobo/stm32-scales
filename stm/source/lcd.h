#ifndef __LCD_H
#define __LCD_H

#include "typedefs.h"

// Структура цвета
typedef uint16_t lcd_color_t;

// Ширина дисплея
#define LCD_SIZE_WIDTH              240
// Высота дисплея
#define LCD_SIZE_HEIGHT             320

// Максимальное значение одной компоненты цвета
#define LCD_COLOR_COMP_MAX          255

// Подсчет компонент цвета
#define LCD_COLOR_COMP(v, m, p)     (((((lcd_color_t)(v)) * (m) / LCD_COLOR_COMP_MAX) & (m)) << (p))
#define LCD_COLOR_COMP_B(v)         LCD_COLOR_COMP(v, 31, 0)
#define LCD_COLOR_COMP_G(v)         LCD_COLOR_COMP(v, 63, 6)
#define LCD_COLOR_COMP_R(v)         LCD_COLOR_COMP(v, 31, 11)

// Макрос расчета цвета
#define LCD_COLOR_MAKE(r, g, b)     \
    SWAP_16(LCD_COLOR_COMP_R(r) |   \
            LCD_COLOR_COMP_G(g) |   \
            LCD_COLOR_COMP_B(b))
     
// Стандартные цвета
#define LCD_COLOR_BLACK             LCD_COLOR_MAKE(0, 0, 0)
#define LCD_COLOR_RED               LCD_COLOR_MAKE(LCD_COLOR_COMP_MAX, 0, 0)
#define LCD_COLOR_GREEN             LCD_COLOR_MAKE(0, LCD_COLOR_COMP_MAX, 0)
#define LCD_COLOR_BLUE              LCD_COLOR_MAKE(0, 0, LCD_COLOR_COMP_MAX)

// Инициализация модуля
void lcd_init(void);
// Обалсти вывода данных
void lcd_area(uint16_t left, uint16_t top, uint16_t width, uint16_t height);
// Вывод пикселей
void lcd_out(const lcd_color_t *data, size_t count);

#endif // __LCD_H
