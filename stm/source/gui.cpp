#include "gui.h"
#include "gfx.h"
#include "lcd.h"
#include "list.h"
#include "font.h"
#include "image.h"
#include "timer.h"

// ���� ����
enum gui_color_t
{
    // ��� �������� ������� �������
    GUI_COLOR_WORKSPACE_BK,
    // ��� ������� �� �������� �������
    GUI_COLOR_EDGE_PANEL_BK,

    // ������� ������� ��������� (��� �������)
    GUI_COLOR_STAUS_HEADER_FK,
    
    // ������� ������ ������ �������� (�� ���������)
    GUI_COLOR_ACTION_BUTTON_FK,
    // ��� ������ ������ �������� (�� ���������)
    GUI_COLOR_ACTION_BUTTON_BK_NORMAL,
    // ��� ������ ������ �������� (������)
    GUI_COLOR_ACTION_BUTTON_BK_PRESSED,
    // ��� ������ ������ �������� (����)
    GUI_COLOR_ACTION_BUTTON_BK_ENTER,
    // ��� ������ ������ �������� (�����)
    GUI_COLOR_ACTION_BUTTON_BK_BACK,
    
    // ���������� ������
    GUI_COLOR_COUNT,
};

// �������� ���� 
typedef lcd_color_t gui_theme_t[GUI_COLOR_COUNT];

// �������� �����
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

// �������� �������� ���� �� ������ ����
static lcd_color_t gui_color_get(gui_color_t color)
{
    return GUI_THEMES[0][color];
}

// ��������������� ����������
class gui_container_t;

// ����� �������� ����������� ��������
class gui_visual_t : list_item_t
{
    friend class gui_container_t;
    
    // ����������
    uint16_t left, top;
    // ��������� �� ������������ ���������
    gui_container_t *parent;
    // ���� ������������� �����������
    bool paint_needed;
protected:
    // ����������� �� ���������
    gui_visual_t(void) : 
        paint_needed(true), parent(NULL), left(0), top(0)
    { }
    
    // �������� ��������� �� ������������ ������� TODO: ��� ��������
    gui_container_t * parent_get(void) const
    {
        return parent;
    }
    
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
        parent = NULL;
        unlink();
    }
};

// ����� �������� ���������� ���������
class gui_container_t : public gui_visual_t
{
    // ������ �������� ���������
    list_template_t<gui_visual_t> childs;
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
            i->paint(offsetX, offsetY, background);
    }
    
    // ���������� ��������� ��������
    virtual void refresh()
    {
        // ������� �����
        paint_needed = true;
        // ����� ��������
        for (auto i = childs.head(); i != NULL; i = LIST_ITEM_NEXT(i))
            i->refresh();
    }
    
    // ���������� ��������� ��������
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

// ����� ������� ������ �� �������� �������
class gui_edge_panel_t : protected gui_container_t
{
protected:
    // �������� ������ ������
    virtual uint8_t height_get(void) const = 0;

    // ���������� ������� ������ �� �����
    virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t background)
    {
        gfx_rect_solid(x, y, LCD_SIZE_WIDTH, height_get(), gui_color_get(background));
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

// ����� �������
class gui_label_base_t : public gui_visual_t
{
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
        // ����� ���������
        struct state_t
        {
            // ������
            bool pressed : 1;
            // ������
            bool enabled : 1;
            
            // ����������� �� ���������
            state_t(void) : pressed(false), enabled(true)
            { }
        } state;
    protected:    
        // ���������� ������� ������ �� �����
        virtual void paint_handler(uint16_t x, uint16_t y, gui_color_t pbackground)
        {
            // ���������� ���� ����
            auto bk = gui_color_get(state.pressed ? GUI_COLOR_ACTION_BUTTON_BK_PRESSED  : background);
            
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
            : image(_image), background(_background)
        { }
        
        // ��������� ��������� �������
        void pressed_set(bool value)
        {
            if (state.pressed == value)
                return;
            state.pressed = value;
            refresh();
        }

        // ��������� ��������� �������
        void enabled_set(bool value)
        {
            if (state.enabled == value)
                return;
            state.enabled = value;
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
        return 36;
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
protected:
    // �������� ������ ������
    uint8_t height_get(void) const
    {
        return 24;
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
        gfx_string("�������", FONT_HEADER_BOLD, x + 24 + 3, y + 1, fk, bk);
    }

} gui_status_panel;

// ������ ����������� ����������
static timer_callback_t gui_paint_timer([](void)
{
    gui_action_panel.repaint();
    gui_status_panel.repaint();
});

void gui_init(void)
{
    // 25 ������ � �������
    gui_paint_timer.start_hz(25, TIMER_FLAG_LOOP);
}

void gui_key_event(key_kind_t key, key_state_t state)
{
    gui_action_panel.key_event(key, state);
}
