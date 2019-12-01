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
    
    // ����� �������
    GUI_COLOR_LABEL_TEXT_FK,
    
    // ����� ������
    GUI_COLOR_BUTTON_TEXT_FK,
    // ������� ������
    GUI_COLOR_BUTTON_FRAME_FK,
    // ��� ������ (�� ���������)
    GUI_COLOR_BUTTON_BK_NORMAL,
    // ��� ������ (� ������)
    GUI_COLOR_BUTTON_BK_FOCUSED,
    
    // ������� ������� �����
    GUI_COLOR_BEVEL_FK,
    // ��� �����
    GUI_COLOR_BEVEL_BK,
    
    // ���������� ������
    GUI_COLOR_COUNT,
};

// �������� ���� 
typedef lcd_color_t gui_theme_t[GUI_COLOR_COUNT];

// ������������ ����� ���
enum gui_theme_kind_t
{
    // �������
    GUI_THEME_KIND_LIGHT,
    // ������
    GUI_THEME_KIND_DARK,
};

// ���������� ����� ���
#define GUI_THEME_KIND_COUNT    ((int)GUI_THEME_KIND_DARK + 1)

// ��������� ��������� ������
static const struct
{
    gui_theme_kind_t theme;
} GUI_SETTINGS @ FLASH_RW_SECTION = { GUI_THEME_KIND_LIGHT };

// --- ������� ���� --- //

#define GUI_THEME_COLOR_LIGHT_SELECT        LCD_COLOR_MAKE(209, 156, 0)
#define GUI_THEME_COLOR_LIGHT_TEXTFK        LCD_COLOR_MAKE(48, 48, 48)

// --- ������ ���� --- //

#define GUI_THEME_COLOR_DARK_SELECT         LCD_COLOR_MAKE(238, 10, 246)
#define GUI_THEME_COLOR_DARK_TEXTFK         LCD_COLOR_MAKE(200, 200, 200)

// �������� �����
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

// �������� �������� ���� �� ������ ����
static lcd_color_t gui_color_get(gui_color_t color)
{
    return GUI_THEMES[GUI_SETTINGS.theme][color];
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
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background) = 0;
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
    
    // ������ ���������
    void location_set(uint16_t left, uint16_t top)
    {
        left_set(left);
        top_set(top);
    }
    
    // ������� ����� �� �����
    virtual void paint(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // ����� �������� ��������
        if (!paint_needed)
            return;
        paint_self(offsetX + left, offsetY + top, background);
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

// ������� ����� ����������� �������� � ���������
class gui_visual_sizeable_t : public gui_visual_t
{
    // ������ � ������
    uint16_t width, height;
protected:
    // ����������� �� ���������
    gui_visual_sizeable_t(void) : width(0), height(0)
    { }
public:
    // �������� ������
    uint16_t width_get(void) const
    {
        return width;
    }
    
    // ������ ������
    void width_set(uint16_t value)
    {
        if (width == value)
            return;
        width = value;
        parent_refresh();
    }
    
    // �������� ������
    uint16_t height_get(void) const
    {
        return height;
    }
    
    // ������ ������
    void height_set(uint16_t value)
    {
        if (height == value)
            return;
        height = value;
        parent_refresh();
    }
    
    // ������ ��������� � ������
    void bounds_set(uint16_t left, uint16_t top, uint16_t width, uint16_t height)
    {
        location_set(left, top);
        width_set(width);
        height_set(height);
    }
};

// ����� �������� ���������� ���������
class gui_container_t : public gui_visual_sizeable_t
{
    // ������ �������� ���������
    list_template_t<gui_visual_wrap_t> childs;
protected:
    // ����������� �� ���������
    gui_container_t(void)
    {
        width_set(LCD_SIZE_WIDTH);
    }
    
    // ����� �������� ���������
    virtual void paint_childs(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // �������
        offsetX += left_get();
        offsetY += top_get();
        // �����
        for (auto i = childs.head(); i != NULL; i = LIST_ITEM_NEXT(i))
            i->holder.paint(offsetX, offsetY, background);
    }
public:
    // ������� ����� �� �����
    virtual void paint(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // ������� �����
        gui_visual_sizeable_t::paint(offsetX, offsetY, background);
        // ����� �������� ���������
        paint_childs(offsetX, offsetY, background);
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
class gui_edge_panel_t : public gui_container_t
{
protected:
    // ���������� ������� ������ �� �����
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        gfx_rect_solid(x, y, width_get(), height_get(), gui_color_get(background));
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
        virtual void paint_self(uint16_t x, uint16_t y, gui_color_t pbackground)
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
public:
    // ����������� �� ���������
    gui_action_panel_t(void) : 
        enter(IMAGE_BUTTON_ENTER, GUI_COLOR_ACTION_BUTTON_BK_ENTER),
        up(IMAGE_BUTTON_UP, GUI_COLOR_ACTION_BUTTON_BK_NORMAL),
        down(IMAGE_BUTTON_DOWN, GUI_COLOR_ACTION_BUTTON_BK_NORMAL),
        back(IMAGE_BUTTON_BACK, GUI_COLOR_ACTION_BUTTON_BK_BACK)
    {
        // ���������������� ������
        height_set(36);
        top_set(LCD_SIZE_HEIGHT - height_get());
        // ���������������� ������
        enter.location_set(10, 6);
        up.location_set(70, 6);
        down.location_set(130, 6);
        back.location_set(190, 6);
        // ���������� ������ � ��������
        add(enter);
        add(up);
        add(down);
        add(back);
    }
    
    // ������� ����� ��������� �������
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
        virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
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
            left_set(LCD_SIZE_WIDTH);
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
    // ���������� ������� ������ �� �����
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        auto bk = gui_color_get(background);
        auto fk = gui_color_get(GUI_COLOR_STAUS_HEADER_FK);
        // ������� �����
        gui_edge_panel_t::paint_self(x, y, background);
        // ������
        gfx_image(IMAGE_STATUS_ICON, x + 1, y, fk, bk);
        // ��������� ����������
        gfx_string("�������", FONT_HEADER_BOLD, x + height_get() + 3, y + 1, fk, bk);
    }
public:
    // ����������� �� ���������
    gui_status_panel_t(void)
    {
        height_set(24);
        add(form_name);
    }
    
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
class gui_control_t : public gui_visual_sizeable_t
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
    virtual bool key_event(const key_event_args_t &key) = 0;
public:
    // ���������� ���� ��������� ��� ������
    void connect(gui_control_t &to)
    {
        to.neighbors.link(neighbors);
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
    
protected:
    // ���������� ������� �������
    virtual bool key_event(const key_event_args_t &key)
    {
        if (focused == NULL)
            return false;
        if (focused->key_event(key))
            return true;
        // ������� �� ����������
        switch (key.kind)
        {
            case KEY_KIND_UP:
                // ����� �����
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
                // ����� ����
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
    
    // ���������� ������� ������ �� �����
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        auto w = width_get();
        auto h = height_get() - 1;
        // �������
        gfx_rect_solid(x, y + 1, w, h - 1, gui_color_get(background));
        // �������
        auto fk = gui_color_get(GUI_COLOR_FORM_FRAME_FK);
        gfx_line_horz(x, y, w, fk);
        gfx_line_horz(x, y + h, w, fk);
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
        auto top = gui_status_panel.height_get();
        top_set(top);
        height_set(LCD_SIZE_HEIGHT - top - gui_action_panel.height_get());
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
    void key_event(const key_event_args_t &key)
    {
        // ������������ ������ �������
        if (key.state != KEY_STATE_PRESSED)
            return;
        auto form = forms.last();
        assert(form != NULL);
        // �������� ������� �����
        if (form->holder.key_event(key))
            return;
        // ��������� �������� �� ����������� �����
        if (key.kind != KEY_KIND_BACK || forms.count() <= 1)
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

// ������� ����� ������
class gui_button_base_t : public gui_control_t
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
        bounds_set(36, 36, 168, 76);
    }
    
    // ���������� ������� ������ �� �����
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
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
    virtual bool key_event(const key_event_args_t &key)
    {
        if (key.kind != KEY_KIND_ENTER)
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
        gui_control_t::paint(offsetX, offsetY, background);
    }
};

// ����� ���������� ������ �� ���������
class gui_button_notify_t : public gui_button_base_t
{
    // ���������� �����
    notify_proc_ptr handler;
protected:
    // ���������� ������� �����
    virtual void click_event(void)
    {
        handler();
    }
public:
    // ����������� �� ���������
    gui_button_notify_t(const char * _text, notify_proc_ptr _handler) 
        : gui_button_base_t(_text), handler(_handler)
    {
        assert(handler != NULL);
    }
};

// ����� ����� �������
class gui_label_t : public gui_control_t
{
    // �����
    const uint8_t * const font;
    // �����
    char text[32];
protected:
    // ���������� ������� ������ �� �����
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        // ���� ���� � ������
        auto bk = gui_color_get(background);
        auto fk = gui_color_get(GUI_COLOR_LABEL_TEXT_FK);
        // ���
        gfx_rect_solid(x, y, width_get(), height_get(), bk);
        // ����� ������
        auto width = gfx_string_measure(text, font);
        gfx_string(text, 
            font, x + width_get() / 2 - width / 2, y + height_get() / 2 - font_height_get(font) / 2, 
            fk, bk);
    }
    
    // ���������� ������� �������
    virtual bool key_event(const key_event_args_t &key)
    {
        return false;
    }
public:
    // ����������� �� ���������
    gui_label_t(const uint8_t *font) : font(font)
    {
        *text = '\0';
    }
    
    // ������ ������������ �����
    void text_set(const char *source)
    {
        assert(source != NULL);
        if (strcmp(source, text) == 0)
            return;
        strncpy(text, source, ARRAY_SIZE(text));
        refresh();
    }
};

// ������ ������� ��������� �� ���� �������
static const uint16_t GUI_NUMBER_INCREMENT_BY_STEP[] =
{
    1, 1, 1, 1, 1,
    // 5
    5, 5, 5, 5, 5,
    // 30
    10, 10, 10, 10,
    10, 10, 10,
};

// ����� ����� ����� � ������������� ������
class gui_number_t : public gui_control_t
{
    // ��������� �����
    int32_t number;
    // ��������� ����� ��� �����
    int32_t number_input;
    // ����, ����������� ��� ���������� ����
    bool input;
    
    // ��������� ���������
    enum
    {
        // ���������� ������� ��������
        FRACTION = 1000,
        MAXIMUM = (int32_t)(99.9999999 * FRACTION),
    };
protected:
    // ���������� ������� ������ �� �����
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        // ���� ����
        auto bk = gui_color_get(focused_get() ? 
                    (input ? 
                        GUI_COLOR_BUTTON_BK_FOCUSED : 
                        GUI_COLOR_BUTTON_BK_NORMAL) :
                    background);
        // ���� �������
        auto fk = gui_color_get(focused_get() ? 
            GUI_COLOR_FOCUSED_FK : 
            background);
        // �������
        gfx_rect_frame_round(x, y, width_get(), height_get(), fk, gui_color_get(background));
        // ���
        gfx_rect_solid(x, y, width_get(), height_get(), bk);
        // ���������� ������
        char text[16];
        auto temp = input ? number_input : number;
        auto neg = temp < 0;
        if (neg)
            temp = -temp;
        sprintf(text, "%s%d.%03d", neg ? "-" : "", temp / FRACTION, temp % FRACTION); 
        // ����� ������
        auto font = FONT_NUMBER_BOLD;
        auto width = gfx_string_measure(text, font);
        gfx_string(text, 
            font, x + width_get() / 2 - width / 2, y + height_get() / 2 - font_height_get(font) / 2, 
            gui_color_get(GUI_COLOR_BUTTON_TEXT_FK), bk);
    }
    
    // ���������� ������� �������
    virtual bool key_event(const key_event_args_t &key)
    {
        // ����� �����
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
                    // TODO: ����������
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
        // ����� ������
        if (key.kind != KEY_KIND_ENTER)
            return false;
        number_input = number;
        input = true;
        refresh();
        return true;
    }
public:
    // ����������� �� ���������
    gui_number_t(void) : number(0), input(false)
    {
        // ���������������� �� ���������
        height_set(54);
    }
    
    // �������� ������� �����
    int32_t number_get(void) const
    {
        return number;
    }
    
    // ������ ������� �����
    void number_set(int32_t value)
    {
        if (number == value)
            return;
        number = value;
        refresh();
    }
};

// ����� ������������ �����
class gui_bevel_t : public gui_container_t
{
protected:
    // ����� �������� ���������
    virtual void paint_childs(uint16_t offsetX, uint16_t offsetY, gui_color_t background)
    {
        // �������������� ���
        gui_container_t::paint_childs(offsetX, offsetY, GUI_COLOR_BEVEL_BK);
    }

    // ���������� ������� ������ �� �����
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        // �������
        gfx_rect_frame_round(x, y, width_get(), height_get(), gui_color_get(GUI_COLOR_BEVEL_FK), gui_color_get(background));
        // ���
        gfx_rect_solid(x, y, width_get(), height_get(), gui_color_get(GUI_COLOR_BEVEL_BK));
    }
    
    // ���������� ������� �������
    virtual bool key_event(const key_event_args_t &key)
    {
        return false;
    }
public:
    // ����������� �� ���������
    gui_bevel_t(void)
    {
        left_set(18);
        width_set(204);
    }
};

// ������� �����
static class gui_form_main_t : public gui_form_t
{
    // ������ ����� � �������
    gui_button_notify_t dispencer;
    // ������ ����� � ����������
    gui_button_notify_t calibration;
    // ������ ����� � ���� ������
    gui_button_notify_t motor_test;
    
    // ���� � �������
    static void dispencer_enter(void);
    // ���� � ����������
    static void calib_enter(void);
    // ���� � ���� ������
    static void motor_test_enter(void);
public:
    // ����������� �� ���������
    gui_form_main_t(void) : gui_form_t("�������"), 
       dispencer("��������", dispencer_enter),
       calibration("����������", calib_enter),
       motor_test("���� ������", motor_test_enter)
    {
        // ���������� ������
        add(dispencer);
        add(calibration);
        add(motor_test);
        // ����������������
        dispencer.bounds_set(36, 24, 168, 54);
        calibration.bounds_set(36, 102, 168, 54);
        motor_test.bounds_set(36, 180, 168, 54);
        // ������
        dispencer.connect(calibration);
        calibration.connect(motor_test);
        focused_set(&dispencer);
    }
} gui_form_main;

// ����� ����������
static class gui_form_calib_t : public gui_form_t
{
    // ������ ���������� � ����
    gui_button_notify_t zero_button;
    // ������ ���������� � �����
    gui_button_notify_t point_button;
    // �����
    gui_bevel_t point_bevel;
    // ���� �������� �����
    gui_number_t point_value;
    
    // ���� �� ������ ���������� � �����
    static void point_click(void);
public:
    // ����������� �� ���������
    gui_form_calib_t(void) : gui_form_t("����������"),
        zero_button("� ����", weight_calib_zero),
        point_button("� �����", point_click)
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
        
        // ������
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

// ����� ��������
static class gui_form_dispencer_t : public gui_form_t
{
    // ������ ������� ����
    gui_button_notify_t tare_button;
    // ���� �������� �����
    gui_number_t weight_value;
    // ��������� ����������� ������� ������ �������� ����
    static void weight_handler_cb(int32_t current);
public:
    // ���������� ������ �������� ����
    weight_event_handler_t weight_handler;
    
    // ����������� �� ���������
    gui_form_dispencer_t(void) 
        : gui_form_t("��������"), 
          weight_handler(weight_handler_cb),
          tare_button("����", weight_tare)
    {
        // ������ ����
        tare_button.top_set(74);
        // ���� ������ ����
        weight_value.top_set(10);
        weight_value.width_set(LCD_SIZE_WIDTH);
        // ���������� ���������
        add(weight_value);
        add(tare_button);
        // �����
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

// ����� ����� ������
static class gui_form_motor_test_t : public gui_form_t
{
    // �����
    gui_bevel_t bevel;
    // ������� ���������
    gui_label_t header;
    // ������� ��������
    gui_label_t speed;
    
    // ������� �������� ��������
    void speed_set(uint8_t value)
    {
        pwm_width_set(value);
        char temp[32];
        sprintf(temp, "%d%%", value);
        speed.text_set(temp);
    }
    
protected:
    // ���������� ������� ������ �� �����
    virtual void paint_self(uint16_t x, uint16_t y, gui_color_t background)
    {
        // ������� �����
        gui_form_t::paint_self(x, y, background);
        // ����� �������� ������
        gfx_image(IMAGE_MISC_MOTOR, x + 56, y + 10, gui_color_get(GUI_COLOR_FORM_FRAME_FK), gui_color_get(background));
    }
    
    // ���������� ������� �������
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
    // ����������� �� ���������
    gui_form_motor_test_t(void) : gui_form_t("���� ������"), 
        header(FONT_HEADER_BOLD), speed(FONT_NUMBER_BOLD)
    {
        // �����
        bevel.bounds_set(36, 148, 168, 76);
        add(bevel);
        // ���������
        header.text_set("�������� ��������");
        header.bounds_set(0, 0, 168, 22);
        bevel.add(header);
        // ��������
        speed_set(0);
        speed.bounds_set(0, 22, 168, 53);
        bevel.add(speed);
    }
} gui_form_motor_test;

void gui_form_main_t::motor_test_enter(void)
{
    gui_form_stack.add(gui_form_motor_test);
}

// ����� ������ ����
static class gui_form_theme_t : public gui_form_t
{
    // �����
    gui_bevel_t bevel;
    // ������� ���������
    gui_label_t header;
    // ������ ���
    gui_button_notify_t light, dark;
    
    // ����� ����
    static void select(gui_theme_kind_t theme)
    {
        // ������ � ���������
        flash_write(&GUI_SETTINGS.theme, &theme, sizeof(theme));
        // ���� � ������� �����
        gui_form_stack.add(gui_form_main);
        // ���������� ������ ��������
        gui_action_panel.refresh();
    }
    
    // ����� ������� ����
    static void select_light(void)
    {
        select(GUI_THEME_KIND_LIGHT);
    }
    
    // ����� ������ ����
    static void select_dark(void)
    {
        select(GUI_THEME_KIND_DARK);
    }
public:
    // ����������� �� ���������
    gui_form_theme_t(void) 
        : gui_form_t("����� ����"), 
          header(FONT_HEADER_BOLD),
          light("�������", select_light),
          dark("������", select_dark)
    {
        // ���������
        header.text_set("����� ����");
        header.bounds_set(0, 23, LCD_SIZE_WIDTH, 22);
        add(header);
        // �����
        bevel.bounds_set(36, 68, 168, 156);
        add(bevel);
        // ������
        light.bounds_set(18, 16, 132, 54);
        bevel.add(light);
        dark.bounds_set(18, 86, 132, 54);
        bevel.add(dark);
        // �����
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

// ������ ����������� ����������
static timer_notify_t gui_paint_timer([](void)
{
    gui_action_panel.repaint();
    gui_status_panel.repaint();
    gui_form_stack.repaint();
});

// ���������� ����� ��������� ������
static key_event_handler_t gui_key_event([](const key_event_args_t &key)
{
    gui_form_stack.key_event(key);
    gui_action_panel.key_event(key);
});

void gui_init(void)
{
    // ��������� �����
    gui_form_stack.add(gui_form_theme);
    gui_form_stack.add(gui_form_main);
    // 25 ������ � �������
    gui_paint_timer.start_hz(25, TIMER_FLAG_LOOP);
    // ���������� � ������ ������������ ������� ������
    key_event_list.add(gui_key_event);
    // ������������� �� ��������� �������� ����
    weight_event_list.add(gui_form_dispencer.weight_handler);
}
