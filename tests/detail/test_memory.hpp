#pragma once

#include "stse/detail/memory.hpp"
#include "tests/test_executor.hpp"

#include <array>
#include <span>
#include <cassert>
#include <ranges>

namespace stse::tests {

constexpr auto make_buffer(auto... items) -> std::array<std::byte, sizeof...(items)> {
    return {static_cast<std::byte>(items)...};
}

constexpr bool test_constexpr_memcpy() {
    auto* indiv_test = +[](std::span<std::byte> buffer) constexpr {
        static constexpr std::size_t MAX_BUFFER_SIZE = 1024;

        const std::size_t size = buffer.size();

        assert(size < MAX_BUFFER_SIZE);
        auto destination = std::array<std::byte, MAX_BUFFER_SIZE>{};
        stse::detail::constexpr_memcpy(destination.data(), buffer.data(), size);

        const auto destination_span = std::span{destination.begin(), size};
        return std::ranges::equal(buffer, destination_span);
    };

    return TestExecutor{"detail::constexpr_memcpy", indiv_test}
        .run_sequence(
            make_buffer('A', 'B', 'C'),
            make_buffer('1', '0', '3', 'S', 'a', 'b')
        );
}

static_assert(test_constexpr_memcpy());

} // namespace stse::tests
