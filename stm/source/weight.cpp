#include "list.h"
#include "flash.h"
#include "weight.h"

// Локальные настройки модуля
static const struct
{
    // Смещение нуля
    int32_t offset;
    // Пропорциональность
    int32_t factor;
} WEIGHT_CALIB @ FLASH_RW_SECTION = { 97543, 125 };

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
        value = process(value);
        // Передача следующему фильтру
        auto nf = (weight_filter_base_t *)next();
        if (nf != NULL)
            nf->input(value);
    }
};

// Фильтр оповещения о смене веса
static class weight_filter_notify_t : public weight_filter_base_t
{
protected:
    // Обработка фильтра
    virtual int32_t process(int32_t value)
    {
        weight_event_list(value);
        return value;
    }
} weight_filter_notify;

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
        KMAX = 100,
        // Коофициент передачи
        K = 30,
    };
    // Предыдущее значение фильтра
    int32_t last;
protected:
    // Обработка фильтра
    virtual int32_t process(int32_t value)
    {
        return last = last * (KMAX - K) / KMAX + value * K / KMAX;
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
        // Порог
        THRESHOLD = 5,
    };
    
    // Переданное значение дальше
    int32_t sended;
    // Акамулятор усреднения
    int32_t accum;
    // Количество усреднений
    int32_t count;
    // Флаг сброса
    bool reseting;
    
    // Сброс акамулятора усреднения
    void accum_clear(bool res)
    {
        accum = count = 0;
        reseting = res;
    }
protected:
    // Обработка фильтра
    virtual int32_t process(int32_t value)
    {
        // Усреднение
        count++;
        int64_t temp = accum;
        if ((temp += value) >= INT32_MAX)
        {
            temp /= count;
            count = 1;
        }
        accum = (int32_t)temp;
        // Проверка порога
        if (reseting || abs(value - sended) > THRESHOLD)
        {
            sended = accum / count;
            accum_clear(false);
        }
        return sended;
    }
public:
    // Конструктор по умолчанию
    weight_filter_threshold_t(void)
    {
        reset();
    }
    
    // Сброс фильтра
    void reset(void)
    {
        accum_clear(true);
    }
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
    
    // Рассчет веса без тары
    int32_t calculate(int32_t raw) const
    {
        return (raw - WEIGHT_CALIB.offset) * WEIGHT_CALIB.factor / ACCURACY;
    }
    
    // Обновление веса
    void refresh(void)
    {
        input(last_raw);
    }
protected:
    // Обработка фильтра
    virtual int32_t process(int32_t value)
    {
        return calculate(last_raw = value) - tare;
    }
public:
    // Конструктор по умолчанию
    weight_filter_converter_t(void) : tare(0), last_raw(0)
    { }

    // Калибровка в нуле
    void zero(void)
    {
        flash_write(&WEIGHT_CALIB.offset, &last_raw, sizeof(last_raw));
        refresh();
    }
    
    // Калибровка в точке
    void point(int32_t weight)
    {
        int32_t f = (weight * ACCURACY) / (last_raw - WEIGHT_CALIB.offset);
        flash_write(&WEIGHT_CALIB.factor, &f, sizeof(f));
        refresh();
    }
    
    // Задание тары
    void tare_set(void)
    {
        auto weight = calculate(last_raw);
        if (tare == weight)
            return;
        tare = weight;
        refresh();
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
    weight_filter_converter.link(weight_filter_chain);
    weight_filter_threshold.link(weight_filter_chain);
    // Обязательно последним в цепочке
    weight_filter_notify.link(weight_filter_chain);
    // Подписываемся на событе получения значения от АЦП
    adc_event_list.add(weight_adc_event);
}

void weight_calib_zero(void)
{
    weight_filter_threshold.reset();
    weight_filter_converter.zero();
}

void weight_calib_point(int32_t value)
{
    weight_filter_threshold.reset();
    weight_filter_converter.point(value);
}

void weight_tare(void)
{
    weight_filter_threshold.reset();
    weight_filter_converter.tare_set();
}
