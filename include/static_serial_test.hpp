#pragma once

#include "static_serial.hpp"

namespace stse::test {
template<auto data>
constexpr bool test_back_and_forth() {
    using T = decltype(data);
    auto bytes    = stse::serialize(data);
    auto restored = stse::deserialize<T>(bytes);
    return data == restored;
}

bool test_back_and_forth(auto data) {
    using T = decltype(data);
    auto bytes    = stse::serialize(data);
    auto restored = stse::deserialize<T>(bytes);
    return data == restored;
}

template<auto data, typename Endian> 
consteval bool test_back_and_forth_endianness(Endian binary_endianness) {
    using T = decltype(data);
    auto raw_bytes = stse::serialize(data, binary_endianness);
    auto restored  = stse::deserialize<T>(raw_bytes, binary_endianness);
    return data == restored;
}

}
