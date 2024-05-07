cmake_minimum_required(VERSION 3.28)
project(lock_free_queue)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        nanobench
        GIT_REPOSITORY https://github.com/martinus/nanobench.git
        GIT_TAG v4.1.0
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(nanobench)

add_compile_options(-march=native -mavx)
add_executable(lock_free_queue main.cpp
        lock_free_queue/Stack.hpp
        lock_free_queue/Element.hpp
        fifo_queue_sp_sc/fifo_queue_sp_sc.hpp
        priority_queue/array_prio_queue.h
        simd/Argmin.h
)

target_link_libraries(lock_free_queue PRIVATE nanobench)
