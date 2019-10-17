#include "io.h"

// Загрузка регистров в буферы
#define IO_LOAD(p)                      \
    odr = 0;                            \
    mode = GPIO##p->MODER;              \
    otype = GPIO##p->OTYPER;            \
    ospeed = GPIO##p->OSPEEDR;          \
    ipupd = GPIO##p->PUPDR

// Возврат в регистры из буферов
#define IO_SAVE(p)                      \
    GPIO##p->ODR = odr;                 \
    GPIO##p->OTYPER = otype;            \
    GPIO##p->OSPEEDR = ospeed;          \
    GPIO##p->PUPDR = ipupd;             \
    GPIO##p->MODER = mode               \

// Инициализация вывода порта
#define IO_INIT(out, m, ot, os, ip)     \
    IO_MODE_SET_EXT(mode, out, m);      \
    IO_OTYPE_SET_EXT(otype, out, ot);   \
    IO_OSPEED_SET_EXT(ospeed, out, os); \
    IO_IPULL_SET_EXT(ipupd, out, ip)

// Установка состояния вывода
#define IO_HIGH(out)                    odr |= IO_MASK(out)

// Альтернативная функция
#define IO_INIT_AF_LS(out, ip)          IO_INIT(out, IO_MODE_ALTFUN, IO_OTYPE_PP, IO_OSPEED_LOW, ip)
#define IO_INIT_AF_HS(out, ip)          IO_INIT(out, IO_MODE_ALTFUN, IO_OTYPE_PP, IO_OSPEED_VERY_HIGH, ip)
// Выход
#define IO_INIT_PP_LS(out)              IO_INIT(out, IO_MODE_OUTPUT, IO_OTYPE_PP, IO_OSPEED_LOW, IO_IPULL_NONE)
#define IO_INIT_PP_HS(out)              IO_INIT(out, IO_MODE_OUTPUT, IO_OTYPE_PP, IO_OSPEED_VERY_HIGH, IO_IPULL_NONE)
// Вход
#define IO_INIT_IN_PU(out)              IO_INIT(out, IO_MODE_INPUT, IO_OTYPE_PP, IO_OSPEED_LOW, IO_IPULL_UP)
#define IO_INIT_IN_PD(out)              IO_INIT(out, IO_MODE_INPUT, IO_OTYPE_PP, IO_OSPEED_LOW, IO_IPULL_DOWN)
#define IO_INIT_IN_FL(out)              IO_INIT(out, IO_MODE_INPUT, IO_OTYPE_PP, IO_OSPEED_LOW, IO_IPULL_NONE)

volatile uint32_t test;
        
OPTIMIZE_SIZE
void io_init(void)
{
    // Промежуточные буферы для формиорвания значений регистров
    uint32_t mode, otype, ospeed, ipupd, odr;
    // Конфигурирование IO по умолчанию
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN |                 // PA, PB clock enable
                    RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIODEN |                 // PC, PD clock enable
                    RCC_AHB2ENR_GPIOHEN;                                        // PH clock enable
    // Порт A
    IO_LOAD(A);
        IO_INIT_IN_PD(0);
        IO_INIT_IN_PD(1);
        IO_INIT_AF_LS(IO_DBG_TX, IO_IPULL_NONE);
        IO_INIT_AF_LS(IO_DBG_RX, IO_IPULL_UP);
        IO_INIT_IN_PD(4);
        IO_INIT_PP_LS(IO_LED);
        IO_INIT_IN_PU(IO_BTN_KEY1);
        IO_INIT_IN_PU(IO_BTN_KEY2);
        IO_INIT_IN_PU(IO_BTN_KEY3);
        IO_INIT_IN_PU(IO_BTN_KEY4);
        IO_INIT_IN_PD(10);
        IO_INIT_PP_LS(IO_GND1);
        IO_INIT_PP_LS(IO_GND2);
        IO_INIT_AF_HS(IO_SWDIO, IO_IPULL_UP);
        IO_INIT_AF_LS(IO_SWCLK, IO_IPULL_DOWN);
        IO_INIT_IN_PD(15);
    IO_SAVE(A);
    // Порт B
    IO_LOAD(B);
        IO_INIT_IN_PD(0);
        IO_INIT_IN_PD(1);
        IO_INIT_IN_PU(IO_BOOT1);
        IO_INIT_IN_PU(IO_SWO);
        IO_INIT_IN_PD(IO_HX_RX);
        IO_INIT_PP_HS(IO_HX_CLK);
        IO_INIT_AF_LS(IO_MT_CH0, IO_IPULL_NONE);
        IO_INIT_AF_LS(IO_MT_CH1, IO_IPULL_NONE);
        IO_INIT_AF_LS(IO_MT_CH2, IO_IPULL_NONE);
        IO_INIT_AF_LS(IO_MT_CH3, IO_IPULL_NONE);
        IO_INIT_IN_PD(10);
        IO_INIT_IN_PD(11);
        IO_INIT_IN_PD(12);
        IO_INIT_IN_PD(13);
        IO_INIT_IN_PD(14);
        IO_INIT_IN_PD(15);
    IO_SAVE(B);
    // Порт C
    IO_LOAD(C);
        IO_INIT_IN_PD(0);
        IO_INIT_IN_PD(1);
        IO_INIT_IN_PD(2);
        IO_INIT_IN_PD(3);
        IO_INIT_IN_PD(4);
        IO_INIT_IN_PD(5);
        IO_INIT_IN_PD(6);
        IO_INIT_IN_PD(7);
        IO_INIT_IN_PD(8);
        IO_INIT_IN_PD(9);
        IO_INIT_AF_HS(IO_LCD_CLK, IO_IPULL_NONE);
        IO_INIT_PP_HS(IO_LCD_CS); IO_HIGH(IO_LCD_CS);
        IO_INIT_AF_HS(IO_LCD_MOSI, IO_IPULL_NONE);
        IO_INIT_IN_PD(IO_BTN);
        IO_INIT_IN_FL(IO_OSC32_IN);
        IO_INIT_IN_FL(IO_OSC32_IN);
    IO_SAVE(C);
    // Порт D
    IO_LOAD(D);
        IO_INIT_PP_HS(IO_LCD_DC);
    IO_SAVE(D);
    // Порт H
    IO_LOAD(H);
        IO_INIT_IN_FL(IO_OSC_IN);
        IO_INIT_IN_FL(IO_OSC_OUT);
    IO_SAVE(H);
}
