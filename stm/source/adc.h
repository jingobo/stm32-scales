#ifndef __ADC_H
#define __ADC_H

#include "typedefs.h"

// Инициализация модуля
void adc_init(void);
// Чтение сырого 24-бит значения
int32_t adc_read(void);

#endif // __ADC_H
