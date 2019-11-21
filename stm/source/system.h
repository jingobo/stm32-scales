#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "typedefs.h"
#include <stm32l4xx.h>

/* Используемая переферия:
   mcu - RCC, SYSTICK
   spi - DMA2 (channel 2), SPI3
   timer - TIM3,
   key - ETTI[5..9]
   adc - EXTI4
   uart - USART2
*/

// Частота ядра при старте
#define FMCU_STARTUP_MHZ    4
#define FMCU_STARTUP_KHZ    XK(FMCU_STARTUP_MHZ)
#define FMCU_STARTUP_HZ     XM(FMCU_STARTUP_MHZ)
// Частота ядра после инициализации
#define FMCU_NORMAL_MHZ     80
#define FMCU_NORMAL_KHZ     XK(FMCU_NORMAL_MHZ)
#define FMCU_NORMAL_HZ      XM(FMCU_NORMAL_MHZ)

// Частота LSI [Гц]
#define FLSI_KHZ            40
#define FLSI_HZ             XK(FLSI_KHZ)

#endif // __SYSTEM_H
