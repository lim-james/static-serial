#include "static_serial_test.hpp"

struct PartialSerializable {
    int a;
    [[=stse::skip]] int* b;
    bool operator==(const PartialSerializable&) const = default;
};

static_assert(stse::is_serializable_v<PartialSerializable>);

static constexpr PartialSerializable ps{1, nullptr};
static_assert(stse::test::test_round_trip<ps>());
static_assert(stse::serial_size_v<PartialSerializable> == sizeof(int));
