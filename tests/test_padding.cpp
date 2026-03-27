#include "static_serial_test.hpp"

#include <cstdint>

struct PaddedStruct {
    char a;
    std::uint64_t b;

    bool operator==(const PaddedStruct&) const = default;
};

static constexpr PaddedStruct padded{'a', 0};
static constexpr auto bytes = stse::serialize(padded);
static_assert(bytes.size() != sizeof(PaddedStruct));
static_assert(bytes.size() == 9);
static_assert(stse::deserialize<PaddedStruct>(bytes).first == padded);


