#include "static_serial_test.hpp"

struct Vec3 {
    float x;
    float y;
    float z;

    bool operator==(const Vec3&) const = default;
};

static constexpr Vec3 position{.x = 0.1, .y = 0.2, .z = 0.3};
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
    .pos = Vec3{.x = 0.1, .y = 0.2, .z = 0.3},
    .inventory = {1, 2, 3, 4}
};
static_assert(stse::test::test_back_and_forth<player>(), "Back-&-Forth failed for Nested Types");
