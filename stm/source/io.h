#ifndef __IO_H
#define __IO_H

#include "system.h"

#define IO_REG_MODIFY(reg, v, m, p)         \
    CODE_BLOCK(                             \
        (reg) &= ~MASK_32(m, p);            \
        (reg) |= MASK_32(v, p);             \
    )
        
// --- Режимы порта --- //

// Вход
#define IO_MODE_INPUT                       0
// Выход
#define IO_MODE_OUTPUT                      1
// Альтернативная функция
#define IO_MODE_ALTFUN                      2
// Аналоговый (по умолчанию)
#define IO_MODE_ANALOG                      3
// Модификация
#define IO_MODE_SET_EXT(reg, n, mode)       IO_REG_MODIFY(reg, mode, 3, (n) * 2)
#define IO_MODE_SET(out, mode)              IO_MODE_SET_EXT(out##_PORT->MODER, out, mode)

// --- Тип вывода --- //

// Push pull (по умолчанию)
#define IO_OTYPE_PP                         0
// Open drain
#define IO_OTYPE_OD                         1
// Модификация
#define IO_OTYPE_SET_EXT(reg, n, mode)      IO_REG_MODIFY(reg, mode, 1, (n) * 1)
#define IO_OTYPE_SET(out, mode)             IO_OTYPE_SET_EXT(out##_PORT->OTYPER, out, mode)

// --- Скорость вывода --- //

// Низкая (5 МГц) (по умолчанию)
#define IO_OSPEED_LOW                       0
// Средняя (25 МГц)
#define IO_OSPEED_MEDIUM                    1
// Высокая (50 МГц)
#define IO_OSPEED_HIGH                      2
// Очень высокая (80 МГц)
#define IO_OSPEED_VERY_HIGH                 3
// Модификация
#define IO_OSPEED_SET_EXT(reg, n, mode)     IO_REG_MODIFY(reg, mode, 3, (n) * 2)
#define IO_OSPEED_SET(out, mode)            IO_OSPEED_SET_EXT(out##_PORT->OSPEEDR, out, mode)

// --- Режим подтяжки входа --- //

// Отключено (по умолчанию)
#define IO_IPULL_NONE                       0
// В питанию
#define IO_IPULL_UP                         1
// В земле
#define IO_IPULL_DOWN                       2
// Резерв
#define IO_IPULL_RESERVED                   3
// Модификация
#define IO_IPULL_SET_EXT(reg, n, mode)      IO_REG_MODIFY(reg, mode, 3, (n) * 2)
#define IO_IPULL_SET(out, mode)             IO_IPULL_SET_EXT(out##_PORT->PUPDR, out, mode)

// --- Разное --- //

// Получает маску порта по индексу
#define IO_MASK(index)                      MASK_32(1, index)
// Чтение данных порта (по маске)
#define IO_PORT_READ_MASK(port, mask)       (((port)->IDR & mask) > 0)
// Чтение данных порта (по выводу)
#define IO_PORT_READ(out)                   IO_PORT_READ_MASK(out##_PORT, IO_MASK(out))
        
// --- Установка/Сброс вывода порта --- //
        
// Установка порта в LOW (по маске)
#define IO_PORT_RESET_MASK(port, mask)      (port)->BSRR |= MASK_32(mask, 16)
// Установка порта в LOW (по выводу)
#define IO_PORT_RESET(out)                  IO_PORT_RESET_MASK(out##_PORT, IO_MASK(out))

// Установка порта в HIGH (по маске)
#define IO_PORT_SET_MASK(port, mask)        (port)->BSRR |= MASK_32(mask, 0)
// Установка порта в HIGH (по выводу)
#define IO_PORT_SET(out)                    IO_PORT_SET_MASK(out##_PORT, IO_MASK(out))
        
// --- Режим альтернативной функции --- //
        
// Модификация
#define IO_AFUNC_SET_EXT(rl, rh, n, mode)   IO_REG_MODIFY((n > 7) ? (rh) : (rl), mode, 0x0F, ((n) - (n > 7 ? 8 : 0)) * 4)
#define IO_AFUNC_SET(out, mode)             IO_AFUNC_SET_EXT(out##_PORT->AFR[0], out##_PORT->AFR[1], out, mode)

// --- Порт A --- //

#define IO_MTR_PWM                      0
#define IO_MTR_PWM_PORT                 GPIOA

#define IO_DBG_TX                       2
#define IO_DBG_TX_PORT                  GPIOA

#define IO_DBG_RX                       3
#define IO_DBG_RX_PORT                  GPIOA

#define IO_LED                          5
#define IO_LED_PORT                     GPIOA

#define IO_BTN_KEY1                     6
#define IO_BTN_KEY1_PORT                GPIOA

#define IO_BTN_KEY2                     7
#define IO_BTN_KEY2_PORT                GPIOA

#define IO_BTN_KEY3                     8
#define IO_BTN_KEY3_PORT                GPIOA

#define IO_BTN_KEY4                     9
#define IO_BTN_KEY4_PORT                GPIOA

#define IO_GND1                         11
#define IO_GND1_PORT                    GPIOA

#define IO_GND2                         12
#define IO_GND2_PORT                    GPIOA
        
#define IO_SWDIO                        13
#define IO_SWDIO_PORT                   GPIOA

#define IO_SWCLK                        14
#define IO_SWCLK_PORT                   GPIOA

// --- Порт B --- //

#define IO_BOOT1                        2
#define IO_BOOT1_PORT                   GPIOB

#define IO_SWO                          3
#define IO_SWO_PORT                     GPIOB

#define IO_HX_RX                        4
#define IO_HX_RX_PORT                   GPIOB

#define IO_HX_CLK                       5
#define IO_HX_CLK_PORT                  GPIOB

#define IO_MT_CH0                       6
#define IO_MT_CH0_PORT                  GPIOB

#define IO_MT_CH1                       7
#define IO_MT_CH0_PORT                  GPIOB

#define IO_MT_CH2                       8
#define IO_MT_CH0_PORT                  GPIOB

#define IO_MT_CH3                       9
#define IO_MT_CH0_PORT                  GPIOB

// --- Порт C --- //

#define IO_LCD_CLK                      10
#define IO_LCD_CLK_PORT                 GPIOC

#define IO_LCD_CS                       11
#define IO_LCD_CS_PORT                  GPIOC

#define IO_LCD_MOSI                     12
#define IO_LCD_MOSI_PORT                GPIOC

#define IO_BTN                          13
#define IO_BTN_PORT                     GPIOC

#define IO_OSC32_IN                     14
#define IO_OSC32_IN_PORT                GPIOC

#define IO_OSC32_OUT                    15
#define IO_OSC32_OUT_PORT               GPIOC

// --- Порт D --- //

#define IO_LCD_DC                       2
#define IO_LCD_DC_PORT                  GPIOD

// --- Порт H --- //

#define IO_OSC_IN                       0
#define IO_OSC_IN_PORT                  GPIOH
        
#define IO_OSC_OUT                      1
#define IO_OSC_OUT_PORT                 GPIOH

// --- Псевдонимы --- //
        
#define IO_MCO                          IO_BTN_KEY3
#define IO_MCO_PORT                     IO_BTN_KEY3_PORT
        
// Инициализация модуля
void io_init(void);

#endif // __IO_H
