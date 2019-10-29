#ifndef __CALLBACK_H
#define __CALLBACK_H

#include "list.h"

// Класс элемента списка функции обратного вызова
template <typename ARGS>
class callback_t : list_item_t
{
    // Прототип функции обратного вызова
    typedef void (* callback_proc_ptr)(ARGS args);
    // Вызываемая функция
    callback_proc_ptr routine;
public:
    // Класс списка функций обратного вызова
    class list_t : list_template_t<callback_t<ARGS>>
    {
    public:
        // Вызов цепочки функций обратного вызова
        void operator ()(ARGS args) const
        {
            for (auto item = this->head(); item != NULL; item = LIST_ITEM_NEXT(item))
                (*item)(args);
        }
        
        // Добавление функции в цепочку
        void add(callback_t<ARGS> &item)
        {
            item.link(*this);
        }
    };
public:
    // Конструктор по умолчанию
    callback_t(callback_proc_ptr _routine) : routine(_routine)
    {
        assert(routine != NULL);
    }
    
    // Сравнение
    bool operator == (const callback_t &a) const
    {
        return routine == a.routine;
    }
    
    // Присвоение
    callback_t& operator = (const callback_t& a)
    {
        routine = a.routine;
        return *this;
    }
    
    // Вызов
    void operator ()(ARGS args) const
    {
        routine(args);
    }
};

#endif // __CALLBACK_H
