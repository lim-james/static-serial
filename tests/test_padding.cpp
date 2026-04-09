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
static_assert(bytes.size() != sizeof(PaddedStruct));
static_assert(bytes.size() == COMPACT_SIZE);
static_assert(bytes.size() == stse::serial_size_v<PaddedStruct>);
static_assert(stse::deserialize<PaddedStruct>(bytes).object == padded);
