#include <iostream>
#include <functional>
#include <thread>

#include "lock_free_queue/Stack.hpp"
#include "lock_free_queue/Element.hpp"

#include "fifo_queue_sp_sc/fifo_queue_sp_sc.hpp"

#include "priority_queue/array_prio_queue.h"

#define ANKERL_NANOBENCH_IMPLEMENT

#include <nanobench.h>

int main() {

    auto b_basic_prio_queue = [number_threads = 2, size = 100000]() -> void {
        ArrayPriorityQueue<LockedBin<int>> priorityQueue(size);

        for (int i = 0; i < number_threads; i++) {
            std::thread producer([&]() -> void {
                for (int j = 0; j < size; j++) {
                    priorityQueue.push(j, 0);
                }
            });

            std::thread consumer([&]() -> void {
                for (int j = 0; j < size; j++) {
                    priorityQueue.pop();
                }
            });

            producer.join();
            consumer.join();
        }
    };

    auto b_basic_prio_queue_lock_free = [number_threads = 2, size = 100000]() -> void {
        ArrayPriorityQueue<LockFreeBin<int>> priorityQueue(size);

        for (int i = 0; i < number_threads; i++) {
            std::thread producer([&]() -> void {
                for (int j = 0; j < size; j++) {
                    priorityQueue.push(j, 0);
                }
            });

            std::thread consumer([&]() -> void {
                for (int j = 0; j < size; j++) {
                    priorityQueue.pop();
                }
            });

            producer.join();
            consumer.join();
        }
    };

    ankerl::nanobench::Bench().minEpochIterations(5).run("basic_prio_queue", b_basic_prio_queue);
    ankerl::nanobench::Bench().minEpochIterations(5).run("basic_prio_queue_lockfree", b_basic_prio_queue_lock_free);

    return 0;
}

// Turbo boost
// echo "1" | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
