#pragma once

#include <type_traits>
#include <array>
#include <cstdint>
#include <cstddef>

namespace stse::tests {

inline constexpr std::size_t WORD = sizeof(void*);
inline constexpr std::size_t N = 12;

using _1b = std::uint8_t;
static_assert(sizeof(_1b)  == 1);
static_assert(alignof(_1b) == 1);

using _2b = std::uint16_t;
static_assert(sizeof(_2b)  == 2);
static_assert(alignof(_2b) == 2);

using _4b = std::uint32_t;
static_assert(sizeof(_4b)  == 4);
static_assert(alignof(_4b) == 4);

using _8b = std::uint64_t;
static_assert(sizeof(_8b)  == 8);
static_assert(alignof(_8b) == 8);

enum UnscopedEnum { option_a, option_b };
enum class ScopedEnum { option_a, option_b };

static_assert(false == std::is_scoped_enum_v<UnscopedEnum>);
static_assert(std::is_scoped_enum_v<ScopedEnum>);

struct NotTriviallyCopyable { NotTriviallyCopyable(const NotTriviallyCopyable&) {} };
static_assert(false == std::is_trivially_copyable_v<NotTriviallyCopyable>);

struct PolymorphicParent { virtual void boo(); };
struct PolymorphicChild : PolymorphicParent { void boo() override; };
static_assert(std::is_polymorphic_v<PolymorphicParent>);
static_assert(std::is_polymorphic_v<PolymorphicChild>);

/// Struct layout : [a ][b ][c ]
///                 [00][01][02]
/// Struct size   : 3
/// Wire size     : 3
struct FlatCompactAggregate_3b {
    _1b a, b, c;
    bool operator==(const FlatCompactAggregate_3b&) const = default;
};

static_assert(std::is_aggregate_v<FlatCompactAggregate_3b>);
static_assert(std::is_trivially_copyable_v<FlatCompactAggregate_3b>);
static_assert(sizeof(FlatCompactAggregate_3b)  == 3);
static_assert(alignof(FlatCompactAggregate_3b) == 1);
static_assert(offsetof(FlatCompactAggregate_3b, a) == 0);
static_assert(offsetof(FlatCompactAggregate_3b, b) == 1);
static_assert(offsetof(FlatCompactAggregate_3b, c) == 2);

/// Struct layout : [a             ][b             ][c             ]
///                 [00][01][02][03][04][05][06][07][08][09][10][11]
/// Struct size   : 12
/// Wire size     : 12
struct FlatCompactAggregate_12b {
    _4b a, b, c;
    bool operator==(const FlatCompactAggregate_12b&) const = default;
};

static_assert(std::is_aggregate_v<FlatCompactAggregate_12b>);
static_assert(std::is_trivially_copyable_v<FlatCompactAggregate_12b>);
static_assert(sizeof(FlatCompactAggregate_12b)  == 12);
static_assert(alignof(FlatCompactAggregate_12b) == 4);
static_assert(offsetof(FlatCompactAggregate_12b, a) == 0);
static_assert(offsetof(FlatCompactAggregate_12b, b) == 4);
static_assert(offsetof(FlatCompactAggregate_12b, c) == 8);

/// Struct layout : [a ][b ][pad   ][c             ]
///                 [00][01][02][03][04][05][06][07]
/// Struct size   : 8
/// Wire size     : 6
struct FlatInternalPaddedAggregate_8b {
    _1b a; _1b b; _4b c;
    bool operator==(const FlatInternalPaddedAggregate_8b&) const = default;
};

static_assert(std::is_aggregate_v<FlatInternalPaddedAggregate_8b>);
static_assert(std::is_trivially_copyable_v<FlatInternalPaddedAggregate_8b>);
static_assert(sizeof(FlatInternalPaddedAggregate_8b)  == 8);
static_assert(alignof(FlatInternalPaddedAggregate_8b) == 4);
static_assert(offsetof(FlatInternalPaddedAggregate_8b, a) == 0);
static_assert(offsetof(FlatInternalPaddedAggregate_8b, b) == 1);
static_assert(offsetof(FlatInternalPaddedAggregate_8b, c) == 4); // +2b pad before c

/// Struct layout : [a ][pad       ][b             ][c ][pad       ]
///                 [00][01][02][03][04][05][06][07][08][09][10][11]
/// Struct size   : 12
/// Wire size     : 6
struct FlatTailPaddedAggregate_12b {
    _1b a; _4b b; _1b c;
    bool operator==(const FlatTailPaddedAggregate_12b&) const = default;
};

static_assert(std::is_aggregate_v<FlatTailPaddedAggregate_12b>);
static_assert(std::is_trivially_copyable_v<FlatTailPaddedAggregate_12b>);
static_assert(sizeof(FlatTailPaddedAggregate_12b)  == 12);
static_assert(alignof(FlatTailPaddedAggregate_12b) == 4);
static_assert(offsetof(FlatTailPaddedAggregate_12b, a) == 0);
static_assert(offsetof(FlatTailPaddedAggregate_12b, b) == 4); // +3b pad before b
static_assert(offsetof(FlatTailPaddedAggregate_12b, c) == 8); // +3b tail pad after c

/// Struct layout : [a ][b0][b1][b2]
///                 [00][01][02][03]
/// Struct size   : 4
/// Wire size     : 4
struct WithCompactArray_4b {
    _1b a; std::array<_1b, 3> b;
    bool operator==(const WithCompactArray_4b&) const = default;
};

static_assert(std::is_aggregate_v<WithCompactArray_4b>);
static_assert(std::is_trivially_copyable_v<WithCompactArray_4b>);
static_assert(sizeof(WithCompactArray_4b)  == 4);
static_assert(alignof(WithCompactArray_4b) == 1);
static_assert(offsetof(WithCompactArray_4b, a) == 0);
static_assert(offsetof(WithCompactArray_4b, b) == 1);

/// Struct layout : [a ][pad       ][b0            ][b1            ]
///                 [00][01][02][03][04][05][06][07][08][09][10][11]
/// Struct size   : 12
/// Wire size     : 9
struct WithPaddedArray_12b {
    _1b a; std::array<_4b, 2> b;
    bool operator==(const WithPaddedArray_12b&) const = default;
};

static_assert(std::is_aggregate_v<WithPaddedArray_12b>);
static_assert(std::is_trivially_copyable_v<WithPaddedArray_12b>);
static_assert(sizeof(WithPaddedArray_12b)  == 12);
static_assert(alignof(WithPaddedArray_12b) == 4);
static_assert(offsetof(WithPaddedArray_12b, a) == 0);
static_assert(offsetof(WithPaddedArray_12b, b) == 4); // +3b pad before b

/// Struct layout : [a         ][b ][c ]
///                 [00][01][02][03][04]
/// Struct size   : 5
/// Wire size     : 5
struct NestedCompactAggregate_5b {
    FlatCompactAggregate_3b a; _1b b, c;
    bool operator==(const NestedCompactAggregate_5b&) const = default;
};

static_assert(std::is_aggregate_v<NestedCompactAggregate_5b>);
static_assert(std::is_trivially_copyable_v<NestedCompactAggregate_5b>);
static_assert(sizeof(NestedCompactAggregate_5b)  == 5);
static_assert(alignof(NestedCompactAggregate_5b) == 1);
static_assert(offsetof(NestedCompactAggregate_5b, a) == 0);
static_assert(offsetof(NestedCompactAggregate_5b, b) == 3);
static_assert(offsetof(NestedCompactAggregate_5b, c) == 4);

/// Struct layout : [a ][pad       ][ba][bb][pad   ][bc            ]
///                 [00][01][02][03][04][05][06][07][08][09][10][11]
/// Struct size   : 12
/// Wire size     : 7
struct NestedPaddedAggregate_12b {
    _1b a; FlatInternalPaddedAggregate_8b b;
    bool operator==(const NestedPaddedAggregate_12b&) const = default;
};

static_assert(std::is_aggregate_v<NestedPaddedAggregate_12b>);
static_assert(std::is_trivially_copyable_v<NestedPaddedAggregate_12b>);
static_assert(sizeof(NestedPaddedAggregate_12b)  == 12);
static_assert(alignof(NestedPaddedAggregate_12b) == 4);
static_assert(offsetof(NestedPaddedAggregate_12b, a) == 0);
static_assert(offsetof(NestedPaddedAggregate_12b, b) == 4); // +3b pad before b

/// Struct layout : [aa        ][ab][ac][pad       ][b                             ]
///                 [00][01][02][03][04][05][06][07][08][09][10][11][12][13][14][15]
/// Struct size   : 16
/// Wire size     : 13
struct DeeplyNestedAggregate_16b {
    NestedCompactAggregate_5b a; _8b b;
    bool operator==(const DeeplyNestedAggregate_16b&) const = default;
};

static_assert(std::is_aggregate_v<DeeplyNestedAggregate_16b>);
static_assert(std::is_trivially_copyable_v<DeeplyNestedAggregate_16b>);
static_assert(sizeof(DeeplyNestedAggregate_16b)  == 16);
static_assert(alignof(DeeplyNestedAggregate_16b) == 8);
static_assert(offsetof(DeeplyNestedAggregate_16b, a) == 0);
static_assert(offsetof(DeeplyNestedAggregate_16b, b) == 8); // +3b pad before b

/// Struct layout : [aa][ab][ac][aa][ab][ac][b     ]
///                 [00][01][02][03][04][05][06][07]
/// Struct size   : 8
/// Wire size     : 8
struct ArrayOfStructsAggregate_8b {
    std::array<FlatCompactAggregate_3b, 2> a; _2b b;
    bool operator==(const ArrayOfStructsAggregate_8b&) const = default;
};

static_assert(std::is_aggregate_v<ArrayOfStructsAggregate_8b>);
static_assert(std::is_trivially_copyable_v<ArrayOfStructsAggregate_8b>);
static_assert(sizeof(ArrayOfStructsAggregate_8b)  == 8);
static_assert(alignof(ArrayOfStructsAggregate_8b) == 2);
static_assert(offsetof(ArrayOfStructsAggregate_8b, a) == 0);
static_assert(offsetof(ArrayOfStructsAggregate_8b, b) == 6);

/// Structs with annotated fields

// GCC complains about offsetof on non-standard-layout, but for this scope its ok.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"

/// Struct layout : [a ][pad       ][*b            ]
///                 [00][01][02][03][04][05][06][07]
/// Wire layout   : [a ]
/// Struct size   : 8 (or 2 words)
/// Wire size     : 1
struct WithSkippedPointer_2w {
    _1b a;
    [[=stse::skip]] _1b* b;
    bool operator==(const WithSkippedPointer_2w&) const = default;
};
static_assert(std::is_aggregate_v<WithSkippedPointer_2w>);
static_assert(std::is_trivially_copyable_v<WithSkippedPointer_2w>);
static_assert(sizeof(WithSkippedPointer_2w)  == 2 * WORD);
static_assert(alignof(WithSkippedPointer_2w) == WORD);
static_assert(offsetof(WithSkippedPointer_2w, a) == 0);
static_assert(offsetof(WithSkippedPointer_2w, b) == WORD);

/// Struct layout : [a ][pad       ][*b            ][c ][pad       ][*d            ]
///                 [00][01][02][03][04][05][06][07][08][09][10][11][12][13][14][15]
/// Wire layout   : [a ][c ]
/// Struct size   : 16
/// Wire size     : 2
struct WithMultiSkippedPointers_4w {
    _1b a;
    [[=stse::skip]] _1b* b;
    _1b c;
    [[=stse::skip]] _1b* d;
    bool operator==(const WithMultiSkippedPointers_4w&) const = default;
};
static_assert(std::is_aggregate_v<WithMultiSkippedPointers_4w>);
static_assert(std::is_trivially_copyable_v<WithMultiSkippedPointers_4w>);
static_assert(sizeof(WithMultiSkippedPointers_4w)  == 4 * WORD);
static_assert(alignof(WithMultiSkippedPointers_4w) == WORD);
static_assert(offsetof(WithMultiSkippedPointers_4w, a) == 0);
static_assert(offsetof(WithMultiSkippedPointers_4w, b) == WORD);
static_assert(offsetof(WithMultiSkippedPointers_4w, c) == 2 * WORD);
static_assert(offsetof(WithMultiSkippedPointers_4w, d) == 3 * WORD);


/// Struct layout : [a ][pad       ][*b            ][c ][pad       ]
///                 [00][01][02][03][04][05][06][07][08][09][10][11]
/// Wire layout   : [a ][c ]
/// Struct size   : 12 (or 3 words)
/// Wire size     : 2
struct WithSkippedParent_3w : WithSkippedPointer_2w {
    _1b c;
    bool operator==(const WithSkippedParent_3w&) const = default;
};
static_assert(std::is_aggregate_v<WithSkippedParent_3w>);
static_assert(std::is_trivially_copyable_v<WithSkippedParent_3w>);
static_assert(sizeof(WithSkippedParent_3w)  == 3 * WORD);
static_assert(alignof(WithSkippedParent_3w) == WORD);
static_assert(offsetof(WithSkippedParent_3w, a) == 0);
static_assert(offsetof(WithSkippedParent_3w, b) == WORD);
static_assert(offsetof(WithSkippedParent_3w, c) == 2 * WORD);

/// Struct layout : [a ][pad       ][*b            ]
///                 [00][01][02][03][04][05][06][07]
/// Wire layout   : [a ][          ]
/// Struct size   : 8 (or 2 words)
/// Wire size     : 5 (or 1b + 1 word)
struct WithIgnoredPointer_2w {
    _1b a;
    [[=stse::ignore]] _1b* b;
    bool operator==(const WithIgnoredPointer_2w&) const = default;
};
static_assert(std::is_aggregate_v<WithIgnoredPointer_2w>);
static_assert(std::is_trivially_copyable_v<WithIgnoredPointer_2w>);
static_assert(sizeof(WithIgnoredPointer_2w)  == 2 * WORD);
static_assert(alignof(WithIgnoredPointer_2w) == WORD);
static_assert(offsetof(WithIgnoredPointer_2w, a) == 0);
static_assert(offsetof(WithIgnoredPointer_2w, b) == WORD);

#pragma GCC diagnostic pop

} // namespace stse::tests
