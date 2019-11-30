#ifndef __PWM_H
#define __PWM_H

#include "typedefs.h"

// Значение максимальной ширины сигнала
#define PWM_WIDTH_MAX       100

// Инициализация модуля
void pwm_init(void);
// Задает текущую ширину сигнала
void pwm_width_set(uint8_t width);
// Получает текущую ширину сигнала
uint8_t pwm_width_get(void);

#endif // __PWM_H
