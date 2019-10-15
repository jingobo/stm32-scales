﻿#include "mcu.h"
#include "nvic.h"

// Режимы групп приоритетов
enum
{
    // 0 bits pre-emption priority, 4 bits subpriority
    NVIC_PRIORITYGROUP_0 = 7,
    // 1 bits pre-emption priority, 3 bits subpriority
    NVIC_PRIORITYGROUP_1 = 6,
    // 2 bits pre-emption priority, 2 bits subpriority
    NVIC_PRIORITYGROUP_2 = 5,
    // 3 bits pre-emption priority, 1 bits subpriority
    NVIC_PRIORITYGROUP_3 = 4,
    // 4 bits pre-emption priority, 0 bits subpriority
    NVIC_PRIORITYGROUP_4 = 3
};

// Текущий используемый режим группировки прерываний
#define NVIC_PRIORITYGROUP      NVIC_PRIORITYGROUP_4

// Имя секции стека
#define NVIC_SECTION_STACK      "CSTACK"
// Имя секции таблицы векторов прерываний
#define NVIC_SECTION_VTBL       ".intvec"

// Прототип процедуры обработчика прерывания
typedef void (* nvic_isr_ptr)(void);

// Структура описания стека и обработчиков прерываний
struct nvic_vtbl_t
{
    // Адрес базы стека
    void *stack_base;
    // Адреса обработчиков прерываний
    nvic_isr_ptr irq_handler[96];
};

void nvic_init(void)
{
    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP);
}

OPTIMIZE_SIZE
void nvic_irq_priority_set(nvic_irq_t irq, nvic_irq_priority_t priority)
{
    NVIC_SetPriority(irq, NVIC_EncodePriority(NVIC_PRIORITYGROUP, priority, NULL));
}

OPTIMIZE_SIZE
nvic_irq_priority_t nvic_irq_priority_get(nvic_irq_t irq)
{
    uint32_t pri, sub;
    NVIC_DecodePriority(NVIC_GetPriority(irq), NVIC_PRIORITYGROUP, &pri, &sub);
    return (nvic_irq_priority_t)pri;
}

OPTIMIZE_SIZE
void nvic_irq_enable_set(nvic_irq_t irq, bool state)
{
    assert(irq >= 0);
    if (state)
        NVIC_EnableIRQ(irq);
    else
        NVIC_DisableIRQ(irq);
}

// Заглушка для обработчиков прерываний
IRQ_ROUTINE
static void nvic_interrupt_dummy(void)
{
    mcu_halt(MCU_HALT_REASON_IRQ);
}

// Обработчик Hard Fault
IRQ_ROUTINE
static void nvic_interrupt_hard(void)
{
    mcu_halt(MCU_HALT_REASON_SYS);
}

// Обработчик Memory Fault
IRQ_ROUTINE
static void nvic_interrupt_memory(void)
{
    mcu_halt(MCU_HALT_REASON_MEM);
}

// Обработчик Usage Fault
IRQ_ROUTINE
static void nvic_interrupt_usage(void)
{
    mcu_halt(MCU_HALT_REASON_USG);
}

// Обработчик Bus Fault
IRQ_ROUTINE
static void nvic_interrupt_bus(void)
{
    mcu_halt(MCU_HALT_REASON_BUS);
}

// Точка старта программы
extern "C" void __iar_program_start(void);

// Модули в которых есть прерывания
#include "mcu.h"

// Обявление сегмента для sfe
SECTION_DECLARE(NVIC_SECTION_STACK)

// Имя не менять, это магическое значение для С-Spy
extern "C" __root const nvic_vtbl_t __vector_table @ NVIC_SECTION_VTBL =
{
    __sfe(NVIC_SECTION_STACK),                  // Stack base
    {
        // Cortex-M3 Interrupts
        __iar_program_start,                    // Reset Handler
        nvic_interrupt_dummy,                   // NMI Handler
        nvic_interrupt_hard,                    // Hard Fault Handler
        nvic_interrupt_memory,                  // MPU Fault Handler
        nvic_interrupt_bus,                     // Bus Fault Handler
        nvic_interrupt_usage,                   // Usage Fault Handler
        NULL,                                   // Reserved
        NULL,                                   // Reserved
        NULL,                                   // Reserved
        NULL,                                   // Reserved
        nvic_interrupt_dummy,                   // SVCall Handler
        nvic_interrupt_dummy,                   // Debug Monitor Handler
        NULL,                                   // Reserved
        nvic_interrupt_dummy,                   // PendSV Handler
        mcu_interrupt_systick,                  // SysTick Handler
        
         // External Interrupts
        
        nvic_interrupt_dummy,                   // WWDG
        nvic_interrupt_dummy,                   // PVD through EXTI
        nvic_interrupt_dummy,                   // Tamper
        nvic_interrupt_dummy,                   // RTC 
        nvic_interrupt_dummy,                   // FLASH
        nvic_interrupt_dummy,                   // RCC
        nvic_interrupt_dummy,                   // EXTI Line0
        nvic_interrupt_dummy,                   // EXTI Line1
        nvic_interrupt_dummy,                   // EXTI Line2
        nvic_interrupt_dummy,                   // EXTI Line3
        nvic_interrupt_dummy,                   // EXTI Line4
        nvic_interrupt_dummy,                   // DMA1 Channel 1
        nvic_interrupt_dummy,                   // DMA1 Channel 2
        nvic_interrupt_dummy,                   // DMA1 Channel 3
        nvic_interrupt_dummy,                   // DMA1 Channel 4
        nvic_interrupt_dummy,                   // DMA1 Channel 5
        nvic_interrupt_dummy,                   // DMA1 Channel 6
        nvic_interrupt_dummy,                   // DMA1 Channel 7
        nvic_interrupt_dummy,                   // ADC1, ADC2
        nvic_interrupt_dummy,                   // CAN1 TX
        nvic_interrupt_dummy,                   // CAN1 RX0
        nvic_interrupt_dummy,                   // CAN1 RX1
        nvic_interrupt_dummy,                   // CAN1 SCE
        nvic_interrupt_dummy,                   // EXTI [9:5]
        nvic_interrupt_dummy,                   // TIM1 Break / TIM15
        nvic_interrupt_dummy,                   // TIM1 Update  / TIM16
        nvic_interrupt_dummy,                   // TIM1 Trigger / TIM17
        nvic_interrupt_dummy,                   // TIM1 CC
        nvic_interrupt_dummy,                   // TIM2
        nvic_interrupt_dummy,                   // TIM3
        nvic_interrupt_dummy,                   // TIM4
        nvic_interrupt_dummy,                   // I2C1 Event
        nvic_interrupt_dummy,                   // I2C1 Error
        nvic_interrupt_dummy,                   // I2C2 Event
        nvic_interrupt_dummy,                   // I2C2 Error
        nvic_interrupt_dummy,                   // SPI1 global
        nvic_interrupt_dummy,                   // SPI2 global
        nvic_interrupt_dummy,                   // USART1
        nvic_interrupt_dummy,                   // USART2
        nvic_interrupt_dummy,                   // USART3
        nvic_interrupt_dummy,                   // EXTI [15:10]
        nvic_interrupt_dummy,                   // RTC Alarm (A and B) through EXTI
        nvic_interrupt_dummy,                   // DFSDM1 Filter 3
        nvic_interrupt_dummy,                   // TIM8 Break
        nvic_interrupt_dummy,                   // TIM8 Update
        nvic_interrupt_dummy,                   // TIM8 Trigger
        nvic_interrupt_dummy,                   // TIM8 CC
        nvic_interrupt_dummy,                   // ADC3
        nvic_interrupt_dummy,                   // FMC
        nvic_interrupt_dummy,                   // SDMMC1
        nvic_interrupt_dummy,                   // TIM5
        nvic_interrupt_dummy,                   // SPI3
        nvic_interrupt_dummy,                   // UART4
        nvic_interrupt_dummy,                   // UART5
        nvic_interrupt_dummy,                   // TIM6 / DAC1/DAC2 underrun error
        nvic_interrupt_dummy,                   // TIM7
        nvic_interrupt_dummy,                   // DMA2 Channel 1
        nvic_interrupt_dummy,                   // DMA2 Channel 2
        nvic_interrupt_dummy,                   // DMA2 Channel 3
        nvic_interrupt_dummy,                   // DMA2 Channel 4
        nvic_interrupt_dummy,                   // DMA2 Channel 5
        nvic_interrupt_dummy,                   // DFSDM1 Filter 0
        nvic_interrupt_dummy,                   // DFSDM1 Filter 1
        nvic_interrupt_dummy,                   // DFSDM1 Filter 2
        nvic_interrupt_dummy,                   // COMP1 / COMP2
        nvic_interrupt_dummy,                   // LPTIM1
        nvic_interrupt_dummy,                   // LPTIM2
        nvic_interrupt_dummy,                   // USB OTG FS
        nvic_interrupt_dummy,                   // DMA2 Channel 6
        nvic_interrupt_dummy,                   // DMA2 Channel 7
        nvic_interrupt_dummy,                   // LPUART1
        nvic_interrupt_dummy,                   // QSPI
        nvic_interrupt_dummy,                   // I2C3
        nvic_interrupt_dummy,                   // I2C3
        nvic_interrupt_dummy,                   // SAI1
        nvic_interrupt_dummy,                   // SAI2
        nvic_interrupt_dummy,                   // SWI1
        nvic_interrupt_dummy,                   // TSC
        nvic_interrupt_dummy,                   // LCD
        nvic_interrupt_dummy,                   // RNG
        nvic_interrupt_dummy,                   // FPU
    }
};
