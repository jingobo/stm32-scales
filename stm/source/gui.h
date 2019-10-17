#ifndef __GUI_H
#define __GUI_H

#include "key.h"

// Инициализация модуля
void gui_init(void);
// Событие смены состояния кнопки
void gui_key_event(key_kind_t key, key_state_t state);

#endif // __GUI_H
