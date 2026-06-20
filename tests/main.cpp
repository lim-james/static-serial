#include "stse/stse.hpp"

#include "tests/test_runner.hpp"
#include "tests/detail/test_memory.hpp"

#include <cassert>

int main() {
    TestExecutor{"detail::constexpr_memcpy", &test_constexpr_memcpy}
        .run_sequence(
            make_buffer('A', 'B', 'C'),
            make_buffer('1', '0', '3', 'S', 'a', 'b')
        );
}

