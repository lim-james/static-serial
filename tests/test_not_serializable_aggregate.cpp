#include "static_serial.hpp"

class PrivatePointer { int* i = nullptr; };
struct NestedPointer { PrivatePointer nested_ptr; };
struct NestedArrayWithPointer {
    struct { std::array<int*, 5> i{}; } nested_ptr;
};

static_assert(stse::is_serializable_v<PrivatePointer>         == false);
static_assert(stse::is_serializable_v<NestedPointer>          == false);
static_assert(stse::is_serializable_v<NestedArrayWithPointer> == false);

struct VirtualBase { virtual void foo() {} };
struct VirtualDerived: VirtualBase { void foo() override {} };
struct NonVirtualDerived: VirtualBase { int a; };

static_assert(stse::is_serializable_v<VirtualBase>       == false);
static_assert(stse::is_serializable_v<VirtualDerived>    == false);
static_assert(stse::is_serializable_v<NonVirtualDerived> == false);
