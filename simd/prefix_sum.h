//
// Created by fran-ois-costa on 01/09/24.
//

#ifndef LOCK_FREE_QUEUE_PREFIX_SUM_H
#define LOCK_FREE_QUEUE_PREFIX_SUM_H

#include <array>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <immintrin.h>
#include <emmintrin.h>

template<unsigned int size>
class PrefixSum {
public:
    static_assert(size % 8 == 0);

    PrefixSum() {
        reset();
    }

    void reset() {
        std::iota(values.begin(), values.end(), 0);
    }

    std::array<int, size> naive() {
        for(int i = 1; i < values.size();i++) {
            values[i] += values[i-1];
        }

        return values;
    }

    std::array<int, size> stl() {
        std::partial_sum(values.begin(), values.end(), values.begin());

        return values;
    }

    void prefix_lane(int idx) {
        // Load lane
        auto lane = _mm_load_si128((__m128i*)(&values[idx]));
        // Update lane
        lane = _mm_add_epi32(lane, _mm_slli_si128(lane,4));
        lane = _mm_add_epi32(lane, _mm_slli_si128(lane, 8));
        // Store lane
        _mm_store_si128((__m128i*)(&values[idx]), lane);
    }

    __m128i accumulate(__m128i accumulator_lane, int *lane_ptr) {
        // Load lane
        __m128i lane = _mm_load_si128((__m128i*)(lane_ptr));
        // Add to lane
        lane = _mm_add_epi32(lane, accumulator_lane);
        // Store lane
        _mm_store_si128((__m128i*) lane_ptr, lane);
        // Add accumulated value to the lane & return
        __m128i accumulated_value = (__m128i)(_mm_broadcast_ss((float*)(&lane_ptr[3])));
        // Return updated accumulated lane
        return _mm_add_epi32(accumulator_lane, accumulated_value);
    }

    std::array<int, size> simd_naive() {
        for(int i = 0; i < size; i += 4) {
            prefix_lane(i);
        }

        __m128i accumulator = _mm_setzero_si128();
        //std::cout << accumulator[0] << " "  << accumulator[1] << " "<< accumulator[2] << " "<< accumulator[3] << " "<< std::endl;


        for(int i = 0; i < size;i += 4) {
            accumulator = accumulate(accumulator, &values[i]);
            //std::cout << accumulator[0] << " "  << accumulator[1] << " "<< accumulator[2] << " "<< accumulator[3] << " "<< std::endl;
        }

        for(auto e: values) std::cout << e << " ";
        std::cout << std::endl;

        return values;
    }

private:
    std::array<int, size> values;
};

#endif //LOCK_FREE_QUEUE_PREFIX_SUM_H
