#include "static_serial_test.hpp"

#include <cstdint>

struct PaddedStruct {
    char a;
    std::uint64_t b;

    bool operator==(const PaddedStruct&) const = default;
};

static constexpr std::size_t COMPACT_SIZE = sizeof(char) + sizeof(std::uint64_t);

static constexpr PaddedStruct padded{'a', 0};
static constexpr auto bytes = stse::serialize(padded);

static constexpr auto [parsed_object] = stse::deserialize<PaddedStruct>(bytes).objects;
static_assert(parsed_object == padded);

///
/// Multiple padding gaps
///

struct MultiPadded {
    char     a;
    uint32_t b;
    char     c;
    uint64_t d;

    bool operator==(const MultiPadded&) const = default;
};

static constexpr std::size_t MULTI_COMPACT_SIZE =
    sizeof(char) + sizeof(uint32_t) + sizeof(char) + sizeof(uint64_t);

static constexpr MultiPadded mp{'a', 1, 'b', 2};
static constexpr auto mp_bytes = stse::serialize(mp);

static_assert(mp_bytes.size() != sizeof(MultiPadded));
static_assert(mp_bytes.size() == MULTI_COMPACT_SIZE);
static_assert(mp_bytes.size() == stse::serial_size_v<MultiPadded>);

static constexpr auto [parsed_mp] = stse::deserialize<MultiPadded>(mp_bytes).objects;
static_assert(parsed_mp == mp);


