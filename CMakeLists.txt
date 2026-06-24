cmake_minimum_required(VERSION 3.25)
project(stse CXX)

if(CMAKE_VERSION VERSION_LESS "3.30")
  add_compile_options(-std=c++26)
else()
  set(CMAKE_CXX_STANDARD 26)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------------------
# Options (driven by presets)
# ---------------------------------------------------------------------------
option(STSE_SANITIZE          "Build tests with ASan + UBSan"           OFF)
option(STSE_BUILD_BENCHMARKS  "Build benchmarks (requires protobuf)"    OFF)
set(STSE_CONTRACT_SEMANTIC "ignore" CACHE STRING "Contract evaluation semantic (observe|ignore)")

# ---------------------------------------------------------------------------
# stse interface library
# ---------------------------------------------------------------------------
add_library(stse INTERFACE)
target_include_directories(stse INTERFACE include)
target_compile_options(stse INTERFACE
    -Wall -Wextra -Wpedantic
    -Wuninitialized -Wunused -Wconversion
    -Wnrvo
    -freflection -fcontracts
    -fcontract-evaluation-semantic=${STSE_CONTRACT_SEMANTIC}
)
target_link_options(stse INTERFACE
    -fcontract-evaluation-semantic=${STSE_CONTRACT_SEMANTIC}
)

# ---------------------------------------------------------------------------
# demo
# ---------------------------------------------------------------------------
add_executable(demo demo.cpp)
target_link_libraries(demo PRIVATE stse)

# ---------------------------------------------------------------------------
# tests
# ---------------------------------------------------------------------------
set(TEST_SOURCES tests/main.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE stse)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(STSE_SANITIZE)
  target_compile_options(tests PRIVATE
      -fsanitize=address,undefined
      -fno-omit-frame-pointer
      -g
      -O1
  )
  target_link_options(tests PRIVATE
      -fsanitize=address,undefined
  )
endif()

# ---------------------------------------------------------------------------
# benchmarks
# ---------------------------------------------------------------------------
if(STSE_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()
