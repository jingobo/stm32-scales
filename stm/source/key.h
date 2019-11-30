#ifndef __KEY_H
#define __KEY_H

#include "callback.h"

// Перечисление видов клавиш
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

// Количество клавиш
#define KEY_COUNT       ((int)KEY_KIND_BACK + 1)

// Состяоние клавиши
enum key_state_t
{
    // Отпущена
    KEY_STATE_RELEASED,
    // Нажата
    KEY_STATE_PRESSED,
};

// Аргументы события клавиш
struct key_event_args_t
{
    // Вид клавиши
    key_kind_t kind;
    // Состояние клавиши
    key_state_t state;
    // Шаг нажатия
    uint32_t step;
};

// Тип обработчика события клавиш
typedef callback_t<const key_event_args_t &> key_event_handler_t;

// Список обработчиков события клавиш
extern key_event_handler_t::list_t key_event_list;

// Инициализация модуля
void key_init(void);
// Обработчик прерывания EXTI
void key_interrupt_exti(void);

#endif // __KEY_H
