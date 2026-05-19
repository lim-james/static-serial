#include "static_serial_test.hpp"

struct Vec3 {
    float x, y, z;
    bool operator==(const Vec3&) const = default;
};

static constexpr Vec3 position{.x = 0.1f, .y = 0.2f, .z = 0.3f};

static_assert(std::is_trivially_copyable_v<Vec3>);
static_assert(stse::is_serializable_v<Vec3>);
static_assert(stse::test::test_round_trip<position>(), "Round trip failed for Aggregate Types");

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

static_assert(std::is_trivially_copyable_v<Player>);
static_assert(stse::is_serializable_v<Player>);
static_assert(stse::test::test_round_trip<player>(), "Round trip failed for Nested Types");

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

static constexpr Derived derived{{1}, 2};

static_assert(std::is_trivially_copyable_v<Base>);
static_assert(std::is_trivially_copyable_v<Derived>);
static_assert(stse::is_serializable_v<Base>);
static_assert(stse::is_serializable_v<Derived>);
static_assert(stse::test::test_round_trip<derived>(), "Round trip failed for Derived Type");

