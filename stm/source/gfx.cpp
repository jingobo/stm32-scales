#include "gfx.h"

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
typedef struct
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
} gfx_color_t;

// Буферы цвета для вывода изображений
static __no_init gfx_color_t gfx_foreground, gfx_background;

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

// Вывод изображения сжатого RLE
static void gfx_image_rle(const uint8_t *data, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint16_t buffer_offset = 0;
    // Подготовка компонентов цвета
    const gfx_color_t dx =
    {
        gfx_foreground.r - gfx_background.r,
        gfx_foreground.g - gfx_background.g,
        gfx_foreground.b - gfx_background.b,
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
            dx.r * grad / LCD_COLOR_COMP_MAX + gfx_background.r,
            dx.g * grad / LCD_COLOR_COMP_MAX + gfx_background.g,
            dx.b * grad / LCD_COLOR_COMP_MAX + gfx_background.b,
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
            lcd_out(gfx_buffer, buffer_offset);
            buffer_offset = 0;
        }
    }
    // Вывод не переданной части буфера
    if (buffer_offset > 0)
        lcd_out(gfx_buffer, buffer_offset);
}

void gfx_image(const uint8_t *image, uint16_t x, uint16_t y, lcd_color_t foreground, lcd_color_t background)
{
    // Конвертирование цветов
    gfx_foreground.decode(foreground);
    gfx_background.decode(background);
    // Вывод
    gfx_image_rle(image + 2, x, y, image[0], image[1]);
}

// Вывод одного символа
static uint16_t gfx_symbol(char symbol, const uint8_t *font, uint16_t x, uint16_t y)
{
    // Высота символа
    uint8_t height = font[0];
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
        gfx_image_rle(font, x, y, width, height);
        return width;
    }
}

uint16_t gfx_string(const char *string, const uint8_t *font, uint16_t x, uint16_t y, lcd_color_t foreground, lcd_color_t background)
{
    // Конвертирование цветов
    gfx_foreground.decode(foreground);
    gfx_background.decode(background);
    // Обход символов
    uint16_t total_width = 0;
    for (; *string != '\0'; string++)
    {
        auto width = gfx_symbol(*string, font, x, y);
        total_width += width;
        x += width;
    }
    return total_width;
}
