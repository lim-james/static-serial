#include "static_serial.hpp"

static_assert(!stse::is_serializable<int*>());

struct NestedObjectWithPointer {
    struct {
        int* i = nullptr;
    } nested_ptr;
};

static_assert(!stse::is_serializable<NestedObjectWithPointer>());

struct NestedArrayObjectWithPointer {
    struct {
        std::array<int*, 5> i{};
    } nested_ptr;
};

static_assert(!stse::is_serializable<NestedArrayObjectWithPointer>());
