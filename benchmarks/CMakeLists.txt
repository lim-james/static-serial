find_package(Protobuf REQUIRED)

add_executable(bench_stse stse.cpp)
target_link_libraries(bench_stse PRIVATE stse)
target_compile_options(bench_stse PRIVATE -O3 -march=native)

add_executable(bench_handrolled handrolled.cpp)
target_link_libraries(bench_handrolled PRIVATE stse)
target_compile_options(bench_handrolled PRIVATE -O3 -march=native)

add_executable(bench_naive_memcpy naive_memcpy.cpp)
target_compile_options(bench_naive_memcpy PRIVATE -O3 -march=native)

add_executable(bench_protobuf
    protobuf.cpp
    market_data.pb.cc
)
target_link_libraries(bench_protobuf PRIVATE stse protobuf::libprotobuf)
target_compile_options(bench_protobuf PRIVATE -O3 -march=native)

