﻿#include "list.h"
#include "flash.h"
#include "weight.h"

// Локальные настройки модуля
static const struct
{
    // Смещение нуля
    int32_t offset;
    // Пропорциональность
    int32_t factor;
} WEIGHT_CALIB @ FLASH_RW_SECTION = { 0, 1 };

// Список обработчиков изменения веса
weight_event_handler_t::list_t weight_event_list;

// Класс базового фильтра
class weight_filter_base_t : public list_item_t
{
protected:
    // Обработка фильтра
    virtual int32_t process(int32_t value) = 0;
public:
    // Ввод нового значения
    void input(int32_t value)
    {
        // Обработка
        auto last = process(value);
        // Передача следующему фильтру
        auto nf = (weight_filter_base_t *)next();
        if (nf != NULL)
            nf->input(last);
    }
};

// Медианный фильтр
static class weight_filter_median_t : public weight_filter_base_t
{
    // Константы
    enum
    {
        // Размер буфера фильтра
        SIZE = 50,
    };
    
    // Буфер скользящего среднего
    int32_t window[SIZE];
    // Буфер отсортированных значений
    int32_t sorted[SIZE];
    // Индекс записи в скользящее окно
    uint32_t index;
protected:
    // Обработка фильтра
    virtual int32_t process(int32_t value)
    {
        // Ввод в скользящее окно
        window[index] = value;
        if (++index >= SIZE)
            index = 0;
        // Сортировка
        memcpy(sorted, window, sizeof(window));
        for (auto i = 0; i < SIZE - 1; i++)
        {
            auto n = SIZE - i - 1;
            for (auto j = 0; j < n; j++)
            {
                auto &a = sorted[j];
                auto &b = sorted[j + 1];
                if (a <= b)
                    continue;
                auto tmp = a;
                a = b;
                b = tmp;
            }
        }
        return sorted[SIZE / 2 - 1];
    }
public:
    // Конструктор по умолчанию
    weight_filter_median_t(void) : index(0)
    {
        memset(window, 0, sizeof(window));
    }

} weight_filter_median;

// Фильтр низких частот
static class weight_filter_low_pass_t : public weight_filter_base_t
{
    // Константы
    enum
    {
        // Множитель отражающий точность рассчета при конвертировании
        ACCURACY = 100,
        
        K = 30,
    };
    // Предыдущее значение фильтра
    int32_t last;
protected:
    // Обработка фильтра
    virtual int32_t process(int32_t value)
    {
        return last = last * (ACCURACY - K) / ACCURACY + value * K / ACCURACY;
    }
public:
    // Конструктор по умолчанию
    weight_filter_low_pass_t(void)
    { }
} weight_filter_low_pass;

// Пороговый фильтр
static class weight_filter_threshold_t : public weight_filter_base_t
{
    // Константы
    enum
    {
        // Размер буфера фильтра
        SIZE = 80,
    };
    
    // Накопительный буфер сэмплов
    int32_t buffer[SIZE];
    // Индекс записи в накопительный буфер
    uint32_t index;
    // Текущее окно
    int32_t window;
    // Текущее значение фильтра
    int32_t filtered;
protected:
    // Обработка фильтра
    virtual int32_t process(int32_t value)
    {
        // Ввод
        buffer[index++] = value;
        if (index > SIZE)
        {
            index = 0;
            // Минимум, маскимум, сумма
            int32_t min = INT32_MAX, max = INT32_MIN;
            int64_t sum = 0;
            for (auto i = 0; i < SIZE; i++)
            {
                auto sample = buffer[i];
                if (min > sample)
                    min = sample;
                if (max < sample)
                    max = sample;
                sum += sample;
            }
            // Среднее
            auto avg = (int32_t)(sum / SIZE);
            // Среднеквадратичное отклонение
            sum = 0;
            for (auto i = 0; i < SIZE; i++)
            {
                auto temp = buffer[i] - avg;
                sum =+ temp * temp;
            }
            sum /= SIZE;
            auto deviation = (int32_t)sqrt((double)sum) * 2;
            // Конечный порог
            deviation += max - min;
            if (window > deviation && deviation > 0)
            {
                window = deviation;
                // TODO: по количеству определять готовность
            }
        }
        // Если окно не готово
        if (window == INT32_MAX)
            return filtered;
        
        // Обработка порогов окна
        if (value > filtered + window)
            filtered = value - window;
        else if (value < filtered - window)
            filtered = value + window;
 
        return filtered;
    }
public:
    // Конструктор по умолчанию
    weight_filter_threshold_t(void) 
        : index(0), window(INT32_MAX), filtered(0)
    { }
} weight_filter_threshold;

// Конвертер веса из сырого значения
static class weight_filter_converter_t : public weight_filter_base_t
{
    // Константы
    enum
    {
        // Множитель отражающий точность рассчета при конвертировании
        ACCURACY = 100,
    };
    
    // Тара
    int32_t tare;
    // Предыдущее введенное сырое значение
    int32_t last_raw;
    // Предыдущее переданное обработчикам значение веса
    int32_t last_send;
    
    // Рассчет веса без тары
    int32_t calculate(int32_t raw) const
    {
        return (raw - WEIGHT_CALIB.offset) * WEIGHT_CALIB.factor / ACCURACY;
    }
    
    // Обновление веса
    int32_t refresh(int32_t raw)
    {
        auto result = calculate(raw) - tare;
        if (abs(last_send - result) > 1)
            weight_event_list(last_send = result);
        return last_send;
    }
protected:
    // Обработка фильтра
    virtual int32_t process(int32_t value)
    {
        return refresh(last_raw = value);
    }
public:
    // Конструктор по умолчанию
    weight_filter_converter_t(void)
        : tare(0), last_raw(0), last_send(0)
    { }

    // Калибровка в нуле
    void zero(void)
    {
        flash_write(&WEIGHT_CALIB.offset, &last_raw, sizeof(last_raw));
        refresh(last_raw);
    }
    
    // Калибровка в точке
    void point(int32_t weight)
    {
        int32_t f = (weight * ACCURACY) / (last_raw - WEIGHT_CALIB.offset);
        flash_write(&WEIGHT_CALIB.factor, &f, sizeof(f));
        refresh(last_raw);
    }
    
    // Задание тары
    void tare_set(void)
    {
        auto weight = calculate(last_raw);
        if (tare == weight)
            return;
        tare = weight;
        // Пересчет
        refresh(last_raw);
    }
} weight_filter_converter;

// Цепочка фильтров
static list_template_t<weight_filter_base_t> weight_filter_chain;

// Обработчик события получения нового значения АЦП
static adc_event_handler_t weight_adc_event([](int32_t raw)
{
    // На предупреждение не реагируем
    if (raw != ADC_VALUE_WATCHDOG)
        weight_filter_chain.head()->input(raw >> 4);
});

void weight_init(void)
{
    // Сбор цепочки фильтров
    weight_filter_median.link(weight_filter_chain);
    weight_filter_low_pass.link(weight_filter_chain);
    //weight_filter_threshold.link(weight_filter_chain);
    weight_filter_converter.link(weight_filter_chain);
    // Подписываемся на событе получения значения от АЦП
    adc_event_list.add(weight_adc_event);
}

void weight_calib_zero(void)
{
    weight_filter_converter.zero();
}

void weight_calib_point(int32_t value)
{
    weight_filter_converter.point(value);
}

void weight_tare(void)
{
    weight_filter_converter.tare_set();
}
