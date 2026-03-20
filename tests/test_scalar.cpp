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
