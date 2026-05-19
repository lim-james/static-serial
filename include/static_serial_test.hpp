#pragma once

#include "static_serial.hpp"
#include <type_traits>

namespace stse::test {

template<auto data>
constexpr bool test_round_trip_heap() {
    auto raw_bytes = stse::serialize(data);
    auto [restored]  = stse::deserialize<decltype(data)>(raw_bytes).objects;
    return data == restored;
}

template<auto data>
constexpr bool test_round_trip_inplace() {
    using T = decltype(data);

    auto raw_bytes = std::array<std::byte, stse::serial_size_v<T>>{};
    auto restored = T{};

    stse::serialize_advance(raw_bytes, data);
    stse::deserialize_advance(raw_bytes, restored);
    return data == restored;
}

template<auto data>
constexpr bool test_round_trip() {
    static_assert(test_round_trip_heap<data>(), "Heap serialization failed");
    static_assert(test_round_trip_inplace<data>(), "Inplace serialization failed");
    return test_round_trip_heap<data>()
        && test_round_trip_inplace<data>();
}

bool test_round_trip_heap(const auto& data) {
    using T = std::remove_cvref_t<decltype(data)>;
    auto bytes = stse::serialize(data);
    auto [restored] = stse::deserialize<T>(bytes).objects;
    return data == restored;
}

bool test_round_trip_inplace(const auto& data) {
    using T = std::remove_cvref_t<decltype(data)>;

    auto raw_bytes = std::array<std::byte, stse::serial_size_v<T>>{};
    auto restored = T{};

    stse::serialize_advance(raw_bytes, data);
    stse::deserialize_advance(raw_bytes, restored);
    return data == restored;
}

bool test_round_trip(const auto& data) {
    return test_round_trip_heap(data);
        // && test_round_trip_inplace(data);
}

template<auto data, typename Endian> 
consteval bool test_round_trip_endianness(Endian binary_endianness) {
    using T = decltype(data);
    auto raw_bytes = stse::serialize(binary_endianness, data);
    auto [restored] = stse::deserialize<T>(binary_endianness, raw_bytes).objects;
    return data == restored;
}
}
