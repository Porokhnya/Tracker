#pragma once

#include <Arduino.h>

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// реализация простого двунаправленного связного списка
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class T>
struct Link
{
  T data;
  Link<T>* next;
  Link<T>* prev;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class T>
class LinkList
{
   private:
       Link<T>* first;
       Link<T>* last;
   public:
    LinkList()
    {
        first = NULL;
        last  = NULL;
    }
    ~LinkList()
    {
        while(first != last)
        {
            Link<T>* temp = first;
            first = first ->prev;
            delete temp;
        }
        delete last;
    }

    Link<T>* head()
    {
      return first; 
    }

    Link<T>* tail()
    {
      return last; 
    }    
    
   void push_back( T data )        //добавление данных в список
   {
    Link<T>* newlink = new Link<T>;
    newlink->data = data;
 
    if(first)
    {
          newlink ->prev = first;
          first ->next =  newlink;
    }
 
    else
        last = newlink;
 
    newlink ->next = last;
    last ->prev = newlink;
    first = newlink;
   }

  void push_front( T data)        //добавление данных в список
   {
    Link<T>* newlink = new Link<T>;
    newlink->data = data;
 
    if(last)
    {
          newlink ->next = last;
          last ->prev =  newlink;
    }
 
    else
        first = newlink;
 
    newlink ->prev = first;
    first ->next = newlink;
    last = newlink;
   }
   
 T pop_front()   //вытолкнуть первое звено из списка
  {
   Link<T>* temp = first;
   T result = temp->data;
   deleteLink(first);
   return result;
 
  }
  T pop_back()                //вытолкнуть последнее звено из списка
  {
    Link<T>* temp = last;
    T result = temp->data;
    deleteLink(last);
    return result;
  }

 void deleteLink(Link<T>* temp)   //удаление звена из списка
  {
     if(first)                   //если в списке есть данные
     {
      if(temp ->next == temp)    //если в списке единственный элемент
        {
          first = NULL;
          last  = NULL;
        }
      else
        {
          if(temp == last)     //если эдемент на удаление последний
             last = last ->next;
 
         if(temp == first)     //если элемент на удаление первый
             first = first ->prev;
 
        (temp ->next) ->prev = temp ->prev;
        (temp ->prev) ->next = temp ->next;
 
        delete temp;
        }
   }
  }

  LinkList operator=( LinkList& l)
  {
   if(l.first)
    {
      Link<T>* temp = l.first;
       do
       {
       pushfront( temp->data);
       temp = temp->prev;
       } while(temp  != l.first);
     }
 
    return *this;
  }

   void clear()            //очистить список
   {
       while(first ->prev != last)
        {
            Link<T>* temp = first ->prev;
            first = first ->prev;
            delete temp;
        }
         last = NULL;
         first = NULL;
   }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

