#include "static_serial_test.hpp"

#include <stack>
#include <queue>

static constexpr std::array std_arr{1, 2, 3, 4, 5};
static_assert(stse::is_serializable_v<std::array<int, 5>>);
static_assert(stse::test::test_back_and_forth<std_arr>(), "Back-&-Forth failed for std::array");

static_assert(stse::is_serializable_v<std::tuple<int,int>>);

// _Tuple_impl has a non-structural type
// static constexpr std::tuple std_tuple{1, 2, 3, 4, 5};
// static_assert(stse::test::test_back_and_forth<std_tuple>(), "Back-&-Forth failed for std::tuple");

static_assert(stse::is_serializable_v<std::stack<int, std::array<int, 10>>>);
static_assert(stse::is_serializable_v<std::queue<int, std::array<int, 10>>>);
static_assert(stse::is_serializable_v<std::priority_queue<int, std::array<int, 10>>>);
