#include "gfx.h"
#include "font.h"
#include "image.h"

// Загрузка U16 в LE из массива байт
#define GFX_LOAD_LE16(ptr)              ((uint16_t)(ptr)[0] | ((uint16_t)(ptr)[1] << 8))
// Размер внутреннего буфера
#define GFX_BUFFER_SIZE                 MAX(LCD_SIZE_WIDTH, LCD_SIZE_HEIGHT)
// Максимальное количество повторов при режиме вывода данных как есть
#define GFX_RLE_REPEAT_ORIG_MAX         4
// Базовое смещение значения повторов при режиме вывода данных как есть
#define GFX_RLE_REPEAT_ORIG_BASE        (255 - GFX_RLE_REPEAT_ORIG_MAX + 1)

// Тип под компоненту цвета
typedef int_fast16_t gfx_comp_t;

// Струтура цвета разложенного по компонентам
struct gfx_color_t
{
    gfx_comp_t r, g, b;
    
    // Декодирование из 16-бит цвета
    void decode(lcd_color_t source)
    {
        source = lcd_color_swap(source);
        b = (source >> LCD_COLOR_POS_B) & LCD_COLOR_MAX_B;
        g = (source >> LCD_COLOR_POS_G) & LCD_COLOR_MAX_G;
        r = (source >> LCD_COLOR_POS_R) & LCD_COLOR_MAX_R;
    }
    
    // Кодирование в 16-бит цвет
    lcd_color_t encode(void) const
    {
        return lcd_color_swap(LCD_COLOR_RAW(r, g, b));
    }
};

// Локальные переменные
static __no_init struct
{
    // Буферы цвета для вывода изображений
    gfx_color_t foreground, background;
    // Внутренний буфер общего назначения
    lcd_color_t buffer[GFX_BUFFER_SIZE];
} gfx;

// Заполнение цветом внутреннего буфера
static void gfx_buffer_fill(lcd_color_t color, uint16_t count, uint16_t offset = 0)
{
    count += offset;
    for (uint16_t i = offset; i < count; i++)
        gfx.buffer[i] = color;
}

void gfx_rect_frame(uint16_t x, uint16_t y, uint16_t width, uint16_t height, lcd_color_t foreground)
{
    // Заполнение буфера цветом
    gfx_buffer_fill(foreground, MAX(width, height));
    // Верх
    lcd_area(x, y, width, 1);
    lcd_out(gfx.buffer, width);
    // Право
    lcd_area(x + width - 1, y, 1, height);
    lcd_out(gfx.buffer, height);
    // Низ
    lcd_area(x, y + height - 1, width, 1);
    lcd_out(gfx.buffer, width);
    // Лево
    lcd_area(x, y, 1, height);
    lcd_out(gfx.buffer, height);
}

void gfx_rect_solid(uint16_t x, uint16_t y, uint16_t width, uint16_t height, lcd_color_t background)
{
    // Заполнение буфера цветом
    gfx_buffer_fill(background, width);
    // Вывод по сторкам
    lcd_area(x, y, width, height);
    for (; height > 0; height--)
        lcd_out(gfx.buffer, width);
}

void gfx_rect_frame_round(uint16_t x, uint16_t y, uint16_t width, uint16_t height, lcd_color_t foreground, lcd_color_t background)
{
    // Толщина границы
    const uint16_t thickness = 3;
    // Границы
        // Верх
    gfx_rect_solid(x, y - thickness, width, thickness, foreground);
        // Право
    gfx_rect_solid(x + width, y, thickness, height, foreground);
        // Низ
    gfx_rect_solid(x, y + height, width, thickness, foreground);
        // Лево
    gfx_rect_solid(x - thickness, y, thickness, height, foreground);
    // Тайлы
        // LT
    gfx_image(IMAGE_ROUND_LT, x - thickness, y - thickness, foreground, background);
        // RT
    gfx_image(IMAGE_ROUND_RT, x + width, y - thickness, foreground, background);
        // RB
    gfx_image(IMAGE_ROUND_RB, x + width, y + height, foreground, background);
        // LB
    gfx_image(IMAGE_ROUND_LB, x - thickness, y + height, foreground, background);
}

// Вывод изображения сжатого RLE
static void gfx_image_rle(const uint8_t *data, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint16_t buffer_offset = 0;
    // Подготовка компонентов цвета
    const gfx_color_t dx =
    {
        gfx.foreground.r - gfx.background.r,
        gfx.foreground.g - gfx.background.g,
        gfx.foreground.b - gfx.background.b,
    };
    // Подготовка области
    lcd_area(x, y, width, height);
    // Обход данных
    for (uint32_t written = width * height; written > 0; data += 2)
    {
        // Количество повторов
        auto repeat = data[0];
        const gfx_comp_t grad = data[1];
        
        // Рассчет цвета
        const gfx_color_t t =
        {
            dx.r * grad / LCD_COLOR_COMP_MAX + gfx.background.r,
            dx.g * grad / LCD_COLOR_COMP_MAX + gfx.background.g,
            dx.b * grad / LCD_COLOR_COMP_MAX + gfx.background.b,
        };
        const lcd_color_t color = t.encode();
        
        // Помещаем в буфер
        while (repeat > 0)
        {
            // Определеие свободной области буфера
            uint16_t avail = GFX_BUFFER_SIZE - buffer_offset;
            avail = MIN(avail, repeat);
            // Заполнение буфера
            gfx_buffer_fill(color, avail, buffer_offset);
            buffer_offset += avail;
            // Смещение
            written -= avail;
            repeat -= avail;
            // Передача
            if (buffer_offset < GFX_BUFFER_SIZE)
                continue;
            lcd_out(gfx.buffer, buffer_offset);
            buffer_offset = 0;
        }
    }
    // Вывод не переданной части буфера
    if (buffer_offset > 0)
        lcd_out(gfx.buffer, buffer_offset);
}

void gfx_image(const uint8_t *image, uint16_t x, uint16_t y, lcd_color_t foreground, lcd_color_t background)
{
    assert(image != NULL);
    // Конвертирование цветов
    gfx.foreground.decode(foreground);
    gfx.background.decode(background);
    // Вывод
    gfx_image_rle(image + 2, x, y, image[0], image[1]);
}

// Вывод одного символа
static uint16_t gfx_symbol(char symbol, const uint8_t *font, uint16_t x, uint16_t y, bool paint)
{
    // Высота символа
    auto height = font_height_get(font);
    font += 1;
    // Обход имеющихся символов
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
        if (paint)
            gfx_image_rle(font, x, y, width, height);
        return width;
    }
}

// Внутренняя функция вывода строки
static uint16_t gfx_string_internal(const char *string, const uint8_t *font, uint16_t x, uint16_t y, bool paint)
{
    uint16_t total_width = 0;
    // Обход символов
    for (; *string != '\0'; string++)
    {
        auto width = gfx_symbol(*string, font, x, y, paint);
        total_width += width;
        x += width;
    }
    return total_width;
}


uint16_t gfx_string(const char *string, const uint8_t *font, uint16_t x, uint16_t y, lcd_color_t foreground, lcd_color_t background)
{
    assert(string != NULL);
    // Конвертирование цветов
    gfx.foreground.decode(foreground);
    gfx.background.decode(background);
    // Вывод
    return gfx_string_internal(string, font, x, y, true);
}

uint16_t gfx_string_measure(const char *string, const uint8_t *font)
{
    return gfx_string_internal(string, font, 0, 0, false);
}
