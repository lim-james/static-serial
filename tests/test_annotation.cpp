#include "static_serial.hpp"


struct PartialSerializable {
    int a;
    [[=stse::skip]] int* b;
};

static_assert(stse::is_serializable<PartialSerializable>());
