#include "gui.h"
#include "gfx.h"
#include "lcd.h"
#include "list.h"
#include "font.h"
#include "image.h"
#include "timer.h"

// Цвет темы
enum gui_color_t
{
    // Фон основной рабочей области
    GUI_COLOR_WORKSPACE_BK,
    // Фон панелей на границах дисплея
    GUI_COLOR_EDGE_PANEL_BK,

    // Заливка надписи заголовка (про дозатор)
    GUI_COLOR_STAUS_HEADER_FK,
    
    // Заливка кнопки панели действий (по умолчанию)
    GUI_COLOR_ACTION_BUTTON_FK,
    // Фон кнопки панели действий (по умолчанию)
    GUI_COLOR_ACTION_BUTTON_BK_NORMAL,
    // Фон кнопки панели действий (нажата)
    GUI_COLOR_ACTION_BUTTON_BK_PRESSED,
    // Фон кнопки панели действий (ввод)
    GUI_COLOR_ACTION_BUTTON_BK_ENTER,
    // Фон кнопки панели действий (назад)
    GUI_COLOR_ACTION_BUTTON_BK_BACK,
    
    // Количество цветов
    GUI_COLOR_COUNT,
};

// Цветовая тема 
typedef lcd_color_t gui_theme_t[GUI_COLOR_COUNT];

// Цветовые схемы
static const gui_theme_t GUI_THEMES[2] =
{
    // Default
    {
        // GUI_COLOR_WORKSPACE_BK
        LCD_COLOR_MAKE(0, 0, 0),
        // GUI_COLOR_EDGE_PANEL_BK
        LCD_COLOR_MAKE(224, 224, 224),
        // GUI_COLOR_STAUS_HEADER_FK
        LCD_COLOR_MAKE(0, 0, 0),
        // GUI_COLOR_ACTION_BUTTON_FK
        LCD_COLOR_MAKE(48, 48, 48),
        // GUI_COLOR_ACTION_BUTTON_BK_NORMAL
        LCD_COLOR_MAKE(128, 128, 128),
        // GUI_COLOR_ACTION_BUTTON_BK_PRESSED
        LCD_COLOR_MAKE(255, 191, 0),
        // GUI_COLOR_ACTION_BUTTON_BK_ENTER
        LCD_COLOR_MAKE(50, 147, 111),
        // GUI_COLOR_ACTION_BUTTON_BK_BACK
        LCD_COLOR_MAKE(232, 63, 111),
    },
};

// ПОлучает реальный цвет по номеру темы
static lcd_color_t gui_color_get(gui_color_t color)
{
    return GUI_THEMES[0][color];
}

// Предварительное объявление
class gui_container_t;

// Класс базового визуального элемента
class gui_visual_t : list_item_t
{
    friend class gui_container_t;
    
    // Координаты
    uint16_t left, top;
    // Указатель на родительский контейнер
    gui_container_t *parent;
    // Флаг необходимости перерисовки
    bool paint_needed;
protected:
    // Конструктор по умолчанию
    gui_visual_t(void) : 
        paint_needed(true), parent(NULL), left(0), top(0)
    { }
    
    // Получает указатель на родительский элемент TODO: под вопросом
    gui_container_t * parent_get(void) const
    {
        return parent;
    }
    
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
        parent = NULL;
        unlink();
    }
};

// Класс базового контейнера элементов
class gui_container_t : public gui_visual_t
{
    // Список дочерних элементов
    list_template_t<gui_visual_t> childs;
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
            i->paint(offsetX, offsetY, background);
    }
    
    // Обновление состояния элемента
    virtual void refresh()
    {
        // Базовый метод
        paint_needed = true;
        // Вывод дочерних
        for (auto i = childs.head(); i != NULL; i = LIST_ITEM_NEXT(i))
            i->refresh();
    }
    
    // Добавление дочернего элемента
    void add(gui_visual_t &child)
    {
        assert(child.parent == NULL);
        child.parent = this;
        child.link(childs);
    }
};

void gui_visual_t::left_set(uint16_t value)
{
    if (left == value)
        return;
    left = value;
    if (parent != NULL)
        parent->refresh();
}

void gui_visual_t::top_set(uint16_t value)
{
    if (top == value)
        return;
    top = value;
    if (parent != NULL)
        parent->refresh();
}

// Класс базовой панели на границах дисплея
class gui_edge_panel_t : protected gui_container_t
{
protected:
    // Получает высоту панели
    virtual uint8_t height_get(void) const = 0;

    // Обработчик события вывода на экран
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
    {
        gfx_rect_solid(x, y, LCD_SIZE_WIDTH, height_get(), gui_color_get(background));
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

// Класс надписи
class gui_label_base_t : public gui_visual_t
{
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
        // Флаги состояния
        struct state_t
        {
            // Нажата
            bool pressed : 1;
            // Видима
            bool enabled : 1;
            
            // Конструктор по умолчанию
            state_t(void) : pressed(false), enabled(true)
            { }
        } state;
    protected:    
        // Обработчик события вывода на экран
        virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t pbackground)
        {
            // Определяем цвет фона
            auto bk = gui_color_get(state.pressed ? GUI_COLOR_ACTION_BUTTON_BK_PRESSED  : background);
            
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
            : image(_image), background(_background)
        { }
        
        // Установка состояния нажатия
        void pressed_set(bool value)
        {
            if (state.pressed == value)
                return;
            state.pressed = value;
            refresh();
        }

        // Установка состояния нажатия
        void enabled_set(bool value)
        {
            if (state.enabled == value)
                return;
            state.enabled = value;
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
        return 36;
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
protected:
    // Получает высоту панели
    uint8_t height_get(void) const
    {
        return 24;
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
        gfx_string("ДОЗАТОР", FONT_HEADER_BOLD, x + 24 + 3, y + 1, fk, bk);
    }

} gui_status_panel;

// Таймер перерисовки интерфейса
static timer_callback_t gui_paint_timer([](void)
{
    gui_action_panel.repaint();
    gui_status_panel.repaint();
});

void gui_init(void)
{
    // 25 кадров в секунду
    gui_paint_timer.start_hz(25, TIMER_FLAG_LOOP);
}

void gui_key_event(key_kind_t key, key_state_t state)
{
    gui_action_panel.key_event(key, state);
}
