#include "static_serial.hpp"

#include <string>
#include <vector>

static_assert(stse::is_serializable<int*>()             == false);
static_assert(stse::is_serializable<char[16]>()         == false);
static_assert(stse::is_serializable<std::string>()      == false);
static_assert(stse::is_serializable<std::vector<int>>() == false);

static_assert(stse::is_serializable<std::array<int, 5>>());
static_assert(stse::is_serializable<std::pair<int, int>>());

class PrivatePointer { int* i = nullptr; };
static_assert(stse::is_serializable<PrivatePointer>() == false);

struct NestedObjectWithPointer {
    PrivatePointer nested_ptr;
};

static_assert(stse::is_serializable<NestedObjectWithPointer>() == false);

struct NestedArrayObjectWithPointer {
    struct { std::array<int*, 5> i{}; } nested_ptr;
};

static_assert(stse::is_serializable<NestedArrayObjectWithPointer>() == false);

///
/// Virtual Types
///

struct VirtualBase { virtual void foo() {} };
struct VirtualDerived: VirtualBase { void foo() override {} };
struct NonVirtualDerived: VirtualBase { int a; };

static_assert(stse::is_serializable<VirtualBase>()       == false);
static_assert(stse::is_serializable<VirtualDerived>()    == false);
static_assert(stse::is_serializable<NonVirtualDerived>() == false);
