#include "static_serial_test.hpp"

static constexpr char   c = '0';
static constexpr int    i = 0;
static constexpr float  f = 3.14f;
static constexpr double d = 123.45;
static constexpr bool   b = false;

static_assert(stse::test::test_back_and_forth<c>(), "Back-&-Forth failed for char");
static_assert(stse::test::test_back_and_forth<i>(), "Back-&-Forth failed for int");
static_assert(stse::test::test_back_and_forth<f>(), "Back-&-Forth failed for float");
static_assert(stse::test::test_back_and_forth<d>(), "Back-&-Forth failed for double");
static_assert(stse::test::test_back_and_forth<b>(), "Back-&-Forth failed for bool");

enum UnscopedEnum { option_a, option_b }; 
static constexpr UnscopedEnum u = UnscopedEnum::option_a;
static_assert(stse::test::test_back_and_forth<u>(), "Back-&-Forth failed for unscoped enum");

enum class ScopedEnum { option_a, option_b }; 
static constexpr ScopedEnum e = ScopedEnum::option_a;
static_assert(stse::test::test_back_and_forth<e>(), "Back-&-Forth failed for scoped enum");

static_assert(stse::is_serializable_v<char>);
static_assert(stse::is_serializable_v<int>);
static_assert(stse::is_serializable_v<short>);
static_assert(stse::is_serializable_v<unsigned int>);
static_assert(stse::is_serializable_v<float>);
static_assert(stse::is_serializable_v<double>);
static_assert(stse::is_serializable_v<long double>);
static_assert(stse::is_serializable_v<bool>);
static_assert(stse::is_serializable_v<UnscopedEnum>);
static_assert(stse::is_serializable_v<ScopedEnum>);
