find_package(Protobuf REQUIRED)

add_executable(bench_stse against_protobuf/stse.cpp)
target_link_libraries(bench_stse PRIVATE stse)
target_compile_options(bench_stse PRIVATE -O3 -march=native)

add_executable(bench_protobuf
    against_protobuf/protobuf.cpp
    against_protobuf/market_data.pb.cc
)
target_link_libraries(bench_protobuf PRIVATE stse protobuf::libprotobuf)
target_compile_options(bench_protobuf PRIVATE -O3 -march=native)
