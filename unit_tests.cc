#include "gtest/gtest.h"
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



