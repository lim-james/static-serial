#include "static_serial_test.hpp"

#include <utility>

struct Vec3 {
    float x;
    float y;
    float z;

    bool operator==(const Vec3&) const = default;
};

static constexpr Vec3 position{.x = 0.1f, .y = 0.2f, .z = 0.3f};
static_assert(stse::test::test_back_and_forth<position>(), "Back-&-Forth failed for Aggregate Types");

///
/// Nested Types
///

struct Player {
    int id;
    Vec3 pos;
    std::array<int, 4> inventory;

    bool operator==(const Player&) const = default;
};

static constexpr Player player{
    .id = 0, 
    .pos = Vec3{.x = 0.1f, .y = 0.2f, .z = 0.3f},
    .inventory = {1, 2, 3, 4}
};
static_assert(stse::test::test_back_and_forth<player>(), "Back-&-Forth failed for Nested Types");

static constexpr std::pair std_pair{1, 2};
static_assert(stse::test::test_back_and_forth<std_pair>(), "Back-&-Forth failed for std::pair");

///
/// Inherited Types
///

struct Base { 
    int a; 
    bool operator==(const Base&) const = default;
};
struct Derived: Base { 
    int b; 
    bool operator==(const Derived&) const = default;
};

static_assert(stse::is_serializable_v<Base>);
static_assert(stse::is_serializable_v<Derived>);

static constexpr Derived derived{{1}, 2};
static_assert(stse::test::test_back_and_forth<derived>(), "Back-&-Forth failed for Derived Type");

