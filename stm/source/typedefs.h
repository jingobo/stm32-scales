#ifndef __TYPEDEFS_H
#define __TYPEDEFS_H

// Стандартная библиотека
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// IAR
#include <intrinsics.h>

// Вещественные типы данных
typedef float float32_t;
typedef double float64_t;

// Вещественный тип по умолчанию
typedef float32_t float_t;

// Прототип указателя на функцию без аргументов
typedef void (* notify_proc_ptr)(void);

// --- Для макроопределений --- //

// Оборачивание кода в блок
#define CODE_BLOCK(code)        \
    do {                        \
        code;                   \
    } while (false)

// Пустой макрос
#define MACRO_EMPTY
// Преобразование в строку значения макроса
#define STRINGIFY(x)            #x
// Опция компилятора как макрос
#define PRAGMA(msg)             _Pragma(STRINGIFY(msg))
// Пустой блок
#define EMPTY_BLOCK             CODE_BLOCK(MACRO_EMPTY)
// Установка опции компилятора через макрос
#define PRAGMA_OPTION(m, v)     PRAGMA(m = v)

// --- Битовые маски --- //

#define LSHIFT(t, v, p)         ((t)(v) << (p))
#define RSHIFT(t, v, p)         ((t)(v) >> (p))
        
#define MASK(type, value, pos)  LSHIFT(type, value, pos)
#define MASK_8(value, pos)     	MASK(uint8_t, value, pos)
#define MASK_16(value, pos)     MASK(uint16_t, value, pos)
#define MASK_32(value, pos)     MASK(uint32_t, value, pos)

// Верванивание данных
#define ALIGN_DATA_8            PRAGMA_OPTION(data_alignment, 1)
#define ALIGN_DATA_16           PRAGMA_OPTION(data_alignment, 2)
#define ALIGN_DATA_32           PRAGMA_OPTION(data_alignment, 4)

// --- Выравнивание полей --- //

#define ALIGN_FIELD_8           PRAGMA(pack(1))
#define ALIGN_FIELD_16          PRAGMA(pack(2))
#define ALIGN_FIELD_32          PRAGMA(pack(4))
#define ALIGN_FIELD_DEF         PRAGMA(pack())

// --- Работа с памятью --- //

// Отчистка памяти
#define MEMORY_CLEAR(var)       memset(&(var), 0, sizeof(var))
// Отчистка памяти по указателю
#define MEMORY_CLEAR_PTR(var)   memset(var, 0, sizeof(*(var)))
// Отчистка памяти для массива
#define MEMORY_CLEAR_ARR(arr)   memset(arr, 0, sizeof(arr))

// Режимы оптимизаций функций
#define OPTIMIZE_NONE           PRAGMA_OPTION(optimize, none)
#define OPTIMIZE_SIZE           PRAGMA_OPTION(optimize, size)
#define OPTIMIZE_SPEED          PRAGMA_OPTION(optimize, speed)
// Подставление функций
#define INLINE_NEVER            PRAGMA_OPTION(inline, never)
#define INLINE_FORCED           PRAGMA_OPTION(inline, forced)

// Включение/Отключение предупреждений
#define WARNING_SUPPRESS(err)   PRAGMA_OPTION(diag_suppress, err)
#define WARNING_DEFAULT(err)    PRAGMA_OPTION(diag_default, err)

// Секции линковщика
#define SECTION_USED(name)      PRAGMA_OPTION(location, name)
#define SECTION_DECLARE(name)   PRAGMA_OPTION(section, name)

// --- Прерывания --- //

// Переход в спящий режим до прерывания
#define WFI()                   __WFI()

// Модификатор для прерываний
#define IRQ_ROUTINE             INLINE_NEVER OPTIMIZE_SPEED
// Сохраняет/Возвращает текущее состояние прерываний
#define IRQ_CTX_SAVE()          __istate_t __istate = __get_interrupt_state()
#define IRQ_CTX_RESTORE()       __set_interrupt_state(__istate)
// Включает/Отключает все маскируемеые прерывания
#define IRQ_CTX_ENABLE()        __enable_interrupt()
#define IRQ_CTX_DISABLE()       __disable_interrupt()

// Вход/Выход в безопасный  от прерываний код
#define IRQ_SAFE_LEAVE()        IRQ_CTX_RESTORE()
#define IRQ_SAFE_ENTER()        \
    IRQ_CTX_SAVE();             \
    IRQ_CTX_DISABLE()

// --- Арифметические операции --- //
        
// Умножение на 1000
#define XK(v)                	(1000l * (v))
// Умножение на 1000000
#define XM(v)                	(1000000l * (v))
// Получает наименьший элемент
#define MIN(a, b)               (((a) < (b)) ? (a) : (b))
// Получает наибольший элемент
#define MAX(a, b)               (((a) > (b)) ? (a) : (b))
// Получает размер массива в элементах
#define ARRAY_SIZE(x)           (sizeof(x) / sizeof((x)[0]))
// Деление с округлением в большую сторону
#define DIV_CEIL(x, y)          ((x) / (y) + ((x) % (y) > 0 ? 1 : 0))
// Смена местами байт в числе
#define SWAP_16(x)              (RSHIFT(uint16_t, x, 8) | LSHIFT(uint16_t, (x) & 0xFF, 8))

// --- Разное --- //

// Не используемый символ
#define UNUSED(x)           	((void)(x))
// Отчистка строки
#define CSTR_CLEAR(x)           (x)[0] = 0
// Ожидание FALSE
#define WAIT_WHILE(expr)         do { } while (expr)
// Ожидание TRUE
#define WAIT_FOR(expr)           WAIT_WHILE(!(expr))

#endif // __TYPEDEFS_H
