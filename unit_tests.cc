#include "gtest/gtest.h"
#include "simd/Argmin.h"
#include "lock_free_queue/LockFreeStack.hpp"
#include "priority_queue/array_prio_queue.h"

// Test for argmin SIMD
TEST(SIMD_ARGMIN, BasicTest) {
    const unsigned int size = 8;

    Argmin<size> minAlgorithm = Argmin<size>();

    EXPECT_EQ(minAlgorithm[minAlgorithm.naive()], minAlgorithm[minAlgorithm.simd_basic()]);
}

// Larger inputs
TEST(SIMD_ARGMIN, LargeTest) {
    const unsigned int size = 1000 * 4096;

    Argmin<size> minAlgorithm = Argmin<size>();

    EXPECT_EQ(minAlgorithm[minAlgorithm.naive()], minAlgorithm[minAlgorithm.simd_basic()]);
}

// Larger inputs
TEST(SIMD_ARGMIN, StressTest) {
    const unsigned int size = 100 * 4096;
    int times = 50;

    while(times--) {
        Argmin<size> minAlgorithm = Argmin<size>();
        EXPECT_EQ(minAlgorithm[minAlgorithm.naive()], minAlgorithm[minAlgorithm.simd_basic()]);
    }

}

// Simple stack test
TEST(STACK, BasicTest) {
    const unsigned int size = 256;

    {
        NaiveStack<int> s = NaiveStack<int>();
        for(int i = 0; i < size;i++) {
            s.Push(new Element<int>(nullptr, i));
        }

        for(int i = size-1; i >= 0;i--) {
            auto val = s.Pop();
            EXPECT_EQ(val->value, i);
        }
    }
    {
        LockFreeStack<int,4,100> s = LockFreeStack<int,4,100>();
        for(int i = 0; i < size;i++) {
            s.Push(new Element<int>(nullptr, i));
        }

        for(int i = size-1; i >= 0;i--) {
            auto val = s.Pop();
            EXPECT_EQ(val->value, i);
        }
    }
}


// Simple priority queue test
TEST(PRIORITY_QUEUE, BasicTest) {
    const unsigned int size = 10;

    {
        ArrayPriorityQueue<LockedBin<int>> priorityQueue(size);
        for(int i = 0; i < size;i++) {
            priorityQueue.push(size-1-i,size-1-i);
        }

        for(int i = 0; i < size;i++) {
            std::optional<int> val = priorityQueue.pop();
            EXPECT_TRUE(val.has_value());
            EXPECT_EQ(val.value(), i);
        }
    }
}