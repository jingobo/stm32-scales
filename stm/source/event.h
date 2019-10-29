#ifndef __EVENT_H
#define __EVENT_H

#include "list.h"

// Предварительное объявление
class event_t;
// Элемент оболочка для события в списках
typedef list_item_template_t<event_t> event_wrap_t;

// Класс базового события
class event_t
{
    // Элемент связанного списка
    event_wrap_t item;
    // Указывает, что элемент добавлен и ожидает обработки
    bool pending;
protected:
    // Обработка события
    virtual void execute(void) = 0;
public:
    // Конструктор по умолчанию
    event_t(void) : pending(false), item(*this)
    { }
    
    // Генерация события
    void raise(void);
    
    // Цикл обработки событий
    static __noreturn void loop(void);
};

// Событие с функцией обратного вызова
class event_notify_t : public event_t
{
    // Вызываемая функция
    notify_proc_ptr proc;
protected:
    // Обработка события
    virtual void execute(void)
    {
        proc();
    }
public:
    // Конструктор по умолчанию
    event_notify_t(notify_proc_ptr _proc) : proc(_proc)
    {
        assert(proc != NULL);
    }
};

#endif // __EVENT_H
