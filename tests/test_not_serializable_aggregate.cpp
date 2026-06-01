#include "stse/stse.hpp"

class PrivatePointer { int* i = nullptr; };
struct NestedPointer { PrivatePointer nested_ptr; };
struct NestedArrayWithPointer {
    struct { std::array<int*, 5> i{}; } nested_ptr;
};

static_assert(stse::Serializable<PrivatePointer>         == false);
static_assert(stse::Serializable<NestedPointer>          == false);
static_assert(stse::Serializable<NestedArrayWithPointer> == false);

struct VirtualBase { virtual void foo() {} };
struct VirtualDerived: VirtualBase { void foo() override {} };
struct NonVirtualDerived: VirtualBase { int a; };

static_assert(stse::Serializable<VirtualBase>       == false);
static_assert(stse::Serializable<VirtualDerived>    == false);
static_assert(stse::Serializable<NonVirtualDerived> == false);
