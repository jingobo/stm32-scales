#include "io.h"
#include "adc.h"
#include "mcu.h"

// Размер скользящего окна усриднения (кратно 10)
#define ADC_WINDOW_SIZE     50

// Локальные переменные модуля
static __no_init struct
{
    // Указатель на текущее записываемое значение
    uint32_t pointer;
    // Сумма всех элементов окна
    int32_t summ;
    // Скользящее окно усреднения
    int32_t window[ADC_WINDOW_SIZE];
} adc;

void adc_init(void)
{
    memset(&adc, 0, sizeof(adc));
}

// Синхронная задержка для ногодрыга
static void adc_delay(void)
{
    // Значение подорбано осцилографом на частоте MCU 80 МГц, выходит ~0.5 мкС
    mcu_delay(5);
}

// Внутрення функция чтения данных из АЦП (1 конвертирование)
static void adc_conversion(void)
{
    // Ожидание окончания преобразования
    WAIT_WHILE(IO_PORT_READ(IO_HX_RX));
    
    // Запрет прерываний
    IRQ_CTX_SAVE();
        IRQ_CTX_DISABLE();
        
    	union
        {
            uint32_t u32;
            int32_t s32;
        } = { 0 };
        
        // Конвертирование
        for (uint8_t i = 0; i < 24; i++)
        {
            // Поднимаем строб
            IO_PORT_SET(IO_HX_CLK);
            adc_delay();
            // Сдвига значения АЦП
            u32 <<= 1;
            // Опускаем строб
            IO_PORT_RESET(IO_HX_CLK);
            adc_delay();
            // Забираем очередной бит
            if (IO_PORT_READ(IO_HX_RX))
                u32++;
        }

        // Старт выборрки по каналу А с коэффициентом усиления 128
        IO_PORT_SET(IO_HX_CLK);
        adc_delay();
        IO_PORT_RESET(IO_HX_CLK);
    // Возвращяем прерывания
	IRQ_CTX_RESTORE();
    
    // Конвертирование в 32-бит знаковое
    if (u32 & 0x800000)
        u32 |= 0xFF000000;
        
    // Обработка скользящего окна
    adc.summ -= adc.window[adc.pointer];
    adc.window[adc.pointer] = s32;
    adc.summ += s32;
    
    // Смещение
    if (++adc.pointer >= ADC_WINDOW_SIZE)
        adc.pointer = 0;
}

OPTIMIZE_SPEED
int32_t adc_read(void)
{
    // Несколько конвертирований за раз
    for (uint8_t i = 0; i < ADC_WINDOW_SIZE / 10; i++)
         adc_conversion();
    // Готово
	return adc.summ / ADC_WINDOW_SIZE;
}
