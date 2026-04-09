#include "static_serial_test.hpp"

static_assert(stse::is_serializable_v<char>);
static_assert(stse::is_serializable_v<int>);
static_assert(stse::is_serializable_v<short>);
static_assert(stse::is_serializable_v<unsigned int>);
static_assert(stse::is_serializable_v<float>);
static_assert(stse::is_serializable_v<double>);
static_assert(stse::is_serializable_v<long double>);
static_assert(stse::is_serializable_v<bool>);

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
static_assert(stse::is_serializable_v<UnscopedEnum>);
static_assert(stse::test::test_round_trip<u>(), "Round trip failed for unscoped enum");

enum class ScopedEnum { option_a, option_b }; 
static constexpr ScopedEnum e = ScopedEnum::option_a;
static_assert(stse::is_serializable_v<ScopedEnum>);
static_assert(stse::test::test_round_trip<e>(), "Round trip failed for scoped enum");
