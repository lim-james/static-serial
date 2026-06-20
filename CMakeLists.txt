cmake_minimum_required(VERSION 3.25)
project(stse CXX)

# CXX26 not recognized until CMake 3.30 — set manually for older versions
if(CMAKE_VERSION VERSION_LESS "3.30")
  add_compile_options(-std=c++26)
else()
  set(CMAKE_CXX_STANDARD 26)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type defaults
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Experimental GCC flags needed project-wide
add_compile_options(-freflection -fcontracts)
add_link_options(-fcontracts)

# stse interface library
add_library(stse INTERFACE)
target_include_directories(stse INTERFACE include)
target_compile_options(stse INTERFACE
    -Wall -Wextra -Wpedantic
    -Wuninitialized -Wunused -Wconversion
)

# Contract semantic per build type
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(-fcontract-evaluation-semantic=ignore)
  add_link_options(-fcontract-evaluation-semantic=ignore)
else()
  add_compile_options(-fcontract-evaluation-semantic=observe)
  add_link_options(-fcontract-evaluation-semantic=observe)
endif()

add_executable(demo demo.cpp)
target_link_libraries(demo PRIVATE stse)
target_compile_options(demo PRIVATE -Werror)

set(TEST_SOURCES tests/main.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE stse)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Benchmarks — opt-in since protobuf is an external dep
option(STSE_BUILD_BENCHMARKS "Build benchmarks (requires protobuf)" OFF)
if(STSE_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()
