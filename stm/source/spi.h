#ifndef __SPI_H
#define __SPI_H

#include "typedefs.h"

// Инициализация модуля
void spi_init(void);
// Старт передачи
void spi_start(void);
// Стоп передачи
void spi_stop(void);
// Передача данных
void spi_write(const void *data, size_t size);

#endif // __SPI_H
