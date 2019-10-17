#include "io.h"
#include "spi.h"
#include "system.h"

/* О использовании DMA.
 * Хоть используется синхронная передача, тем не менее что бы процесс передачи
 * не прерывать возникающими прерываниями, было решено использовать DMA. */

// Alias
#define SPI_DMA_CHANNEL         DMA2_Channel2

// Конвертирование адреса в значение регистра
#define SPI_DMA_ADR_TO_REG(a)   ((uint32_t)(a))

void spi_init(void)
{
    // DMA
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;                                         // DMA2 clock gate
    SPI_DMA_CHANNEL->CCR = DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_PL_1;           // To peripheral, From memory, Circular off, PINC off, MINC on, 8 bit, High priority
    SPI_DMA_CHANNEL->CPAR = SPI_DMA_ADR_TO_REG(&SPI3->DR);                      // Dest peripheral address
    DMA2_CSELR->CSELR = DMA_CSELR_C2S & (3 << 4);                               // Select peripheral (SPI3) on channel 2
    // IO
    IO_AFUNC_SET(IO_LCD_CLK, 0x06);
    IO_AFUNC_SET(IO_LCD_MOSI, 0x06);
    // SPI
    RCC->APB1ENR1 |= RCC_APB1ENR1_SPI3EN;                                       // SPI3 clock gate
    SPI3->CR1 = SPI_CR1_MSTR | SPI_CR1_BIDIOE |  SPI_CR1_SSI | SPI_CR1_SSM;     // CPHA first, CPOL low, Master, /2, MSB, SSM on, SSI active, TX only
    SPI3->CR2 = SPI_CR2_TXDMAEN | SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;   // DMA on tx, SSO off, NSS off, Motorola, 8 bit, IE off
    SPI3->CR1 |= SPI_CR1_SPE;                                                   // SPI3 enable
}

void spi_start(void)
{
    IO_PORT_RESET(IO_LCD_CS);                                                   // Slave select
}

void spi_stop(void)
{
    WAIT_WHILE(SPI3->SR & SPI_SR_BSY);                                          // Wait BUSY
    IO_PORT_SET(IO_LCD_CS);                                                     // Slave deselect
}

void spi_write(const void *data, size_t size)
{
    // В обход DMA если 1 байт
    if (size <= 1)
    {
        *(volatile uint8_t *)&SPI3->DR = *(const uint8_t *)data;                // Write data
        return;
    }
    // По DMA
    SPI_DMA_CHANNEL->CMAR = SPI_DMA_ADR_TO_REG(data);                           // Source memory address
    SPI_DMA_CHANNEL->CNDTR = size;                                              // Nuber of transfers
    SPI_DMA_CHANNEL->CCR |= DMA_CCR_EN;                                         // Channel enable
        WAIT_FOR(DMA2->ISR & (DMA_ISR_TCIF2 | DMA_ISR_TEIF2));                  // Wait for complete or error
    SPI_DMA_CHANNEL->CCR &= ~DMA_CCR_EN;                                        // Channel disable
    DMA2->IFCR |= DMA_ISR_GIF2;                                                 // Clear all interrupt flags
}
