#include "stse/stse.hpp"

#include "tests/types.hpp"
#include "tests/detail/test_memory.hpp"
#include "tests/detail/test_meta_utils.hpp"
#include "tests/detail/test_layout.hpp"
#include "tests/detail/test_concepts.hpp"
#include "tests/detail/test_serialize.hpp"
#include "tests/test_annotation.hpp"
#include "tests/test_executor.hpp"
#include "tests/test_scalar.hpp"

int main() {
    stse::tests::test_constexpr_memcpy();
    stse::tests::test_skip_annotation();
    stse::tests::test_ignore_annotation();
    stse::tests::test_serialize_scalar();
    stse::tests::test_serialize_container();
    stse::tests::test_serialize_aggregate();
    stse::tests::test_scalar();
}

