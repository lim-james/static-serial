#pragma once

#include "stse/stse.hpp"
#include <type_traits>

namespace stse::tests {

constexpr bool test_round_trip_inplace(const auto& data) {
    using T = std::remove_cvref_t<decltype(data)>;

    auto raw_bytes = std::array<std::byte, stse::serial_size_v<T>>{};
    auto restored = T{};

    stse::serialize_advance(raw_bytes, data);
    stse::deserialize_advance(raw_bytes, restored);
    return data == restored;
}

constexpr bool test_round_trip_heap(const auto& data) {
    using T = std::remove_cvref_t<decltype(data)>;
    auto bytes = stse::serialize(data);
    auto restored = stse::deserialize<T>(bytes).result;
    return data == restored;
}

constexpr bool test_round_trip(const auto& data) {
    return test_round_trip_heap(data)
        && test_round_trip_inplace(data);
}

template<typename Endian> 
consteval bool test_round_trip_endianness(Endian binary_endianness, const auto& data) {
    using T = decltype(data);
    auto raw_bytes = stse::serialize(binary_endianness, data);
    auto restored  = stse::deserialize<T>(binary_endianness, raw_bytes).result;
    return data == restored;
}

consteval bool test_variadic_round_trip(const auto&... data) {
    auto raw_bytes = stse::serialize(data...);
    auto result    = stse::deserialize<decltype(data)...>(raw_bytes);
    auto& [...restored] = result.result;
    return ((data == restored) && ...);
}

consteval bool test_variadic_advance_round_trip(const auto&... data) {
    auto raw_bytes = stse::serialize(data...);

    std::tuple<decltype(data)...> parsed{};
    auto& [...restored] = parsed;
    stse::deserialize_advance(std::span<const std::byte>{raw_bytes}, restored...);
    return ((data == restored) && ...);
}

template<auto endianness>
consteval bool test_variadic_round_trip_endian(const auto&... data) {
    auto raw_bytes = stse::serialize(endianness, data...);
    auto result    = stse::deserialize<decltype(data)...>(endianness, raw_bytes);
    auto& [...restored] = result.result;
    return ((data == restored) && ...);
}

} // namespace stse::tests
