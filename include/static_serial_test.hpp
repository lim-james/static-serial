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
}
