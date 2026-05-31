#include "stse/stse.hpp"

#include <cassert>

struct A {
    int price;
    int quantity;

    bool operator==(const A&) const = default;
};

struct B {
    int price;
    int quantity;
    int order_id;

    bool operator==(const B&) const = default;
};

int main() {
    auto a = A{.price=42, .quantity=9};
    auto b = B{.price=67, .quantity=12, .order_id=7};

    auto buffer = stse::serialize(a, b);
    std::span<const std::byte> buffer_ptr = buffer;

    A a_parsed;
    B b_parsed;
    buffer_ptr = stse::deserialize_advance(buffer_ptr, a_parsed, b_parsed);

    assert(a == a_parsed);
    assert(b == b_parsed);
}
