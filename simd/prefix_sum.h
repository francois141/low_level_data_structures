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

template<unsigned int size>
class PrefixSum {
public:

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

private:
    std::array<int, size> values;
};

#endif //LOCK_FREE_QUEUE_PREFIX_SUM_H
