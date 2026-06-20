#include "stse/stse.hpp"

#include "tests/test_executor.hpp"
#include "tests/test_scalar.hpp"
#include "tests/detail/test_memory.hpp"

int main() {
    test_scalar();
    test_constexpr_memcpy();
}

