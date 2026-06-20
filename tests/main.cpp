#include "stse/stse.hpp"

#include "tests/detail/test_memory.hpp"
#include "tests/detail/test_layout.hpp"
#include "tests/test_executor.hpp"
#include "tests/test_scalar.hpp"

int main() {
    test_constexpr_memcpy();
    test_scalar();
}

