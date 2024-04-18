#include <iostream>
#include <assert.h>
#include <functional>
#include <thread>

#include "Stack.h"
#include "Element.h"

int main()
{
    auto lock_free_stack = Stack<int>();

    std::function<void()> pusher = [&lock_free_stack]() -> void {
        for(int i = 0; i < 100;i++) {
            lock_free_stack.Push(new Element<int>(nullptr, 0));
        }
    };

    std::function<void()> popper = [&lock_free_stack]() -> void {
        for(int i = 0; i < 100;i++) {
            Element<int> *pointer = lock_free_stack.Pop();
            delete pointer;
        }
    };

    // Launch both at the same time
    std::thread push_thread(pusher);
    std::thread pop_thread(popper);

    push_thread.join();
    pop_thread.join();

    std::cout << "Computation is done" << std::endl;

    return 0;
}
