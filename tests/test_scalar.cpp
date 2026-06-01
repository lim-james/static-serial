#include "static_serial_test.hpp"

static_assert(stse::Serializable<char>);
static_assert(stse::Serializable<int>);
static_assert(stse::Serializable<short>);
static_assert(stse::Serializable<unsigned int>);
static_assert(stse::Serializable<float>);
static_assert(stse::Serializable<double>);
static_assert(stse::Serializable<long double>);
static_assert(stse::Serializable<bool>);

static constexpr char   c = '0';
static constexpr int    i = 0;
static constexpr float  f = 3.14f;
static constexpr double d = 123.45;
static constexpr bool   b = false;

static_assert(stse::test::test_round_trip<c>(), "Round trip failed for char");
static_assert(stse::test::test_round_trip<i>(), "Round trip failed for int");
static_assert(stse::test::test_round_trip<f>(), "Round trip failed for float");
static_assert(stse::test::test_round_trip<d>(), "Round trip failed for double");
static_assert(stse::test::test_round_trip<b>(), "Round trip failed for bool");

enum UnscopedEnum { option_a, option_b }; 
static constexpr UnscopedEnum u = UnscopedEnum::option_a;
static_assert(stse::Serializable<UnscopedEnum>);
static_assert(stse::test::test_round_trip<u>(), "Round trip failed for unscoped enum");

enum class ScopedEnum { option_a, option_b }; 
static constexpr ScopedEnum e = ScopedEnum::option_a;
static_assert(stse::Serializable<ScopedEnum>);
static_assert(stse::test::test_round_trip<e>(), "Round trip failed for scoped enum");
