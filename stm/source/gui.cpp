// This file uses cp1251 encoding for russian C strings
#include "gui.h"
#include "gfx.h"
#include "lcd.h"
#include "list.h"
#include "font.h"
#include "image.h"
#include "timer.h"
#include "callback.h"

// ���� ����
enum gui_color_t
{
    // ��� �������� ������� �������
    GUI_COLOR_FORM_BK,
    // ������� ������� ������� �������
    GUI_COLOR_FORM_FRAME_FK,
    // ��� ������� �� �������� �������
    GUI_COLOR_EDGE_PANEL_BK,

    // ������� ������� ��������� (��� �������)
    GUI_COLOR_STAUS_HEADER_FK,
    // ������� ������� ����� �������� �����
    GUI_COLOR_STAUS_FORM_NAME_FK,
    
    // ������� ������ ������ ��������
    GUI_COLOR_ACTION_BUTTON_FK,
    // ��� ������ ������ �������� (�� ���������)
    GUI_COLOR_ACTION_BUTTON_BK_NORMAL,
    // ��� ������ ������ �������� (������)
    GUI_COLOR_ACTION_BUTTON_BK_PRESSED,
    // ��� ������ ������ �������� (����)
    GUI_COLOR_ACTION_BUTTON_BK_ENTER,
    // ��� ������ ������ �������� (�����)
    GUI_COLOR_ACTION_BUTTON_BK_BACK,

    // ������� ��� ������ (��� ������ ���������)
    GUI_COLOR_FOCUSED_FK,
    
    // ����� ������
    GUI_COLOR_BUTTON_TEXT_FK,
    // ������� ������
    GUI_COLOR_BUTTON_FRAME_FK,
    // ��� ������ (�� ���������)
    GUI_COLOR_BUTTON_BK_NORMAL,
    // ��� ������ (� ������)
    GUI_COLOR_BUTTON_BK_FOCUSED,
    
    // ���������� ������
    GUI_COLOR_COUNT,
};

// �������� ���� 
typedef lcd_color_t gui_theme_t[GUI_COLOR_COUNT];

#define GUI_THEME_COLOR_WHITE_ORANGE    LCD_COLOR_MAKE(209, 156, 0)

// �������� �����
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

// �������� �������� ���� �� ������ ����
static lcd_color_t gui_color_get(gui_color_t color)
{
    return GUI_THEMES[0][color];
}

// ��������������� ����������
class gui_visual_t;
class gui_container_t;
// ������� �������� ��� �������� � �������
typedef list_item_template_t<gui_visual_t> gui_visual_wrap_t;

// ����� �������� ����������� ��������
class gui_visual_t
{
    friend class gui_container_t;
    
    // ����������
    uint16_t left, top;
    // ��������� �� ������������ ���������
    gui_container_t *parent;
    // ������� ��� ������ �������� ���������
    gui_visual_wrap_t child_item;
    // ���� ������������� �����������
    bool paint_needed;
protected:
    // ����������� �� ���������
    gui_visual_t(void) : 
        paint_needed(true), parent(NULL), left(0), top(0), child_item(*this)
    { }
    
    // �������� ��������� �� ������������ ������� TODO: ��� ��������
    gui_container_t * parent_get(void) const
    {
        return parent;
    }

    // ���������� ������������� ��������
    void parent_refresh(void);
    
    // ���������� ������� ������ �� �����
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background) = 0;
public:
    // �������� ���������� �� X
    uint16_t left_get(void) const
    {
        return left;
    }
    
    // �������� ���������� �� Y
    uint16_t top_get(void) const
    {
        return top;
    }
    
    // ������ ���������� �� X
    void left_set(uint16_t value);

    // ������ ���������� �� Y
    void top_set(uint16_t value);
    
    // ������� ����� �� �����
    virtual void paint(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // ����� �������� ��������
        if (!paint_needed)
            return;
        paint_handler(offsetX + left, offsetY + top, background);
        paint_needed = false;
    }
    
    // ���������� ��������� ��������
    virtual void refresh()
    {
        paint_needed = true;
    }
    
    // �������� �������� �������� �� ������������� ����������
    void remove(void)
    {
        assert(parent != NULL);
        child_item.unlink();
        parent = NULL;
    }
};

// ����� �������� ���������� ���������
class gui_container_t : public gui_visual_t
{
    // ������ �������� ���������
    list_template_t<gui_visual_wrap_t> childs;
public:
    // ������� ����� �� �����
    virtual void paint(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // ������� �����
        gui_visual_t::paint(offsetX, offsetY, background);
        // ������� ���������
        offsetX += left_get();
        offsetY += top_get();
        // ����� ��������
        for (auto i = childs.head(); i != NULL; i = LIST_ITEM_NEXT(i))
            i->holder.paint(offsetX, offsetY, background);
    }
    
    // ���������� ��������� ��������
    virtual void refresh()
    {
        // ������� �����
        paint_needed = true;
        // ����� ��������
        for (auto i = childs.head(); i != NULL; i = LIST_ITEM_NEXT(i))
            i->holder.refresh();
    }
    
    // ���������� ��������� ��������
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

// ����� ������� ������ �� �������� �������
class gui_edge_panel_t : protected gui_container_t
{
protected:
    // ��������� ���������
    enum
    {
        // ������ ������
        WIDTH = LCD_SIZE_WIDTH,
    };
    
    // �������� ������ ������
    virtual uint8_t height_get(void) const = 0;

    // ���������� ������� ������ �� �����
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
    {
        gfx_rect_solid(x, y, WIDTH, height_get(), gui_color_get(background));
    }
public:
    // ����� �� �����
    void repaint(void)
    {
        gui_container_t::paint(0, 0, GUI_COLOR_EDGE_PANEL_BK);
    }
    
    // ������ �� �����������
    void refresh(void)
    {
        gui_container_t::refresh();
    }
};

// ������ ��������
static class gui_action_panel_t : public gui_edge_panel_t
{
    // ������
    class button_t : public gui_visual_t
    {
        // ��������� �� ������ �����������
        uint8_t const * const image;
        // ���� ��������
        const gui_color_t background;
        // ���� ��������� �������
        bool pressed;
    protected:    
        // ���������� ������� ������ �� �����
        virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t pbackground)
        {
            // ���������� ���� ����
            auto bk = gui_color_get(pressed ? GUI_COLOR_ACTION_BUTTON_BK_PRESSED  : background);
            
            // ����� ������������� ������
            gfx_rect_frame_round(x, y, 40, 24, bk, gui_color_get(pbackground));
            // ������� ��� ���������� ������ �� ������
            gfx_rect_solid(x + 0, y, 8, 24, bk);
            gfx_rect_solid(x + 32, y, 8, 24, bk);
            // ������
            gfx_image(image, x + 8, y, gui_color_get(GUI_COLOR_ACTION_BUTTON_FK),  bk);
        }
    public:
        // ����������� �� ���������
        button_t(uint8_t const * _image, gui_color_t _background) 
            : image(_image), background(_background), pressed(false)
        { }
        
        // ��������� ��������� �������
        void pressed_set(bool value)
        {
            if (pressed == value)
                return;
            pressed = value;
            refresh();
        }
    } enter, up, down, back;

    // �������� ������ �� ����� �� �������
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
    // �������� ������ ������
    uint8_t height_get(void) const
    {
        return HEIGHT;
    }
public:
    // ��������� ���������
    enum
    {
        // ������ ������
        HEIGHT = 36,
    };

    // ����������� �� ���������
    gui_action_panel_t(void) : 
        enter(IMAGE_BUTTON_ENTER, GUI_COLOR_ACTION_BUTTON_BK_ENTER),
        up(IMAGE_BUTTON_UP, GUI_COLOR_ACTION_BUTTON_BK_NORMAL),
        down(IMAGE_BUTTON_DOWN, GUI_COLOR_ACTION_BUTTON_BK_NORMAL),
        back(IMAGE_BUTTON_BACK, GUI_COLOR_ACTION_BUTTON_BK_BACK)
    {
        // ���������������� ������
        top_set(LCD_SIZE_HEIGHT - height_get());
        // ���������������� ������
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
        // ���������� ������ � ��������
        add(enter);
        add(up);
        add(down);
        add(back);
    }
    
    // ������� ����� ��������� �������
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


// ������ �������
static class gui_status_panel_t : public gui_edge_panel_t
{
    // ����� ������� ����� �������� �����
    class form_name_t : public gui_visual_t
    {
        // ��������� �����
        const char *text;
    protected:
        // ���������� ������� ������ �� �����
        virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
        {
            // ������ ������
            auto width = gfx_string_measure(text, FONT_HEADER_BOLD);
            // ����� ������
            gfx_string(text, FONT_HEADER_BOLD, x - width - 3, y + 1, 
                gui_color_get(GUI_COLOR_STAUS_FORM_NAME_FK), 
                gui_color_get(background));
        }
    public:
        // ����������� �� ���������
        form_name_t(void) : text(NULL)
        {
            // ����������������
            left_set(WIDTH);
        }
        
        // ��������� ������ ������
        void text_set(const char *value)
        {
            // ������������ ���������, �� �����
            if (text == value)
                return;
            assert(value != NULL);
            text = value;
            refresh();
        }
    } form_name;
protected:
    // �������� ������ ������
    uint8_t height_get(void) const
    {
        return HEIGHT;
    }

    // ���������� ������� ������ �� �����
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
    {
        auto bk = gui_color_get(background);
        auto fk = gui_color_get(GUI_COLOR_STAUS_HEADER_FK);
        // ������� �����
        gui_edge_panel_t::paint_handler(x, y, background);
        // ������
        gfx_image(IMAGE_STATUS_ICON, x + 1, y, fk, bk);
        // ��������� ����������
        gfx_string("�������", FONT_HEADER_BOLD, x + HEIGHT + 3, y + 1, fk, bk);
    }
public:
    // ����������� �� ���������
    gui_status_panel_t(void)
    {
        add(form_name);
    }

    // ��������� ���������
    enum
    {
        // ������ ������
        HEIGHT = 24,
    };
    
    // ��������� ������ ����� ������� �����
    void form_name_set(const char *value)
    {
        form_name.text_set(value);
        refresh();
    }
} gui_status_panel;

// ��������������� ����������
class gui_control_t;
// �������� ���������� �������� ��� �������� ����������
typedef list_item_template_t<gui_control_t> gui_control_wrap_t;

// ����� �������� ���������� ������������ �����
class gui_control_t : public gui_visual_t
{
    friend class gui_form_t;
    
    // �������� ��������
    gui_control_wrap_t neighbors;
    // ��������� ������
    bool focused;
    
    // ������������� ��������� ������
    void focused_set(bool state)
    {
        if (focused == state)
            return;
        focused = state;
        refresh();
    }
protected:
    // ����������� �� ���������
    gui_control_t(void) : neighbors(*this), focused(false)
    { }

    // �������� ��������� ������
    bool focused_get(void) const
    {
        return focused;
    }
    
    // ���������� ������� �������
    virtual bool key_event(key_kind_t key) = 0;
public:
    // ���������� ���� ��������� ��� ������
    void connect(gui_control_t &to)
    {
        neighbors.link(to.neighbors);
    }
};

// ������� ����� �������� ����������, �������� �������
class gui_control_sizeable_t : public gui_control_t
{
    // ������ � ������
    uint16_t width, height;
    
protected:
    // ����������� �� ���������
    gui_control_sizeable_t(void) : width(0), height(0)
    { }

    // �������� ������
    uint16_t width_get(void) const
    {
        return width;
    }
    
    // �������� ������
    uint16_t height_get(void) const
    {
        return height;
    }
public:
    // ������ ������
    void width_set(uint16_t value)
    {
        if (width == value)
            return;
        width = value;
        parent_refresh();
    }
    
    // ������ ������
    void height_set(uint16_t value)
    {
        if (height == value)
            return;
        height = value;
        parent_refresh();
    }
};

// ��������������� ����������
class gui_form_t;
// ������� �������� ��� ����� � ������ �����
typedef list_item_template_t<gui_form_t> gui_form_wrap_t;

// ����� ����� ����������������� ����������
class gui_form_t : protected gui_container_t
{
    friend class gui_form_stack_t;
    
    // ��������� ���������
    enum
    {
        // ������ ������
        HEIGHT = LCD_SIZE_HEIGHT - gui_status_panel_t::HEIGHT - gui_action_panel_t::HEIGHT,
        // ������ ������
        WIDTH = LCD_SIZE_WIDTH,
    };
    
    // ��� �����
    char const * const name;
    // ������� ����� ����
    gui_form_wrap_t stack_item;
    // �������� �������
    gui_control_t *focused;
    
    // ����� �� �����
    void repaint(void)
    {
        gui_container_t::paint(0, 0, GUI_COLOR_FORM_BK);
    }
    
    // ������ �� �����������
    void refresh(void)
    {
        gui_container_t::refresh();
    }
    
    // ���������� ������� �������
    bool key_event(key_kind_t key)
    {
        if (focused == NULL)
            return false;
        if (focused->key_event(key))
            return true;
        // ������� �� ����������
        switch (key)
        {
            case KEY_KIND_UP:
                // ����� �����
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
                // ����� ����
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
    // ���������� ������� ������ �� �����
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
    {
        gfx_rect_solid(x + 1, y + 1, WIDTH - 2, HEIGHT - 2, gui_color_get(background));
        gfx_rect_frame(x, y, WIDTH, HEIGHT, gui_color_get(GUI_COLOR_FORM_FRAME_FK));
    }
    
    // ��������� ������ ������ ��������
    void focused_set(gui_control_t *control)
    {
        // ������������ ���������, �� �����
        if (focused == control)
            return;
        // ������
        if (focused != NULL)
            focused->focused_set(false);
        focused = control;
        // ���������
        if (focused != NULL)
            focused->focused_set(true);
    }
public:
    // ����������� �� ���������
    gui_form_t(char const * _name) 
        : name(_name), stack_item(*this), focused(NULL)
    {
        assert(name != NULL);
        // ����������������
        top_set(gui_status_panel_t::HEIGHT);
    }
};

// ���� �������� ����
static class gui_form_stack_t
{
    // ������ �������� ����
    list_template_t<gui_form_wrap_t> forms;
    
    // ������� ����� �������� �����
    void active_changed(void)
    {
        auto active = forms.last();
        assert(active != NULL);
        // ������ �� �����������
        active->holder.refresh();
        // ��������� ����� �����
        gui_status_panel.form_name_set(active->holder.name);
    }
public:
    // ������� ����� ��������� �������
    void key_event(key_kind_t key, key_state_t state)
    {
        // ������������ ������ �������
        if (state != KEY_STATE_PRESSED)
            return;
        auto form = forms.last();
        assert(form != NULL);
        // �������� ������� �����
        if (form->holder.key_event(key))
            return;
        // ��������� �������� �� ����������� �����
        if (key != KEY_KIND_BACK || forms.count() <= 1)
            return;
        // ������� ������� �����
        form->unlink();
        // ������� �����
        active_changed();
    }
    
    // ����� �� �����
    void repaint(void)
    {
        auto active = forms.last();
        assert(active != NULL);
        active->holder.repaint();
    }
    
    // ���������� ����� (������ ��������)
    void add(gui_form_t &form)
    {
        // ���������� � �����
        form.stack_item.link(forms);
        // ������� �����
        active_changed();
    }
} gui_form_stack;

// ����� ����� ������ ��������� �����
class gui_label_int_t : public gui_control_sizeable_t
{
    // ��������� �����
    int32_t number;
protected:
    // ���������� ������� ������ �� �����
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
    {
        auto bk = gui_color_get(background);
        // ���
        gfx_rect_solid(x, y, width_get(), height_get(), bk);
        // �����
        char buffer[16];
        if (number > 0)
            sprintf(buffer, "+%d", number);
        else
            sprintf(buffer, "%d", number);
        auto font = FONT_NUMBER_BOLD;
        auto width = gfx_string_measure(buffer, font);
        gfx_string(buffer, font, x + width_get() / 2 - width / 2, y + height_get() / 2 - font_height_get(font) / 2, gui_color_get(GUI_COLOR_BUTTON_TEXT_FK), bk);
    }
    
    // ���������� ������� �������
    virtual bool key_event(key_kind_t key)
    {
        return false;
    }
public:
    // ����������� �� ���������
    gui_label_int_t(void) : number(0)
    {
        width_set(LCD_SIZE_WIDTH);
        height_set(font_height_get(FONT_NUMBER_BOLD));
    }
    
    // ��������� �������� ���������� �����
    void number_set(int32_t value)
    {
        if (number == value)
            return;
        number = value;
        refresh();
    }
};

// ������� ����� ������
class gui_button_base_t : public gui_control_sizeable_t
{
    // ������ ������� ��� �����
    uint8_t flash;
    // ����� ������
    char const * const text;
protected:
    // ����������� �� ���������
    gui_button_base_t(const char * _text) : text(_text), flash(0)
    {
        assert(text != NULL);
        // ���������������� �� ���������
        left_set(36);
        top_set(36);
        width_set(168);
        height_set(76);
    }
    
    // ���������� ������� ������ �� �����
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
    {
        // ���� ����
        auto bk = gui_color_get(flash > 0 ? 
            GUI_COLOR_FOCUSED_FK : 
            (focused_get() ? 
                GUI_COLOR_BUTTON_BK_FOCUSED : 
                GUI_COLOR_BUTTON_BK_NORMAL));
        // ���� �������
        auto fk = gui_color_get(focused_get() ? 
            GUI_COLOR_FOCUSED_FK : 
            GUI_COLOR_BUTTON_FRAME_FK);
        // �������
        gfx_rect_frame_round(x, y, width_get(), height_get(), fk, gui_color_get(background));
        // ���
        gfx_rect_solid(x, y, width_get(), height_get(), bk);
        // �����
        auto font = FONT_BUTTON_BOLD;
        auto width = gfx_string_measure(text, font);
        gfx_string(text, 
            font, x + width_get() / 2 - width / 2, y + height_get() / 2 - font_height_get(font) / 2, 
            gui_color_get(GUI_COLOR_BUTTON_TEXT_FK), bk);
    }
    
    // ���������� ������� �������
    virtual bool key_event(key_kind_t key)
    {
        if (key != KEY_KIND_ENTER)
            return false;
        // �������
        flash = 5;
        refresh();
        return true;
    }
    
    // ������� �����
    virtual void click_event(void) = 0;
public:
    // ������� ����� �� �����
    virtual void paint(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // ��������� �������
        if (flash > 0 && --flash == 0)
        {
            refresh();
            click_event();
        }
        // ������� �����
        gui_control_sizeable_t::paint(offsetX, offsetY, background);
    }
};

// ����� ���������� ������ �� ���������
class gui_button_t : public gui_button_base_t
{
    // ���������� �����
    callback_proc_ptr handler;
    
protected:
    // ���������� ������� �����
    virtual void click_event(void)
    {
        handler();
    }
public:
    // ����������� �� ���������
    gui_button_t(const char * _text, callback_proc_ptr _handler) 
        : gui_button_base_t(_text), handler(_handler)
    {
        assert(handler != NULL);
    }
};

// ������� �����
static class gui_form_main_t : public gui_form_t
{
    // ������ ����� � �������
    gui_button_t dispencer;
    // ������ ����� � ����������
    gui_button_t calibration;
    
    // ���� � �������
    static void dispencer_enter(void);
    // ���� � ����������
    static void dummy(void)
    { }
public:
    // ����������� �� ���������
    gui_form_main_t(void) : gui_form_t("�������"), 
       dispencer("��������", dispencer_enter),
       calibration("����������", dummy)
    {
        // ���������� ������
        add(dispencer);
        add(calibration);
        // ����������������
        calibration.top_set(148);
        // ������
        dispencer.connect(calibration);
        focused_set(&dispencer);
    }
} gui_form_main;

// ����� ��������
static class gui_form_dispencer_t : public gui_form_t
{
public:
    gui_label_int_t label;
    // ����������� �� ���������
    gui_form_dispencer_t(void) : gui_form_t("��������")
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

// ���� � �������
void gui_form_main_t::dispencer_enter(void)
{ 
    gui_form_stack.add(gui_form_dispencer);
}

// ������ ����������� ����������
static timer_callback_t gui_paint_timer([](void)
{
    gui_action_panel.repaint();
    gui_status_panel.repaint();
    gui_form_stack.repaint();
});

void gui_init(void)
{
    adc_timer.start_us(100, TIMER_FLAG_LOOP);
    // ��������� ������� �����
    gui_form_stack.add(gui_form_main);
    // 25 ������ � �������
    gui_paint_timer.start_hz(25, TIMER_FLAG_LOOP);
}

void gui_key_event(key_kind_t key, key_state_t state)
{
    gui_form_stack.key_event(key, state);
    gui_action_panel.key_event(key, state);
}
