#pragma once

#include <cstddef>
#include <array>

namespace stse::tests {
consteval auto make_bytes(auto... bytes) -> std::array<std::byte, sizeof...(bytes)> {
    return std::array{static_cast<std::byte>(bytes)...};
}
} // namespace stse::tests 

