#include "io.h"
#include "key.h"
#include "nvic.h"
#include "timer.h"

// Список обработчиков события клавиш
key_event_handler_t::list_t key_event_list;

// Предварительное объявление
class key_data_t;

// Класс таймера для клавиши
class key_timer_t : timer_t
{
    // Хозяин
    key_data_t &owner;
protected:
    // Обработка тика таймера
    virtual void execute(void);
public:
    // Конструктор по умолчанию
    key_timer_t(key_data_t &_owner) : owner(_owner)
    { }
    
    // Запуск таймера
    void start(uint32_t ms)
    {
        start_us(ms * 1000);
    }
};

// Класс данных кнопки
class key_data_t
{
    friend class key_timer_t;
    
    // Вид кнопки
    key_kind_t kind;
    // Текущее состоние кнопки
    key_state_t state;
    // Таймер
    key_timer_t timer;
    // Шаг в нажатии кнопки
    uint32_t press_step;
    // Используемый порт вывода
    struct
    {
        // Порт
        const GPIO_TypeDef *port;
        // Индекс вывода
        uint8_t index;
    } io;
    
    // Получает состояние вывода
    key_state_t io_state_get(void) const
    {
        return IO_PORT_READ_MASK(io.port, IO_MASK(io.index)) ?
            KEY_STATE_RELEASED :
            KEY_STATE_PRESSED;
    }
    
    // Вызов обработчиков событий клавиш
    void call_event(void) const
    {
        key_event_args_t args =
        {
            kind,
            state,
            press_step,
        };
        key_event_list(args);
    }
    
    // Рестарт таймера на текущий шаг нажатия
    void press_step_delay(void)
    {
        assert(press_step > 0);
        // Расчет задержки
        uint32_t delay = 100;
        switch (press_step)
        {
            case 1:
                delay = 1000;
                break;
            case 2:
            case 3:
            case 4:
            case 5:
                delay = 500;
                break;
        }
        // Запуск таймера задержки
        timer.start(delay);
    }
    
    // Обработчик таймера
    void timer_execute(void)
    {
        // Обработка залипания
        if (press_step > 0)
        {
            press_step++;
            press_step_delay();
            // Передача события
            call_event();
            return;
        }
        // Реальная смена состояния
        auto state_new = io_state_get();
        if (state == state_new)
            return;
        state = state_new;
        // Если кнопки нажата - залипание
        if (state == KEY_STATE_PRESSED)
        {
            press_step = 1;
            press_step_delay();
        }
        // Передача события
        call_event();
    }
public:
    // Конструктор по умолчанию
    key_data_t(const GPIO_TypeDef *io_port, uint8_t io_index, key_kind_t _kind) : kind(_kind), timer(*this), press_step(0)
    {
        assert(io_port != NULL);
        assert(io_index < 16);
        // Инициализация
        io.port = io_port;
        io.index = io_index;
        state = io_state_get();
    }
    
    // Обработка кнопки по прерыванию
    void process(void)
    {
        if (!(EXTI->PR1 & IO_MASK(io.index)))                                   // Check interrupt pending flag
            return;
        EXTI->PR1 |= IO_MASK(io.index);                                         // Clear interrupt pending flag
        // Сброс шага
        press_step = 0;
        // Запуск таймера (на антидребезг 25 мС)
        timer.start(25);
    }
};

// Используемые кнопки
static key_data_t key_data[KEY_COUNT] =
{
    key_data_t(IO_BTN_KEY1_PORT, IO_BTN_KEY1, KEY_KIND_ENTER),
    key_data_t(IO_BTN_KEY2_PORT, IO_BTN_KEY2, KEY_KIND_UP),
    key_data_t(IO_BTN_KEY3_PORT, IO_BTN_KEY3, KEY_KIND_DOWN),
    key_data_t(IO_BTN_KEY4_PORT, IO_BTN_KEY4, KEY_KIND_BACK),
};

void key_timer_t::execute(void)
{
    owner.timer_execute();
}

void key_init(void)
{
    EXTI->RTSR1 |= EXTI_RTSR1_RT6 | EXTI_RTSR1_RT7 |                            // EXTI interrupt on rising edge
                   EXTI_RTSR1_RT8 | EXTI_RTSR1_RT9;
    EXTI->FTSR1 |= EXTI_FTSR1_FT6 | EXTI_FTSR1_FT7 |                            // EXTI interrupt on faiilng edge
                   EXTI_FTSR1_FT8 | EXTI_FTSR1_FT9;
    EXTI->IMR1 |= EXTI_IMR1_IM6 | EXTI_IMR1_IM7 |                               // EXTI interrupt unmask
                  EXTI_IMR1_IM8 | EXTI_IMR1_IM9;
    // Вклшючение прерывания
    nvic_irq_enable_set(EXTI9_5_IRQn, true);
}

IRQ_ROUTINE
void key_interrupt_exti(void)
{
    for (auto i = 0; i < KEY_COUNT; i++)
        key_data[i].process();
}
