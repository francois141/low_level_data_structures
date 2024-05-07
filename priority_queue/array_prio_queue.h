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

    static std::vector<T> getBuffer() {}

private:
    std::vector<T> array;
    std::mutex lock;
};

template<typename T>
class LockFreeBin {
public:
    // TODO: We assume no overflow for the moment
    LockFreeBin() : index(0) {};

    // TODO: This doesn't work, we need to add transactions
    void Put(T element) {
        int idx = index;
        while(!index.compare_exchange_weak(idx, idx+1)) {}
        array[idx] = element;
    }

    // TODO: This doesn't work, we need to add transactions
    std::optional<T> Get() {
        int idx = index;
        do {
            if(idx == 0) {
                return -1;
            }
        } while((!index.compare_exchange_weak(idx, idx-1)));

        return array[idx];
    }

    static std::vector<T> getBuffer() {}

private:
    std::array<T, 51200> array;
    std::atomic<int> index;
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

    std::optional<T> pop() {
        for(int i = 0; i < size-1;i++) {
            std::optional<T> out = this->bins[i].Get();
            if(out.has_value()) {
                return out.value();
            }
        }
        return this->bins.back().Get();
    }

private:
    int size;
    std::vector<LockedBin<T>> bins;

};



#endif //ARRAY_PRIO_QUEUE_H
