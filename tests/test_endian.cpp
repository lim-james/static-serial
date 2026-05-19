#include "static_serial_test.hpp"

static constexpr int i = 1;
static_assert(stse::test::test_round_trip_endianness<i>(stse::native_endian));
static_assert(stse::test::test_round_trip_endianness<i>(stse::little_endian));
static_assert(stse::test::test_round_trip_endianness<i>(stse::big_endian));

static constexpr float f = 1.0f;
static_assert(stse::test::test_round_trip_endianness<f>(stse::native_endian));
static_assert(stse::test::test_round_trip_endianness<f>(stse::little_endian));
static_assert(stse::test::test_round_trip_endianness<f>(stse::big_endian));

///
/// Aggregate endian round trip
///

struct EndianAggregate {
    int   a;
    float b;
    bool operator==(const EndianAggregate&) const = default;
};

static constexpr EndianAggregate ea{.a = 42, .b = 3.14f};
static_assert(stse::test::test_round_trip_endianness<ea>(stse::native_endian));
static_assert(stse::test::test_round_trip_endianness<ea>(stse::little_endian));
static_assert(stse::test::test_round_trip_endianness<ea>(stse::big_endian));

///
/// Big endian produces different bytes than little endian for multi-byte scalars
///

static constexpr int endian_probe = 0x01020304;
static constexpr auto be_bytes = stse::serialize(stse::big_endian,    endian_probe);
static constexpr auto le_bytes = stse::serialize(stse::little_endian, endian_probe);
static_assert(be_bytes != le_bytes, "Big and little endian should produce different byte layouts");

