#include "static_serial_test.hpp"

#include <tuple>
#include <cassert>

int main() {
    std::tuple sample{1, 2};
    assert(stse::test::test_round_trip(sample));
}
