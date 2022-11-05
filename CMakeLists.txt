cmake_minimum_required(VERSION 3.23)
project(bvh_precision)

set(CMAKE_CXX_STANDARD 17)

add_executable(bvh_precision main.cpp)
add_subdirectory(bvh)
target_link_libraries(bvh_precision PUBLIC bvh mpfr)

add_executable(test test.cpp)
target_link_libraries(test mpfr)
