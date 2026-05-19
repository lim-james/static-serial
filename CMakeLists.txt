cmake_minimum_required(VERSION 3.25)
project(stse CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Don't use CMAKE_CXX_STANDARD — CMake 3.25 doesn't know C++26
# Set the flag directly instead
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++26")

# Common flags
add_compile_options(
    -Wall -Wextra -Wpedantic
    -Wuninitialized -Wunused -Wconversion
    -freflection -fcontracts
    -fcontract-evaluation-semantic=ignore
)

# demo target
add_executable(demo demo.cpp)
target_include_directories(demo PRIVATE include)
target_compile_options(demo PRIVATE -Werror)
target_link_options(demo PRIVATE -fcontracts -fcontract-evaluation-semantic=observe)

# test target
file(GLOB TEST_SOURCES tests/*.cpp)
add_executable(tests ${TEST_SOURCES})
target_include_directories(tests PRIVATE include)
target_link_options(tests PRIVATE -fcontracts -fcontract-evaluation-semantic=ignore)
