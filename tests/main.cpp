#include "static_serial_test.hpp"

#include <tuple>
#include <cassert>
#include <print>

int main() {
    std::tuple sample{1, 2};
    assert(stse::test::test_round_trip(sample));
    std::println("All tests passed.");
}
