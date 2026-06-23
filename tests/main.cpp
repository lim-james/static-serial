#include "stse/stse.hpp"

#include "tests/types.hpp"
#include "tests/detail/test_memory.hpp"
#include "tests/detail/test_meta_utils.hpp"
#include "tests/detail/test_layout.hpp"
#include "tests/detail/test_concepts.hpp"
#include "tests/detail/test_serialize.hpp"
#include "tests/detail/test_deserialize.hpp"
#include "tests/test_annotation.hpp"
#include "tests/test_executor.hpp"
#include "tests/test_scalar.hpp"

enum class ExitCode: int { SUCCESS = 0, FAILED = 1 };

int main() {
    bool all_passed = true;

    all_passed &= stse::tests::test_constexpr_memcpy();
    all_passed &= stse::tests::test_skip_annotation();
    all_passed &= stse::tests::test_ignore_annotation();
    all_passed &= stse::tests::test_serialize_scalar();
    all_passed &= stse::tests::test_serialize_container_base();
    all_passed &= stse::tests::test_serialize_container_flat();
    all_passed &= stse::tests::test_serialize_container_final();
    all_passed &= stse::tests::test_serialize_aggregate_base();
    all_passed &= stse::tests::test_serialize_aggregate_flat();
    all_passed &= stse::tests::test_serialize_aggregate_final();
    all_passed &= stse::tests::test_serialize_annotated_aggregate_base();
    all_passed &= stse::tests::test_serialize_annotated_aggregate_final();
    all_passed &= stse::tests::test_deserialize_scalar();
    all_passed &= stse::tests::test_deserialize_container_base();
    all_passed &= stse::tests::test_deserialize_container_flat();
    all_passed &= stse::tests::test_deserialize_container_final();
    all_passed &= stse::tests::test_deserialize_aggregate_base();
    all_passed &= stse::tests::test_deserialize_aggregate_flat();
    all_passed &= stse::tests::test_deserialize_aggregate_final();
    all_passed &= stse::tests::test_deserialize_annotated_aggregate_base();
    all_passed &= stse::tests::test_deserialize_annotated_aggregate_final();
    all_passed &= stse::tests::test_scalar();

    return all_passed ? static_cast<int>(ExitCode::SUCCESS)
                      : static_cast<int>(ExitCode::FAILED);
}
