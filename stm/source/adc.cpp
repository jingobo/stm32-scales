// Включение режима передачи значений АЦП по UART
#define ADC_SEND_TO_FLOW

#include "io.h"
#include "adc.h"
#include "mcu.h"
#include "nvic.h"
#include "flow.h"
#include "event.h"
#include "timer.h"

// Список обработчиков события АЦП
adc_event_handler_t::list_t adc_event_list;

// Значение АЦП в прерывании
static __no_init uint32_t adc_value_irq;

// Сторожевой таймер отказа АЦП
static timer_notify_t adc_watchdog([](void)
{
    adc_event_list(ADC_VALUE_WATCHDOG);
});

// Рестарт сторожевого таймера АЦП
static void adc_watchdog_reset(void)
{
    adc_watchdog.start_hz(2);
}

void adc_init(void)
{
    EXTI->FTSR1 |= EXTI_FTSR1_FT4;                                              // EXTI interrupt on faiilng edge
    EXTI->IMR1 |= EXTI_IMR1_IM4;                                                // EXTI interrupt unmask
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PB;                               // Select PB4 on EXTI line
    // Вклшючение прерывания
    nvic_irq_enable_set(EXTI4_IRQn, true);
    // Запуск сторожевого таймера
    adc_watchdog_reset();
    // Запуск в ручную
    if (!IO_PORT_READ(IO_HX_RX))
        adc_interrupt_exti();
}

// Событие готовности значения
static event_notify_t adc_value_ready([](void)
{
    // Забор значения
    IRQ_CTX_SAVE();
        IRQ_CTX_DISABLE();
        union
        {
            uint32_t u32;
            int32_t s32;
        };
        u32 = adc_value_irq;
    IRQ_CTX_RESTORE();
    // Конвертирование в 32-бит знаковое
    if (u32 & 0x800000)
        u32 |= 0xFF000000;
    #ifdef ADC_SEND_TO_FLOW
        flow_value = (float32_t)s32;
    #endif
    // Вызов события
    adc_event_list(s32);
    // Сброс сторожевого таймера
    adc_watchdog_reset();
});

// Синхронная задержка для ногодрыга
static void adc_delay(void)
{
    // Значение подорбано осцилографом на частоте MCU 80 МГц, выходит ~0.5 мкС
    mcu_delay(5);
}

IRQ_ROUTINE
void adc_interrupt_exti(void)
{
    // Буфер для значения
    register uint32_t raw = 0;
    // Конвертирование
    for (uint8_t i = 0; i < 24; i++)
    {
        // Поднимаем строб
        IO_PORT_SET(IO_HX_CLK);
            adc_delay();
            raw <<= 1;
        // Опускаем строб
        IO_PORT_RESET(IO_HX_CLK);
        adc_delay();
        // Забираем очередной бит
        if (IO_PORT_READ(IO_HX_RX))
            raw++;
    }

    // Старт выборрки по каналу А с коэффициентом усиления 128
    IO_PORT_SET(IO_HX_CLK);
        adc_delay();
    IO_PORT_RESET(IO_HX_CLK);

    EXTI->PR1 |= IO_MASK(IO_HX_RX);                                             // Clear interrupt pending flag
    
    // Результат
    adc_value_irq = raw;
    adc_value_ready.raise();
}
