#include <iostream>
#include <functional>
#include <thread>

#include "lock_free_queue/Stack.hpp"
#include "lock_free_queue/Element.hpp"

#include "fifo_queue_sp_sc/fifo_queue_sp_sc.hpp"

#include "priority_queue/array_prio_queue.h"
#include "simd/Argmin.h"

#define ANKERL_NANOBENCH_IMPLEMENT

#include <nanobench.h>

int main() {

    int size = 10000;
    int number_threads = 5;

    ArrayPriorityQueue<LockedBin<int>> priorityQueue1(size);
    ArrayPriorityQueue<LockFreeBin<int>> priorityQueue2(size);

    auto b_basic_prio_queue = [&]() -> void {
        for (int i = 0; i < number_threads; i++) {
            std::thread producer([&]() -> void {
                for (int j = 0; j < size; j++) {
                    priorityQueue1.push(j, 0);
                }
            });

            std::thread consumer([&]() -> void {
                for (int j = 0; j < size; j++) {
                    priorityQueue1.pop();
                }
            });

            producer.join();
            consumer.join();
        }
    };

    auto b_basic_prio_queue_lock_free = [&]() -> void {
        for (int i = 0; i < number_threads; i++) {
            std::thread producer([&]() -> void {
                for (int j = 0; j < size; j++) {
                    priorityQueue2.push(j, 0);
                }
            });

            std::thread consumer([&]() -> void {
                for (int j = 0; j < size; j++) {
                    priorityQueue2.pop();
                }
            });

            producer.join();
            consumer.join();
        }
    };

    ankerl::nanobench::Bench().minEpochIterations(15).run("basic_prio_queue", b_basic_prio_queue);
    ankerl::nanobench::Bench().minEpochIterations(15).run("basic_prio_queue_lockfree", b_basic_prio_queue_lock_free);

    return 0;
}

// Turbo boost
// echo "1" | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
