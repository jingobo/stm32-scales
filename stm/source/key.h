#ifndef __KEY_H
#define __KEY_H

#include "typedefs.h"

// Перечисление видов кнопок
enum key_kind_t
{
    // Ввод
    KEY_KIND_ENTER,
    // Вверх
    KEY_KIND_UP,
    // Вниз
    KEY_KIND_DOWN,
    // Назад
    KEY_KIND_BACK,
};

// Состяоние кнопки
enum key_state_t
{
    // Отпущена
    KEY_STATE_RELEASED,
    // Нажата
    KEY_STATE_PRESSED,
};

// Инициализация модуля
void key_init(void);
// Обработчик прерывания EXTI
void key_interrupt_exti(void);

#endif // __KEY_H
