#include "gui.h"
#include "io.h"
#include "timer.h"

static timer_callback_t led_timer([](void)
{
    IO_PORT_RESET(IO_LED);
});

void gui_init(void)
{ }

void gui_key_event(key_kind_t key, key_state_t state)
{
    IO_PORT_SET(IO_LED);
    led_timer.start_us(10 * 1000);
}
