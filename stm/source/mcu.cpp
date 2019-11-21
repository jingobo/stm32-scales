#include "io.h"
#include "mcu.h"
#include "nvic.h"

// Точность измерения времени SysTick [1...1000]
#define MCU_SYSTICK_DIVIDER        10

// Количество тиков системного таймера в мС
static volatile __no_init uint32_t mcu_systicks;

// Проверки готовности HSE
static bool mcu_check_hse(void)
{
    return (RCC->CR & RCC_CR_HSERDY) > 0;                                       // Check HSE ready flag
}

// Проверка готовности PLL
static bool mcu_check_pll(void)
{
    return (RCC->CR & RCC_CR_PLLRDY) > 0;                                       // Check PLL ready flag
}

// Проверка PLL как источкник HCLK
static bool mcu_check_pll_hclk(void)
{
    return (RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL;                      // Check PLL as system clock
}

// Обновление системного таймера при смене тактирования
static void mcu_systick_update(void)
{
    // Определяем частоту ядра
    uint32_t f_mcu = mcu_check_pll_hclk() ? FMCU_NORMAL_MHZ : FMCU_STARTUP_MHZ;
    // Инициализация SysTick на 1 мС
    SysTick_Config(f_mcu * (1000 / MCU_SYSTICK_DIVIDER));
}

// Внутрення функция инициализация системы тактирования (пришлось разогнать до 96МГц)
static bool mcu_init_rcc(void)
{
    // Подготовка MSI
    RCC->CR |= RCC_CR_MSIRGSEL;                                                 // Main MSI range
    mcu_reg_update_32(&RCC->CR, RCC_CR_MSIRANGE_7, RCC_CR_MSIRANGE);            // MSI 8 MGz
    // Запуск HSE
    RCC->CR &= ~RCC_CR_HSEBYP;                                                  // HSE bypass disable
    RCC->CR |= RCC_CR_HSEON;                                                    // HSE enable
    // Ожидание запуска HSE, 100 мС
    if (!mcu_pool_ms(mcu_check_hse, 100))                                       // Wait for HSE ready
        return true;
    RCC->CR |= RCC_CR_CSSON;                                                    // HSE CSS enable
    // Конфигурация PLL
    RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE | RCC_PLLCFGR_PLLN_2 |                // HSE, PLLM /1, x20, PLLR /2, PLLP /2, PLLQ /2, PLLR on
                   RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLREN;
    // Запуск PLL и ожидание стабилизации, 2 мС
    RCC->CR |= RCC_CR_PLLON;                                                    // PLL enable
    if (!mcu_pool_ms(mcu_check_pll))                                            // Wait for PLL ready
        return true;
    mcu_reg_update_32(&FLASH->ACR, FLASH_ACR_LATENCY_4WS, FLASH_ACR_LATENCY);   // Flash latency 4 wait states
    // Переключение тактовой частоты
    mcu_reg_update_32(&RCC->CFGR, RCC_CFGR_SW_PLL, RCC_CFGR_SW);                // PLL select
    // Ожидание перехода на PLL, 1 мС
    if (!mcu_pool_ms(mcu_check_pll_hclk))                                       // Wait for PLL ready
        return true;
    mcu_systick_update();
    // Запуск CSS на HSE
    RCC->CR |= RCC_CR_CSSON;                                                    // CSS on HSE enable
    return false;
}

void mcu_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;                                       // SYSCFG clock gate
    // Явно включать прерывание SysTick нельзя, оно немаскируемое
    mcu_systick_update();
    nvic_irq_priority_set(SysTick_IRQn, NVIC_IRQ_PRIORITY_LOWEST);              // Lowest SysTick IRQ priority
    // Инициализация тактирования
    if (mcu_init_rcc())
        mcu_halt(MCU_HALT_REASON_RCC);
}

__noreturn void mcu_halt(mcu_halt_reason_t reason)
{
    UNUSED(reason); // TODO: куданить сохранить причину
    IRQ_CTX_DISABLE();
    while (true)
    { }
}

OPTIMIZE_SPEED
void mcu_reg_update_32(volatile uint32_t *reg, uint32_t value_bits, uint32_t valid_bits)
{
    // Проверка указателя опущена
    uint32_t buffer = *reg;
    // Снимаем значащие биты
    buffer &= ~valid_bits;
    // Устанавливаем новые биты
    buffer |= value_bits;
    // Возвращаем в регистр
    *reg = buffer;
}

void mcu_mco_output(mcu_mco_source_t source)
{
    RCC->CFGR &= ~RCC_CFGR_MCOSEL;
    switch (source)
    {
        case MCU_MCO_SOURCE_NONE:
            IO_MODE_SET(IO_MCO, IO_MODE_INPUT);
            break;
        case MCU_MCO_SOURCE_SYS:
        case MCU_MCO_SOURCE_HSI:
        case MCU_MCO_SOURCE_HSE:
        case MCU_MCO_SOURCE_PLL:
            IO_MODE_SET(IO_MCO, IO_MODE_ALTFUN);
            RCC->CFGR |= source;
            break;
    }
}

OPTIMIZE_NONE
void mcu_delay(uint32_t delay)
{
    for (volatile uint32_t i = 0; i < delay; i++)
    { }
}

// Шаблон для функции ожидания
#define MCU_DELAY_TEMPLATE(code)                            \
    assert(delay <= UINT32_MAX / MCU_SYSTICK_DIVIDER);      \
    delay *= MCU_SYSTICK_DIVIDER;                           \
    uint32_t old = mcu_systicks, cur;                       \
    do                                                      \
    {                                                       \
        code;                                               \
        cur = mcu_systicks;                                 \
    } while (cur - old < delay)


INLINE_NEVER
OPTIMIZE_NONE
void mcu_delay_ms(uint32_t delay)
{
    MCU_DELAY_TEMPLATE(MACRO_EMPTY);
}

INLINE_NEVER
OPTIMIZE_NONE
bool mcu_pool_ms(mcu_pool_handler_ptr proc, uint32_t delay)
{
    assert(proc != NULL);
    MCU_DELAY_TEMPLATE(
        if (proc()) 
            return true
    );
    return false;
}

IRQ_ROUTINE
void mcu_interrupt_systick(void)
{
    mcu_systicks++;
}
