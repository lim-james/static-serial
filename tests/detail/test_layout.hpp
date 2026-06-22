#pragma once

#include "tests/types.hpp"
#include "stse/detail/layout.hpp"

#include <cstdint>
#include <array>

namespace stse::tests {

static_assert(stse::detail::raw_size<char>   == sizeof(char));
static_assert(stse::detail::raw_size<bool>   == sizeof(bool));
static_assert(stse::detail::raw_size<int>    == sizeof(int));
static_assert(stse::detail::raw_size<double> == sizeof(double));
static_assert(stse::detail::raw_size<float>  == sizeof(float));

static_assert(stse::detail::raw_size<std::uint8_t>  == sizeof(std::uint8_t));
static_assert(stse::detail::raw_size<std::uint16_t> == sizeof(std::uint16_t));
static_assert(stse::detail::raw_size<std::uint32_t> == sizeof(std::uint32_t));
static_assert(stse::detail::raw_size<std::uint64_t> == sizeof(std::uint64_t));


static_assert(stse::detail::raw_size<FlatCompactAggregate_3b>  == sizeof(FlatCompactAggregate_3b));
static_assert(stse::detail::raw_size<FlatCompactAggregate_3b>  == 3);

static_assert(stse::detail::raw_size<FlatCompactAggregate_12b> == sizeof(FlatCompactAggregate_12b));
static_assert(stse::detail::raw_size<FlatCompactAggregate_12b> == 12);


static_assert(stse::detail::raw_size<FlatInternalPaddedAggregate_8b> != sizeof(FlatInternalPaddedAggregate_8b));
static_assert(stse::detail::raw_size<FlatInternalPaddedAggregate_8b> == 6);


static_assert(stse::detail::raw_size<FlatTailPaddedAggregate_12b> != sizeof(FlatTailPaddedAggregate_12b));
static_assert(stse::detail::raw_size<FlatTailPaddedAggregate_12b> == 6);


static_assert(stse::detail::raw_size<WithCompactArray_4b> == sizeof(WithCompactArray_4b));
static_assert(stse::detail::raw_size<WithCompactArray_4b> == 4);

static_assert(stse::detail::raw_size<WithPaddedArray_12b> != sizeof(WithPaddedArray_12b));
static_assert(stse::detail::raw_size<WithPaddedArray_12b> == 9);


static_assert(stse::detail::raw_size<NestedCompactAggregate_5b> == sizeof(NestedCompactAggregate_5b));
static_assert(stse::detail::raw_size<NestedCompactAggregate_5b> == 5);

static_assert(stse::detail::raw_size<NestedPaddedAggregate_12b> != sizeof(NestedPaddedAggregate_12b));
static_assert(stse::detail::raw_size<NestedPaddedAggregate_12b> == 7);

static_assert(stse::detail::raw_size<DeeplyNestedAggregate_16b> != sizeof(DeeplyNestedAggregate_16b));
static_assert(stse::detail::raw_size<DeeplyNestedAggregate_16b> == 13);

static_assert(stse::detail::raw_size<ArrayOfStructsAggregate_8b> == sizeof(ArrayOfStructsAggregate_8b));
static_assert(stse::detail::raw_size<ArrayOfStructsAggregate_8b> == 8);

static_assert(stse::detail::raw_size<std::array<_1b, N>> == N);
static_assert(stse::detail::raw_size<std::array<_4b, N>> == 4 * N);
static_assert(stse::detail::raw_size<std::array<_8b, N>> == 8 * N);
static_assert(stse::detail::raw_size<std::array<FlatCompactAggregate_3b, N>>
    == stse::detail::raw_size<FlatCompactAggregate_3b> * N);

static_assert(stse::detail::raw_size<WithSkippedPointer_2w> != sizeof(WithSkippedPointer_2w));
static_assert(stse::detail::raw_size<WithSkippedPointer_2w> == 1);
static_assert(stse::detail::raw_size<WithMultiSkippedPointers_4w> != sizeof(WithMultiSkippedPointers_4w));
static_assert(stse::detail::raw_size<WithMultiSkippedPointers_4w> == 2);
static_assert(stse::detail::raw_size<WithSkippedParent_3w> != sizeof(WithSkippedParent_3w));
static_assert(stse::detail::raw_size<WithSkippedParent_3w> == 2);
static_assert(stse::detail::raw_size<WithIgnoredPointer_2w> != sizeof(WithIgnoredPointer_2w));
static_assert(stse::detail::raw_size<WithIgnoredPointer_2w> == 1 + WORD);


static_assert(stse::detail::count_byte_ranges<FlatCompactAggregate_3b>() == 1);
static_assert(stse::detail::byte_layout_of<FlatCompactAggregate_3b>[0] ==
    stse::detail::ByteRange{.struct_offset = 0, .wire_offset = 0, .count = 3});

static_assert(stse::detail::count_byte_ranges<FlatCompactAggregate_12b>() == 1);
static_assert(stse::detail::byte_layout_of<FlatCompactAggregate_12b>[0] ==
    stse::detail::ByteRange{.struct_offset = 0, .wire_offset = 0, .count = 12});

static_assert(stse::detail::count_byte_ranges<FlatInternalPaddedAggregate_8b>() == 2);
static_assert(stse::detail::byte_layout_of<FlatInternalPaddedAggregate_8b>[0] ==
    stse::detail::ByteRange{.struct_offset = 0, .wire_offset = 0, .count = 2});
static_assert(stse::detail::byte_layout_of<FlatInternalPaddedAggregate_8b>[1] ==
    stse::detail::ByteRange{.struct_offset = 4, .wire_offset = 2, .count = 4});

static_assert(stse::detail::count_byte_ranges<FlatTailPaddedAggregate_12b>() == 2);
static_assert(stse::detail::byte_layout_of<FlatTailPaddedAggregate_12b>[0] ==
    stse::detail::ByteRange{.struct_offset = 0, .wire_offset = 0, .count = 1});
static_assert(stse::detail::byte_layout_of<FlatTailPaddedAggregate_12b>[1] ==
    stse::detail::ByteRange{.struct_offset = 4, .wire_offset = 1, .count = 5});

static_assert(stse::detail::count_byte_ranges<WithCompactArray_4b>() == 1);
static_assert(stse::detail::byte_layout_of<WithCompactArray_4b>[0] ==
    stse::detail::ByteRange{.struct_offset = 0, .wire_offset = 0, .count = 4});

static_assert(stse::detail::count_byte_ranges<WithPaddedArray_12b>() == 2);
static_assert(stse::detail::byte_layout_of<WithPaddedArray_12b>[0] ==
    stse::detail::ByteRange{.struct_offset = 0, .wire_offset = 0, .count = 1});
static_assert(stse::detail::byte_layout_of<WithPaddedArray_12b>[1] ==
    stse::detail::ByteRange{.struct_offset = 4, .wire_offset = 1, .count = 8});

static_assert(stse::detail::count_byte_ranges<NestedCompactAggregate_5b>() == 1);
static_assert(stse::detail::byte_layout_of<NestedCompactAggregate_5b>[0] ==
    stse::detail::ByteRange{.struct_offset = 0, .wire_offset = 0, .count = 5});

static_assert(stse::detail::count_byte_ranges<NestedPaddedAggregate_12b>() == 3);
static_assert(stse::detail::byte_layout_of<NestedPaddedAggregate_12b>[0] ==
    stse::detail::ByteRange{.struct_offset = 0, .wire_offset = 0, .count = 1});
static_assert(stse::detail::byte_layout_of<NestedPaddedAggregate_12b>[1] ==
    stse::detail::ByteRange{.struct_offset = 4, .wire_offset = 1, .count = 2});
static_assert(stse::detail::byte_layout_of<NestedPaddedAggregate_12b>[2] ==
    stse::detail::ByteRange{.struct_offset = 8, .wire_offset = 3, .count = 4});

static_assert(stse::detail::count_byte_ranges<DeeplyNestedAggregate_16b>() == 2);
static_assert(stse::detail::byte_layout_of<DeeplyNestedAggregate_16b>[0] ==
    stse::detail::ByteRange{.struct_offset = 0, .wire_offset = 0, .count = 5});
static_assert(stse::detail::byte_layout_of<DeeplyNestedAggregate_16b>[1] ==
    stse::detail::ByteRange{.struct_offset = 8, .wire_offset = 5, .count = 8});

static_assert(stse::detail::count_byte_ranges<ArrayOfStructsAggregate_8b>() == 1);
static_assert(stse::detail::byte_layout_of<ArrayOfStructsAggregate_8b>[0] ==
    stse::detail::ByteRange{.struct_offset = 0, .wire_offset = 0, .count = 8});

static_assert(stse::detail::count_byte_ranges<std::array<_1b, N>>() == 1);
static_assert(stse::detail::byte_layout_of<std::array<_1b, N>>[0] ==
    stse::detail::ByteRange{.struct_offset = 0, .wire_offset = 0, .count = N});

static_assert(stse::detail::count_byte_ranges<std::array<_4b, N>>() == 1);
static_assert(stse::detail::byte_layout_of<std::array<_4b, N>>[0] ==
    stse::detail::ByteRange{.struct_offset = 0, .wire_offset = 0, .count = 4 * N});

// [a ][b ][pad   ][c             ][a ][b ][pad   ][c             ][a ][b ][pad   ][c             ]
// [00][01][02][03][04][05][06][07][08][09][10][11][12][13][14][15][16][17][18][19][20][21][22][23]
// [======][pad   ][======================][pad   ][======================][pad   ][==============]

static_assert(stse::detail::count_byte_ranges<std::array<FlatInternalPaddedAggregate_8b, 3>>() == 4);
static_assert(stse::detail::byte_layout_of<std::array<FlatInternalPaddedAggregate_8b, 3>>[0] ==
    stse::detail::ByteRange{.struct_offset =  0, .wire_offset =  0, .count = 2});
static_assert(stse::detail::byte_layout_of<std::array<FlatInternalPaddedAggregate_8b, 3>>[1] ==
    stse::detail::ByteRange{.struct_offset =  4, .wire_offset =  2, .count = 6});
static_assert(stse::detail::byte_layout_of<std::array<FlatInternalPaddedAggregate_8b, 3>>[2] ==
    stse::detail::ByteRange{.struct_offset = 12, .wire_offset =  8, .count = 6});
static_assert(stse::detail::byte_layout_of<std::array<FlatInternalPaddedAggregate_8b, 3>>[3] ==
    stse::detail::ByteRange{.struct_offset = 20, .wire_offset = 14, .count = 4});

static_assert(stse::detail::count_byte_ranges<WithSkippedPointer_2w>() == 1);
static_assert(stse::detail::byte_layout_of<WithSkippedPointer_2w>[0] ==
    stse::detail::ByteRange{.struct_offset =  0, .wire_offset =  0, .count = 1});

static_assert(stse::detail::count_byte_ranges<WithMultiSkippedPointers_4w>() == 2);
static_assert(stse::detail::byte_layout_of<WithMultiSkippedPointers_4w>[0] ==
    stse::detail::ByteRange{.struct_offset =  0, .wire_offset =  0, .count = 1});
static_assert(stse::detail::byte_layout_of<WithMultiSkippedPointers_4w>[1] ==
    stse::detail::ByteRange{.struct_offset =  2 * WORD, .wire_offset =  1, .count = 1});

static_assert(stse::detail::count_byte_ranges<WithSkippedParent_3w>() == 2);
static_assert(stse::detail::byte_layout_of<WithSkippedParent_3w>[0] ==
    stse::detail::ByteRange{.struct_offset =  0, .wire_offset =  0, .count = 1});
static_assert(stse::detail::byte_layout_of<WithSkippedParent_3w>[1] ==
    stse::detail::ByteRange{.struct_offset =  2 * WORD, .wire_offset =  1, .count = 1});

static_assert(stse::detail::count_byte_ranges<WithIgnoredPointer_2w>() == 1);
static_assert(stse::detail::byte_layout_of<WithIgnoredPointer_2w>[0] ==
    stse::detail::ByteRange{.struct_offset =  0, .wire_offset =  0, .count = 1});


} // namespace stse::tests
