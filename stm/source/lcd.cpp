#include "io.h"
#include "lcd.h"
#include "mcu.h"
#include "spi.h"

// Коды команд контроллера LCD
typedef enum
{
    // Level 1
    LCD_COMMAND_SWRESET             = 0x01, // Software Reset
    LCD_COMMAND_READ_DISPLAY_ID     = 0x04, // Read display identification information
    LCD_COMMAND_RDDST               = 0x09, // Read Display Status
    LCD_COMMAND_RDDPM               = 0x0A, // Read Display Power Mode
    LCD_COMMAND_RDDMADCTL           = 0x0B, // Read Display MADCTL
    LCD_COMMAND_RDDCOLMOD           = 0x0C, // Read Display Pixel Format
    LCD_COMMAND_RDDIM               = 0x0D, // Read Display Image Format
    LCD_COMMAND_RDDSM               = 0x0E, // Read Display Signal Mode
    LCD_COMMAND_RDDSDR              = 0x0F, // Read Display Self-Diagnostic Result
    LCD_COMMAND_SPLIN               = 0x10, // Enter Sleep Mode
    LCD_COMMAND_SLEEP_OUT           = 0x11, // Sleep out register
    LCD_COMMAND_PTLON               = 0x12, // Partial Mode ON
    LCD_COMMAND_NORMAL_MODE_ON      = 0x13, // Normal Display Mode ON
    LCD_COMMAND_DINVOFF             = 0x20, // Display Inversion OFF
    LCD_COMMAND_DINVON              = 0x21, // Display Inversion ON
    LCD_COMMAND_GAMMA               = 0x26, // Gamma register
    LCD_COMMAND_DISPLAY_OFF         = 0x28, // Display off register
    LCD_COMMAND_DISPLAY_ON          = 0x29, // Display on register
    LCD_COMMAND_COLUMN_ADDR         = 0x2A, // Colomn address register
    LCD_COMMAND_PAGE_ADDR           = 0x2B, // Page address register
    LCD_COMMAND_GRAM                = 0x2C, // GRAM register
    LCD_COMMAND_RGBSET              = 0x2D, // Color SET
    LCD_COMMAND_RAMRD               = 0x2E, // Memory Read
    LCD_COMMAND_PLTAR               = 0x30, // Partial Area
    LCD_COMMAND_VSCRDEF             = 0x33, // Vertical Scrolling Definition
    LCD_COMMAND_TEOFF               = 0x34, // Tearing Effect Line OFF
    LCD_COMMAND_TEON                = 0x35, // Tearing Effect Line ON
    LCD_COMMAND_MAC                 = 0x36, // Memory Access Control register
    LCD_COMMAND_VSCRSADD            = 0x37, // Vertical Scrolling Start Address
    LCD_COMMAND_IDMOFF              = 0x38, // Idle Mode OFF
    LCD_COMMAND_IDMON               = 0x39, // Idle Mode ON
    LCD_COMMAND_PIXEL_FORMAT        = 0x3A, // Pixel Format register
    LCD_COMMAND_WRITE_MEM_CONTINUE  = 0x3C, // Write Memory Continue
    LCD_COMMAND_READ_MEM_CONTINUE   = 0x3E, // Read Memory Continue
    LCD_COMMAND_SET_TEAR_SCANLINE   = 0x44, // Set Tear Scanline
    LCD_COMMAND_GET_SCANLINE        = 0x45, // Get Scanline
    LCD_COMMAND_WDB                 = 0x51, // Write Brightness Display register
    LCD_COMMAND_RDDISBV             = 0x52, // Read Display Brightness
    LCD_COMMAND_WCD                 = 0x53, // Write Control Display register*/
    LCD_COMMAND_RDCTRLD             = 0x54, // Read CTRL Display
    LCD_COMMAND_WRCABC              = 0x55, // Write Content Adaptive Brightness Control
    LCD_COMMAND_RDCABC              = 0x56, // Read Content Adaptive Brightness Control
    LCD_COMMAND_WRITE_CABC          = 0x5E, // Write CABC Minimum Brightness
    LCD_COMMAND_READ_CABC           = 0x5F, // Read CABC Minimum Brightness
    LCD_COMMAND_READ_ID1            = 0xDA, // Read ID1
    LCD_COMMAND_READ_ID2            = 0xDB, // Read ID2
    LCD_COMMAND_READ_ID3            = 0xDC, // Read ID3

    // Level 2
    LCD_COMMAND_RGB_INTERFACE       = 0xB0, // RGB Interface Signal Control
    LCD_COMMAND_FRMCTR1             = 0xB1, // Frame Rate Control (In Normal Mode)
    LCD_COMMAND_FRMCTR2             = 0xB2, // Frame Rate Control (In Idle Mode)
    LCD_COMMAND_FRMCTR3             = 0xB3, // Frame Rate Control (In Partial Mode)
    LCD_COMMAND_INVTR               = 0xB4, // Display Inversion Control
    LCD_COMMAND_BPC                 = 0xB5, // Blanking Porch Control register
    LCD_COMMAND_DFC                 = 0xB6, // Display Function Control register
    LCD_COMMAND_ETMOD               = 0xB7, // Entry Mode Set
    LCD_COMMAND_BACKLIGHT1          = 0xB8, // Backlight Control 1
    LCD_COMMAND_BACKLIGHT2          = 0xB9, // Backlight Control 2
    LCD_COMMAND_BACKLIGHT3          = 0xBA, // Backlight Control 3
    LCD_COMMAND_BACKLIGHT4          = 0xBB, // Backlight Control 4
    LCD_COMMAND_BACKLIGHT5          = 0xBC, // Backlight Control 5
    LCD_COMMAND_BACKLIGHT7          = 0xBE, // Backlight Control 7
    LCD_COMMAND_BACKLIGHT8          = 0xBF, // Backlight Control 8
    LCD_COMMAND_POWER1              = 0xC0, // Power Control 1 register
    LCD_COMMAND_POWER2              = 0xC1, // Power Control 2 register
    LCD_COMMAND_VCOM1               = 0xC5, // VCOM Control 1 register
    LCD_COMMAND_VCOM2               = 0xC7, // VCOM Control 2 register
    LCD_COMMAND_NVMWR               = 0xD0, // NV Memory Write
    LCD_COMMAND_NVMPKEY             = 0xD1, // NV Memory Protection Key
    LCD_COMMAND_RDNVM               = 0xD2, // NV Memory Status Read
    LCD_COMMAND_READ_ID4            = 0xD3, // Read ID4
    LCD_COMMAND_PGAMMA              = 0xE0, // Positive Gamma Correction register
    LCD_COMMAND_NGAMMA              = 0xE1, // Negative Gamma Correction register
    LCD_COMMAND_DGAMCTRL1           = 0xE2, // Digital Gamma Control 1
    LCD_COMMAND_DGAMCTRL2           = 0xE3, // Digital Gamma Control 2
    LCD_COMMAND_INTERFACE           = 0xF6, // Interface control register

    // Extend register
    LCD_COMMAND_POWERA              = 0xCB, // Power control A register
    LCD_COMMAND_POWERB              = 0xCF, // Power control B register
    LCD_COMMAND_DTCA                = 0xE8, // Driver timing control A
    LCD_COMMAND_DTCB                = 0xEA, // Driver timing control B
    LCD_COMMAND_POWER_SEQ           = 0xED, // Power on sequence register
    LCD_COMMAND_3GAMMA_EN           = 0xF2, // 3 Gamma enable register
    LCD_COMMAND_PRC                 = 0xF7, // Pump ratio control register
} lcd_command_t;

// Передача данных
static void lcd_data(const void *data, size_t size)
{
    if (size <= 0)
        return;
    IO_PORT_SET(IO_LCD_DC);                                                     // Select data mode
    spi_start();
        spi_write(data, size);
    spi_stop();
}

// Передача команды без данных
static void lcd_command(lcd_command_t command)
{
    IO_PORT_RESET(IO_LCD_DC);                                                   // Select command mode
    spi_start();
        spi_write(&command, sizeof(command));
    spi_stop();
}

// Передача команды с данными через буфер
static void lcd_command(lcd_command_t command, const void *data, uint8_t size = 0)
{
    lcd_command(command);
    lcd_data(data, size);
}

// Размер буфера на стэеке под данные команды
#define LCD_COMMAND_BUFFER_SIZE     16

// Передача команды
static void lcd_command_va(lcd_command_t command, uint8_t size = 0, ...)
{
    // Проверка аргументов
    assert(size <= LCD_COMMAND_BUFFER_SIZE);
    // Формирование буфера данных
    uint8_t buffer[LCD_COMMAND_BUFFER_SIZE];
    va_list args;
    va_start(args, size);
    for (uint8_t i = 0; i < size; i++) 
        buffer[i] = va_arg(args, int);
    va_end(args);
    // Передача
    lcd_command(command, buffer, size);
}

void lcd_init(void)
{
    assert(sizeof(lcd_command_t) == 1);
    // Инициалзиация SPI
    spi_init();
    
    // SOFTWARE RESET
    lcd_command(LCD_COMMAND_SWRESET);
    mcu_delay_ms(1000);

    // POWER CONTROL A
    lcd_command_va(LCD_COMMAND_POWERA, 5, 0x39, 0x2C, 0x00, 0x34, 0x02);
    // POWER CONTROL B
    lcd_command_va(LCD_COMMAND_POWERB, 3, 0x00, 0xC1, 0x30);

    // DRIVER TIMING CONTROL A
    lcd_command_va(LCD_COMMAND_DTCA, 3, 0x85, 0x00, 0x78);
    
    // DRIVER TIMING CONTROL B
    lcd_command_va(LCD_COMMAND_DTCB, 2, 0x00, 0x00);

    // POWER ON SEQUENCE CONTROL
    lcd_command_va(LCD_COMMAND_POWER_SEQ, 4, 0x64, 0x03, 0x12, 0x81);

    // PUMP RATIO CONTROL
    lcd_command_va(LCD_COMMAND_PRC, 1, 0x20);

    // POWER CONTROL, VRH[5:0]
    lcd_command_va(LCD_COMMAND_POWER1, 1, 0x23);

    // POWER CONTROL, SAP[2:0], BT[3:0]
    lcd_command_va(LCD_COMMAND_POWER2, 1, 0x10);

    // VCM CONTROL 1
    lcd_command_va(LCD_COMMAND_VCOM1, 1, 0x3E, 0x28);

    // VCM CONTROL 2
    lcd_command_va(LCD_COMMAND_VCOM2, 1, 0x86);

    // MEMORY ACCESS CONTROL
    lcd_command_va(LCD_COMMAND_MAC, 1, 0x48);

    // PIXEL FORMAT
    lcd_command_va(LCD_COMMAND_PIXEL_FORMAT, 1, 0x55);

    // FRAME RATIO CONTROL, STANDARD RGB COLOR
    lcd_command_va(LCD_COMMAND_FRMCTR1, 2, 0x00, 0x18);

    // DISPLAY FUNCTION CONTROL
    lcd_command_va(LCD_COMMAND_DFC, 3, 0x08, 0x82, 0x27);

    // 3GAMMA FUNCTION DISABLE
    lcd_command_va(LCD_COMMAND_3GAMMA_EN, 1, 0x00);

    // GAMMA CURVE SELECTED
    lcd_command_va(LCD_COMMAND_GAMMA, 1, 0x01);

    // POSITIVE GAMMA CORRECTION
    lcd_command_va(LCD_COMMAND_PGAMMA, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00);
    
    // NEGATIVE GAMMA CORRECTION
    lcd_command_va(LCD_COMMAND_NGAMMA, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F);

    // EXIT SLEEP
    lcd_command(LCD_COMMAND_SLEEP_OUT);
    mcu_delay_ms(120);

    // TURN ON DISPLAY
    lcd_command(LCD_COMMAND_DISPLAY_ON);
}

// Сохранение в буфер курсора в BE формате
#define LCD_AREA_BE(x, offset)          \
    buffer[offset + 0] = (x) >> 8;      \
    buffer[offset + 1] = (x) & 0xFF;

void lcd_area(uint16_t left, uint16_t top, uint16_t width, uint16_t height)
{
    // Подсчет координат
    width = left + width - 1;
    height = top + height - 1;
    uint8_t buffer[4];
    // X
    LCD_AREA_BE(left, 0);
    LCD_AREA_BE(width, 2);
    lcd_command(LCD_COMMAND_COLUMN_ADDR, buffer, sizeof(buffer));
    // Y
    LCD_AREA_BE(top, 0);
    LCD_AREA_BE(height, 2);
    lcd_command(LCD_COMMAND_PAGE_ADDR, buffer, sizeof(buffer));
    // Переключение в вывод данных
	lcd_command(LCD_COMMAND_GRAM);
}

void lcd_out(const lcd_color_t *data, size_t count)
{
    lcd_data(data, count * sizeof(lcd_color_t));
}
