#include "static_serial_test.hpp"

#include <stack>
#include <queue>

using static_container = std::array<int, 10>;

static_assert(std::is_trivially_copyable_v<static_container>);
static_assert(std::is_trivially_copyable_v<std::stack<int, static_container>>);
static_assert(std::is_trivially_copyable_v<std::queue<int, static_container>>);

static_assert(stse::is_serializable_v<static_container>);
static_assert(stse::is_serializable_v<std::stack<int, static_container>>);
static_assert(stse::is_serializable_v<std::queue<int, static_container>>);

static constexpr std::array std_arr{1, 2, 3, 4, 5};
static_assert(stse::test::test_round_trip<std_arr>(), "Round trip failed for std::array");
