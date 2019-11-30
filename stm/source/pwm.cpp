#include "io.h"
#include "pwm.h"

// Частота ШИМ в [Гц]
#define PWM_FREQUENCY           50
// Позиция до которой происходит форсированный запуск
#define PWM_FORCE_START_STEP    10

void pwm_init(void)
{
    // IO
    IO_AFUNC_SET(IO_MTR_PWM, 0x01);
    // TIM2
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;                                       // TIM2 clock enable
    TIM2->CCMR1 = TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;        // CC1 output, CC1 Fast off, CC1 preload on, CC1 mode PWM 1 (0110)
    TIM2->CCER = TIM_CCER_CC1E;                                                 // CC1 output on
    TIM2->PSC = FMCU_NORMAL_HZ / PWM_WIDTH_MAX / PWM_FREQUENCY;                 // TIM prescaler
    TIM2->ARR = PWM_WIDTH_MAX - 1;                                              // Period (100 steps)
    TIM2->CR1 |= TIM_CR1_CEN;                                                   // TIM enable
}

void pwm_width_set(uint8_t width)
{
    assert(width <= PWM_WIDTH_MAX);
    // Форсированный пуск с позиции 0 и 1
    auto current = pwm_width_get();
    if (current < PWM_FORCE_START_STEP && current < width)
    {
        TIM2->CCR1 = PWM_FORCE_START_STEP;                                      // Update CC1 value
        TIM2->EGR |= TIM_EGR_UG;                                                // UG generation
    }
    TIM2->CCR1 = width;                                                         // Update CC1 value
}

uint8_t pwm_width_get(void)
{
    return (uint8_t)TIM2->CCR1;                                                 // Read CC1 value
}
