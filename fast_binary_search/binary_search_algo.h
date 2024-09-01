//
// Created by François Costa on 09.06.2024.
//

#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H

#include <cstdlib>

class binary_search_algo {

public:
    explicit binary_search_algo(int n): n(n){
        values = (int*)malloc(sizeof(int) * n);
        for(int i = 0; i < n;i++) {
            values[i] = i;
        }
        shuffled_values = (int*)malloc(sizeof(int) * (n + 1));
        this->_shuffle(1);
    }

    void _shuffle(int currIdx) {
        static int idx = 0;
        if(currIdx <= n) {
            this->_shuffle(2*currIdx);
            this->shuffled_values[currIdx] = this->values[idx++];
            this->_shuffle(2*currIdx+1);
        }
    };

    // Optimized binary search
    bool cache_friendly_binary_search(int x) {
        int current = 1;
        while(current <= n) {
            current = 2*current + (shuffled_values[current] < x);
        }

        return shuffled_values[current >> __builtin_ffs(~current)] == x;
    }

    // Default algorithm
    bool base_binary_search(int x) {
        int left = 0;
        int right = n-1;

        while(left != right) {
            int middle = (left + right) / 2;
            if (values[middle] < x) {
                left = middle + 1;
            } else if(values[middle] > x) {
                right = middle - 1;
            } else {
                return true;
            }
        }

        return values[left] == x;
    }

    // Branchless binary search
    bool branchless_binary_search(int x) {
        int *arr = &values[0];
        int size = n;

        while(size > 1) {
            int half = size / 2;
            arr += half * (arr[half-1] < x);
            size -= half;
        }

        return *arr == x;
    }

private:
    int n;
    int* values;
    int* shuffled_values;
};


#endif //BINARY_SEARCH_H
