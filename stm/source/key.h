#ifndef __KEY_H
#define __KEY_H

#include "typedefs.h"

// Перечисление видов кнопок
typedef enum
{
    // Ввод
    KEY_KIND_ENTER,
    // Вверх
    KEY_KIND_UP,
    // Вниз
    KEY_KIND_DOWN,
    // Назад
    KEY_KIND_BACK,
} key_kind_t;

// Состяоние кнопки
typedef enum
{
    // Отпущена
    KEY_STATE_RELEASED,
    // Нажата
    KEY_STATE_PRESSED,
} key_state_t;

// Инициализация модуля
void key_init(void);
// Обработчик прерывания EXTI
void key_interrupt_exti(void);

#endif // __KEY_H
