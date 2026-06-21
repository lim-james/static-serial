#include "stse/stse.hpp"

#include "tests/types.hpp"
#include "tests/detail/test_memory.hpp"
#include "tests/detail/test_layout.hpp"
#include "tests/detail/test_concepts.hpp"
#include "tests/test_executor.hpp"
#include "tests/test_scalar.hpp"

int main() {
    stse::tests::test_constexpr_memcpy();
    stse::tests::test_scalar();
}

