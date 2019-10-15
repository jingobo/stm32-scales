#ifndef __GFX_H
#define __GFX_H

#include "lcd.h"

// Прямоугольник
void gfx_rect_frame(lcd_color_t color, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void gfx_rect_solid(lcd_color_t color, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
// Изображение
void gfx_image_raw(const lcd_color_t *data, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void gfx_image_rle(const uint8_t *data, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
// Текст
uint16_t gfx_font_symbol(char symbol, uint16_t x, uint16_t y, const uint8_t *font);
uint16_t gfx_font_string(const char *string, uint16_t x, uint16_t y, const uint8_t *font);

#endif // __GFX_H
