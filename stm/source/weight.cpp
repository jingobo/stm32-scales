#include "weight.h"

// Список обработчиков изменения веса
weight_event_handler_t::list_t weight_event_list;

// Прескалер сырого значения
#define WEIGHT_RAW_PRESCALER        100
// Размер скользящего окна усриднения
#define WEIGHT_RAW_WINDOW_SIZE      50
// Множитель отражающий точность рассчета при конвертировании
#define WEIGHT_CONVERT_ACCURACY     100

// Фильтр сырого значения
static struct weight_filter_raw_t
{
    // Указатель на текущее записываемое значение
    uint32_t pointer;
    // Сумма всех элементов окна
    int32_t summ;
    // Предыдущее фильтрованное значение
    int32_t last;
    // Скользящее окно усреднения
    int32_t window[WEIGHT_RAW_WINDOW_SIZE];
    
public:
    // Конструктор по умолчанию
    weight_filter_raw_t(void) 
        : pointer(0), summ(0), last(ADC_VALUE_WATCHDOG)
    {
        memset(window, 0, sizeof(window));
    }
    
    // Ввод значения
    bool input(int32_t raw)
    {
        assert(raw != ADC_VALUE_WATCHDOG);
        
        // Усредление
        raw /= WEIGHT_RAW_PRESCALER;
        
        // Обработка скользящего окна
        summ -= window[pointer];
        window[pointer] = raw;
        summ += raw;
        
        // Смещение
        if (++pointer >= WEIGHT_RAW_WINDOW_SIZE)
            pointer = 0;
        
        // Текущее значение
        auto current = summ / WEIGHT_RAW_WINDOW_SIZE;
        
        // Фильтрация по изменению значения
        if (last == current)
            return true;
        last = current;
        return false;
    }
    
    // Получает фильтрованное значение
    int32_t value_get(void) const
    {
        return last;
    }
} weight_filter_raw;

// Конвертер веса из сырого значения
static class weight_converter_t
{
    // Смещение нуля
    int32_t offset;
    // Пропорциональность
    int32_t factor;
    // Тара
    int32_t tare;
    
    // Рассчет веса без тары
    int32_t calculate(int32_t raw) const
    {
        return (raw - offset) * factor / WEIGHT_CONVERT_ACCURACY;
    }
public:
    weight_converter_t(void) 
        : offset(10000), factor((int32_t)(1.2 * WEIGHT_CONVERT_ACCURACY)), tare(0)
    { }

    // Конвертирование
    void convert(int32_t raw) const
    {
        weight_event_list(calculate(raw) - tare);
    }
    
    // Калибровка в нуле
    void zero(void)
    {
        auto raw = weight_filter_raw.value_get();
        offset = raw;
        convert(raw);
    }
    
    // Калибровка в точке
    void point(int32_t weight)
    {
        auto raw = weight_filter_raw.value_get();
        factor =  (weight * WEIGHT_CONVERT_ACCURACY) / (raw - offset);
        convert(raw);
    }
    
    // Задание тары
    void tare_set(void)
    {
        auto raw = weight_filter_raw.value_get();
        auto weight = calculate(raw);
        if (tare == weight)
            return;
        tare = weight;
        // Пересчет
        convert(raw);
    }
} weight_converter;

// Обработчик получения сырого значения АЦП
static void weight_raw_input(int32_t raw)
{
    // На предупреждение не реагируем
    if (raw == ADC_VALUE_WATCHDOG)
        return;
    
    // Начальный фильтр
    if (weight_filter_raw.input(raw))
        return;
    
    // Конвертирование
    weight_converter.convert(weight_filter_raw.value_get());
}

// Обработчик события получения нового значения АЦП
static adc_event_handler_t weight_adc_event(weight_raw_input);

void weight_init(void)
{
    // Подписываемся на событе получения значения от АЦП
    adc_event_list.add(weight_adc_event);
}

void weight_calib_zero(void)
{
    weight_converter.zero();
}

void weight_calib_point(int32_t value)
{
    weight_converter.point(value);
}

void weight_tare(void)
{
    weight_converter.tare_set();
}
