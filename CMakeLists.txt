cmake_minimum_required(VERSION 3.25)
project(stse CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++26")

add_compile_options(
    -Wall -Wextra -Wpedantic
    -Wuninitialized -Wunused -Wconversion
    -freflection -fcontracts
    -fcontract-evaluation-semantic=ignore
)

# interface library — lets targets depend on stse cleanly
add_library(stse INTERFACE)
target_include_directories(stse INTERFACE include)

# demo target
add_executable(demo demo.cpp)
target_link_libraries(demo PRIVATE stse)
target_compile_options(demo PRIVATE -Werror)
target_link_options(demo PRIVATE -fcontracts -fcontract-evaluation-semantic=observe)

# test target
file(GLOB TEST_SOURCES tests/*.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE stse)
target_link_options(tests PRIVATE -fcontracts -fcontract-evaluation-semantic=ignore)
