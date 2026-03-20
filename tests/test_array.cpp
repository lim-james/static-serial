#include "static_serial_test.hpp"

static constexpr std::array std_arr{1, 2, 3, 4, 5};
static_assert(stse::test::test_back_and_forth<std_arr>(), "Back-&-Forth failed for std::array");
