#include "gtest/gtest.h"

#include "lock_free_queue/LockFreeStack.hpp"
#include "priority_queue/array_prio_queue.h"
#include "linked_list/linked_list.hpp"


#if defined(__x86_64__)

#include "simd/Argmin.h"

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

#endif



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
            s.Push(i);
        }

        for(int i = size-1; i >= 0;i--) {
            auto val = s.Pop();
            EXPECT_EQ(val.value(), i);
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

// Test linked list
TEST(LINKED_LIST, CoarseGrained) {
    srand((unsigned) time(NULL));
    const int size = 256;

    CoarseLinkedList<int> coarseLinkedList;
    SetOracle<int> oracle;

    // Get a random number
    for(int i = 0; i < 10000;i++) {
        int action = rand() % 3;
        int random = rand() % size;
        switch (action) {
            case 0:
                EXPECT_EQ(coarseLinkedList.Add(random), oracle.Add(random));
            case 1:
                EXPECT_EQ(coarseLinkedList.Remove(random), oracle.Remove(random));
            case 2:
                EXPECT_EQ(coarseLinkedList.Contains(random), oracle.Contains(random));
        }
    }
}


// Test linked list
TEST(LINKED_LIST, FinedGrained) {
    srand((unsigned) time(NULL));
    const int size = 256;

    FineLinkedList<int> coarseLinkedList;
    SetOracle<int> oracle;

    // Get a random number
    for(int i = 0; i < 10000;i++) {
        int action = rand() % 3;
        int random = rand() % size;
        switch (action) {
            case 0:
                EXPECT_EQ(coarseLinkedList.Add(random), oracle.Add(random));
            case 1:
                EXPECT_EQ(coarseLinkedList.Remove(random), oracle.Remove(random));
            case 2:
                EXPECT_EQ(coarseLinkedList.Contains(random), oracle.Contains(random));
        }
    }
}