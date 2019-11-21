#include "io.h"
#include "uart.h"
#include "system.h"

// Скорость [бод/с]
#define UART_BITRATE    115200
// Частота тактирования
#define UART_FCLK       ((uint32_t)(FMCU_NORMAL_HZ * 2))
// Делитель частоты
#define UART_FDIV       (UART_FCLK / UART_BITRATE)

void uart_init(void)
{
    // IO
    IO_AFUNC_SET(IO_DBG_TX, 0x07);
    IO_AFUNC_SET(IO_DBG_RX, 0x07);
    // UART
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;                                     // USART2 clock gate
    USART2->CR1 = USART_CR1_OVER8;                                              // UART off, Parity off, 8 samples, 8Nx
    USART2->CR2 = 0;                                                            // Async, LSB, 1 stop bit
    USART2->BRR = (UART_FDIV & 0xFFFFFFF0) | ((UART_FDIV & 0x0F) >> 1);         // Baudrate divider
    USART2->CR1 |= USART_CR1_UE | USART_CR1_TE;                                 // UART enable on Tx
}

void uart_transmit(const void *data, size_t count)
{
    auto buffer = (const uint8_t *)data;
    for (; count > 0; count--, buffer++)
    {
        while (!(USART2->ISR & USART_ISR_TXE))                                  // Wait for TXE
        { }
        USART2->TDR = *buffer;                                                  // Transmit data
    }
}
