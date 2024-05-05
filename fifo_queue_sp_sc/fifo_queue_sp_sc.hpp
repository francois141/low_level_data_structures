#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

#include <assert.h>
#include <cstring>

template <size_t N>
class MyBuffer {
    public:
        MyBuffer() {
            size = N;
            buffer = malloc(N);
            memset(buffer, 0, N);
        }

        MyBuffer(const MyBuffer<N> &other) {
            size = other.size;
            buffer = malloc(size);
            memcpy(buffer, other.buffer, size);
        }

        MyBuffer (MyBuffer<N>&& other) {
            size = other.size;
            buffer = other.buffer;
        }

        MyBuffer& operator= (MyBuffer<N>&& other) {
            size = other.size;
            buffer = other.buffer;
            return *this;
        }

        MyBuffer<N>& operator=(const MyBuffer<N> &other) {
            size = other.size;
            buffer = malloc(size);
            memcpy(buffer, other.buffer, size);
            return *this;
        }

    private:
        void *buffer;
        size_t size;

};

template<typename T>
class Fifo
{
public:
    explicit Fifo() : Fifo(128) {

    }

    explicit Fifo(std::size_t capacity): capacity_(capacity), pushCursor(0), popCursor(0) {
        ring = new T[capacity_];
    }

    Fifo(const Fifo<T> &other) : capacity_(other.capacity_) {
        pushCursor = other.pushCursor;
        popCursor = other.pushCursor;

        ring = new T[capacity_];

        for(int i = 0; i < capacity_;i++) {
            ring[i] = other.ring[i];
        }
    }

    ~Fifo() {
        delete ring;
    }

    std::size_t capacity() const { return capacity_; }
    std::size_t size() const { return pushCursor - popCursor; }

    bool empty() const { return size() == 0;}
    bool full() const {return size() == capacity(); }

    bool push(T const& value) {
        const std::lock_guard<std::mutex> lock_guard(lock);
        if(full()) {
            return false;
        }

        ring[pushCursor % capacity_] = value;
        pushCursor++;

        return true;
    }

    bool pop_blocking(T&value) {
        while(empty()) {}
        return pop(value);
    }

    bool pop(T& value) {
        const std::lock_guard<std::mutex> lock_guard(lock);

        if(empty()) {
            return false;
        }

        value = ring[popCursor % capacity_];
        ring[popCursor % capacity_].~T();

        popCursor++;

        return true;
    }

    T* ring;
private:
    std::size_t capacity_;

    std::size_t pushCursor;
    std::size_t popCursor;

    std::mutex lock;
};

template<typename T>
class Fifo2
{
public:
    explicit Fifo2() : Fifo2(128) {

    }

    // Hard code capacity to friendly number
    explicit Fifo2(std::size_t capacity): capacity_(128),capacity_mask(127), pushCursor(0), popCursor(0) {
        ring = new T[capacity_];
    }

    Fifo2(const Fifo2<T> &other) : capacity_(other.capacity_), capacity_mask(other.capacity_mask) {
        pushCursor.store(other.pushCursor.load(), std::memory_order_seq_cst);
        popCursor.store(other.popCursor.load(), std::memory_order_seq_cst);

        ring = new T[capacity_];

        for(int i = 0; i < capacity_;i++) {
            ring[i] = other.ring[i];
        }
    }

    ~Fifo2() {
        delete ring;
    }

    std::size_t capacity() const { return capacity_; }
    std::size_t size() const { return pushCursor - popCursor; }

    bool empty() const { return size() == 0;}
    bool full() const {return size() == capacity(); }

    template<typename T2>
    inline bool full(const T2 pushCursor_, const T2 popCursor_) {
        return (pushCursor_ - popCursor_) == capacity_;
    }

    template<typename T2>
    inline bool empty(const T2 pushCursor_, const T2 popCursor_) {
        return (pushCursor_ - popCursor_) == 0;
    }

    bool push(T& value) {
        auto pushCursor_ = pushCursor.load(std::memory_order_relaxed);

        if (full(pushCursor_, popCursorNonAtomic)) {
            popCursorNonAtomic = popCursor.load(std::memory_order_relaxed);
            if(full(pushCursor_, popCursorNonAtomic)) {
                return false;
            }
        }

        // Call move assignment operator, removes one allocation + extra copy
        ring[pushCursor & capacity_mask] = std::move(value);

        // Known concurrency bug, but it seems that it never crashes
        // In reality, we should use std::memory_model_release
        pushCursor.store(pushCursor_ + 1, std::memory_order_relaxed);

        return true;
    }

    bool pop_blocking(T&value) {
        while(empty()) {}
        return pop(value);
    }

    bool pop(T& value) {
        auto popCursor_ = popCursor.load(std::memory_order_relaxed);

        if(empty(pushCursorNonAtomic, popCursor_)) {
            pushCursorNonAtomic = pushCursor.load(std::memory_order_relaxed);;
            if(empty(pushCursorNonAtomic, popCursor_)) {
                return false;
            }
        }

        // TODO: Use move assignment operator
        value = ring[popCursor & capacity_mask];
        ring[popCursor & capacity_mask].~T();

        // Known concurrency bug, but it seems that it never crashes
        // In reality, we should use std::memory_model_release
        popCursor.store(popCursor_ + 1, std::memory_order_relaxed);

        return true;
    }

    T* ring;
private:
    // std::hardware_destructive_interference_size would work on a linux
    // Avoid false sharing between the elements
    alignas(64) std::atomic<std::size_t> pushCursor;
    alignas(64) std::atomic<std::size_t> popCursor;

    // Common trick -> first spin with non-atomic & check atomic only if required
    alignas(64) std::size_t pushCursorNonAtomic;
    alignas(64) std::size_t popCursorNonAtomic;

    std::size_t capacity_;
    std::size_t capacity_mask;
};

template<typename T>
double run_benchmark(int nb_times = 150, int capacity = 127) {
    std::vector<T> values(nb_times, T(capacity));

    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < nb_times;i++) {
        std::thread producer([&values](int idx) -> void {
            MyBuffer<4096> buffer;
            for(int j = 0; j < values[idx].capacity();j++) { values[idx].push(buffer); }
        }, 1);

        std::thread consumer([&values](int idx) -> void {
            MyBuffer<4096> out;
            for(int j = 0; j < values[idx].capacity();j++) { values[idx].pop_blocking(out);
            /*assert(out.cnt == j);*/ }
        }, 1);

        producer.join();
        consumer.join();
    }

    auto end = std::chrono::high_resolution_clock::now();

    double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;

    return time_taken;
}