#pragma once

#include <cstddef>
#include <cstring>

namespace stse::detail {

constexpr void constexpr_memcpy(std::byte* destination, const std::byte* source, std::size_t count) {
    if consteval {
        // note: intentionally kept as loop rather than std::copy_n 
        // when tested on Compiler Explorer on -O3, loops yield 
        // better optimisation options such as vectorisation
        for (std::size_t i{}; i < count; ++i) {
            destination[i] = source[i];
        }
    } else {
        std::memcpy(destination, source, count);
    }
}

}
