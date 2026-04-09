#pragma once

#include "static_serial.hpp"

namespace stse::test {

template<auto data>
constexpr bool test_round_trip_heap() {
    auto raw_bytes = stse::serialize(data);
    auto restored  = stse::deserialize<decltype(data)>(raw_bytes).object;
    return data == restored;
}

template<auto data>
constexpr bool test_round_trip_inplace() {
    auto raw_bytes = std::array<std::byte, sizeof(data)>{};
    auto restored = decltype(data){};

    stse::serialize_advance(data, raw_bytes);
    stse::deserialize_advance(restored, raw_bytes);
    return data == restored;
}

template<auto data>
constexpr bool test_round_trip() {
    static_assert(test_round_trip_heap<data>(), "Heap serialization failed");
    static_assert(test_round_trip_inplace<data>(), "Inplace serialization failed");
    return test_round_trip_heap<data>()
        && test_round_trip_inplace<data>();
}

bool test_round_trip(auto data) {
    using T = decltype(data);
    auto bytes = stse::serialize(data);
    auto [restored, _] = stse::deserialize<T>(bytes);
    return data == restored;
}

template<auto data, typename Endian> 
consteval bool test_round_trip_endianness(Endian binary_endianness) {
    using T = decltype(data);
    auto raw_bytes = stse::serialize(data, binary_endianness);
    auto [restored, _] = stse::deserialize<T>(raw_bytes, binary_endianness);
    return data == restored;
}
}
