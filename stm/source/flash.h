#ifndef __FLASH_H
#define __FLASH_H

#include "typedefs.h"

// Имя секции перезаписываемого хранилища
#define FLASH_RW_SECTION    ".storage"

// Запись во флеш
void flash_write(const void *dest, const void *source, size_t size);

#endif // __FLASH_H
