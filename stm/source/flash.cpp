#include "flash.h"
#include "system.h"

// Размер одной страницы Flash в байтах
#define FLASH_PAGE_SIZE     2048

// Буфер для операций записи
static __no_init union 
{
    uint8_t u8[FLASH_PAGE_SIZE / sizeof(uint8_t)];
    uint32_t u32[FLASH_PAGE_SIZE / sizeof(uint32_t)];
} flash_buffer;

// Обявление секции для __sfb
SECTION_DECLARE(FLASH_RW_SECTION)

void flash_write(const void *dest, const void *source, size_t size)
{
    uint32_t *flash_address = (uint32_t *)__sfb(FLASH_RW_SECTION);
    // Копирование страницы в ОЗУ
    memcpy(flash_buffer.u8, flash_address, FLASH_PAGE_SIZE);
    memcpy(flash_buffer.u8 + ((uint8_t *)dest - (uint8_t *)flash_address), source, size);
    // Запрет предываний
    IRQ_SAFE_ENTER();
        FLASH->KEYR = 0x45670123;                                               // First unlock key
        FLASH->KEYR = 0xCDEF89AB;                                               // Second unlock key
            WAIT_WHILE(FLASH->SR & FLASH_SR_BSY);                               // Wait busy flag
            
            // Стирание страницы
            FLASH->SR = 0xFFFFFFFF;                                             // Clear all errors
            FLASH->CR |= FLASH_CR_PER | (0xFF << 3) | FLASH_CR_BKER;            // Erase, Page 255, Bank 2
            FLASH->CR |= FLASH_CR_STRT;                                         // Start operation
            WAIT_WHILE(FLASH->SR & FLASH_SR_BSY);                               // Wait busy flag
            FLASH->CR &= ~FLASH_CR_PER;                                         // Reset erase mode
            
            // Запись новых данных
            FLASH->SR = 0xFFFFFFFF;                                             // Clear all errors
            FLASH->CR |= FLASH_CR_PG;                                           // Fast programming
            for (auto i = 0; i < FLASH_PAGE_SIZE / 4; i += 2)
            {
                flash_address[i + 0] = flash_buffer.u32[i + 0];                 // First word
                flash_address[i + 1] = flash_buffer.u32[i + 1];                 // Second word
                WAIT_WHILE(FLASH->SR & FLASH_SR_BSY);                           // Wait busy flag
            }
            
            // Завершение
            FLASH->CR &= ~FLASH_CR_PG;                                          // Reset fast programming mode
        FLASH->CR |= FLASH_CR_LOCK;                                             // Lock Flash controller
    IRQ_SAFE_LEAVE();
}
