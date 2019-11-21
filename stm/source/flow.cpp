#include "flow.h"
#include "uart.h"
#include "timer.h"

// Передаваемое потоком значение
__no_init float32_t flow_value;

// Таймер передкачи потока АЦП по UART
static timer_notify_t flow_timer([](void)
{
    uint8_t data[] = { 0xAA, 0x00, 0x00, 0x00, 0x00, 0x0A };
    // Упаковка показания
    memcpy(data + 1, &flow_value, sizeof(flow_value));
    // Контрольная сумма
    for (auto i = 1; i < 5; i++)
        data[5] += data[i];
    // Передача
    uart_transmit(data, sizeof(data));
});

// Инициализация модуля
void flow_init(void)
{
    flow_value = NAN;
    // Старт таймера передачи потока
#ifndef NDEBUG
    flow_timer.start_hz(1000, TIMER_FLAG_LOOP);
#endif
}
