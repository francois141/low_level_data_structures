#include <iostream>
#include <functional>
#include <thread>

#include "lock_free_queue/Stack.hpp"
#include "lock_free_queue/Element.hpp"

#include "fifo_queue_sp_sc/fifo_queue_sp_sc.hpp"

#include "priority_queue/array_prio_queue.h"

#define ANKERL_NANOBENCH_IMPLEMENT

#include <nanobench.h>

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


int test_fifo_queue() {

    std::cout << "===== Warm up =====" << std::endl;
    run_benchmark<Fifo<MyBuffer<4096>>>();

    double total = 0.0;
    int nb_runs = 5;

    std::cout << "===== Benchmark running =====" << std::endl;
    for (int i = 0; i < nb_runs; i++) {
        std::cout << "Iteration : " << i << std::endl;
        double naive = run_benchmark<Fifo<MyBuffer<4096>>>();
        double optimized = run_benchmark<Fifo2<MyBuffer<4096>>>();
        double speedup = naive / optimized;
        total += speedup;
    }

    std::cout << "Speedup : " << total / nb_runs << std::endl;

    return 0;
}


int main() {

    auto b_basic_prio_queue = [number_threads = 10, size = 10000]() -> void {
        ArrayPriorityQueue<LockedBin<int>> priorityQueue(size);

        for (int i = 0; i < number_threads; i++) {
            std::thread producer([&]() -> void {
                for (int j = 0; j < size; j++) {
                    priorityQueue.push(j, 0);
                }
            });

            std::thread consumer([&]() -> void {
                for (int j = 0; j < size; j++) {
                    priorityQueue.pop(j);
                }
            });

            producer.join();
            consumer.join();
        }
    };

    auto b_basic_prio_queue_2 = [number_threads = 10, size = 10000]() -> void {
        ArrayPriorityQueue<LockedBin<int>> priorityQueue(size);

        for (int i = 0; i < number_threads; i++) {
            std::thread producer([&]() -> void {
                for (int j = 0; j < size; j++) {
                    priorityQueue.push(j, 0);
                }
            });

            std::thread consumer([&]() -> void {
                for (int j = 0; j < size; j++) {
                    priorityQueue.pop(j);
                }
            });

            producer.join();
            consumer.join();
        }
    };

    ankerl::nanobench::Bench().run("basic_prio_queue", b_basic_prio_queue);
    ankerl::nanobench::Bench().run("basic_prio_queue_2", b_basic_prio_queue_2);
    return 0;
}

// Turbo boost
// echo "1" | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
