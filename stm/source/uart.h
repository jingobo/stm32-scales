#ifndef __UART_H
#define __UART_H

#include "typedefs.h"

// Инициализация модуля
void uart_init(void);
// Синхронная передача данных
void uart_transmit(const void *data, size_t count);

#endif // __UART_H
