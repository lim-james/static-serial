#include "static_serial.hpp"

static constexpr int* i = nullptr;
static_assert(stse::serialize(i));

struct NestedObjectWithPointer {
    struct {
        int* i = nullptr;
    } nested_ptr;
};

static constexpr NestedObject obj{};
static_assert(stse::serialize(obj));

struct NestedArrayObjectWithPointer {
    struct {
        std::array<int*, 5> i{};
    } nested_ptr;
};

static constexpr NestedArrayObject array_obj{};
static_assert(stse::serialize(array_obj));
