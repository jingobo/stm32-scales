#ifndef __ADC_H
#define __ADC_H

#include "callback.h"

// Значение АЦП при срабатывании сторожевого таймера
#define ADC_VALUE_WATCHDOG      INT32_MIN

// Тип обработчика события АЦП
typedef callback_t<int32_t> adc_event_handler_t;

// Список обработчиков события АЦП
extern adc_event_handler_t::list_t adc_event_list;

// Инициализация модуля
void adc_init(void);
// Обработчик прерывания по линии EXTI
void adc_interrupt_exti(void);

#endif // __ADC_H
