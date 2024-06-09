cmake_minimum_required(VERSION 3.27)
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
        lock_free_queue/LockFreeStack.hpp
        lock_free_queue/Element.hpp
        fifo_queue_sp_sc/fifo_queue_sp_sc.hpp
        priority_queue/array_prio_queue.h
        simd/Argmin.h
        linked_list/linked_list.hpp
        fast_binary_search/binary_search_algo.h
)

target_link_libraries(lock_free_queue PRIVATE nanobench)


include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        unit_tests
        unit_tests.cc
        linked_list/linked_list.hpp
        fast_binary_search/binary_search_algo.h
)
target_link_libraries(
        unit_tests
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

