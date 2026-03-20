#include "static_serial.hpp"

#include <cstdint>

template<auto data>
constexpr bool test_back_and_forth() {
    using T = decltype(data);
    auto bytes    = stse::serialize(data);
    auto restored = stse::deserialize<T>(bytes);
    return data == restored;
}

///
/// 1. Test scalars
///

static constexpr char   c = '0';
static constexpr int    i = 0;
static constexpr float  f = 3.14f;
static constexpr double d = 123.45;
static constexpr bool   b = false;

static_assert(test_back_and_forth<c>(), "Back-&-Forth failed for char");
static_assert(test_back_and_forth<i>(), "Back-&-Forth failed for int");
static_assert(test_back_and_forth<f>(), "Back-&-Forth failed for float");
static_assert(test_back_and_forth<d>(), "Back-&-Forth failed for double");
static_assert(test_back_and_forth<b>(), "Back-&-Forth failed for bool");

///
/// 2. Test std::array
///

static constexpr std::array std_arr{1, 2, 3, 4, 5};
static_assert(test_back_and_forth<std_arr>(), "Back-&-Forth failed for std::array");

///
/// 3. Test aggregate type
///

struct Vec3 {
    float x;
    float y;
    float z;

    bool operator==(const Vec3&) const = default;
};

static constexpr Vec3 position{.x = 0.1, .y = 0.2, .z = 0.3};
static_assert(test_back_and_forth<position>(), "Back-&-Forth failed for Aggregate Types");

///
/// 4. Ensure no padding
///

struct PaddedStruct {
    char a;
    std::uint64_t b;

    bool operator==(const PaddedStruct&) const = default;
};

static constexpr PaddedStruct padded{'a', 0};
static constexpr auto bytes = stse::serialize(padded);
static_assert(bytes.size() != sizeof(PaddedStruct));
static_assert(bytes.size() == 9);
static_assert(stse::deserialize<PaddedStruct>(bytes) == padded);

struct Player {
    std::uint32_t id;
    Vec3 pos;
    std::array<int, 4> inventory;

    bool operator==(const Player&) const = default;
};

///
/// 5. Test nested types
///

static constexpr Player player{
    .id = 0, 
    .pos = Vec3{.x = 0.1, .y = 0.2, .z = 0.3},
    .inventory = {1, 2, 3, 4}
};
static_assert(test_back_and_forth<player>(), "Back-&-Forth failed for Nested Types");
 
///
/// 6. Rejected Types
///

struct NonTrivialPlayer {
    std::uint32_t  id;
    std::uint32_t* ptr;

    bool operator==(const NonTrivialPlayer&) const = default;
};

// static constexpr NonTrivialPlayer non_trivial_player{.id = 0, .ptr = nullptr};
// static_assert(test_back_and_forth<non_trivial_player>());

int main() {}

