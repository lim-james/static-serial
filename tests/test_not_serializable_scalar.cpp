#include "stse/stse.hpp"

static_assert(stse::is_serializable_v<void*>          == false);
static_assert(stse::is_serializable_v<std::nullptr_t> == false);
