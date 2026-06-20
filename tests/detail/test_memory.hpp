#pragma once

#include "stse/detail/memory.hpp"

#include <array>
#include <span>
#include <cassert>
#include <ranges>

constexpr auto make_buffer(auto... items) -> std::array<std::byte, sizeof...(items)> {
    return {static_cast<std::byte>(items)...};
}

bool runtime_test_constexpr_memcpy(std::span<std::byte> buffer) {
    static constexpr std::size_t MAX_BUFFER_SIZE = 1024;

    const std::size_t size = buffer.size();

    assert(size < MAX_BUFFER_SIZE);
    auto destination = std::array<std::byte, MAX_BUFFER_SIZE>{};
    stse::detail::constexpr_memcpy(destination.data(), buffer.data(), size);

    const auto destination_span = std::span{destination.begin(), size};
    return std::ranges::equal(buffer, destination_span);
}

template<std::size_t N>
constexpr bool compile_time_test_constexpr_memcpy(std::array<std::byte, N> buffer) {
    auto destination = std::array<std::byte, N>{};
    stse::detail::constexpr_memcpy(destination.data(), buffer.data(), N);
    return std::ranges::equal(buffer, destination_span);
}

static_assert(compile_time_test_constexpr_memcpy(make_buffer('A', 'B', 'C')));
