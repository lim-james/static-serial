#pragma once

#include "static_serial_test.hpp"
#include "tests/types.hpp"
#include "tests/test_executor.hpp"

namespace stse::tests {

constexpr bool test_skip_annotation() {
    bool skipped_field = TestExecutor{
        "Round trip for skipped field", 
        test_round_trip<WithSkippedPointer_2w>
    }.run_single(WithSkippedPointer_2w{1, nullptr});

    bool skipped_multiple_fields = TestExecutor{
        "Round trip for multiple skipped field", 
        test_round_trip<WithMultiSkippedPointers_4w>
    }.run_single(WithMultiSkippedPointers_4w{1, nullptr, 2, nullptr});

    bool skipped_fields_in_parent = TestExecutor{
        "Round trip for skipped field in parent", 
        test_round_trip<WithSkippedParent_3w>
    }.run_single(WithSkippedParent_3w{1, nullptr, 2});

    return skipped_field && skipped_multiple_fields && skipped_fields_in_parent;
}

constexpr bool test_ignore_annotation() {
    return TestExecutor{
        "Round trip for ignored field", 
        test_round_trip<WithIgnoredPointer_2w>
    }.run_single(WithIgnoredPointer_2w{1, nullptr});
}

static_assert(test_skip_annotation());
static_assert(test_ignore_annotation());

} // namespace stse::tests
