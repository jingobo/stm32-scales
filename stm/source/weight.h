#ifndef __WEIGHT_H
#define __WEIGHT_H

#include "adc.h"

// Тип обработчика события изменения веса
typedef callback_t<int32_t> weight_event_handler_t;

// Список обработчиков изменения веса
extern weight_event_handler_t::list_t weight_event_list;

// Инициализация модуля
void weight_init(void);
// Задание тары
void weight_tare(void);

// Калибровка в нуле
void weight_calib_zero(void);
// Калибровка в точке
void weight_calib_point(int32_t value);

#endif // __WEIGHT_H
