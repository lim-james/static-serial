#pragma once

#include <type_traits>

namespace stse::tests {

static constexpr std::size_t N = 12;

using _1b = std::uint8_t;  
static_assert(sizeof(_1b)  == 1);
static_assert(alignof(_1b) == 1);

using _2b = std::uint16_t; 
static_assert(sizeof(_2b)  == 2);
static_assert(alignof(_2b) == 2);

using _4b = std::uint32_t; 
static_assert(sizeof(_4b)  == 4);
static_assert(alignof(_4b) == 4);

using _8b = std::uint64_t; 
static_assert(sizeof(_8b)  == 8);
static_assert(alignof(_8b) == 8);

enum UnscopedEnum { option_a, option_b }; 
enum class ScopedEnum { option_a, option_b }; 

static_assert(false == std::is_scoped_enum_v<UnscopedEnum>);
static_assert(std::is_scoped_enum_v<ScopedEnum>);

struct NotTriviallyCopyable {NotTriviallyCopyable(const NotTriviallyCopyable&){}};
static_assert(false == std::is_trivially_copyable_v<NotTriviallyCopyable>);

struct PolymorphicParent { virtual void boo(); };
struct PolymorphicChild: PolymorphicParent { void boo() override; };
static_assert(std::is_polymorphic_v<PolymorphicParent>);
static_assert(std::is_polymorphic_v<PolymorphicChild>);

struct FlatAggregate { 
    _1b a; _4b b; _1b c; 
    bool operator==(const FlatAggregate&) const = default;
};
static_assert(std::is_aggregate_v<FlatAggregate>);
static_assert(std::is_trivially_copyable_v<FlatAggregate>);

struct B { _1b a; _1b b; _4b c; };
struct C { _1b a; _1b b; std::array<_1b, N> c; };
struct D { _1b a; std::array<_4b, N> b; _4b c; };
struct E { B a; std::array<_8b, N> b; C c; };
struct F { _1b a; _4b b; _4b c; _1b d; _2b e; };
struct G { _1b a; _2b b; _8b c; }; 

}
