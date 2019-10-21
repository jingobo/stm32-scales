#include "gui.h"
#include "list.h"
#include "timer.h"

// Класс базового визуального элемента
class gui_visual_t : list_item_t
{
    friend class gui_container_t;
    // Флаг необходимости перерисовки
    bool paint_needed;
protected:
    // Конструктор по умолчанию
    gui_visual_t(void) : paint_needed(true)
    { }

    // Обработчик события вывода на экран
    virtual void paint_handler(void) = 0;
public:
    // Событие смены состояния кнопки
    virtual void key_event(key_kind_t key, key_state_t state)
    { }
    
    // Событие вывод на экран
    void paint_event(void)
    {
        if (!paint_needed)
            return;
        paint_handler();
        paint_needed = true;
    }
    
    // Обновление состояния элемента
    virtual void refresh()
    {
        paint_needed = true;
    }
};

// Класс базового контейнера
class gui_container_t : public gui_visual_t
{
    // Список дочерних элементов
    list_template_t<gui_visual_t> childs;
    
};

class gui_overlay_t
{
    
};

void gui_init(void)
{ 
}

void gui_key_event(key_kind_t key, key_state_t state)
{
    
}
