#pragma once

#include "tests/types.hpp"

#include "stse/detail/meta_utils.hpp"

namespace stse::tests {

using namespace stse::detail;

static_assert(has_annotation<skipserialization>(^^WithSkippedPointer_2w::b));
static_assert(false == has_annotation<skipserialization>(^^WithSkippedPointer_2w::a));

static_assert(has_annotation<ignoreserialization>(^^WithIgnoredPointer_2w::b));
static_assert(false == has_annotation<ignoreserialization>(^^WithIgnoredPointer_2w::a));

static_assert(all_data_members_of<FlatCompactAggregate_3b>.size()     == 3);
static_assert(all_data_members_of<WithCompactArray_4b>.size()         == 2);
static_assert(all_data_members_of<WithSkippedParent_3w>.size()        == 3);
static_assert(all_data_members_of<WithMultiSkippedPointers_4w>.size() == 4);
static_assert(all_data_members_of<WithIgnoredPointer_2w>.size()       == 2);

static_assert(serializable_members_of<FlatCompactAggregate_3b>.size()     == 3);
static_assert(serializable_members_of<WithCompactArray_4b>.size()         == 2);
static_assert(serializable_members_of<WithSkippedParent_3w>.size()        == 2);
static_assert(serializable_members_of<WithMultiSkippedPointers_4w>.size() == 2);
static_assert(serializable_members_of<WithIgnoredPointer_2w>.size()       == 1);

static_assert(non_skipped_members_of<FlatCompactAggregate_3b>.size()     == 3);
static_assert(non_skipped_members_of<WithCompactArray_4b>.size()         == 2);
static_assert(non_skipped_members_of<WithSkippedParent_3w>.size()        == 2);
static_assert(non_skipped_members_of<WithMultiSkippedPointers_4w>.size() == 2);
static_assert(non_skipped_members_of<WithIgnoredPointer_2w>.size()       == 2);

} // namespace stse::tests
