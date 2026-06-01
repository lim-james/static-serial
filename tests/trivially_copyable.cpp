#include "static_serial_test.hpp"

struct TriviallyCopyable { int a, b; };

static_assert(std::is_trivially_copyable_v<TriviallyCopyable>);
static_assert(stse::Serializable<TriviallyCopyable>);
