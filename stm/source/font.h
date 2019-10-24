#ifndef __FONT_H
#define __FONT_H

#include "typedefs.h"

// Шрифты
extern const uint8_t FONT_HEADER_BOLD[];
extern const uint8_t FONT_BUTTON_BOLD[];
extern const uint8_t FONT_NUMBER_BOLD[];

// Получает высоту шрифта
uint8_t font_height_get(const uint8_t *font);

#endif // __FONT_H
