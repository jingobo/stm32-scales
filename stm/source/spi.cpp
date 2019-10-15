#include "io.h"
#include "spi.h"
#include "system.h"

void spi_init(void)
{
    // IO
    IO_AFUNC_SET(IO_LCD_CLK, 0x06);
    IO_AFUNC_SET(IO_LCD_MOSI, 0x06);
    // SPI
    RCC->APB1ENR1 |= RCC_APB1ENR1_SPI3EN;                                       // SPI3 clock gate
    SPI3->CR1 = SPI_CR1_MSTR | SPI_CR1_BIDIOE |  SPI_CR1_SSI | SPI_CR1_SSM;     // CPHA first, CPOL low, Master, /2, MSB, SSM on, SSI active, TX only
    SPI3->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;                     // DMA off, SSO off, NSS off, Motorola, 8 bit, IE off
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
    for (const uint8_t *buf = (const uint8_t *)data; size > 0; buf++, size--)
    {
        WAIT_FOR(SPI3->SR & SPI_SR_TXE);                                        // Wait TXE
        *((volatile uint8_t *)&SPI3->DR) = *buf;                                // Write out data (byte access)
    }
}
