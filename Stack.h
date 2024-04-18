//
// Created by François Costa on 18.04.2024.
//

#ifndef STACK_H
#define STACK_H
#include <atomic>

#include "Element.h"

template <typename T>
class Stack {
public:
    // Make sure data structure is lock free
    static_assert(std::atomic<Element<T> *>::is_always_lock_free);

    Stack() {
        head.store(new Element<T>(nullptr, 0));
    };

    void Push(Element<T> *element) {
        element->previous_element() = head;
        while(!head.compare_exchange_weak(element->previous_element(), element));
    }

    Element<T>* Pop() {
        Element<T>* current = head.load();

        while(true) {
            if(current == nullptr) {
                return nullptr;
            }

            if(head.compare_exchange_weak( current, current->previous_element())) {
                return current;
            }
        }
    }

private:
    std::atomic<Element<T> *> head{};
};

#endif //STACK_H
