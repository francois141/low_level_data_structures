//
// Created by francois on 07.05.24.
//

#ifndef LOCK_FREE_QUEUE_ARGMIN_H
#define LOCK_FREE_QUEUE_ARGMIN_H

#include <vector>
#include <cstdlib>
#include <iostream>
#include <immintrin.h>

template<unsigned int size>
class Argmin {
    static_assert(size > 0 && size % 8 == 0);
public:

    Argmin(){
        // Align correctly for SIMD
        arr = (int*) aligned_alloc(sizeof(int) * size, 32);
        for(int i = 0; i < size;i++) {
            arr[i] = rand() % 100;
        }
    }

    int naive() {
        int idx = 0;
        int val = arr[0];
        for(int i = 1; i < size;i++) {
            if(arr[i] < val) {
                val = arr[i];
                idx = i;
            }
        }
        return idx;
    }

    int simd_basic() {
        __m256i value = _mm256_setr_epi32(0,1,2,3,4,5,6,7);
        __m256i min_values = _mm256_set1_epi32(std::numeric_limits<int>::max());
        __m256i index = _mm256_setzero_si256();
        __m256i mask_8 = _mm256_set1_epi32(8);

        // Vector part
        for(size_t i = 0; i < size;i += 8) {
            // Load block
           __m256i new_values = mask_8;//_mm256_load_epi32((__m256i*) &arr[i]);

            // Blend indices
            __m256i mask = _mm256_cmpgt_epi32(min_values, new_values);
            _mm256_blendv_epi8(index, new_values, mask);

            // Blend values
            min_values = _mm256_min_epi32(min_values, new_values);

            // Multiply by 8 at the end
            _mm256_add_epi32(value, mask_8);
        }

        // Sequential part
        int idx = 0;
        int best = std::numeric_limits<int>::max();

        for(int i = 0; i < 8;i++) {
            if(value[i] < best) {
                idx = index[i];
                best = value[i];
            }
        }

        return idx;
    }

    // TODO: Write this function
    int simd_optimized() {
        return 0;
    }

private:
    int *arr;
};


#endif //LOCK_FREE_QUEUE_ARGMIN_H
