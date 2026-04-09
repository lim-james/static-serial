#include "static_serial.hpp"

#include <string>
#include <vector>

static_assert(stse::is_serializable_v<std::string>      == false);
static_assert(stse::is_serializable_v<std::vector<int>> == false);

static_assert(stse::is_serializable_v<std::pair<int, int>>);

class PrivatePointer { int* i = nullptr; };
static_assert(stse::is_serializable_v<PrivatePointer> == false);

struct NestedObjectWithPointer {
    PrivatePointer nested_ptr;
};

static_assert(stse::is_serializable_v<NestedObjectWithPointer> == false);

struct NestedArrayObjectWithPointer {
    struct { std::array<int*, 5> i{}; } nested_ptr;
};

static_assert(stse::is_serializable_v<NestedArrayObjectWithPointer> == false);

///
/// Virtual Types
///

struct VirtualBase { virtual void foo() {} };
struct VirtualDerived: VirtualBase { void foo() override {} };
struct NonVirtualDerived: VirtualBase { int a; };

static_assert(stse::is_serializable_v<VirtualBase>       == false);
static_assert(stse::is_serializable_v<VirtualDerived>    == false);
static_assert(stse::is_serializable_v<NonVirtualDerived> == false);
