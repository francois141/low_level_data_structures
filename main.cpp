#include <iostream>
#include <functional>
#include <thread>
#include <string>

#include "lock_free_queue/LockFreeStack.hpp"
#include "lock_free_queue/Element.hpp"

#include "fifo_queue_sp_sc/fifo_queue_sp_sc.hpp"

#include "priority_queue/array_prio_queue.h"


#if defined(__x86_64__)
/* 64 bit detected */
#include "simd/Argmin.h"

#endif



#define ANKERL_NANOBENCH_IMPLEMENT

#include <nanobench.h>

using namespace std;

void benchmark_prio_queue() {
    std::cout << "### Priority queue benchmark" << std::endl;

    int size = 1000;
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
}

#if defined(__x86_64__)
void benchmark_simd_min() {
    std:cout << "### Benchmark minimum simd" << std::endl;

    const unsigned int size = 100 *4096;

    Argmin<size> minAlgorithm = Argmin<size>();

    ankerl::nanobench::Bench().minEpochIterations(15).run("naive_argmin", [&]() {
        minAlgorithm.naive();
    });

    ankerl::nanobench::Bench().minEpochIterations(15).run("simd_naive_argmin", [&]() {
        minAlgorithm.simd_basic();
    });
}

#endif


void benchmark_stack() {
    std::cout << "### Benchmark stack" << std::endl;

    auto lock_stack = NaiveStack<int>();
    auto lock_free_stack = LockFreeStack<int,4,100>();

    ankerl::nanobench::Bench().minEpochIterations(15).run("naive_stack", [&]() {
        benchmark_stack(lock_stack);
    });

    ankerl::nanobench::Bench().minEpochIterations(15).run("lock_free_stack", [&]() {
        benchmark_stack(lock_free_stack);
    });
}

int main() {
    // sudo $(which pyperf) system tune

#if defined(__x86_64__)
    // Simd
    benchmark_simd_min();
#endif

    // Priority queue
    benchmark_prio_queue();

    // Stack
    benchmark_stack();

    return 0;
}

// Turbo boost
// echo "1" | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
