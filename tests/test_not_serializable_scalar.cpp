#include "stse/stse.hpp"

static_assert(stse::Serializable<void*>          == false);
static_assert(stse::Serializable<std::nullptr_t> == false);
