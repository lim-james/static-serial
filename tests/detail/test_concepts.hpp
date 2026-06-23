#pragma once

#include "tests/types.hpp"
#include "stse/detail/concepts.hpp"

#include <string>
#include <string_view>

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

namespace stse::tests {

static_assert(stse::detail::NotSerializable<void*>);
static_assert(stse::detail::NotSerializable<std::nullptr_t>);
static_assert(stse::detail::NotSerializable<PolymorphicParent>);
static_assert(stse::detail::NotSerializable<PolymorphicChild>);
static_assert(stse::detail::NotSerializable<NotTriviallyCopyable>);

static_assert(stse::detail::Scalar<char>);
static_assert(stse::detail::Scalar<unsigned char>);
static_assert(stse::detail::Scalar<bool>);
static_assert(stse::detail::Scalar<short>);
static_assert(stse::detail::Scalar<int>);
static_assert(stse::detail::Scalar<unsigned int>);
static_assert(stse::detail::Scalar<float>);
static_assert(stse::detail::Scalar<double>);
static_assert(stse::detail::Scalar<long double>);
static_assert(stse::detail::Scalar<long long>);
static_assert(false == stse::detail::Scalar<int[5]>);

static_assert(stse::detail::StaticContainer<std::array<int, 5>>);
static_assert(false == stse::detail::StaticContainer<int>);
static_assert(false == stse::detail::StaticContainer<int[5]>); // should review
static_assert(false == stse::detail::StaticContainer<std::vector<int>>);

struct concepts_A { int a; char b; };
static_assert(stse::detail::Aggregate<concepts_A>);
static_assert(false == stse::detail::Aggregate<int>);
static_assert(false == stse::detail::Aggregate<std::array<int, 5>>);

static_assert(stse::Serializable<char>);
static_assert(stse::Serializable<int>);
static_assert(stse::Serializable<short>);
static_assert(stse::Serializable<unsigned int>);
static_assert(stse::Serializable<float>);
static_assert(stse::Serializable<double>);
static_assert(stse::Serializable<long double>);
static_assert(stse::Serializable<bool>);
static_assert(stse::Serializable<UnscopedEnum>);
static_assert(stse::Serializable<ScopedEnum>);

static_assert(stse::Serializable<FlatCompactAggregate_3b>);
static_assert(stse::Serializable<FlatCompactAggregate_12b>);
static_assert(stse::Serializable<FlatInternalPaddedAggregate_8b>);
static_assert(stse::Serializable<FlatTailPaddedAggregate_12b>);
static_assert(stse::Serializable<WithCompactArray_4b>);
static_assert(stse::Serializable<WithPaddedArray_12b>);
static_assert(stse::Serializable<NestedCompactAggregate_5b>);
static_assert(stse::Serializable<NestedPaddedAggregate_12b>);
static_assert(stse::Serializable<DeeplyNestedAggregate_16b>);
static_assert(stse::Serializable<ArrayOfStructsAggregate_8b>);

static_assert(stse::Serializable<WithSkippedPointer_2w>);
static_assert(stse::Serializable<WithMultiSkippedPointers_4w>);
static_assert(stse::Serializable<WithSkippedParent_3w>);
static_assert(stse::Serializable<WithIgnoredPointer_2w>);

using serializable_t = int;
using static_container = std::array<int, 10>;

static_assert(false == std::is_trivially_copyable_v<std::string>);
static_assert(false == std::is_trivially_copyable_v<std::vector<serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::deque<serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::forward_list<serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::list<serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::set<serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::multiset<serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::unordered_set<serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::unordered_multiset<serializable_t>>);

static_assert(false == std::is_trivially_copyable_v<std::map<serializable_t,serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::multimap<serializable_t,serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::unordered_map<serializable_t,serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::unordered_multimap<serializable_t,serializable_t>>);

static_assert(false == std::is_trivially_copyable_v<std::tuple<serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::pair<serializable_t,serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::priority_queue<int, static_container>>);

static_assert(false == std::is_trivially_copyable_v<std::stack<serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::queue<serializable_t>>);
static_assert(false == std::is_trivially_copyable_v<std::priority_queue<serializable_t>>);


static_assert(false == stse::Serializable<WithPrivatePointer>);
static_assert(false == stse::Serializable<WithNestedPointer>);
static_assert(false == stse::Serializable<WithNestedArrayWithPointer>);

/// open to re-exploring this in the future for C support
/// currently is_serializable decays T - char[] -> char* 
/// but currently in alignment w/ modern C++ opt for 
/// std::array instead

static_assert(false == stse::Serializable<char[1]>);
static_assert(false == stse::Serializable<decltype("Hello")>);
static_assert(false == stse::Serializable<std::string>);
static_assert(false == stse::Serializable<std::string_view>);

static_assert(false == stse::Serializable<std::vector<serializable_t>>);
static_assert(false == stse::Serializable<std::deque<serializable_t>>);
static_assert(false == stse::Serializable<std::forward_list<serializable_t>>);
static_assert(false == stse::Serializable<std::list<serializable_t>>);

static_assert(false == stse::Serializable<std::set<serializable_t>>);
static_assert(false == stse::Serializable<std::multiset<serializable_t>>);
static_assert(false == stse::Serializable<std::unordered_set<serializable_t>>);
static_assert(false == stse::Serializable<std::unordered_multiset<serializable_t>>);

static_assert(false == stse::Serializable<std::map<serializable_t,serializable_t>>);
static_assert(false == stse::Serializable<std::multimap<serializable_t,serializable_t>>);
static_assert(false == stse::Serializable<std::unordered_map<serializable_t,serializable_t>>);
static_assert(false == stse::Serializable<std::unordered_multimap<serializable_t,serializable_t>>);

static_assert(false == stse::Serializable<std::stack<serializable_t>>);
static_assert(false == stse::Serializable<std::queue<serializable_t>>);
static_assert(false == stse::Serializable<std::priority_queue<serializable_t>>);

static_assert(false == stse::Serializable<std::tuple<serializable_t>>);
static_assert(false == stse::Serializable<std::pair<serializable_t,serializable_t>>);
static_assert(false == stse::Serializable<std::priority_queue<int, static_container>>);

static_assert(false == stse::Serializable<std::span<serializable_t>>);
}
