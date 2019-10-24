// This file uses cp1251 encoding for russian C strings
#include "gui.h"
#include "gfx.h"
#include "lcd.h"
#include "list.h"
#include "font.h"
#include "image.h"
#include "timer.h"
#include "callback.h"

// Цвет темы
enum gui_color_t
{
    // Фон основной рабочей области
    GUI_COLOR_FORM_BK,
    // Заливка границы рабочей области
    GUI_COLOR_FORM_FRAME_FK,
    // Фон панелей на границах дисплея
    GUI_COLOR_EDGE_PANEL_BK,

    // Заливка надписи заголовка (про дозатор)
    GUI_COLOR_STAUS_HEADER_FK,
    // Заливка надписи имени активной формы
    GUI_COLOR_STAUS_FORM_NAME_FK,
    
    // Заливка кнопки панели действий
    GUI_COLOR_ACTION_BUTTON_FK,
    // Фон кнопки панели действий (по умолчанию)
    GUI_COLOR_ACTION_BUTTON_BK_NORMAL,
    // Фон кнопки панели действий (нажата)
    GUI_COLOR_ACTION_BUTTON_BK_PRESSED,
    // Фон кнопки панели действий (ввод)
    GUI_COLOR_ACTION_BUTTON_BK_ENTER,
    // Фон кнопки панели действий (назад)
    GUI_COLOR_ACTION_BUTTON_BK_BACK,

    // Заливка при фокусе (для разных элементов)
    GUI_COLOR_FOCUSED_FK,
    
    // Текст кнопки
    GUI_COLOR_BUTTON_TEXT_FK,
    // Граница кнопки
    GUI_COLOR_BUTTON_FRAME_FK,
    // Фон кнопки (по умолчанию)
    GUI_COLOR_BUTTON_BK_NORMAL,
    // Фон кнопки (в фокусе)
    GUI_COLOR_BUTTON_BK_FOCUSED,
    
    // Количество цветов
    GUI_COLOR_COUNT,
};

// Цветовая тема 
typedef lcd_color_t gui_theme_t[GUI_COLOR_COUNT];

#define GUI_THEME_COLOR_WHITE_ORANGE    LCD_COLOR_MAKE(209, 156, 0)

// Цветовые схемы
static const gui_theme_t GUI_THEMES[2] =
{
    // Default
    {
        // GUI_COLOR_FORM_BK
        LCD_COLOR_MAKE(175, 175, 175),
        // GUI_COLOR_FORM_FRAME_FK
        LCD_COLOR_MAKE(120, 120, 120),
        // GUI_COLOR_EDGE_PANEL_BK
        LCD_COLOR_MAKE(224, 224, 224),
        
        // GUI_COLOR_STAUS_HEADER_FK
        LCD_COLOR_MAKE(0, 0, 0),
        // GUI_COLOR_STAUS_FORM_NAME_FK
        LCD_COLOR_MAKE(110, 110, 110),
        
        // GUI_COLOR_ACTION_BUTTON_FK
        LCD_COLOR_MAKE(48, 48, 48),
        // GUI_COLOR_ACTION_BUTTON_BK_NORMAL
        LCD_COLOR_MAKE(128, 128, 128),
        // GUI_COLOR_ACTION_BUTTON_BK_PRESSED
        GUI_THEME_COLOR_WHITE_ORANGE,
        // GUI_COLOR_ACTION_BUTTON_BK_ENTER
        LCD_COLOR_MAKE(50, 147, 111),
        // GUI_COLOR_ACTION_BUTTON_BK_BACK
        LCD_COLOR_MAKE(232, 63, 111),
        
        // GUI_COLOR_FOCUSED_FK
        GUI_THEME_COLOR_WHITE_ORANGE,
        
        // GUI_COLOR_BUTTON_TEXT_FK,
        LCD_COLOR_MAKE(48, 48, 48),
        // GUI_COLOR_BUTTON_FRAME_FK,
        LCD_COLOR_MAKE(80, 80, 80),
        // GUI_COLOR_BUTTON_BK_NORMAL,
        LCD_COLOR_MAKE(34, 116, 165),
        // GUI_COLOR_BUTTON_BK_FOCUSED,
        LCD_COLOR_MAKE(0, 143, 226),
    },
};

// ПОлучает реальный цвет по номеру темы
static lcd_color_t gui_color_get(gui_color_t color)
{
    return GUI_THEMES[0][color];
}

// Предварительное объявление
class gui_visual_t;
class gui_container_t;
// Элемент оболочка для элемента в списках
typedef list_item_template_t<gui_visual_t> gui_visual_wrap_t;

// Класс базового визуального элемента
class gui_visual_t
{
    friend class gui_container_t;
    
    // Координаты
    uint16_t left, top;
    // Указатель на родительский контейнер
    gui_container_t *parent;
    // Элемент для списка дочерних элементов
    gui_visual_wrap_t child_item;
    // Флаг необходимости перерисовки
    bool paint_needed;
protected:
    // Конструктор по умолчанию
    gui_visual_t(void) : 
        paint_needed(true), parent(NULL), left(0), top(0), child_item(*this)
    { }
    
    // Получает указатель на родительский элемент TODO: под вопросом
    gui_container_t * parent_get(void) const
    {
        return parent;
    }

    // Обновление родительского элемента
    void parent_refresh(void);
    
    // Обработчик события вывода на экран
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background) = 0;
public:
    // Получает координату по X
    uint16_t left_get(void) const
    {
        return left;
    }
    
    // Получает координату по Y
    uint16_t top_get(void) const
    {
        return top;
    }
    
    // Задает координату по X
    void left_set(uint16_t value);

    // Задает координату по Y
    void top_set(uint16_t value);
    
    // Событие вывод на экран
    virtual void paint(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // Вывод текущего элемента
        if (!paint_needed)
            return;
        paint_handler(offsetX + left, offsetY + top, background);
        paint_needed = false;
    }
    
    // Обновление состояния элемента
    virtual void refresh()
    {
        paint_needed = true;
    }
    
    // Удаления текущего элемента из родительского контейнера
    void remove(void)
    {
        assert(parent != NULL);
        child_item.unlink();
        parent = NULL;
    }
};

// Класс базового контейнера элементов
class gui_container_t : public gui_visual_t
{
    // Список дочерних элементов
    list_template_t<gui_visual_wrap_t> childs;
public:
    // Событие вывод на экран
    virtual void paint(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // Базовый метод
        gui_visual_t::paint(offsetX, offsetY, background);
        // Перенос координат
        offsetX += left_get();
        offsetY += top_get();
        // Вывод дочерних
        for (auto i = childs.head(); i != NULL; i = LIST_ITEM_NEXT(i))
            i->holder.paint(offsetX, offsetY, background);
    }
    
    // Обновление состояния элемента
    virtual void refresh()
    {
        // Базовый метод
        paint_needed = true;
        // Вывод дочерних
        for (auto i = childs.head(); i != NULL; i = LIST_ITEM_NEXT(i))
            i->holder.refresh();
    }
    
    // Добавление дочернего элемента
    void add(gui_visual_t &child)
    {
        assert(child.parent == NULL);
        child.child_item.link(childs);
        child.parent = this;
    }
};

void gui_visual_t::parent_refresh(void)
{
    if (parent != NULL)
        parent->refresh();
    else
        refresh();
}

void gui_visual_t::left_set(uint16_t value)
{
    if (left == value)
        return;
    left = value;
    parent_refresh();
}

void gui_visual_t::top_set(uint16_t value)
{
    if (top == value)
        return;
    top = value;
    parent_refresh();
}

// Класс базовой панели на границах дисплея
class gui_edge_panel_t : protected gui_container_t
{
protected:
    // Локальные константы
    enum
    {
        // Ширина панели
        WIDTH = LCD_SIZE_WIDTH,
    };
    
    // Получает высоту панели
    virtual uint8_t height_get(void) const = 0;

    // Обработчик события вывода на экран
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
    {
        gfx_rect_solid(x, y, WIDTH, height_get(), gui_color_get(background));
    }
public:
    // Вывод на экран
    void repaint(void)
    {
        gui_container_t::paint(0, 0, GUI_COLOR_EDGE_PANEL_BK);
    }
    
    // Запрос на перерисовку
    void refresh(void)
    {
        gui_container_t::refresh();
    }
};

// Панель действий
static class gui_action_panel_t : public gui_edge_panel_t
{
    // Кнопки
    class button_t : public gui_visual_t
    {
        // Указатель на данные изображения
        uint8_t const * const image;
        // Цвет закраски
        const gui_color_t background;
        // Флаг состояния нажатия
        bool pressed;
    protected:    
        // Обработчик события вывода на экран
        virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t pbackground)
        {
            // Определяем цвет фона
            auto bk = gui_color_get(pressed ? GUI_COLOR_ACTION_BUTTON_BK_PRESSED  : background);
            
            // Вывод скруглденного фрейма
            gfx_rect_frame_round(x, y, 40, 24, bk, gui_color_get(pbackground));
            // Заливка для расширения кнопки по ширине
            gfx_rect_solid(x + 0, y, 8, 24, bk);
            gfx_rect_solid(x + 32, y, 8, 24, bk);
            // Иконка
            gfx_image(image, x + 8, y, gui_color_get(GUI_COLOR_ACTION_BUTTON_FK),  bk);
        }
    public:
        // Конструктор по умолчанию
        button_t(uint8_t const * _image, gui_color_t _background) 
            : image(_image), background(_background), pressed(false)
        { }
        
        // Установка состояния нажатия
        void pressed_set(bool value)
        {
            if (pressed == value)
                return;
            pressed = value;
            refresh();
        }
    } enter, up, down, back;

    // Получает ссылку на батон по клавише
    button_t & button_from_key(key_kind_t key) 
    {
        switch (key)
        {
            case KEY_KIND_ENTER:
                return enter;
            case KEY_KIND_UP:
                return up;
            case KEY_KIND_DOWN:
                return down;
            case KEY_KIND_BACK:
                return back;
            default:
                assert(false);
        }
        // Dummy
        return back;
    }
protected:
    // Получает высоту панели
    uint8_t height_get(void) const
    {
        return HEIGHT;
    }
public:
    // Локальные константы
    enum
    {
        // Высота панели
        HEIGHT = 36,
    };

    // Конструктор по умолчанию
    gui_action_panel_t(void) : 
        enter(IMAGE_BUTTON_ENTER, GUI_COLOR_ACTION_BUTTON_BK_ENTER),
        up(IMAGE_BUTTON_UP, GUI_COLOR_ACTION_BUTTON_BK_NORMAL),
        down(IMAGE_BUTTON_DOWN, GUI_COLOR_ACTION_BUTTON_BK_NORMAL),
        back(IMAGE_BUTTON_BACK, GUI_COLOR_ACTION_BUTTON_BK_BACK)
    {
        // Позиционирование панели
        top_set(LCD_SIZE_HEIGHT - height_get());
        // Позиционирование кнопок
            // X
        enter.left_set(10); 
        up.left_set(70);    
        down.left_set(130);  
        back.left_set(190);
            // Y
        enter.top_set(6);
        up.top_set(6);
        down.top_set(6);
        back.top_set(6);
        // Добавление кнопок в дочерние
        add(enter);
        add(up);
        add(down);
        add(back);
    }
    
    // Событие смены состояния клавиши
    void key_event(key_kind_t key, key_state_t state)
    {
        auto &button = button_from_key(key);
        switch (state)
        {
            case KEY_STATE_RELEASED:
                button.pressed_set(false);
                break;
            case KEY_STATE_PRESSED:
                button.pressed_set(true);
                break;
            default:
                assert(false);
        }
    }
} gui_action_panel;


// Панель статуса
static class gui_status_panel_t : public gui_edge_panel_t
{
    // Класс надписи имени активной формы
    class form_name_t : public gui_visual_t
    {
        // Выводимый текст
        const char *text;
    protected:
        // Обработчик события вывода на экран
        virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
        {
            // Ширина текста
            auto width = gfx_string_measure(text, FONT_HEADER_BOLD);
            // Вывод текста
            gfx_string(text, FONT_HEADER_BOLD, x - width - 3, y + 1, 
                gui_color_get(GUI_COLOR_STAUS_FORM_NAME_FK), 
                gui_color_get(background));
        }
    public:
        // Конструктор по умолчанию
        form_name_t(void) : text(NULL)
        {
            // Позиционирование
            left_set(WIDTH);
        }
        
        // Установка нового текста
        void text_set(const char *value)
        {
            // Сравниваются указатели, всё верно
            if (text == value)
                return;
            assert(value != NULL);
            text = value;
            refresh();
        }
    } form_name;
protected:
    // Получает высоту панели
    uint8_t height_get(void) const
    {
        return HEIGHT;
    }

    // Обработчик события вывода на экран
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
    {
        auto bk = gui_color_get(background);
        auto fk = gui_color_get(GUI_COLOR_STAUS_HEADER_FK);
        // Базовый метод
        gui_edge_panel_t::paint_handler(x, y, background);
        // Иконка
        gfx_image(IMAGE_STATUS_ICON, x + 1, y, fk, bk);
        // Заголовок приложения
        gfx_string("ДОЗАТОР", FONT_HEADER_BOLD, x + HEIGHT + 3, y + 1, fk, bk);
    }
public:
    // Конструктор по умолчанию
    gui_status_panel_t(void)
    {
        add(form_name);
    }

    // Локальные константы
    enum
    {
        // Высота панели
        HEIGHT = 24,
    };
    
    // Установка текста имени текущей формы
    void form_name_set(const char *value)
    {
        form_name.text_set(value);
        refresh();
    }
} gui_status_panel;

// Предварительное объявление
class gui_control_t;
// Оболочка списочного элемента для элемента управления
typedef list_item_template_t<gui_control_t> gui_control_wrap_t;

// Класс элемента управления принимающего фокус
class gui_control_t : public gui_visual_t
{
    friend class gui_form_t;
    
    // Соседние элементы
    gui_control_wrap_t neighbors;
    // Состояние фокуса
    bool focused;
    
    // Устанавливает состояние фокуса
    void focused_set(bool state)
    {
        if (focused == state)
            return;
        focused = state;
        refresh();
    }
protected:
    // Конструктор по умолчанию
    gui_control_t(void) : neighbors(*this), focused(false)
    { }

    // Получает состояние фокуса
    bool focused_get(void) const
    {
        return focused;
    }
    
    // Обработчик события клавиши
    virtual bool key_event(key_kind_t key) = 0;
public:
    // Соединение двух элементов для фокуса
    void connect(gui_control_t &to)
    {
        neighbors.link(to.neighbors);
    }
};

// Базовый класс элемента управления, имеющего размеры
class gui_control_sizeable_t : public gui_control_t
{
    // Высота и ширина
    uint16_t width, height;
    
protected:
    // Конструктор по умолчанию
    gui_control_sizeable_t(void) : width(0), height(0)
    { }

    // Получает ширину
    uint16_t width_get(void) const
    {
        return width;
    }
    
    // Получает высоту
    uint16_t height_get(void) const
    {
        return height;
    }
public:
    // Задает ширину
    void width_set(uint16_t value)
    {
        if (width == value)
            return;
        width = value;
        parent_refresh();
    }
    
    // Задает высоту
    void height_set(uint16_t value)
    {
        if (height == value)
            return;
        height = value;
        parent_refresh();
    }
};

// Предварительное объявление
class gui_form_t;
// Элемент оболочка для формы в списке стэка
typedef list_item_template_t<gui_form_t> gui_form_wrap_t;

// Класс формы пользовательского интерфейса
class gui_form_t : protected gui_container_t
{
    friend class gui_form_stack_t;
    
    // Локальные константы
    enum
    {
        // Высота панели
        HEIGHT = LCD_SIZE_HEIGHT - gui_status_panel_t::HEIGHT - gui_action_panel_t::HEIGHT,
        // Ширина панели
        WIDTH = LCD_SIZE_WIDTH,
    };
    
    // Имя формы
    char const * const name;
    // Элемент стэка форм
    gui_form_wrap_t stack_item;
    // Фокусный элемент
    gui_control_t *focused;
    
    // Вывод на экран
    void repaint(void)
    {
        gui_container_t::paint(0, 0, GUI_COLOR_FORM_BK);
    }
    
    // Запрос на перерисовку
    void refresh(void)
    {
        gui_container_t::refresh();
    }
    
    // Обработчик события клавиши
    bool key_event(key_kind_t key)
    {
        if (focused == NULL)
            return false;
        if (focused->key_event(key))
            return true;
        // Событие не обработано
        switch (key)
        {
            case KEY_KIND_UP:
                // Фокус вверх
                {
                    auto next = (gui_control_wrap_t *)focused->neighbors.next();
                    if (next != NULL)
                    {
                        focused_set(&next->holder);
                        return true;
                    }
                }
                break;
            case KEY_KIND_DOWN:
                // Фокус вниз
                {
                    auto next = (gui_control_wrap_t *)focused->neighbors.prev();
                    if (next != NULL)
                    {
                        focused_set(&next->holder);
                        return true;
                    }
                }
                break;
        }
        return false;
    }
protected:
    // Обработчик события вывода на экран
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
    {
        gfx_rect_solid(x + 1, y + 1, WIDTH - 2, HEIGHT - 2, gui_color_get(background));
        gfx_rect_frame(x, y, WIDTH, HEIGHT, gui_color_get(GUI_COLOR_FORM_FRAME_FK));
    }
    
    // Установка фокуса новому элементу
    void focused_set(gui_control_t *control)
    {
        // Сравниваются указатели, всё верно
        if (focused == control)
            return;
        // Снятие
        if (focused != NULL)
            focused->focused_set(false);
        focused = control;
        // Установка
        if (focused != NULL)
            focused->focused_set(true);
    }
public:
    // Конструктор по умолчанию
    gui_form_t(char const * _name) 
        : name(_name), stack_item(*this), focused(NULL)
    {
        assert(name != NULL);
        // Позиционирование
        top_set(gui_status_panel_t::HEIGHT);
    }
};

// Стэк активных форм
static class gui_form_stack_t
{
    // Список активных форм
    list_template_t<gui_form_wrap_t> forms;
    
    // Событие смены активной формы
    void active_changed(void)
    {
        auto active = forms.last();
        assert(active != NULL);
        // Запрос на перерисовку
        active->holder.refresh();
        // Установка имени формы
        gui_status_panel.form_name_set(active->holder.name);
    }
public:
    // Событие смены состояния клавиши
    void key_event(key_kind_t key, key_state_t state)
    {
        // Обрабатываем только нажатия
        if (state != KEY_STATE_PRESSED)
            return;
        auto form = forms.last();
        assert(form != NULL);
        // Передача события форме
        if (form->holder.key_event(key))
            return;
        // Обработка перехода не прендыдущую форму
        if (key != KEY_KIND_BACK || forms.count() <= 1)
            return;
        // Удаляем текущую форму
        form->unlink();
        // Событие смены
        active_changed();
    }
    
    // Вывод на экран
    void repaint(void)
    {
        auto active = forms.last();
        assert(active != NULL);
        active->holder.repaint();
    }
    
    // Добавление формы (станет активной)
    void add(gui_form_t &form)
    {
        // Добавление в конец
        form.stack_item.link(forms);
        // Событие смены
        active_changed();
    }
} gui_form_stack;

// Класс метки вывода знакового числа
class gui_label_int_t : public gui_control_sizeable_t
{
    // Выводимое число
    int32_t number;
protected:
    // Обработчик события вывода на экран
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
    {
        auto bk = gui_color_get(background);
        // Фон
        gfx_rect_solid(x, y, width_get(), height_get(), bk);
        // Текст
        char buffer[16];
        if (number > 0)
            sprintf(buffer, "+%d", number);
        else
            sprintf(buffer, "%d", number);
        auto font = FONT_NUMBER_BOLD;
        auto width = gfx_string_measure(buffer, font);
        gfx_string(buffer, font, x + width_get() / 2 - width / 2, y + height_get() / 2 - font_height_get(font) / 2, gui_color_get(GUI_COLOR_BUTTON_TEXT_FK), bk);
    }
    
    // Обработчик события клавиши
    virtual bool key_event(key_kind_t key)
    {
        return false;
    }
public:
    // Конструктор по умолчанию
    gui_label_int_t(void) : number(0)
    {
        width_set(LCD_SIZE_WIDTH);
        height_set(font_height_get(FONT_NUMBER_BOLD));
    }
    
    // Установка значения выводимого числа
    void number_set(int32_t value)
    {
        if (number == value)
            return;
        number = value;
        refresh();
    }
};

// Базовый класс кнопки
class gui_button_base_t : public gui_control_sizeable_t
{
    // Эффект вспышки при клике
    uint8_t flash;
    // Текст кнопки
    char const * const text;
protected:
    // Конструктор по умолчанию
    gui_button_base_t(const char * _text) : text(_text), flash(0)
    {
        assert(text != NULL);
        // Позиционирование по умолчанию
        left_set(36);
        top_set(36);
        width_set(168);
        height_set(76);
    }
    
    // Обработчик события вывода на экран
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
    {
        // Цвет фона
        auto bk = gui_color_get(flash > 0 ? 
            GUI_COLOR_FOCUSED_FK : 
            (focused_get() ? 
                GUI_COLOR_BUTTON_BK_FOCUSED : 
                GUI_COLOR_BUTTON_BK_NORMAL));
        // Цвет границы
        auto fk = gui_color_get(focused_get() ? 
            GUI_COLOR_FOCUSED_FK : 
            GUI_COLOR_BUTTON_FRAME_FK);
        // Граница
        gfx_rect_frame_round(x, y, width_get(), height_get(), fk, gui_color_get(background));
        // Фон
        gfx_rect_solid(x, y, width_get(), height_get(), bk);
        // Текст
        auto font = FONT_BUTTON_BOLD;
        auto width = gfx_string_measure(text, font);
        gfx_string(text, 
            font, x + width_get() / 2 - width / 2, y + height_get() / 2 - font_height_get(font) / 2, 
            gui_color_get(GUI_COLOR_BUTTON_TEXT_FK), bk);
    }
    
    // Обработчик события клавиши
    virtual bool key_event(key_kind_t key)
    {
        if (key != KEY_KIND_ENTER)
            return false;
        // Вспышка
        flash = 5;
        refresh();
        return true;
    }
    
    // Событие клика
    virtual void click_event(void) = 0;
public:
    // Событие вывод на экран
    virtual void paint(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // Обработка вспышки
        if (flash > 0 && --flash == 0)
        {
            refresh();
            click_event();
        }
        // Базовый метод
        gui_control_sizeable_t::paint(offsetX, offsetY, background);
    }
};

// Класс реализации кнопки по умолчанию
class gui_button_t : public gui_button_base_t
{
    // Обработчик клика
    callback_proc_ptr handler;
    
protected:
    // Обработчик событие клика
    virtual void click_event(void)
    {
        handler();
    }
public:
    // Конструктор по умолчанию
    gui_button_t(const char * _text, callback_proc_ptr _handler) 
        : gui_button_base_t(_text), handler(_handler)
    {
        assert(handler != NULL);
    }
};

// Главная форма
static class gui_form_main_t : public gui_form_t
{
    // Кнопка входа в дозатор
    gui_button_t dispencer;
    // Кнопка входа в калибровку
    gui_button_t calibration;
    
    // Вход в дозатор
    static void dispencer_enter(void);
    // Вход в отсыпатель
    static void dummy(void)
    { }
public:
    // Конструктор по умолчанию
    gui_form_main_t(void) : gui_form_t("ГЛАВНАЯ"), 
       dispencer("НАСЫПАТЬ", dispencer_enter),
       calibration("КАЛИБРОВКА", dummy)
    {
        // Добавление кнопок
        add(dispencer);
        add(calibration);
        // Позиционирование
        calibration.top_set(148);
        // Фоккус
        dispencer.connect(calibration);
        focused_set(&dispencer);
    }
} gui_form_main;

// Форма дозатора
static class gui_form_dispencer_t : public gui_form_t
{
public:
    gui_label_int_t label;
    // Конструктор по умолчанию
    gui_form_dispencer_t(void) : gui_form_t("НАСЫПАТЬ")
    {
        label.top_set(30);
        add(label);
    }
} gui_form_dispencer;

// TODO: test!

#include "adc.h"

static timer_callback_t adc_timer([](void)
{
    gui_form_dispencer.label.number_set(adc_read() / 100);
});

// TODO: /test!

// Вход в дозатор
void gui_form_main_t::dispencer_enter(void)
{ 
    gui_form_stack.add(gui_form_dispencer);
}

// Таймер перерисовки интерфейса
static timer_callback_t gui_paint_timer([](void)
{
    gui_action_panel.repaint();
    gui_status_panel.repaint();
    gui_form_stack.repaint();
});

void gui_init(void)
{
    adc_timer.start_us(100, TIMER_FLAG_LOOP);
    // Добавляем главную форму
    gui_form_stack.add(gui_form_main);
    // 25 кадров в секунду
    gui_paint_timer.start_hz(25, TIMER_FLAG_LOOP);
}

void gui_key_event(key_kind_t key, key_state_t state)
{
    gui_form_stack.key_event(key, state);
    gui_action_panel.key_event(key, state);
}
