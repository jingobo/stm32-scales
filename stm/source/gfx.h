﻿#ifndef __GFX_H
#define __GFX_H

#include "lcd.h"

// Прямоугольник
void gfx_rect_frame(uint16_t x, uint16_t y, uint16_t width, uint16_t height, lcd_color_t foreground);
void gfx_rect_solid(uint16_t x, uint16_t y, uint16_t width, uint16_t height, lcd_color_t background);
void gfx_rect_frame_round(uint16_t x, uint16_t y, uint16_t width, uint16_t height, lcd_color_t foreground, lcd_color_t background);
// Изображение
void gfx_image(const uint8_t *image, uint16_t x, uint16_t y, lcd_color_t foreground, lcd_color_t background);
// Текст
uint16_t gfx_string(const char *string, const uint8_t *font, uint16_t x, uint16_t y, lcd_color_t foreground, lcd_color_t background);

#endif // __GFX_H
