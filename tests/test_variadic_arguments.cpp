#include "static_serial_test.hpp"

///
/// Variadic Serialize / Deserialize
///

struct A {
    int price;
    int quantity;
    bool operator==(const A&) const = default;
};

struct B {
    int price;
    int quantity;
    int order_id;
    bool operator==(const B&) const = default;
};

struct C {
    float x;
    float y;
    bool operator==(const C&) const = default;
};

static constexpr A a{.price = 42,  .quantity = 9};
static constexpr B b{.price = 67,  .quantity = 12, .order_id = 7};
static constexpr C c{.x = 1.0f, .y = 2.0f};

///
/// Buffer size is sum of individual serial sizes
///

static_assert(
    stse::serialize(a, b).size() == stse::serial_size_v<A, B>,
    "Variadic buffer size mismatch for two aggregates"
);

static_assert(
    stse::serialize(a, b, c).size() == stse::serial_size_v<A, B, C>,
    "Variadic buffer size mismatch for three aggregates"
);

///
/// Variadic round trip via deserialize
///

static_assert(
    stse::test::test_variadic_round_trip<a, b>(),
    "Variadic round trip failed for A, B");
static_assert(
    stse::test::test_variadic_round_trip<a, b, c>(),
    "Variadic round trip failed for A, B, C"
);

///
/// Variadic round trip via deserialize_advance
///

static_assert(
    stse::test::test_variadic_advance_round_trip<a, b>(),
    "Variadic advance round trip failed for A, B"
);
static_assert(
    stse::test::test_variadic_advance_round_trip<a, b, c>(),
    "Variadic advance round trip failed for A, B, C"
);

///
/// Scalars in variadic
///

static constexpr int   i = 42;
static constexpr float f = 3.14f;
static constexpr char  ch = 'z';

static_assert(
    stse::test::test_variadic_round_trip<i, f>(),
    "Variadic round trip failed for int, float"
);
static_assert(
    stse::test::test_variadic_round_trip<i, f, ch>(),
    "Variadic round trip failed for int, float, char"
);

///
/// Mixed scalar and aggregate in variadic
///

static_assert(
    stse::test::test_variadic_round_trip<i, a>(),
    "Variadic round trip failed for int, A"
);
static_assert(
    stse::test::test_variadic_round_trip<a, i, b>(),
    "Variadic round trip failed for A, int, B"
);

///
/// Endian-aware variadic round trip
///

static_assert(
    stse::test::test_variadic_round_trip_endian<stse::big_endian, a, b>(),
    "Big endian variadic round trip failed"
);
static_assert(
    stse::test::test_variadic_round_trip_endian<stse::little_endian, a, b>(),
    "Little endian variadic round trip failed"
);
static_assert(
    stse::test::test_variadic_round_trip_endian<stse::native_endian, a, b>(),
    "Native endian variadic round trip failed"
);

///
/// Remaining span is correct after variadic deserialize
///

static_assert([] {
    constexpr auto buffer = stse::serialize(a, b);
    auto result = stse::deserialize<A, B>(buffer);
    return result.remaining.empty();
}(), "Remaining span should be empty after full deserialize");

static_assert([] {
    constexpr auto buffer_ab = stse::serialize(a, b);
    constexpr auto buffer_c  = stse::serialize(c);

    std::array<std::byte, buffer_ab.size() + buffer_c.size()> combined{};
    auto write = std::span<std::byte>{combined};
    write = stse::serialize_advance(write, a, b);
    stse::serialize_advance(write, c);

    auto result = stse::deserialize<A, B>(combined);
    return result.remaining.size() == stse::serial_size_v<C>;
}(), "Remaining span size mismatch after partial deserialize");
