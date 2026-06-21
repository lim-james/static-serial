#pragma once

#include "static_serial_test.hpp"

#include "tests/test_executor.hpp"

namespace stse::tests {

constexpr bool round_trip_char() noexcept {
    return TestExecutor{"Round trip for char", &test_round_trip<char>}
        .run_sequence('0', '1', 'a', 'A');
}

constexpr bool round_trip_int() noexcept {
    return TestExecutor{"Round trip for int", &test_round_trip<int>}
        .run_sequence(0, 1, 2, 67, 1024, -249);
}

constexpr bool round_trip_double() noexcept {
    return TestExecutor{"Round trip for double", &test_round_trip<double>}
        .run_sequence(0, 0.2, 67.25829, 0.495848, 2958295.28958386829);
}

constexpr bool round_trip_float() noexcept {
    return TestExecutor{"Round trip failed for float", &test_round_trip<float>}
        .run_sequence(0.f, 0.2f, 67.25829f, 0.495848f, 2958295.28829f);
}

constexpr bool round_trip_bool() noexcept {
    return TestExecutor{"Round trip for bool", &test_round_trip<bool>}
        .run_sequence(true, false);
}

constexpr bool round_trip_unscoped_enum() noexcept {
    return TestExecutor{"Round trip for unscoped enum", &test_round_trip<UnscopedEnum>}
        .run_sequence(UnscopedEnum::option_a, UnscopedEnum::option_b);
}

constexpr bool round_trip_scoped_enum() noexcept {
    return TestExecutor{"Round trip for scoped enum", &test_round_trip<ScopedEnum>}
        .run_sequence(ScopedEnum::option_a, ScopedEnum::option_b);
}

static_assert(round_trip_char());
static_assert(round_trip_int());
static_assert(round_trip_double());
static_assert(round_trip_float());
static_assert(round_trip_bool());
static_assert(round_trip_unscoped_enum());
static_assert(round_trip_scoped_enum());

void test_scalar() noexcept {
    round_trip_char();
    round_trip_int();
    round_trip_double();
    round_trip_float();
    round_trip_bool();
    round_trip_unscoped_enum();
    round_trip_scoped_enum();
}

}
