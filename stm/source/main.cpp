#include "io.h"
#include "adc.h"
#include "mcu.h"
#include "lcd.h"
#include "key.h"
#include "gui.h"
#include "pwm.h"
#include "flow.h"
#include "nvic.h"
#include "uart.h"
#include "event.h"
#include "timer.h"
#include "weight.h"

// Точка входа в приложение
__task __noreturn void main(void)
{
    // Системные модули (порядок не менять)
    nvic_init();
    mcu_init();
    io_init();
    pwm_init();
    uart_init();
    timer_init();
    flow_init();
    // Остальные модули
    key_init();
    adc_init();
    lcd_init();
    gui_init();
    weight_init();
    
    // Главный цикл событий
    event_t::loop();
}
