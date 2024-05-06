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


template <typename T>
concept Bin = requires(T t, decltype(T::getBuffer())::value_type v) {
    { t.Get() } -> std::same_as<std::optional<decltype(v)>>;
    { t.Put(v) } -> std::same_as<void>;
};


template<typename T>
class LockedBin {
public:
    LockedBin() = default;

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

    // TODO: Find better way
    static std::vector<T> getBuffer() {}

private:
    std::vector<T> array;
    std::mutex lock;
};

template<Bin B>
class ArrayPriorityQueue {
public:
    using T = decltype(B::getBuffer())::value_type;

    ArrayPriorityQueue(int size) : bins(std::vector<LockedBin<T>>(size)), size(size) {};

    void push(int key, T value) {
        assert(0 <= key && key < size);
        this->bins[key].Put(value);
    }

    std::optional<T> pop(int key) {
        return this->bins[key].Get();
    }

private:
    int size;
    std::vector<LockedBin<T>> bins;

};

double run_benchmark_2(int nb_times = 150) {
    int size = 100;
    int nb_runs = 100;
    ArrayPriorityQueue<LockedBin<int>> priorityQueue(size);

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