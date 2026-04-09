#include "static_serial.hpp"

#include <string>

#include <vector>
#include <deque>
#include <forward_list>
#include <list>

#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include <stack>
#include <queue>

static_assert(stse::is_serializable_v<std::string> == false);

using serializable_t = int;

static_assert(stse::is_serializable_v<std::vector<serializable_t>>       == false);
static_assert(stse::is_serializable_v<std::deque<serializable_t>>        == false);
static_assert(stse::is_serializable_v<std::forward_list<serializable_t>> == false);
static_assert(stse::is_serializable_v<std::list<serializable_t>>         == false);

static_assert(stse::is_serializable_v<std::set<serializable_t>>                == false);
static_assert(stse::is_serializable_v<std::multiset<serializable_t>>           == false);
static_assert(stse::is_serializable_v<std::unordered_set<serializable_t>>      == false);
static_assert(stse::is_serializable_v<std::unordered_multiset<serializable_t>> == false);

static_assert(stse::is_serializable_v<std::map<serializable_t,serializable_t>>                == false);
static_assert(stse::is_serializable_v<std::multimap<serializable_t,serializable_t>>           == false);
static_assert(stse::is_serializable_v<std::unordered_map<serializable_t,serializable_t>>      == false);
static_assert(stse::is_serializable_v<std::unordered_multimap<serializable_t,serializable_t>> == false);

static_assert(stse::is_serializable_v<std::stack<serializable_t>>          == false);
static_assert(stse::is_serializable_v<std::queue<serializable_t>>          == false);
static_assert(stse::is_serializable_v<std::priority_queue<serializable_t>> == false);

static_assert(stse::is_serializable_v<std::span<serializable_t>> == false);
