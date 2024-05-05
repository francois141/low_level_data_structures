//
// Created by fran-ois-costa on 05/05/24.
//

#ifndef ARRAY_PRIO_QUEUE_H
#define ARRAY_PRIO_QUEUE_H

#include <vector>
#include <mutex>
#include <optional>
#include <array>
#include <assert.h>
#include <thread>

template<typename T>
class Bin {
public:
    Bin() = default;

    void Put(T element) {
        const std::lock_guard<std::mutex> guard(this->lock);
        array.push_back(element);
    }

    std::optional<T> Get() {
        const std::lock_guard<std::mutex> guard(this->lock);
        if(this->array.empty()) {
            return -1;
        }

        T output = array.back();
        array.pop_back();
        return output;
    }

private:
    std::mutex lock;
    std::vector<T> array;
};


template<typename T>
class ArrayPriorityQueue {
public:
    ArrayPriorityQueue(int size) : bins(std::vector<Bin<T>>(size)), size(size) {};

    void push(int key, T value) {
        assert(0 <= key && key < size);
        this->bins[key].Put(value);
    }

    std::optional<T> pop(int key) {
        return this->bins[key].Get();
    }

private:
    int size;
    std::vector<Bin<T>> bins;
};

double run_benchmark_2(int nb_times = 150) {
    int size = 100;
    int nb_runs = 100;
    ArrayPriorityQueue<int> priorityQueue(size);

    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < nb_times;i++) {
        std::thread producer([&](int idx) -> void {
            for(int k = 0; k < nb_runs;k++) {
                for(int j = 0; j < size;j++) {
                    priorityQueue.push(j, 0);
                }
            }
        }, 1);

        std::thread consumer([&, nb_runs](int idx) -> void {
            for(int k = 0; k < nb_runs;k++) {
                for(int j = 0; j < size;j++) {
                    priorityQueue.pop(j);
                }
            }
        }, 1);

        producer.join();
        consumer.join();
    }

    auto end = std::chrono::high_resolution_clock::now();

    double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;

    return time_taken;
}


#endif //ARRAY_PRIO_QUEUE_H
