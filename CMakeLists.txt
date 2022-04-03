cmake_minimum_required(VERSION 3.5)


add_library(context SHARED src/context.cpp src/context.hpp)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall --std=c++17")
project(context)

add_executable(test src/test.cpp)
target_link_libraries(test PUBLIC context)
