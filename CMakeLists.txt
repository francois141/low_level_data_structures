cmake_minimum_required(VERSION 3.28)
project(lock_free_queue)

set(CMAKE_CXX_STANDARD 17)

add_executable(lock_free_queue main.cpp
        Stack.cpp
        Stack.h
        Element.cpp
        Element.h
)
