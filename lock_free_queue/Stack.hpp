//
// Created by François Costa on 18.04.2024.
//

#ifndef STACK_H
#define STACK_H

#include <atomic>
#include <algorithm>
#include <chrono>
#include <thread>

#include "Element.hpp"

template <typename T, int backoffMin, int backoffMax>
class Stack {
public:
    // Make sure data structure is lock free
    static_assert(std::atomic<Element<T> *>::is_always_lock_free);

    // Backoff must be always positive
    static_assert(backoffMin > 0);
    static_assert(backoffMax > 0);

    Stack() {
        head.store(new Element<T>(nullptr, 0));
    };

    Stack(const Stack &other) = delete;

    Stack(Stack &&other) noexcept = delete;

    Stack & operator=(const Stack &other) = delete;

    Stack & operator=(Stack &&other) noexcept = delete;

    void Push(Element<T> *element) {
        element->previous_element() = head;
        int currentBackoff = backoffMin;

        while(!head.compare_exchange_weak(element->previous_element(), element)) {
            sleep_and_increaseBackoff(currentBackoff);
        }
    }

    Element<T>* Pop() {
        Element<T>* current = head.load();

        int currentBackoff = backoffMin;
        while(true) {
            if(current == nullptr) {
                return nullptr;
            }

            if(head.compare_exchange_weak( current, current->previous_element())) {
                sleep_and_increaseBackoff(currentBackoff);

                return current;
            }
        }
    }

private:
    std::atomic<Element<T> *> head{};

    inline void sleep_and_increaseBackoff(int& currentBackoff) {
        currentBackoff *= 2;
        currentBackoff = std::min(currentBackoff, backoffMax);

        std::this_thread::sleep_for(std::chrono::nanoseconds(currentBackoff));
    }
};


int run_lock_free_stack() {
    auto lock_free_stack = Stack<int, 4, 100>();

    std::function<void()> pusher = [&lock_free_stack]() -> void {
        for (int i = 0; i < 100; i++) {
            lock_free_stack.Push(new Element<int>(nullptr, 0));
        }
    };

    std::function<void()> pusher2 = [&lock_free_stack]() -> void {
        for (int i = 0; i < 100; i++) {
            lock_free_stack.Push(new Element<int>(nullptr, 0));
        }
    };

    std::function<void()> popper = [&lock_free_stack]() -> void {
        for (int i = 0; i < 100; i++) {
            Element<int> *pointer = lock_free_stack.Pop();
            delete pointer;
        }
    };

    std::function<void()> popper2 = [&lock_free_stack]() -> void {
        for (int i = 0; i < 100; i++) {
            Element<int> *pointer = lock_free_stack.Pop();
            delete pointer;
        }
    };

    // Launch both at the same time
    std::thread push_thread(pusher);
    std::thread push_thread2(pusher2);

    std::thread pop_thread(popper);
    std::thread pop_thread2(popper2);

    push_thread.join();
    push_thread2.join();
    pop_thread.join();
    pop_thread2.join();

    std::cout << "Computation is done" << std::endl;

    return 0;
}

#endif //STACK_H
