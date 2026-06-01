#include "static_serial_test.hpp"

#include <queue>
#include <tuple>
#include <utility>

/// open to re-exploring this in the future for C support
/// currently is_serializable decays T - char[] -> char* 
/// but currently in alignment w/ modern C++ opt for 
/// std::array instead

static_assert(stse::Serializable<char[1]>           == false);
static_assert(stse::Serializable<decltype("Hello")> == false);

using serializable_t = int;
using static_container = std::array<int, 10>;

static_assert(std::is_trivially_copyable_v<std::tuple<serializable_t>>                 == false);
static_assert(std::is_trivially_copyable_v<std::pair<serializable_t,serializable_t>>   == false);
static_assert(std::is_trivially_copyable_v<std::priority_queue<int, static_container>> == false);

static_assert(stse::Serializable<std::tuple<serializable_t>>                 == false);
static_assert(stse::Serializable<std::pair<serializable_t,serializable_t>>   == false);
static_assert(stse::Serializable<std::priority_queue<int, static_container>> == false);
