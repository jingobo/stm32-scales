// This file uses cp1251 encoding for russian C strings
#include "gui.h"
#include "gfx.h"
#include "key.h"
#include "pwm.h"
#include "list.h"
#include "font.h"
#include "flash.h"
#include "image.h"
#include "timer.h"
#include "weight.h"

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
    
    // Текст надписи
    GUI_COLOR_LABEL_TEXT_FK,
    
    // Текст кнопки
    GUI_COLOR_BUTTON_TEXT_FK,
    // Граница кнопки
    GUI_COLOR_BUTTON_FRAME_FK,
    // Фон кнопки (по умолчанию)
    GUI_COLOR_BUTTON_BK_NORMAL,
    // Фон кнопки (в фокусе)
    GUI_COLOR_BUTTON_BK_FOCUSED,
    
    // Заливка границы рамки
    GUI_COLOR_BEVEL_FK,
    // Фон рамки
    GUI_COLOR_BEVEL_BK,
    
    // Количество цветов
    GUI_COLOR_COUNT,
};

// Цветовая тема 
typedef lcd_color_t gui_theme_t[GUI_COLOR_COUNT];

// Перечисление видов тем
enum gui_theme_kind_t
{
    // Светлая
    GUI_THEME_KIND_LIGHT,
    // Темная
    GUI_THEME_KIND_DARK,
};

// Количество видов тем
#define GUI_THEME_KIND_COUNT    ((int)GUI_THEME_KIND_DARK + 1)

// Локальные настройки модуля
static const struct
{
    gui_theme_kind_t theme;
} GUI_SETTINGS @ FLASH_RW_SECTION = { GUI_THEME_KIND_LIGHT };

// --- Светлая тема --- //

#define GUI_THEME_COLOR_LIGHT_SELECT        LCD_COLOR_MAKE(209, 156, 0)
#define GUI_THEME_COLOR_LIGHT_TEXTFK        LCD_COLOR_MAKE(48, 48, 48)

// --- Темная тема --- //

#define GUI_THEME_COLOR_DARK_SELECT         LCD_COLOR_MAKE(238, 10, 246)
#define GUI_THEME_COLOR_DARK_TEXTFK         LCD_COLOR_MAKE(200, 200, 200)

// Цветовые схемы
static const gui_theme_t GUI_THEMES[GUI_THEME_KIND_COUNT] =
{
    // Light
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
        GUI_THEME_COLOR_LIGHT_TEXTFK,
        // GUI_COLOR_ACTION_BUTTON_BK_NORMAL
        LCD_COLOR_MAKE(128, 128, 128),
        // GUI_COLOR_ACTION_BUTTON_BK_PRESSED
        GUI_THEME_COLOR_LIGHT_SELECT,
        // GUI_COLOR_ACTION_BUTTON_BK_ENTER
        LCD_COLOR_MAKE(50, 147, 111),
        // GUI_COLOR_ACTION_BUTTON_BK_BACK
        LCD_COLOR_MAKE(232, 63, 111),
        
        // GUI_COLOR_FOCUSED_FK
        GUI_THEME_COLOR_LIGHT_SELECT,
        
        // GUI_COLOR_LABEL_TEXT_FK
        GUI_THEME_COLOR_LIGHT_TEXTFK,
        
        // GUI_COLOR_BUTTON_TEXT_FK,
        GUI_THEME_COLOR_LIGHT_TEXTFK,
        // GUI_COLOR_BUTTON_FRAME_FK,
        LCD_COLOR_MAKE(80, 80, 80),
        // GUI_COLOR_BUTTON_BK_NORMAL,
        LCD_COLOR_MAKE(34, 116, 165),
        // GUI_COLOR_BUTTON_BK_FOCUSED,
        LCD_COLOR_MAKE(0, 143, 226),

        // GUI_COLOR_BEVEL_FK,
        LCD_COLOR_MAKE(140, 140, 140),
        // GUI_COLOR_BEVEL_BK,
        LCD_COLOR_MAKE(150, 150, 150),
    },
    
    // Dark
    {
        // GUI_COLOR_FORM_BK
        LCD_COLOR_MAKE(10, 10, 10),
        // GUI_COLOR_FORM_FRAME_FK
        LCD_COLOR_MAKE(60, 60, 60),
        // GUI_COLOR_EDGE_PANEL_BK
        LCD_COLOR_MAKE(30, 30, 30),
        
        // GUI_COLOR_STAUS_HEADER_FK
        LCD_COLOR_MAKE(180, 106, 0),
        // GUI_COLOR_STAUS_FORM_NAME_FK
        LCD_COLOR_MAKE(180, 180, 180),
        
        // GUI_COLOR_ACTION_BUTTON_FK
        GUI_THEME_COLOR_DARK_TEXTFK,
        // GUI_COLOR_ACTION_BUTTON_BK_NORMAL
        LCD_COLOR_MAKE(70, 70, 100),
        // GUI_COLOR_ACTION_BUTTON_BK_PRESSED
        GUI_THEME_COLOR_DARK_SELECT,
        // GUI_COLOR_ACTION_BUTTON_BK_ENTER
        LCD_COLOR_MAKE(10, 130, 10),
        // GUI_COLOR_ACTION_BUTTON_BK_BACK
        LCD_COLOR_MAKE(255, 10, 10),
        
        // GUI_COLOR_FOCUSED_FK
        GUI_THEME_COLOR_DARK_SELECT,
        
        // GUI_COLOR_LABEL_TEXT_FK
        GUI_THEME_COLOR_DARK_TEXTFK,
        
        // GUI_COLOR_BUTTON_TEXT_FK,
        GUI_THEME_COLOR_DARK_TEXTFK,
        // GUI_COLOR_BUTTON_FRAME_FK,
        LCD_COLOR_MAKE(80, 80, 80),
        // GUI_COLOR_BUTTON_BK_NORMAL,
        LCD_COLOR_MAKE(40, 40, 40),
        // GUI_COLOR_BUTTON_BK_FOCUSED,
        LCD_COLOR_MAKE(70, 70, 70),

        // GUI_COLOR_BEVEL_FK,
        LCD_COLOR_MAKE(0, 0x3F, 0x6E),
        // GUI_COLOR_BEVEL_BK,
        LCD_COLOR_MAKE(0, 0x1F, 0x4E),
    },
};

// ПОлучает реальный цвет по номеру темы
static lcd_color_t gui_color_get(gui_color_t color)
{
    return GUI_THEMES[GUI_SETTINGS.theme][color];
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
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background) = 0;
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
    
    // Задает положение
    void location_set(uint16_t left, uint16_t top)
    {
        left_set(left);
        top_set(top);
    }
    
    // Событие вывод на экран
    virtual void paint(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // Вывод текущего элемента
        if (!paint_needed)
            return;
        paint_self(offsetX + left, offsetY + top, background);
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

// Базовый класс визуального элемента с размерами
class gui_visual_sizeable_t : public gui_visual_t
{
    // Высота и ширина
    uint16_t width, height;
protected:
    // Конструктор по умолчанию
    gui_visual_sizeable_t(void) : width(0), height(0)
    { }
public:
    // Получает ширину
    uint16_t width_get(void) const
    {
        return width;
    }
    
    // Задает ширину
    void width_set(uint16_t value)
    {
        if (width == value)
            return;
        width = value;
        parent_refresh();
    }
    
    // Получает высоту
    uint16_t height_get(void) const
    {
        return height;
    }
    
    // Задает высоту
    void height_set(uint16_t value)
    {
        if (height == value)
            return;
        height = value;
        parent_refresh();
    }
    
    // Задает положение и размер
    void bounds_set(uint16_t left, uint16_t top, uint16_t width, uint16_t height)
    {
        location_set(left, top);
        width_set(width);
        height_set(height);
    }
};

// Класс базового контейнера элементов
class gui_container_t : public gui_visual_sizeable_t
{
    // Список дочерних элементов
    list_template_t<gui_visual_wrap_t> childs;
protected:
    // Конструктор по умолчанию
    gui_container_t(void)
    {
        width_set(LCD_SIZE_WIDTH);
    }
    
    // Вывод дочерних элементов
    virtual void paint_childs(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // Перенос
        offsetX += left_get();
        offsetY += top_get();
        // Вывод
        for (auto i = childs.head(); i != NULL; i = LIST_ITEM_NEXT(i))
            i->holder.paint(offsetX, offsetY, background);
    }
public:
    // Событие вывод на экран
    virtual void paint(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // Базовый метод
        gui_visual_sizeable_t::paint(offsetX, offsetY, background);
        // Вывод дочерних элементов
        paint_childs(offsetX, offsetY, background);
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
class gui_edge_panel_t : public gui_container_t
{
protected:
    // Обработчик события вывода на экран
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        gfx_rect_solid(x, y, width_get(), height_get(), gui_color_get(background));
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
        virtual void paint_self(uint16_t x, uint16_t y, gui_color_t pbackground)
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
public:
    // Конструктор по умолчанию
    gui_action_panel_t(void) : 
        enter(IMAGE_BUTTON_ENTER, GUI_COLOR_ACTION_BUTTON_BK_ENTER),
        up(IMAGE_BUTTON_UP, GUI_COLOR_ACTION_BUTTON_BK_NORMAL),
        down(IMAGE_BUTTON_DOWN, GUI_COLOR_ACTION_BUTTON_BK_NORMAL),
        back(IMAGE_BUTTON_BACK, GUI_COLOR_ACTION_BUTTON_BK_BACK)
    {
        // Позиционирование панели
        height_set(36);
        top_set(LCD_SIZE_HEIGHT - height_get());
        // Позиционирование кнопок
        enter.location_set(10, 6);
        up.location_set(70, 6);
        down.location_set(130, 6);
        back.location_set(190, 6);
        // Добавление кнопок в дочерние
        add(enter);
        add(up);
        add(down);
        add(back);
    }
    
    // Событие смены состояния клавиши
    void key_event(const key_event_args_t &key)
    {
        auto &button = button_from_key(key.kind);
        switch (key.state)
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
        virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
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
            left_set(LCD_SIZE_WIDTH);
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
    // Обработчик события вывода на экран
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        auto bk = gui_color_get(background);
        auto fk = gui_color_get(GUI_COLOR_STAUS_HEADER_FK);
        // Базовый метод
        gui_edge_panel_t::paint_self(x, y, background);
        // Иконка
        gfx_image(IMAGE_STATUS_ICON, x + 1, y, fk, bk);
        // Заголовок приложения
        gfx_string("ДОЗАТОР", FONT_HEADER_BOLD, x + height_get() + 3, y + 1, fk, bk);
    }
public:
    // Конструктор по умолчанию
    gui_status_panel_t(void)
    {
        height_set(24);
        add(form_name);
    }
    
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
class gui_control_t : public gui_visual_sizeable_t
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
    virtual bool key_event(const key_event_args_t &key) = 0;
public:
    // Соединение двух элементов для фокуса
    void connect(gui_control_t &to)
    {
        to.neighbors.link(neighbors);
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
    
protected:
    // Обработчик события клавиши
    virtual bool key_event(const key_event_args_t &key)
    {
        if (focused == NULL)
            return false;
        if (focused->key_event(key))
            return true;
        // Событие не обработано
        switch (key.kind)
        {
            case KEY_KIND_UP:
                // Фокус вверх
                {
                    auto next = (gui_control_wrap_t *)focused->neighbors.prev();
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
                    auto next = (gui_control_wrap_t *)focused->neighbors.next();
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
    
    // Обработчик события вывода на экран
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        auto w = width_get();
        auto h = height_get() - 1;
        // Заливка
        gfx_rect_solid(x, y + 1, w, h - 1, gui_color_get(background));
        // Граница
        auto fk = gui_color_get(GUI_COLOR_FORM_FRAME_FK);
        gfx_line_horz(x, y, w, fk);
        gfx_line_horz(x, y + h, w, fk);
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
        auto top = gui_status_panel.height_get();
        top_set(top);
        height_set(LCD_SIZE_HEIGHT - top - gui_action_panel.height_get());
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
    void key_event(const key_event_args_t &key)
    {
        // Обрабатываем только нажатия
        if (key.state != KEY_STATE_PRESSED)
            return;
        auto form = forms.last();
        assert(form != NULL);
        // Передача события форме
        if (form->holder.key_event(key))
            return;
        // Обработка перехода не прендыдущую форму
        if (key.kind != KEY_KIND_BACK || forms.count() <= 1)
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

// Базовый класс кнопки
class gui_button_base_t : public gui_control_t
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
        bounds_set(36, 36, 168, 76);
    }
    
    // Обработчик события вывода на экран
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
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
    virtual bool key_event(const key_event_args_t &key)
    {
        if (key.kind != KEY_KIND_ENTER)
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
        gui_control_t::paint(offsetX, offsetY, background);
    }
};

// Класс реализации кнопки по умолчанию
class gui_button_notify_t : public gui_button_base_t
{
    // Обработчик клика
    notify_proc_ptr handler;
protected:
    // Обработчик событие клика
    virtual void click_event(void)
    {
        handler();
    }
public:
    // Конструктор по умолчанию
    gui_button_notify_t(const char * _text, notify_proc_ptr _handler) 
        : gui_button_base_t(_text), handler(_handler)
    {
        assert(handler != NULL);
    }
};

// Класс ввода надписи
class gui_label_t : public gui_control_t
{
    // Шрифт
    const uint8_t * const font;
    // Текст
    char text[32];
protected:
    // Обработчик события вывода на экран
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        // Цвет фона и текста
        auto bk = gui_color_get(background);
        auto fk = gui_color_get(GUI_COLOR_LABEL_TEXT_FK);
        // Фон
        gfx_rect_solid(x, y, width_get(), height_get(), bk);
        // Вывод текста
        auto width = gfx_string_measure(text, font);
        gfx_string(text, 
            font, x + width_get() / 2 - width / 2, y + height_get() / 2 - font_height_get(font) / 2, 
            fk, bk);
    }
    
    // Обработчик события клавиши
    virtual bool key_event(const key_event_args_t &key)
    {
        return false;
    }
public:
    // Конструктор по умолчанию
    gui_label_t(const uint8_t *font) : font(font)
    {
        *text = '\0';
    }
    
    // Задает отображаемый текст
    void text_set(const char *source)
    {
        assert(source != NULL);
        if (strcmp(source, text) == 0)
            return;
        strncpy(text, source, ARRAY_SIZE(text));
        refresh();
    }
};

// Массив размера инкремена по шагу нажатия
static const uint16_t GUI_NUMBER_INCREMENT_BY_STEP[] =
{
    1, 1, 1, 1, 1,
    // 5
    5, 5, 5, 5, 5,
    // 30
    10, 10, 10, 10,
    10, 10, 10,
};

// Класс ввода числа с фиксированной точкой
class gui_number_t : public gui_control_t
{
    // Выводимое число
    int32_t number;
    // Выводимое число при вводе
    int32_t number_input;
    // Флаг, указывающий что происходит ввод
    bool input;
    
    // Локальные константы
    enum
    {
        // Количество дробных разрядов
        FRACTION = 1000,
        MAXIMUM = (int32_t)(99.9999999 * FRACTION),
    };
protected:
    // Обработчик события вывода на экран
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        // Цвет фона
        auto bk = gui_color_get(focused_get() ? 
                    (input ? 
                        GUI_COLOR_BUTTON_BK_FOCUSED : 
                        GUI_COLOR_BUTTON_BK_NORMAL) :
                    background);
        // Цвет границы
        auto fk = gui_color_get(focused_get() ? 
            GUI_COLOR_FOCUSED_FK : 
            background);
        // Граница
        gfx_rect_frame_round(x, y, width_get(), height_get(), fk, gui_color_get(background));
        // Фон
        gfx_rect_solid(x, y, width_get(), height_get(), bk);
        // Подготовка текста
        char text[16];
        auto temp = input ? number_input : number;
        auto neg = temp < 0;
        if (neg)
            temp = -temp;
        sprintf(text, "%s%d.%03d", neg ? "-" : "", temp / FRACTION, temp % FRACTION); 
        // Вывод текста
        auto font = FONT_NUMBER_BOLD;
        auto width = gfx_string_measure(text, font);
        gfx_string(text, 
            font, x + width_get() / 2 - width / 2, y + height_get() / 2 - font_height_get(font) / 2, 
            gui_color_get(GUI_COLOR_BUTTON_TEXT_FK), bk);
    }
    
    // Обработчик события клавиши
    virtual bool key_event(const key_event_args_t &key)
    {
        // Режим ввода
        if (input)
        {
            auto index = key.step - 1;
            auto step = 100;
            if (index <= ARRAY_SIZE(GUI_NUMBER_INCREMENT_BY_STEP))
                step = GUI_NUMBER_INCREMENT_BY_STEP[index];
            switch (key.kind)
            {
                case KEY_KIND_ENTER:
                    number = number_input;
                    input = false;
                    // TODO: оповещение
                    break;
                case KEY_KIND_UP:
                    if (number_input + step < MAXIMUM)
                        number_input += step;
                    else
                        number_input = MAXIMUM;
                    break;
                case KEY_KIND_DOWN:
                    if (number_input > step)
                        number_input -= step;
                    else
                        number_input = 0;
                    break;
                case KEY_KIND_BACK:
                    input = false;
                    break;
            }
            refresh();
            return true;
        }
        // Режим фокуса
        if (key.kind != KEY_KIND_ENTER)
            return false;
        number_input = number;
        input = true;
        refresh();
        return true;
    }
public:
    // Конструктор по умолчанию
    gui_number_t(void) : number(0), input(false)
    {
        // Позиционирование по умолчанию
        height_set(54);
    }
    
    // Получает текущее число
    int32_t number_get(void) const
    {
        return number;
    }
    
    // Задает текущее число
    void number_set(int32_t value)
    {
        if (number == value)
            return;
        number = value;
        refresh();
    }
};

// Класс декоративной рамки
class gui_bevel_t : public gui_container_t
{
protected:
    // Вывод дочерних элементов
    virtual void paint_childs(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // Переопределяем фон
        gui_container_t::paint_childs(offsetX, offsetY, GUI_COLOR_BEVEL_BK);
    }

    // Обработчик события вывода на экран
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        // Граница
        gfx_rect_frame_round(x, y, width_get(), height_get(), gui_color_get(GUI_COLOR_BEVEL_FK), gui_color_get(background));
        // Фон
        gfx_rect_solid(x, y, width_get(), height_get(), gui_color_get(GUI_COLOR_BEVEL_BK));
    }
    
    // Обработчик события клавиши
    virtual bool key_event(const key_event_args_t &key)
    {
        return false;
    }
public:
    // Конструктор по умолчанию
    gui_bevel_t(void)
    {
        left_set(18);
        width_set(204);
    }
};

// Главная форма
static class gui_form_main_t : public gui_form_t
{
    // Кнопка входа в дозатор
    gui_button_notify_t dispencer;
    // Кнопка входа в калибровку
    gui_button_notify_t calibration;
    // Кнопка входа в тест мотора
    gui_button_notify_t motor_test;
    
    // Вход в дозатор
    static void dispencer_enter(void);
    // Вход в калибровку
    static void calib_enter(void);
    // Вход в тест мотора
    static void motor_test_enter(void);
public:
    // Конструктор по умолчанию
    gui_form_main_t(void) : gui_form_t("ГЛАВНАЯ"), 
       dispencer("НАСЫПАТЬ", dispencer_enter),
       calibration("КАЛИБРОВКА", calib_enter),
       motor_test("ТЕСТ МОТОРА", motor_test_enter)
    {
        // Добавление кнопок
        add(dispencer);
        add(calibration);
        add(motor_test);
        // Позиционирование
        dispencer.bounds_set(36, 24, 168, 54);
        calibration.bounds_set(36, 102, 168, 54);
        motor_test.bounds_set(36, 180, 168, 54);
        // Фоккус
        dispencer.connect(calibration);
        calibration.connect(motor_test);
        focused_set(&dispencer);
    }
} gui_form_main;

// Форма калибровки
static class gui_form_calib_t : public gui_form_t
{
    // Кнопка калибровки в нуле
    gui_button_notify_t zero_button;
    // Кнопка калибровки в точке
    gui_button_notify_t point_button;
    // Рамка
    gui_bevel_t point_bevel;
    // Ввод значения точки
    gui_number_t point_value;
    
    // Клик по кнопке калибровки в точке
    static void point_click(void);
public:
    // Конструктор по умолчанию
    gui_form_calib_t(void) : gui_form_t("КАЛИБРОВКА"),
        zero_button("В НУЛЕ", weight_calib_zero),
        point_button("В ТОЧКЕ", point_click)
    {
        zero_button.height_set(54);
        point_button.height_set(54);
        
        zero_button.top_set(17);
        
        point_bevel.top_set(88);
        point_bevel.height_set(156);
        
        point_button.top_set(16);
        point_button.left_set(18);
        
        point_value.top_set(86);
        point_value.left_set(18);
        point_value.width_set(168);
        
        add(zero_button);
        add(point_bevel);
        point_bevel.add(point_button);
        point_bevel.add(point_value);
        
        // Фоккус
        zero_button.connect(point_button);
        point_button.connect(point_value);
        focused_set(&zero_button);
    }
} gui_form_calib;

void gui_form_calib_t::point_click(void)
{
    weight_calib_point(gui_form_calib.point_value.number_get());
}

void gui_form_main_t::calib_enter(void)
{
    gui_form_stack.add(gui_form_calib);
}

// Форма дозатора
static class gui_form_dispencer_t : public gui_form_t
{
    // Кнопка задания тары
    gui_button_notify_t tare_button;
    // Ввод значения точки
    gui_number_t weight_value;
    // Процедура обработчика собтыия нового значения веса
    static void weight_handler_cb(int32_t current);
public:
    // Обработчик нового значения веса
    weight_event_handler_t weight_handler;
    
    // Конструктор по умолчанию
    gui_form_dispencer_t(void) 
        : gui_form_t("НАСЫПАТЬ"), 
          weight_handler(weight_handler_cb),
          tare_button("ТАРА", weight_tare)
    {
        // Кнопка тары
        tare_button.top_set(74);
        // Поле вывода веса
        weight_value.top_set(10);
        weight_value.width_set(LCD_SIZE_WIDTH);
        // Добавление элементов
        add(weight_value);
        add(tare_button);
        // Фокус
        focused_set(&tare_button);
    }
} gui_form_dispencer;

void gui_form_main_t::dispencer_enter(void)
{
    gui_form_stack.add(gui_form_dispencer);
}

void gui_form_dispencer_t::weight_handler_cb(int32_t current)
{
    gui_form_dispencer.weight_value.number_set(current);
}

// Форма теста мотора
static class gui_form_motor_test_t : public gui_form_t
{
    // Рамка
    gui_bevel_t bevel;
    // Надпись заголовка
    gui_label_t header;
    // Надпись скорости
    gui_label_t speed;
    
    // Задание скорости вращения
    void speed_set(uint8_t value)
    {
        pwm_width_set(value);
        char temp[32];
        sprintf(temp, "%d%%", value);
        speed.text_set(temp);
    }
    
protected:
    // Обработчик события вывода на экран
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        // Базовый метод
        gui_form_t::paint_self(x, y, background);
        // Вывод картинки мотора
        gfx_image(IMAGE_MISC_MOTOR, x + 56, y + 10, gui_color_get(GUI_COLOR_FORM_FRAME_FK), gui_color_get(background));
    }
    
    // Обработчик события клавиши
    virtual bool key_event(const key_event_args_t &key)
    {
        if (gui_form_t::key_event(key))
            return true;
        if (key.state != KEY_STATE_PRESSED)
            return false;
        auto current = pwm_width_get();
        switch (key.kind)
        {
            case KEY_KIND_UP:
                if (current < PWM_WIDTH_MAX)
                    speed_set(current + 1);
                return true;
            case KEY_KIND_DOWN:
                if (current > 0)
                    speed_set(current - 1);
                return true;
        }
        return false;
    }
public:
    // Конструктор по умолчанию
    gui_form_motor_test_t(void) : gui_form_t("ТЕСТ МОТОРА"), 
        header(FONT_HEADER_BOLD), speed(FONT_NUMBER_BOLD)
    {
        // Рамка
        bevel.bounds_set(36, 148, 168, 76);
        add(bevel);
        // Заголовок
        header.text_set("СКОРОСТЬ ВРАЩЕНИЯ");
        header.bounds_set(0, 0, 168, 22);
        bevel.add(header);
        // Значение
        speed_set(0);
        speed.bounds_set(0, 22, 168, 53);
        bevel.add(speed);
    }
} gui_form_motor_test;

void gui_form_main_t::motor_test_enter(void)
{
    gui_form_stack.add(gui_form_motor_test);
}

// Форма выбора темы
static class gui_form_theme_t : public gui_form_t
{
    // Рамка
    gui_bevel_t bevel;
    // Надпись заголовка
    gui_label_t header;
    // Кнопки тем
    gui_button_notify_t light, dark;
    
    // Выбор темы
    static void select(gui_theme_kind_t theme)
    {
        // Запись в настройки
        flash_write(&GUI_SETTINGS.theme, &theme, sizeof(theme));
        // Вход в главную форму
        gui_form_stack.add(gui_form_main);
        // Обновление панели действий
        gui_action_panel.refresh();
    }
    
    // Выбор светлой темы
    static void select_light(void)
    {
        select(GUI_THEME_KIND_LIGHT);
    }
    
    // Выбор темной темы
    static void select_dark(void)
    {
        select(GUI_THEME_KIND_DARK);
    }
public:
    // Конструктор по умолчанию
    gui_form_theme_t(void) 
        : gui_form_t("ВЫБОР ТЕМЫ"), 
          header(FONT_HEADER_BOLD),
          light("СВЕТЛАЯ", select_light),
          dark("ТЕМНАЯ", select_dark)
    {
        // Заголовок
        header.text_set("ВЫБОР ТЕМЫ");
        header.bounds_set(0, 23, LCD_SIZE_WIDTH, 22);
        add(header);
        // Рамка
        bevel.bounds_set(36, 68, 168, 156);
        add(bevel);
        // Кнопки
        light.bounds_set(18, 16, 132, 54);
        bevel.add(light);
        dark.bounds_set(18, 86, 132, 54);
        bevel.add(dark);
        // Фокус
        light.connect(dark);
        switch (GUI_SETTINGS.theme)
        {
            case GUI_THEME_KIND_LIGHT:
                focused_set(&light);
                break;
            case GUI_THEME_KIND_DARK:
                focused_set(&dark);
                break;
        }
    }
} gui_form_theme;

// Таймер перерисовки интерфейса
static timer_notify_t gui_paint_timer([](void)
{
    gui_action_panel.repaint();
    gui_status_panel.repaint();
    gui_form_stack.repaint();
});

// Обработчик смены состояния клавиш
static key_event_handler_t gui_key_event([](const key_event_args_t &key)
{
    gui_form_stack.key_event(key);
    gui_action_panel.key_event(key);
});

void gui_init(void)
{
    // Добавляем формы
    gui_form_stack.add(gui_form_theme);
    gui_form_stack.add(gui_form_main);
    // 25 кадров в секунду
    gui_paint_timer.start_hz(25, TIMER_FLAG_LOOP);
    // Добавляемя в список обработчиков событий клавиш
    key_event_list.add(gui_key_event);
    // Подписываемся на получение текущего веса
    weight_event_list.add(gui_form_dispencer.weight_handler);
}
