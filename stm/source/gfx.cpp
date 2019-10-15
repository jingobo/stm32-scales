#include "gfx.h"

// Загрузка U16 в LE из массива байт
#define GFX_LOAD_LE16(ptr)  ((uint16_t)(ptr)[0] | ((uint16_t)(ptr)[1] << 8))
// Размер внутреннего буфера
#define GFX_BUFFER_SIZE     MAX(LCD_SIZE_WIDTH, LCD_SIZE_HEIGHT)

// Внутренний буфер общего назначения
static __no_init lcd_color_t gfx_buffer[GFX_BUFFER_SIZE];

// Заполнение цветом внутреннего буфера
static void gfx_buffer_fill(lcd_color_t color, uint16_t count, uint16_t offset = 0)
{
    count += offset;
    for (uint16_t i = offset; i < count; i++)
        gfx_buffer[i] = color;
}

void gfx_rect_frame(lcd_color_t color, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    // Заполнение буфера цветом
    gfx_buffer_fill(color, MAX(width, height));
    // Верх
    lcd_area(x, y, width, 1);
    lcd_out(gfx_buffer, width);
    // Право
    lcd_area(x + width - 1, y, 1, height);
    lcd_out(gfx_buffer, height);
    // Низ
    lcd_area(x, y + height - 1, width, 1);
    lcd_out(gfx_buffer, width);
    // Лево
    lcd_area(x, y, 1, height);
    lcd_out(gfx_buffer, height);
}

void gfx_rect_solid(lcd_color_t color, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    // Заполнение буфера цветом
    gfx_buffer_fill(color, width);
    // Вывод по сторкам
    lcd_area(x, y, width, height);
    for (; height > 0; height--)
        lcd_out(gfx_buffer, width);
}

void gfx_image_raw(const lcd_color_t *data, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    lcd_area(x, y, width, height);
    lcd_out(data, width * height);
}

void gfx_image_rle(const uint8_t *data, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint16_t buffer_offset = 0;
    // Подготовка области
    lcd_area(x, y, width, height);
    // Обход данных
    for (uint32_t written = width * height; written > 0;)
    {
        // Выводимый цвет
        lcd_color_t color = GFX_LOAD_LE16(data + 1);
        // Помещаем в буфер
        for (uint8_t repeat = data[0]; repeat > 0;)
        {
            // Определеие свободной области буфера
            uint16_t avail = GFX_BUFFER_SIZE - buffer_offset;
            avail = MIN(avail, repeat);
            // Заполнение буфера
            gfx_buffer_fill(color, avail, buffer_offset);
            // Смещение
            buffer_offset += avail;
            written -= avail;
            repeat -= avail;
            // Передача
            if (buffer_offset < GFX_BUFFER_SIZE)
                continue;
            lcd_out(gfx_buffer, buffer_offset);
            buffer_offset = 0;
        }
        // Переход к следующему чанку
        data += 3;
    }
    // Вывод не переданной части буфера
    if (buffer_offset > 0)
        lcd_out(gfx_buffer, buffer_offset);
}

uint16_t gfx_font_symbol(char symbol, uint16_t x, uint16_t y, const uint8_t *font)
{
    uint8_t height = font[0];
    font += 1;
    for (;;)
    {
        // Чтение заголовка
        char fsymbol = font[0];
        uint8_t width = font[1];
        uint16_t size = GFX_LOAD_LE16(font + 2);
        font += 4;
        // Проверка на символ
        if (fsymbol == '\0')
            return 0;
        if (fsymbol != symbol)
        {
            font += size;
            continue;
        }
        // Вывод
        gfx_image_rle(font, x, y, width, height);
        return width;
    }
}

uint16_t gfx_font_string(const char *string, uint16_t x, uint16_t y, const uint8_t *font)
{
    uint16_t total_width = 0;
    // Обход символов
    for (; *string != '\0'; string++)
    {
        auto width = gfx_font_symbol(*string, x, y, font);
        total_width += width;
        x += width;
    }
    return total_width;
}
