cmake_minimum_required(VERSION 3.28)
project(lock_free_queue)

set(CMAKE_CXX_STANDARD 17)

add_executable(lock_free_queue main.cpp
        lock_free_queue/Stack.hpp
        lock_free_queue/Element.hpp
        fifo_queue_sp_sc/fifo_queue_sp_sc.hpp
)
