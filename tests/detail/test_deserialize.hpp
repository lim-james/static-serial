#pragma once

#include "tests/types.hpp"
#include "tests/utils.hpp"

#include "stse/stse.hpp"

#include <cstdint>

namespace stse::tests {

template<Scalar T, stse::detail::EndianType Endian>
constexpr bool validate_scalar_deserialization(T expected, std::span<std::byte> raw_bytes) noexcept {
    T result{};
    stse::detail::deserialize_scalar(result, std::span<const std::byte>{raw_bytes}, Endian{});
    return result == expected;
}

template<typename T, std::size_t N, auto deserialize_fn>
constexpr bool validate_container_deserialization(
    std::array<T, N> expected,
    std::span<std::byte> raw_bytes
) noexcept {
    static_assert(StaticContainer<decltype(expected)>);
    std::array<T, N> result{};
    deserialize_fn(result, raw_bytes);
    return std::ranges::equal(expected, result);
}

template<Aggregate T, auto deserialize_fn>
constexpr bool validate_aggregate_deserialization(
    T expected,
    std::span<std::byte> raw_bytes
) noexcept {
    T result{};
    deserialize_fn(result, raw_bytes);
    return result == expected;
}

constexpr bool test_deserialize_scalar() noexcept {
    bool native_u8 = TestExecutor{
        "detail::deserialize_scalar<u8, native>",
        &validate_scalar_deserialization<std::uint8_t, stse::detail::NativeEndian>
    }
    .run_single(static_cast<std::uint8_t>(0),   make_bytes(0x00))
    .run_single(static_cast<std::uint8_t>(67),  make_bytes(0x43))
    .run_single(static_cast<std::uint8_t>(128), make_bytes(0x80))
    .run_single(static_cast<std::uint8_t>(255), make_bytes(0xFF));

    bool big_u8 = TestExecutor{
        "detail::deserialize_scalar<u8, big>",
        &validate_scalar_deserialization<std::uint8_t, stse::detail::BigEndian>
    }
    .run_single(static_cast<std::uint8_t>(0),   make_bytes(0x00))
    .run_single(static_cast<std::uint8_t>(67),  make_bytes(0x43))
    .run_single(static_cast<std::uint8_t>(128), make_bytes(0x80))
    .run_single(static_cast<std::uint8_t>(255), make_bytes(0xFF));

    bool little_u8 = TestExecutor{
        "detail::deserialize_scalar<u8, little>",
        &validate_scalar_deserialization<std::uint8_t, stse::detail::LittleEndian>
    }
    .run_single(static_cast<std::uint8_t>(0),   make_bytes(0x00))
    .run_single(static_cast<std::uint8_t>(67),  make_bytes(0x43))
    .run_single(static_cast<std::uint8_t>(128), make_bytes(0x80))
    .run_single(static_cast<std::uint8_t>(255), make_bytes(0xFF));

    bool native_u16 = TestExecutor{
        "detail::deserialize_scalar<u16, native>",
        &validate_scalar_deserialization<std::uint16_t, stse::detail::NativeEndian>
    }
    .run_single(static_cast<std::uint16_t>(0),      make_bytes(0x00, 0x00))
    .run_single(static_cast<std::uint16_t>(128),    make_bytes(0x80, 0x00))
    .run_single(static_cast<std::uint16_t>(255),    make_bytes(0xFF, 0x00))
    .run_single(static_cast<std::uint16_t>(1024),   make_bytes(0x00, 0x04))
    .run_single(static_cast<std::uint16_t>(0x1234), make_bytes(0x34, 0x12))
    .run_single(static_cast<std::uint16_t>(65535),  make_bytes(0xFF, 0xFF));

    bool big_u16 = TestExecutor{
        "detail::deserialize_scalar<u16, big>",
        &validate_scalar_deserialization<std::uint16_t, stse::detail::BigEndian>
    }
    .run_single(static_cast<std::uint16_t>(0),      make_bytes(0x00, 0x00))
    .run_single(static_cast<std::uint16_t>(128),    make_bytes(0x00, 0x80))
    .run_single(static_cast<std::uint16_t>(255),    make_bytes(0x00, 0xFF))
    .run_single(static_cast<std::uint16_t>(1024),   make_bytes(0x04, 0x00))
    .run_single(static_cast<std::uint16_t>(0x1234), make_bytes(0x12, 0x34))
    .run_single(static_cast<std::uint16_t>(65535),  make_bytes(0xFF, 0xFF));

    bool little_u16 = TestExecutor{
        "detail::deserialize_scalar<u16, little>",
        &validate_scalar_deserialization<std::uint16_t, stse::detail::LittleEndian>
    }
    .run_single(static_cast<std::uint16_t>(0),      make_bytes(0x00, 0x00))
    .run_single(static_cast<std::uint16_t>(128),    make_bytes(0x80, 0x00))
    .run_single(static_cast<std::uint16_t>(255),    make_bytes(0xFF, 0x00))
    .run_single(static_cast<std::uint16_t>(1024),   make_bytes(0x00, 0x04))
    .run_single(static_cast<std::uint16_t>(0x1234), make_bytes(0x34, 0x12))
    .run_single(static_cast<std::uint16_t>(65535),  make_bytes(0xFF, 0xFF));

    bool native_u32 = TestExecutor{
        "detail::deserialize_scalar<u32, native>",
        &validate_scalar_deserialization<std::uint32_t, stse::detail::NativeEndian>
    }
    .run_single(static_cast<std::uint32_t>(0),          make_bytes(0x00, 0x00, 0x00, 0x00))
    .run_single(static_cast<std::uint32_t>(255),        make_bytes(0xFF, 0x00, 0x00, 0x00))
    .run_single(static_cast<std::uint32_t>(65535),      make_bytes(0xFF, 0xFF, 0x00, 0x00))
    .run_single(static_cast<std::uint32_t>(0x12345678), make_bytes(0x78, 0x56, 0x34, 0x12))
    .run_single(static_cast<std::uint32_t>(0xDEADBEEF), make_bytes(0xEF, 0xBE, 0xAD, 0xDE))
    .run_single(static_cast<std::uint32_t>(0xFFFFFFFF), make_bytes(0xFF, 0xFF, 0xFF, 0xFF));

    bool big_u32 = TestExecutor{
        "detail::deserialize_scalar<u32, big>",
        &validate_scalar_deserialization<std::uint32_t, stse::detail::BigEndian>
    }
    .run_single(static_cast<std::uint32_t>(0),          make_bytes(0x00, 0x00, 0x00, 0x00))
    .run_single(static_cast<std::uint32_t>(255),        make_bytes(0x00, 0x00, 0x00, 0xFF))
    .run_single(static_cast<std::uint32_t>(65535),      make_bytes(0x00, 0x00, 0xFF, 0xFF))
    .run_single(static_cast<std::uint32_t>(0x12345678), make_bytes(0x12, 0x34, 0x56, 0x78))
    .run_single(static_cast<std::uint32_t>(0xDEADBEEF), make_bytes(0xDE, 0xAD, 0xBE, 0xEF))
    .run_single(static_cast<std::uint32_t>(0xFFFFFFFF), make_bytes(0xFF, 0xFF, 0xFF, 0xFF));

    bool little_u32 = TestExecutor{
        "detail::deserialize_scalar<u32, little>",
        &validate_scalar_deserialization<std::uint32_t, stse::detail::LittleEndian>
    }
    .run_single(static_cast<std::uint32_t>(0),          make_bytes(0x00, 0x00, 0x00, 0x00))
    .run_single(static_cast<std::uint32_t>(255),        make_bytes(0xFF, 0x00, 0x00, 0x00))
    .run_single(static_cast<std::uint32_t>(65535),      make_bytes(0xFF, 0xFF, 0x00, 0x00))
    .run_single(static_cast<std::uint32_t>(0x12345678), make_bytes(0x78, 0x56, 0x34, 0x12))
    .run_single(static_cast<std::uint32_t>(0xDEADBEEF), make_bytes(0xEF, 0xBE, 0xAD, 0xDE))
    .run_single(static_cast<std::uint32_t>(0xFFFFFFFF), make_bytes(0xFF, 0xFF, 0xFF, 0xFF));

    bool native_u64 = TestExecutor{
        "detail::deserialize_scalar<u64, native>",
        &validate_scalar_deserialization<std::uint64_t, stse::detail::NativeEndian>
    }
    .run_single(static_cast<std::uint64_t>(0),                   make_bytes(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00))
    .run_single(static_cast<std::uint64_t>(0xFF),                make_bytes(0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00))
    .run_single(static_cast<std::uint64_t>(0xFFFFFFFF),          make_bytes(0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00))
    .run_single(static_cast<std::uint64_t>(0x123456789ABCDEF0),  make_bytes(0xF0, 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12))
    .run_single(static_cast<std::uint64_t>(0xDEADBEEFCAFEBABE), make_bytes(0xBE, 0xBA, 0xFE, 0xCA, 0xEF, 0xBE, 0xAD, 0xDE))
    .run_single(static_cast<std::uint64_t>(0xFFFFFFFFFFFFFFFF),  make_bytes(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF));

    bool big_u64 = TestExecutor{
        "detail::deserialize_scalar<u64, big>",
        &validate_scalar_deserialization<std::uint64_t, stse::detail::BigEndian>
    }
    .run_single(static_cast<std::uint64_t>(0),                   make_bytes(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00))
    .run_single(static_cast<std::uint64_t>(0xFF),                make_bytes(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF))
    .run_single(static_cast<std::uint64_t>(0xFFFFFFFF),          make_bytes(0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF))
    .run_single(static_cast<std::uint64_t>(0x123456789ABCDEF0),  make_bytes(0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0))
    .run_single(static_cast<std::uint64_t>(0xDEADBEEFCAFEBABE), make_bytes(0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE))
    .run_single(static_cast<std::uint64_t>(0xFFFFFFFFFFFFFFFF),  make_bytes(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF));

    bool little_u64 = TestExecutor{
        "detail::deserialize_scalar<u64, little>",
        &validate_scalar_deserialization<std::uint64_t, stse::detail::LittleEndian>
    }
    .run_single(static_cast<std::uint64_t>(0),                   make_bytes(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00))
    .run_single(static_cast<std::uint64_t>(0xFF),                make_bytes(0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00))
    .run_single(static_cast<std::uint64_t>(0xFFFFFFFF),          make_bytes(0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00))
    .run_single(static_cast<std::uint64_t>(0x123456789ABCDEF0),  make_bytes(0xF0, 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12))
    .run_single(static_cast<std::uint64_t>(0xDEADBEEFCAFEBABE), make_bytes(0xBE, 0xBA, 0xFE, 0xCA, 0xEF, 0xBE, 0xAD, 0xDE))
    .run_single(static_cast<std::uint64_t>(0xFFFFFFFFFFFFFFFF),  make_bytes(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF));

    return native_u8  && big_u8  && little_u8
        && native_u16 && big_u16 && little_u16
        && native_u32 && big_u32 && little_u32
        && native_u64 && big_u64 && little_u64;
}

template<auto deserialize_fn>
constexpr bool test_deserialize_container() noexcept {
    return TestExecutor{
        "detail::deserialize_container<array<u16, 6>>",
        &validate_container_deserialization<std::uint16_t, 6, deserialize_fn>
    }
    .run_single(
        std::array<std::uint16_t, 6>{0, 0x80, 0xFF, 0x400, 0x1234, 0xFFFF},
        make_bytes(0x00, 0x00, 0x80, 0x00, 0xFF, 0x00,
                   0x00, 0x04, 0x34, 0x12, 0xFF, 0xFF)
    );
}

template<auto deserialize_fn>
constexpr bool test_deserialize_aggregate() noexcept {
    bool flat_compact = TestExecutor{
        "detail::deserialize_aggregate<FlatCompactAggregate_3b>",
        &validate_aggregate_deserialization<FlatCompactAggregate_3b, deserialize_fn>
    }
    .run_single(
        FlatCompactAggregate_3b{0x80, 0xFF, 0x40},
        make_bytes(0x80, 0xFF, 0x40)
    );

    bool flat_internal_padded = TestExecutor{
        "detail::deserialize_aggregate<FlatInternalPaddedAggregate_8b>",
        &validate_aggregate_deserialization<FlatInternalPaddedAggregate_8b, deserialize_fn>
    }
    .run_single(
        FlatInternalPaddedAggregate_8b{0x80, 0xFF, 0x4012'3456},
        make_bytes(0x80, 0xFF, 0x56, 0x34, 0x12, 0x40)
    );

    bool with_compact_array = TestExecutor{
        "detail::deserialize_aggregate<WithCompactArray_4b>",
        &validate_aggregate_deserialization<WithCompactArray_4b, deserialize_fn>
    }
    .run_single(
        WithCompactArray_4b{0x80, {0xFF, 0x40, 0x12}},
        make_bytes(0x80, 0xFF, 0x40, 0x12)
    );

    bool nested_compact = TestExecutor{
        "detail::deserialize_aggregate<NestedCompactAggregate_5b>",
        &validate_aggregate_deserialization<NestedCompactAggregate_5b, deserialize_fn>
    }
    .run_single(
        NestedCompactAggregate_5b{{0xFF, 0x40, 0x12}, 0x80, 0xEA},
        make_bytes(0xFF, 0x40, 0x12, 0x80, 0xEA)
    );

    bool deeply_nested = TestExecutor{
        "detail::deserialize_aggregate<DeeplyNestedAggregate_16b>",
        &validate_aggregate_deserialization<DeeplyNestedAggregate_16b, deserialize_fn>
    }
    .run_single(
        DeeplyNestedAggregate_16b{{{0xFF, 0x40, 0x12}, 0x80, 0xEA}, 0xDEADBEEFCAFEBABE},
        make_bytes(
            0xFF, 0x40, 0x12, 0x80, 0xEA,
            0xBE, 0xBA, 0xFE, 0xCA, 0xEF, 0xBE, 0xAD, 0xDE
        )
    );

    return flat_compact && flat_internal_padded
        && with_compact_array
        && nested_compact && deeply_nested;
}

template<auto deserialize_fn>
constexpr bool test_deserialize_annotated_aggregate() noexcept {
    bool skipped_pointer = TestExecutor{
        "detail::deserialize_aggregate<WithSkippedPointer_2w>",
        &validate_aggregate_deserialization<WithSkippedPointer_2w, deserialize_fn>
    }
    .run_single(
        WithSkippedPointer_2w{0xBE, nullptr},
        make_bytes(0xBE)
    );

    bool ignored_pointer;

    if constexpr (sizeof(void*) == 4) {
        ignored_pointer = TestExecutor{
            "detail::deserialize_aggregate<WithIgnoredPointer_2w>",
            &validate_aggregate_deserialization<WithIgnoredPointer_2w, deserialize_fn>
        }
        .run_single(
            WithIgnoredPointer_2w{0xBE, nullptr},
            make_bytes(0xBE, 0x00, 0x00, 0x00, 0x00)
        );
    } else {
        ignored_pointer = TestExecutor{
            "detail::deserialize_aggregate<WithIgnoredPointer_2w>",
            &validate_aggregate_deserialization<WithIgnoredPointer_2w, deserialize_fn>
        }
        .run_single(
            WithIgnoredPointer_2w{0xBE, nullptr},
            make_bytes(0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
        );
    }

    return skipped_pointer && ignored_pointer;
}

inline constexpr auto deserialize_static_container = [](
    auto& destination,
    std::span<std::byte> raw_bytes
) {
    stse::detail::deserialize_static_container(destination, std::span<const std::byte>{raw_bytes}, stse::native_endian);
};

inline constexpr auto deserialize_aggregate = [](
    auto& destination,
    std::span<std::byte> raw_bytes
) {
    stse::detail::deserialize_aggregate(destination, std::span<const std::byte>{raw_bytes}, stse::native_endian);
};

inline constexpr auto deserialize_flat = [](
    auto& destination,
    std::span<std::byte> raw_bytes
) {
    stse::detail::deserialize_flat(destination, std::span<const std::byte>{raw_bytes});
};

inline constexpr auto deserialize_final = [](
    auto& destination,
    std::span<std::byte> raw_bytes
) {
    stse::detail::deserialize(destination, std::span<const std::byte>{raw_bytes}, stse::native_endian);
};

inline constexpr auto test_deserialize_container_base  = test_deserialize_container<deserialize_static_container>;
inline constexpr auto test_deserialize_container_flat  = test_deserialize_container<deserialize_flat>;
inline constexpr auto test_deserialize_container_final = test_deserialize_container<deserialize_final>;

inline constexpr auto test_deserialize_aggregate_base  = test_deserialize_aggregate<deserialize_aggregate>;
inline constexpr auto test_deserialize_aggregate_flat  = test_deserialize_aggregate<deserialize_flat>;
inline constexpr auto test_deserialize_aggregate_final = test_deserialize_aggregate<deserialize_final>;

inline constexpr auto test_deserialize_annotated_aggregate_base  = test_deserialize_annotated_aggregate<deserialize_aggregate>;
inline constexpr auto test_deserialize_annotated_aggregate_flat  = test_deserialize_annotated_aggregate<deserialize_flat>;
inline constexpr auto test_deserialize_annotated_aggregate_final = test_deserialize_annotated_aggregate<deserialize_final>;

static_assert(test_deserialize_scalar());
static_assert(test_deserialize_container_base());

static_assert(test_deserialize_container_base());
static_assert(test_deserialize_container_flat());
static_assert(test_deserialize_container_final());

static_assert(test_deserialize_aggregate_base());
static_assert(test_deserialize_aggregate_flat());
static_assert(test_deserialize_aggregate_final());

static_assert(test_deserialize_annotated_aggregate_base());
static_assert(test_deserialize_annotated_aggregate_final());

} // namespace stse::tests
