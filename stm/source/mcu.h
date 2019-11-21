#ifndef __MCU_H
#define __MCU_H

#include "system.h"

// Причина остановки
enum mcu_halt_reason_t
{
    // Не удалось запустить HSE осцилятор или он встал
    MCU_HALT_REASON_RCC,
    // Не удалось запустить LSE осцилятор или он встал
    MCU_HALT_REASON_RTC,
    // Ошибка при записи Fдash
    MCU_HALT_REASON_FLASH,
    
    // Не обработнанное прерывание
    MCU_HALT_REASON_IRQ,
    // Исключение Hard Fault
    MCU_HALT_REASON_SYS,
    // Исключение Memory Fault
    MCU_HALT_REASON_MEM,
    // Исключение Bus Fault
    MCU_HALT_REASON_BUS,
    // Исключение Usage Fault
    MCU_HALT_REASON_USG
};

// Источник тактирования вывода частоты
enum mcu_mco_source_t
{
    MCU_MCO_SOURCE_NONE = 0x00,
    MCU_MCO_SOURCE_SYS = RCC_CFGR_MCOSEL_0,
    MCU_MCO_SOURCE_HSI = RCC_CFGR_MCOSEL_0 | RCC_CFGR_MCOSEL_1,
    MCU_MCO_SOURCE_HSE = RCC_CFGR_MCOSEL_2,
    MCU_MCO_SOURCE_PLL = RCC_CFGR_MCOSEL_0 | RCC_CFGR_MCOSEL_2
};

// Прототип функции опроса для операций с таймаутом
typedef bool (*mcu_pool_handler_ptr)(void);

// Инициализация модуля
void mcu_init(void);
// Вывод частоты на IO MCO
void mcu_mco_output(mcu_mco_source_t source);
// Обработчик аварийной остановки приложения
__noreturn void mcu_halt(mcu_halt_reason_t reason);

// Задержка в попугайных тиках, завязана на частоту MCU
void mcu_delay(uint32_t delay);
// Задержка в мС, не вызывать из прерываний
void mcu_delay_ms(uint32_t delay = 1);
// Обработка функций опроса, таймут в мС, не вызывать из прерываний
bool mcu_pool_ms(mcu_pool_handler_ptr pool, uint32_t delay = 1);

// Обновение участка бит регистра
void mcu_reg_update_32(volatile uint32_t *reg, uint32_t value_bits, uint32_t valid_bits);

// Обработчик прерывания системного таймера
void mcu_interrupt_systick(void);

#endif // __MCU_H
