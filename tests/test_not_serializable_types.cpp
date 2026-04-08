#include "static_serial.hpp"

#include <string>
#include <vector>

static_assert(!stse::is_serializable<int*>());

class PrivatePointer {
    int* i = nullptr;
};

static_assert(!stse::is_serializable<PrivatePointer>());

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

static_assert(!stse::is_serializable<char[16]>());
static_assert(!stse::is_serializable<std::string>());
static_assert(!stse::is_serializable<std::vector<int>>());

static_assert(stse::is_serializable<std::array<int, 5>>());
static_assert(stse::is_serializable<std::pair<int, int>>());

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

static_assert(!stse::is_serializable<Derived>());

