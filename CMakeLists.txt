cmake_minimum_required(VERSION 3.0)
project(Example LANGUAGES C CXX VERSION 1.0.0)

add_executable(example_1 src/example_1.cpp)
target_include_directories(example_1 PRIVATE src)

add_executable(example_2 src/example_2.cpp)
target_include_directories(example_2 PRIVATE src)

add_executable(example_3 src/example_3.cpp)
target_include_directories(example_3 PRIVATE src)
