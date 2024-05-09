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
        arr = (float*) malloc(sizeof(float) * size);
        for(int i = 0; i < size;i++) {
            arr[i] = static_cast<float>(rand() % 15000);
        }
    }

    float naive() {
        int idx = 0;
        float val = arr[0];
        for(int i = 1; i < size;i++) {
            if(arr[i] <= val) {
                val = arr[i];
                idx = i;
            }
        }
        return idx;
    }

    float simd_basic() {
        __m256 value = _mm256_set_ps(7,6,5,4,3,2,1,0);
        __m256 min_values = _mm256_set1_ps(std::numeric_limits<float>::max());
        __m256 index = _mm256_setzero_ps();
        __m256 mask_8 = _mm256_set1_ps(8);

        // Vector part
        for(size_t i = 0; i < size;i += 8) {
            // Load block
           __m256 new_values = _mm256_loadu_ps( &arr[i]);

            // Blend indices
            __m256 mask = _mm256_cmp_ps(min_values, new_values,_CMP_GT_OS);
            index = _mm256_blendv_ps(index, value, mask);

            // Blend values
            min_values = _mm256_min_ps(min_values, new_values);

            // Multiply by 8 at the end
            value = _mm256_add_ps(value, mask_8);
        }

        // Sequential part
        int idx = 0;
        int best = std::numeric_limits<float>::max();

        for(int i = 0; i < 8;i++) {
            if(min_values[i] < best) {
                idx = index[i];
                best = min_values[i];
            }
        }

        return idx;
    }

    // TODO: Write this function
    float simd_optimized() {
        return 0;
    }



private:
    float *arr;

};


#endif //LOCK_FREE_QUEUE_ARGMIN_H
