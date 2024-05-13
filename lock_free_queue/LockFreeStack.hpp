//
// Created by François Costa on 18.04.2024.
//

#ifndef STACK_H
#define STACK_H

#include <atomic>
#include <algorithm>
#include <chrono>
#include <thread>
#include <stack>

#include "Element.hpp"

template <typename T, int backoffMin, int backoffMax>
class LockFreeStack {
public:
    // Make sure data structure is lock free
    static_assert(std::atomic<Element<T> *>::is_always_lock_free);

    // Backoff must be always positive
    static_assert(backoffMin > 0);
    static_assert(backoffMax > 0);

    LockFreeStack() {
        head.store(new Element<T>(nullptr, 0));
    };

    LockFreeStack(const LockFreeStack &other) = delete;

    LockFreeStack(LockFreeStack &&other) noexcept = delete;

    LockFreeStack & operator=(const LockFreeStack &other) = delete;

    LockFreeStack & operator=(LockFreeStack &&other) noexcept = delete;

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
                return current;
            }

            sleep_and_increaseBackoff(currentBackoff);
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

template <typename T>
class NaiveStack {
public:

    NaiveStack() {};

    NaiveStack(const NaiveStack &other) = delete;

    NaiveStack(NaiveStack &&other) noexcept = delete;

    NaiveStack & operator=(const NaiveStack &other) = delete;

    NaiveStack & operator=(NaiveStack &&other) noexcept = delete;

    void Push(Element<T> *element) {
        std::lock_guard<std::mutex> lock(mutex);
        internalStack.push(element);
    }

    Element<T>* Pop() {
        std::lock_guard<std::mutex> lock(mutex);
        if(internalStack.empty()) {
            return nullptr;
        }

        auto val = internalStack.top();
        internalStack.pop();

        return val;
    }

private:
    std::mutex mutex;
    std::stack<Element<T>*> internalStack;
};

template<typename T>
void benchmark_stack(T& stack_to_benchmark) {
    std::function<void()> pusher = [&stack_to_benchmark]() -> void {
        for (int i = 0; i < 100; i++) {
            stack_to_benchmark.Push(new Element<int>(nullptr, 0));
        }
    };

    std::function<void()> popper = [&stack_to_benchmark]() -> void {
        for (int i = 0; i < 100; i++) {
            Element<int> *pointer = stack_to_benchmark.Pop();
        }
    };


    // Launch both at the same time
    std::thread push_thread(pusher);
    std::thread pop_thread(popper);

    push_thread.join();
    pop_thread.join();
}

#endif //STACK_H
