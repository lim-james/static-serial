#include "static_serial_test.hpp"

static constexpr int i = 1;
static_assert(stse::test::test_back_and_forth_endianness<i>(stse::native_endian));
static_assert(stse::test::test_back_and_forth_endianness<i>(stse::little_endian));
static_assert(stse::test::test_back_and_forth_endianness<i>(stse::big_endian));

static constexpr float f = 1.0f;
static_assert(stse::test::test_back_and_forth_endianness<f>(stse::native_endian));
static_assert(stse::test::test_back_and_forth_endianness<f>(stse::little_endian));
static_assert(stse::test::test_back_and_forth_endianness<f>(stse::big_endian));
